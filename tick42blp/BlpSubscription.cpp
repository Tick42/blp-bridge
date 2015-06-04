/*
* BlpBridge: The Bloomberg v3 API Bridge for OpenMama
	* Copyright (C) 2012 Tick42 Ltd.
	*
	* This library is free software; you can redistribute it and/or
	* modify it under the terms of the GNU Lesser General Public
	* License as published by the Free Software Foundation; either
	* version 2.1 of the License, or (at your option) any later version.
	*
	* This library is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	* Lesser General Public License for more details.
	*
	* You should have received a copy of the GNU Lesser General Public
	* License along with this library; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
	* 02110-1301 USA
	*
	*Distributed under the Boost Software License, Version 1.0.
	*    (See accompanying file LICENSE_1_0.txt or copy at
	*         http://www.boost.org/LICENSE_1_0.txt)
	*

	*/

#include "common.h"
#include <string.h>

#include <transportimpl.h>
#include <subscriptionimpl.h>
#include <msgimpl.h>
#include <datetimeimpl.h>

#include "transportbridge.h"

#include "blpsubscription.h"
#include "blpservice.h"
#include "blpitem.h"
#include "blputils.h"

#include <list>
#include <boost/assign/list_of.hpp>
#include <boost/scope_exit.hpp>

static const std::string FLD_INFO_SERVICE("//blp/apiflds");

using namespace std;
using namespace blpapi;


// Bloomberg schema identifiers strings 
namespace {
	Name EXCEPTIONS("exceptions");
	Name FIELD_ID("fieldId");
	Name REASON("reason");
	Name CATEGORY("category");
	Name DESCRIPTION("description");
	Name SUBSCRIPTIONSTARTED("SubscriptionStarted");
	Name SUBSCRIPTIONFAILED("SubscriptionFailure");
	Name SUBSCRIPTIONTERMINATED("SubscriptionTerminated");
	Name INVALIDSECURITY("BAD_SEC");

	const Name SECURITY_DATA("securityData");
	const Name SECURITY("security");
	const Name FIELD_DATA("fieldData");
	const Name RESPONSE_ERROR("responseError");
	const Name SECURITY_ERROR("securityError");
	const Name FIELD_EXCEPTIONS("fieldExceptions");
	const Name ERROR_INFO("errorInfo");
	const Name MESSAGE("message");
	const Name SESSION_TERMINATED("SessionTerminated");
	const Name SESSION_STARTUP_FAILURE("SessionStartupFailure");

}



BlpSubscription::BlpSubscription(const std::string& source,
	const std::string& symbol,
	mamaTransport transport,
	mamaQueue queue,
	mamaMsgCallbacks callback,
	mamaSubscription subscription,
	void* closure) :
source_ (source),
	symbol_ (symbol),
	transport_ (transport),
	queue_ (queue),
	callback_ (callback),
	subscription_ (subscription),
	closure_ (closure),
	msg_ (0),
	isNotMuted_ (true),
	isValid_ (true),
	isSubscribed_ (false),
	//subscriptions_ (),
	type_ (UNKNOWN_SUBSCRIPTION),
	msgTotal_(0),
	msgNum_(0)
{
	using namespace std;

	// create a mama msg object for notifying updates

	mamaMsg_create (&msg_);

	// create the blp Item

	// first get the service
	BlpTransportBridge* pTransportBridge;
	mama_status status = mamaTransport_getBridgeTransport (transport, (transportBridge*)&pTransportBridge);
	if ((status != MAMA_STATUS_OK) || (pTransportBridge == NULL))
	{
		isValid_ = false;
		mama_log(MAMA_LOG_LEVEL_ERROR,"BlpSubscription - failed to obtain transportBridge");
		return ;
	}
	blpServicePtr_t service = pTransportBridge->GetService(string(source));

	// if the service is invalid the warn and exit
	if(service.get() == 0)
	{
		mama_log(MAMA_LOG_LEVEL_WARN, "Failed to get service for source %s", source.c_str());
		isValid_ = false;
		return;		
	}

	AddRef();
	blpItem_ = service->CreateItem(symbol, this);
}

BlpSubscription::~BlpSubscription()
{
	mamaMsg_destroy(msg_);
}

