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
#include "blpsession.h"
#include "blpitem.h"

#include <fstream>
#include <boost/format.hpp>


const char* blpapiMktDataServiceName = "//blp/mktdata";
const char* blpapiRefDataServiceName = "//blp/refdata";

BlpSession::BlpSession()
	
	: started_(false), stopped_(false), blpapiSession_(0)
{
}


BlpSession::~BlpSession(void)
{
}


BlpMktDataSession::BlpMktDataSession()
{

}

BlpMktDataSession::~BlpMktDataSession()
{

}


bool BlpMktDataSession::Start( blpapi::SessionOptions & options, blpapi::EventHandler * pEvtHandler )
{

	if (started_)
	{
		// TODO warn about bad state
		return false;
	}

	if(blpapiSession_ )
	{
		// theres an old session around, so clean it up
		blpapiSession_->stop();
		delete blpapiSession_; blpapiSession_=0;
	}

	// make a new session
	
	blpapiSession_ = new (std::nothrow) blpapi::Session(options, pEvtHandler);

	if (!blpapiSession_)
		return false;
	// try to start it
	if(!blpapiSession_->start())
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "blpMktDataSession::Start: Failed to start blp session");
		delete blpapiSession_; blpapiSession_=0;
		return false;
	}

	// Try to open the service
	if(!blpapiSession_->openService(blpapiMktDataServiceName))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "blpMktDataSession::Start: Failed to open the blp service");
		blpapiSession_->stop();
		delete blpapiSession_; blpapiSession_=0;
		return false;
	}

	// note that in this implementation we dont actually do anything much with the service, but we include some commented out code to 
	// dump the content of the //blp/mktdata schema (which enumerates the set of fields available)
	blpapiService_ = blpapiSession_->getService(blpapiMktDataServiceName);

	// compi8led this in (and change the filename if you want) to create a listing of all the available real time fields
#if 0
	DumpSchema("mktDataShema.txt");

#endif

	started_ = true;

	return true;



}


bool BlpMktDataSession::Stop()
{
	stopped_=true;
	if(!started_)
	{
		mama_log (MAMA_LOG_LEVEL_WARN, "BlpMktDataSession::Stop: service is already stopped");
		return false;
	}

	if(! blpapiSession_)
	{
		mama_log (MAMA_LOG_LEVEL_WARN, "BlpRefDataBlpMktDataSessionSession::Stop: no api session");
		return false;
	}

	blpapiSession_->stop();
	delete blpapiSession_; blpapiSession_=0;
	
	started_ = false;
	return true;
}

void BlpMktDataSession::DumpSchema(char * filename)
{

	std::ofstream out(filename);

	//walk the schema and format the data out into the file

	int numEvents = blpapiService_.numEventDefinitions();

	out << boost::str(boost::format("service has %1% events") % numEvents) << std::endl;

	for (int Index = 0; Index < numEvents; Index++)
	{
		SchemaElementDefinition eltDef = blpapiService_.getEventDefinition(Index);

		out << boost::str(boost::format("Event %1% : %2% : %3%") % Index % eltDef.name().string() % eltDef.description()) << std::endl;

		const SchemaTypeDefinition tDef = eltDef.typeDefinition();

		out << boost::str(boost::format("Schema Type Defn %1% : %2% : %3%") % Index % tDef.name().string() % tDef.description()) << std::endl;
		int numElts = tDef.numElementDefinitions();

		out << boost::str(boost::format("%1% has %2% elements") % tDef.name().string() % numElts) << std::endl;

		for (int eltIndex = 0; eltIndex < numElts; eltIndex++)
		{
			SchemaElementDefinition eltDef2 = tDef.getElementDefinition(eltIndex);

			out << boost::str(boost::format("Element %1% : %2% : %3%, type is %4%") % eltIndex % eltDef2.name().string() 
				% eltDef2.description() 
				% eltDef2.typeDefinition().datatype()) << std::endl;
		}
	}

}

bool BlpMktDataSession::OpenItem( blpItemPtr_t item )
{
	if (stopped_)
		return false;
	// TODO the correlation id may need to be something else.
	blpapi::CorrelationId correlationId((void*)item.get());

	FieldSet_t fields = item->Fields();
	std::vector<std::string> fieldList(fields.begin(), fields.end());

	blpapi::SubscriptionList subscriptions;
	std::vector<std::string> options;

	if(((BlpMktDataItem*)(item).get())->Isdelayed())
	{
		options.push_back("delayed");
	}

	subscriptions.add(item->SecurityName().c_str(), fieldList, options, correlationId);
	try {
		if (blpapiSession_)
		{
			blpapiSession_->subscribe(subscriptions);
			return true;
		}
		else
			return false;
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool BlpMktDataSession::CloseItem( blpItemPtr_t item )
{

	if (stopped_)
		return false;
	if(!started_)
	{
		// the session may have been closed down before the client tries to close the items
		return false;
	}

	blpapi::SubscriptionList subscriptions;
	
	// we unsubscribe from the session by correlation id
	blpapi::CorrelationId correlationId((void*)item.get());
	subscriptions.add(correlationId);

	try {
		if (blpapiSession_)
		{
			blpapiSession_->unsubscribe(subscriptions);
			return true;
		}
		else
			return false;
	}
	catch(...)
	{
		return false;
	}

	return true;
}



///////////////////////////////////
//
// blpRefDataSession implementation
//
///////////////////////////////////

BlpRefDataSession::BlpRefDataSession()
{

}

BlpRefDataSession::~BlpRefDataSession()
{

}


bool BlpRefDataSession::Start( blpapi::SessionOptions & options, blpapi::EventHandler * pEvtHandler )
{

	if (started_)
	{
		// TODO warn about bad state
		return false;
	}

	if(blpapiSession_ )
	{
		// theres an old session around, so clean it up
		try {
			blpapiSession_->stop();
		}
		catch (...)
		{
			delete blpapiSession_; blpapiSession_=0;
			return false;
		}
		delete blpapiSession_; blpapiSession_=0;
	}

	// make a new session
	try {
		blpapiSession_ = new (std::nothrow) blpapi::Session(options, pEvtHandler);
	}
	catch(...)
	{
		blpapiSession_=0;
		return false;
	}

	// try to start it
	if(!blpapiSession_->start())
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "BlpRefDataSession::Start: Failed to start blp session");
		delete blpapiSession_; blpapiSession_=0;
		return false;
	}

	// Try to open the service
	if(!blpapiSession_->openService(blpapiRefDataServiceName))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "BlpRefDataSession::Start: Failed to open the blp service");
		blpapiSession_->stop();
		delete blpapiSession_; blpapiSession_=0;
		return false;
	}



	started_ = true;

	return true;



}


bool BlpRefDataSession::Stop()
{
	if(!started_)
	{
		mama_log (MAMA_LOG_LEVEL_WARN, "BlpRefDataSession::Stop: service is already stopped");

		return false;
	}

	if(! blpapiSession_)
	{
		mama_log (MAMA_LOG_LEVEL_WARN, "BlpRefDataSession::Stop: no api session");
		return false;
	}

	blpapiSession_->stop();
	delete blpapiSession_; blpapiSession_=0;
	//blpapiSession_ = 0;

	started_ = false;
	return true;
}


bool BlpRefDataSession::OpenItem( blpItemPtr_t item )
{
	if (stopped_ || !blpapiSession_)
		return false;

	blpapi::CorrelationId correlationId((void*)item.get());

	Service refDataService = blpapiSession_->getService(blpapiRefDataServiceName);
	Request request = refDataService.createRequest("ReferenceDataRequest");

	// append securities to request
	request.append("securities", item->SecurityName().c_str());
	FieldSet_t fields = item->Fields();


	for (std::set<std::string>::iterator it = fields.begin(); it != fields.end(); ++it)
	{
		// append fields to request
		request.append("fields", it->c_str());
	}

	//TODO: check all places in that file for the line below try-catch
	blpapiSession_->sendRequest(request, correlationId);

	return true;

}

bool BlpRefDataSession::CloseItem( blpItemPtr_t item )
{

	return true;
}