bool BlpSubscription::Subscribe(BlpTransportBridge& blpTransportBridge)
{
	isSubscribed_ = blpItem_->Open();
		
	return isSubscribed_;

}

void BlpSubscription::Unsubscribe(BlpTransportBridge& blpTransportBridge)
{
	isSubscribed_ = false;

	blpItem_->Close();

	return;
}


void BlpSubscription::OnUpdate(blpapi::Message& msg, FieldSet_t & fields, bool filterFields)

{
	if (!isSubscribed_)
		return;

	if(!isNotMuted_)
		return;

#ifdef ENABLE_TESTING
	onUpdateTest(*this);
	return;
#endif
	mama_status status;

	// Make sure that the subscription is in a state to process messages
	if ((!isNotMuted_) || (!isValid_)) return;

	if (MAMA_STATUS_OK != (status = mamaSubscription_setSymbol(subscription_, "Update")))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR,
			"BlpSubscription::onUpdate(): "
			"mamaSubscription_setSymbol failed. [%d]",
			status);
		return;
	}

	// we reuse a single mamaMsg object
	mamaMsg_clear(msg_);

	//// build the payload
	//mamaBridgeImpl* bridge = mamaSubscription_getBridgeImpl (subscription_);
	//mamaMsgImpl_useBridgePayload(msg_, bridge);


	if (!msg_)
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "blp_callback(): "
			"Could not get cached mamaMsg from event queue.");
		return;
	}


	// Set correct type of mamaMsg from the  blp message type
	setMsgType(msg);

	// Add fields from blp message to mamaMsg
	if (setMsgFields(msg.asElement(), fields, filterFields) > 0)
	{
		setMsgNum();
		/*Process the message as normal*/
		if (MAMA_STATUS_OK != (status = mamaSubscription_processMsg(subscription_, msg_)))
		{
			mama_log (MAMA_LOG_LEVEL_ERROR,
				"blp_callback(): "
				"mamaSubscription_processMsg() failed. [%d]",
				status);
		}
	}
}

void BlpSubscription::OnInvalidSec(blpapi::Message& msg)
{
	mama_status status;

//	mamaMsg_addI32(msg_, "MdMsgStatus", 2, MAMA_MSG_STATUS_NOT_FOUND);
	mamaMsg_addI32(msg_, "MdMsgType", 1, MAMA_MSG_TYPE_SEC_STATUS);
	mamaMsg_addI32(msg_, "MdMsgStatus", 2, MAMA_MSG_STATUS_BAD_SYMBOL);

	if (MAMA_STATUS_OK != (status=mamaSubscription_processMsg(subscription_, msg_)))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR,
			"blp_callback(): "
			"mamaSubscription_processMsg() failed. [%d]",
			status);
	}
}


void BlpSubscription::ProcessResponse(blpapi::Message& msg, FieldSet_t & fields, bool filterFields)
{
#ifdef ENABLE_TESTING
	onResponseTest(*this);
	return;
#endif
	mama_status status;

	mamaMsg_clear(msg_);

	mamaBridgeImpl* bridge = mamaSubscription_getBridgeImpl (subscription_);
	mamaMsgImpl_useBridgePayload    (msg_, bridge);

	if (msg.asElement().hasElement(RESPONSE_ERROR))
	{
		// todo - need to deal with failurew of whole request
		// for the moment just deal with individual securities
		return;
	}

	Element securities = msg.getElement(SECURITY_DATA);
	size_t numSecurities = securities.numValues();
	int fieldsUsed = 0;

	for (size_t i = 0; i < numSecurities; ++i) 
	{
		Element security = securities.getValueAsElement(i);
		std::string ticker = security.getElementAsString(SECURITY);
		int seqNum = security.getElementAsInt32("sequenceNumber");

		if (security.hasElement("securityError"))
		{

			string secError = security.getElement(SECURITY_ERROR).getElementAsString(MESSAGE);
			continue;
		}

		// first we need to count the fields to make a field list
		int numFields = 0;
		if (security.hasElement(FIELD_DATA))
		{
			const Element fields = security.getElement(FIELD_DATA);
			numFields += fields.numElements();
		}

		if (security.hasElement(FIELD_EXCEPTIONS))
		{
			const Element fieldExceptions = security.getElement(FIELD_EXCEPTIONS);
			numFields += fieldExceptions.numValues();
		}

		if (security.hasElement(FIELD_DATA))
		{
			const Element fieldData = security.getElement(FIELD_DATA);
			fieldsUsed += setMsgFields(fieldData, fields, filterFields );
		}

		Element fieldExceptions = security.getElement(FIELD_EXCEPTIONS);
		if (fieldExceptions.numValues() > 0)
		{
			for (size_t k = 0; k < fieldExceptions.numValues(); ++k)
			{
				Element fieldException = fieldExceptions.getValueAsElement(k);
				Element errInfo = fieldException.getElement(ERROR_INFO);

				mamaMsg_addString(msg_, fieldException.getElementAsString(FIELD_ID), 0, errInfo.getElementAsString(MESSAGE));
				++fieldsUsed;
			}
		}
	}

	/*Add fields from blp message to mamaMsg*/
	if (fieldsUsed > 0)
	{
		mamaMsg_addI32(msg_, "MdMsgType", 1, MAMA_MSG_TYPE_INITIAL);
		mamaMsg_addI32(msg_, "MdMsgStatus", 2, MAMA_MSG_STATUS_OK);

		/*Process the message as normal*/
		if (MAMA_STATUS_OK != (status=mamaSubscription_processMsg(subscription_, msg_)))
		{
			mama_log (MAMA_LOG_LEVEL_ERROR,
				"blp_callback(): "
				"mamaSubscription_processMsg() failed. [%d]",
				status);
		}
	}


}

void BlpSubscription::setMsgType(const blpapi::Message& blpmsg)
{
	static const blpapi::Name MKTDATA_EVENT_TYPE("MKTDATA_EVENT_TYPE");
	static const blpapi::Name MKTDATA_EVENT_SUBTYPE("MKTDATA_EVENT_SUBTYPE");

	mamaMsgType msgType = MAMA_MSG_TYPE_UNKNOWN;

	if (blpmsg.hasElement(MKTDATA_EVENT_TYPE))
	{
		const blpapi::Element eventType = blpmsg.getElement(MKTDATA_EVENT_TYPE);
		std::string value = eventType.getValueAsString();

		if (value == "SUMMARY")
		{
			if (blpmsg.hasElement(MKTDATA_EVENT_SUBTYPE))
			{
				const blpapi::Element eventSubType = blpmsg.getElement(MKTDATA_EVENT_SUBTYPE);
				std::string value = eventSubType.getValueAsString();
				if (value == "INITPAINT")
				{
					msgType = MAMA_MSG_TYPE_INITIAL;
				}
				else
				{
					msgType = MAMA_MSG_TYPE_UPDATE;
				}
			}
		}
		else if (value == "QUOTE")
		{
			msgType = MAMA_MSG_TYPE_QUOTE;
		}
		else if (value == "TRADE")
		{
			msgType = MAMA_MSG_TYPE_TRADE;
		}
		else if (value == "MARKETDEPTH")
		{
			msgType = MAMA_MSG_TYPE_BOOK_UPDATE;
		}

	}

	BLP_PRINT(stdout, "message type %d\n", msgType );
	mamaMsg_addI32(msg_, "MdMsgType", 1, msgType);
	mamaMsg_addI32(msg_, "MdMsgStatus", 2, MAMA_MSG_STATUS_OK);
	mamaMsg_addString(msg_, "wIssueSymbol",305, symbol_.c_str());

}

int BlpSubscription::setMsgFields(const blpapi::Element& blpmsg, FieldSet_t & fields, bool filterFields)
{
	int numProcessedFields = 0;
	int numFields = blpmsg.numElements();

	BLP_PRINT(stdout, "number of fields: %d\n", numFields);
	for (int i = 0; i < numFields; ++i) 
	{
		const blpapi::Element field = blpmsg.getElement(i);
		if (field.numValues() < 1) 
		{
			BLP_PRINT(stdout, "        %s is NULL\n", field.name().string());
			continue;
		}

		std::string fieldName(field.name().string());

		if (filterFields && fields.find(fieldName) == fields.end())
		{
			// filtered field
			continue;
		}

		bool handled = true;
		mama_status res;
		int fieldDataType = field.datatype();
		mama_fid_t fid =0; 

		//Find the field in the transport dictionary (look for concrete data type) after that. if there is no one in the map then fall back to generic type translation
		BlpTransportBridge* blpTransportBridge = BlpTransportBridge::GetTransport(GetTransport());

		BlpFieldDictionary::fieldValue_t fieldValue;
		BlpFieldDictionary::operationOnType_t operationOnType;
		bool hasOoperationOnType=false;
		if (blpTransportBridge->BlpWombatDictionary_->FindField(fieldName,fieldValue))
		{
			// Translate from Bloomberg to mama names/types
			fieldName =  fieldValue.wombat_field_name;
			fid = fieldValue.wombat_fid;
			auto operationTypesKey = BlpFieldDictionary::typesKey_t((blpapi_DataType_t)fieldDataType, fieldValue.wombat_field_type);
			if (blpTransportBridge->BlpWombatDictionary_->FindOperationForTypes(operationTypesKey, operationOnType))
			{
				mama_log (MAMA_LOG_LEVEL_FINEST, "setMsgFields: up-cast type translation from Blp to MAMA for field %s", fieldName.c_str());
				hasOoperationOnType = true;
			}

		}
		else
		{
			handled = blpTransportBridge->dictIncludeUnmappedFields_;
			if (!handled) continue;
			fid = blpTransportBridge->BlpWombatDictionary_->GetNonTranslatedFieldFid(fieldName);
		}


		// Run mapped operation for the combination of Blp type & MAMA type, if there is none, then fall back to default type translation from Blp to MAMA
		if (hasOoperationOnType )
			res = operationOnType(msg_, fieldName, fid, field);
		else
		{
			//fall back to default 
			switch (fieldDataType)
			{
			case BLPAPI_DATATYPE_BOOL:
				res = mamaMsg_addBool(msg_, fieldName.c_str(), fid, (mama_bool_t)field.getValueAsBool());
				break;
			case BLPAPI_DATATYPE_CHAR:
				res = mamaMsg_addChar(msg_, fieldName.c_str(), fid, field.getValueAsChar());
				break;
			case BLPAPI_DATATYPE_BYTE:
				res = mamaMsg_addU8(msg_, fieldName.c_str(), fid, (mama_u8_t)field.getValueAsChar());
				break;
			case BLPAPI_DATATYPE_INT32:
				res = mamaMsg_addI32(msg_, fieldName.c_str(), fid, (mama_i32_t)field.getValueAsInt32());
				break;
			case BLPAPI_DATATYPE_INT64:
				res = mamaMsg_addI64(msg_, fieldName.c_str(), fid, (mama_i64_t)field.getValueAsInt64());
				break;
			case BLPAPI_DATATYPE_FLOAT32:
				res = mamaMsg_addF32(msg_, fieldName.c_str(), fid, (mama_f32_t)field.getValueAsFloat32());
				break;
			case BLPAPI_DATATYPE_FLOAT64:
				res = mamaMsg_addF64(msg_, fieldName.c_str(), fid, (mama_f64_t)field.getValueAsFloat64());
				break;
			case BLPAPI_DATATYPE_STRING:
			case BLPAPI_DATATYPE_ENUMERATION:
			case BLPAPI_DATATYPE_SEQUENCE:
				res = mamaMsg_addString(msg_, fieldName.c_str(), fid, field.getValueAsString());
				break;
			case BLPAPI_DATATYPE_DATE:
				{
					Datetime dateTime = field.getValueAsDatetime();
					mamaDateTime dt;

					mamaDateTime_create(&dt);
					BOOST_SCOPE_EXIT( (&dt) )
					{
						mamaDateTime_destroy(dt);
					} BOOST_SCOPE_EXIT_END;

					mamaDateTime_setToMidnightToday(dt,	NULL);
					mamaDateTime_setDate(dt, dateTime.year(), dateTime.month(), dateTime.day());
					mamaMsg_addDateTime(msg_, fieldName.c_str(), fid, dt);

					//log as FINEST the date time conversion, due to the differences in convention between Bloomberg and OpenMAMA
					char dtBuf[50] = {0};
					mamaDateTime_getAsString(dt, dtBuf, sizeof(dtBuf));
					mama_log (MAMA_LOG_LEVEL_FINEST, "Convert Date/Time BLPAPI_DATATYPE_DATE->MAMA_FIELD_TYPE_TIME field %s = [%s]", fieldName.c_str(), dtBuf);
				}
				break;
			case BLPAPI_DATATYPE_TIME:
				{
					Datetime dateTime = field.getValueAsDatetime();
					mamaDateTime dt;

					mamaDateTime_create(&dt);
					BOOST_SCOPE_EXIT( (&dt) )
					{
						mamaDateTime_destroy(dt);
					} BOOST_SCOPE_EXIT_END;

					mamaDateTime_setToMidnightToday(dt,	NULL);
					mamaDateTime_setTime(dt, dateTime.hours(), dateTime.minutes(), dateTime.seconds(), dateTime.milliSeconds() * 1000);
					mamaMsg_addDateTime(msg_, fieldName.c_str(), fid, dt);

					//log as FINEST the date time conversion, due to the differences in convention between Bloomberg and OpenMAMA
					char dtBuf[50] = {0};
					mamaDateTime_getAsString(dt, dtBuf, sizeof(dtBuf));
					mama_log (MAMA_LOG_LEVEL_FINEST, "Convert Date/Time BLPAPI_DATATYPE_TIME->MAMA_FIELD_TYPE_TIME field %s =  [%s]",fieldName.c_str(), dtBuf);
				}
				break;
			case BLPAPI_DATATYPE_DATETIME:
				{
					Datetime dateTime = field.getValueAsDatetime();
					mamaDateTime dt;

					mamaDateTime_create(&dt);
					BOOST_SCOPE_EXIT( (&dt) )
					{
						mamaDateTime_destroy(dt);
					} BOOST_SCOPE_EXIT_END;

					mamaDateTime_setToMidnightToday(dt,	NULL);

					bool hasDate = false;
					if (dateTime.day() != 1 && dateTime.month() != 1 && dateTime.year() != 1)
					{
						mamaDateTime_setDate(dt, dateTime.year(), dateTime.month(), dateTime.day());
						hasDate = true;
					}

					mamaDateTime_setTime(dt, dateTime.hours(), dateTime.minutes(), dateTime.seconds(), dateTime.milliSeconds() * 1000);
					mamaMsg_addDateTime(msg_, fieldName.c_str(), fid, dt);
					time_t seconds = (time_t) mamaDateTimeImpl_getSeconds(*dt);

					//log as FINE the date time conversion, due to the differences in convention between Bloomberg and OpenMAMA
					char dtBuf[50] = {0};
					mamaDateTime_getAsString(dt, dtBuf, sizeof(dtBuf));
					mama_log (MAMA_LOG_LEVEL_FINEST, "Convert Date/Time BLPAPI_DATATYPE_DATETIME->MAMA_FIELD_TYPE_TIME field %s = [%s] [%s]",fieldName.c_str(), dtBuf,(hasDate) ? "field has date value" : "field doesn't have date value");
				}
				break;
			case BLPAPI_DATATYPE_BYTEARRAY:
			case BLPAPI_DATATYPE_DECIMAL:
			case BLPAPI_DATATYPE_CHOICE:
			case BLPAPI_DATATYPE_CORRELATION_ID:
			default:
				handled = false;
				break;
			}
		}

		// Assume all values are scalar.
		//BLP_PRINT(stdout, "        %s = %s\n", fieldName.c_str(), field.getValueAsString());
		//DUMP_FIELD(fieldName.c_str(), (blpapi_DataType_t)fieldDataType, field.getValueAsString());

		if (handled)
		{
			++numProcessedFields;
		}

	}

	//DUMP_NL();

	BLP_PRINT(stdout,"Number of processed fields: %d\n", numProcessedFields);

	return numProcessedFields;
}


void BlpSubscription::setMsgNum()
{

	mamaMsg_addI32(msg_, "MdMsgTotal", 8, msgTotal_);
	mamaMsg_addI32(msg_, "MdMsgNum", 7, msgNum_);
	mamaMsg_addI64(msg_, "MdSeqNum", 10, msgNum_);  //Support for message sequence

	++msgNum_;
	++msgTotal_;
}

void BlpSubscription::AddRef()
{
	refCount_.increment();
}

void BlpSubscription::Release()
{
	if (refCount_.decrement() == 0)
	{
		delete this;
	}
}

