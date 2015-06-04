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

#include <mama/mama.h>
#include <mama/inbox.h>
#include <mama/publisher.h>
#include <transportimpl.h>
#include "blpbridgeimpl.h"
#include <bridge.h>
#include <inboximpl.h>

#include "subinitial.h"
#include "blpbridgefunctions.h"
#include "transportbridge.h"
#include "msgimpl.h"
#include "blpdefs.h"
#include "subinitial.h"
#include "msgimpl.h"

struct Attributes;

typedef struct blpPublisherBridge_t
{
	mamaTransport  transport_;
    const char*    topic_;
    const char*    source_;
    const char*    root_;
    char*          subject_;
} blpPublisherBridge_t;




typedef struct _dict_closure_t
{
	mamaMsg msg;
	mamaInbox inbox;

} dict_closure_t;

#define blpPublisher(publisher) ((blpPublisherBridge_t*) publisher)
#define CHECK_PUBLISHER(publisher) \
    do {  \
       if (blpPublisher(publisher) == 0) return MAMA_STATUS_NULL_ARG; \
    } while(0)

static mama_status
blpBridgeMamaPublisherImpl_buildSendSubject (blpPublisherBridge_t* impl);

mama_status
tick42blpBridgeMamaPublisher_createByIndex (publisherBridge* result,
                                        mamaTransport    tport,
                                        int              tportIndex,
                                        const char*      topic,
                                        const char*      source,
                                        const char*      root,
                                        void*            nativeQueueHandle,
                                        mamaPublisher    parent)
{
	blpPublisherBridge_t* publisher = NULL;

	if (!result || !tport) return MAMA_STATUS_NULL_ARG;

	*result = NULL;
	publisher = (blpPublisherBridge_t*) calloc (1, sizeof(blpPublisherBridge_t));
	if (publisher == NULL)
		return MAMA_STATUS_NOMEM;
	publisher->transport_ = tport;

	if (topic != NULL)
		publisher->topic_ = strdup (topic);
	if (source != NULL)
		publisher->source_ = strdup(source);
	if (root != NULL)
		publisher->root_ = strdup (root);

	blpBridgeMamaPublisherImpl_buildSendSubject (publisher);

	*result = (publisherBridge) publisher;

	return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaPublisher_create (publisherBridge* result,
                                 mamaTransport    tport,
                                 const char*      topic,
                                 const char*      source,
                                 const char*      root,
                                 void*            nativeQueueHandle,
                                 mamaPublisher    parent)
{
	
	return tick42blpBridgeMamaPublisher_createByIndex (result,
                                                 tport,
                                                 0,
                                                 topic,
                                                 source,
                                                 root,
                                                 nativeQueueHandle,
                                                 parent);
}

/* Build up the RV subject. This should only need to be set once for the
   publisher. Duplication of some of the logic  */
static mama_status
blpBridgeMamaPublisherImpl_buildSendSubject (blpPublisherBridge_t* impl)
{
    char lSubject[256];

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->root_ != NULL && impl->source_ != NULL )
    {
            snprintf (lSubject, sizeof(lSubject),"%s.%s",
                      impl->root_, impl->source_);
    }
    else if (impl->source_ != NULL && impl->topic_ != NULL)
    {
        snprintf (lSubject, sizeof(lSubject), "%s.%s",
                  impl->source_, impl->topic_);
    }
    else if (impl->topic_ != NULL)
    {
        snprintf (lSubject, sizeof(lSubject), "%s",
                  impl->topic_);
    }

    impl->subject_ = strdup(lSubject);

    return MAMA_STATUS_OK;
}


/*Send a message.*/
mama_status
tick42blpBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg)
{
    mama_size_t        dataLen=0;
    mama_status        status;
    Attributes* attributes = NULL;
    
    CHECK_PUBLISHER(publisher);

	blpPublisherBridge_t* concrete_publisher = (blpPublisherBridge_t*)(publisher);

    status = mamaMsgImpl_getPayloadBuffer (msg, (const void**)&attributes, &dataLen);

    if (attributes == NULL)
        return MAMA_STATUS_INVALID_ARG;

	blpPublisherBridge_t* before = (blpPublisherBridge_t*)(publisher);
    mamaMsg_updateString(msg, SUBJECT_FIELD_NAME, 0, blpPublisher(publisher)->subject_);

	blpPublisherBridge_t* after = (blpPublisherBridge_t*)(publisher);

	BlpTransportBridge *transport = (BlpTransportBridge *)&(publisher);
    mama_log (MAMA_LOG_LEVEL_FINEST, "tick42blpBridgeMamaPublisher_send(): "
                   "Send message. %s", mamaMsg_toString(msg));
    return MAMA_STATUS_OK;
}

/* Send reply to inbox. */
mama_status
tick42blpBridgeMamaPublisher_sendReplyToInbox (publisherBridge  publisher,
                                           void*          request,
                                           mamaMsg          reply)
{
    Attributes* requestMsg = NULL;
    Attributes* replyMsg = NULL;
    const char*  replyAddr  = NULL;
    mama_size_t        dataLen;
    mama_status  status;
    
    CHECK_PUBLISHER(publisher);

	mamaMsg requestMessgage = reinterpret_cast<const mamaMsg>(request);
	mamaMsg_getNativeHandle(requestMessgage, (void**) &requestMsg);
    mamaMsgImpl_getPayloadBuffer (reply, (const void**)&replyMsg, &dataLen);

    if (!requestMsg || !replyMsg) return MAMA_STATUS_NULL_ARG;

    status = mamaMsg_getString(requestMessgage, INBOX_FIELD_NAME, 0, &replyAddr);
    if ((status != MAMA_STATUS_OK) || (replyAddr == NULL) || (strlen(replyAddr) == 0)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeMamaPublisher_sendReplyToInbox(): "
                  "No reply address in message.");
        return MAMA_STATUS_INVALID_ARG;
    }

    status = mamaMsg_updateString(reply, SUBJECT_FIELD_NAME, 0, replyAddr);
    if (status != MAMA_STATUS_OK)
        return status;

    return MAMA_STATUS_OK;
}

/* Destroy the publisher.*/
mama_status
tick42blpBridgeMamaPublisher_destroy (publisherBridge publisher)
{
    free ((char*)blpPublisher(publisher)->source_);
    free ((char*)blpPublisher(publisher)->topic_);
    free ((char*)blpPublisher(publisher)->root_);
    free (blpPublisher(publisher)->subject_);
    free (blpPublisher(publisher));
    return MAMA_STATUS_OK;
}

typedef struct _dictionaryReplyClosure
{
	mamaInbox       inbox;   //the inbox used for sending
	mamaMsg         msg;     //A prepared dictionary message
} dictionaryReplyClosure;
/*
 * The callback to run asynchronously as a reply for a dictionary request
 */
void MAMACALLTYPE dictionaryReplyCb(mamaQueue queue,void *closure)
{
	dictionaryReplyClosure *param = (dictionaryReplyClosure*) closure;
	blpMamaInbox_send( param->inbox, param->msg);
	free(closure);
}


/**
 * tick42blpBridgeMamaPublisher_getDictionaryMessage
 * Get the underlying message for the data dictionary.
 * 
 * A new message instance is created each time this function is called. It is
 * the responsibility for the caller to destroy the message when no longer
 * required.
 * The message will have only relevant fields of the dictionary, rest of the
 * fields needed to be added later on order for the message to successfully 
 * delivered to it's requester.
 *
 * @param transport The transport that holds internal dictionary
 * @param msg The address of the mamaMsg where the result is to be written
 */
static mama_status 
	tick42blpBridgeMamaPublisher_getDictionaryMessage(
	mamaTransport  transport,
	mamaMsg         *msg)
{
	mama_status status;

	// get hold of the bridge implementation
	mamaBridgeImpl* bridgeImpl = mamaTransportImpl_getBridgeImpl(transport);
	if (!bridgeImpl) {
		mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeCreateDictionary(): Could not get bridge");
		return MAMA_STATUS_PLATFORM;
	}

	BlpBridgeImpl*      blpBridge = NULL;
	if (MAMA_STATUS_OK != (status = mamaBridgeImpl_getClosure((mamaBridge) bridgeImpl, (void**) &blpBridge))) 
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeCreateDictionary(): Could not get Blp bridge object");
		return status;
	}
	if (blpBridge->hasTransportBridge()) 
	{
		mamaDictionary  dictionary= NULL;
		if (MAMA_STATUS_OK != (status = mamaDictionary_create (&dictionary)))
		{
			mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeCreateDictionary(): Could not create dictionary object");
			return status;
		}
		auto bridge = blpBridge->getTransportBridge();
		auto &fieldsMap = bridge->BlpWombatDictionary_->BlpToWombatFieldsMap_;

		// Create an OpenMAMA dictionary fields
		for (auto cit = fieldsMap.cbegin(); cit != fieldsMap.end(); ++cit)
		{
			mamaDictionary_createFieldDescriptor (
				dictionary,
				cit->second.wombat_fid,
				cit->second.wombat_field_name.c_str(),
				cit->second.wombat_field_type,
				NULL);
		}

		mamaMsg         dictMsg=NULL;

		// Prepare the OpenMAMA dictionary part of the message 
		auto res = mamaDictionary_getDictionaryMessage (dictionary, &dictMsg);
		*msg = dictMsg;
		return res;

	}

	return status;
}

/* Send a message from the specified inbox using the throttle. */
mama_status
tick42blpBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge publisher,
                                               int             tportIndex,
                                               mamaInbox       inbox,
                                               mamaMsg         msg)
{
	const char* replyAddr = NULL;
    mama_status status;
  //  CHECK_PUBLISHER(publisher);
    if (blpPublisher(publisher) == 0)
    	return MAMA_STATUS_NULL_ARG;

    // get reply address from inbox
    replyAddr = blpInboxImpl_getReplySubject(mamaInboxImpl_getInboxBridge(inbox));

    // set reply address in msg
    status = mamaMsg_updateString(msg, INBOX_FIELD_NAME, 0, replyAddr);
    if (status != MAMA_STATUS_OK)
        return status;

	blpPublisherBridge_t* blpPublisherBridge = (blpPublisherBridge_t*)(publisher);
	std::string topic = blpPublisherBridge->topic_;
	std::string subject = blpPublisherBridge->subject_;
	
	// Special treatment for Dictionary request
	if (topic=="DATA_DICT" && subject == "_MDDD.WOMBAT")
	{
		mamaMsg         reply;
		if(MAMA_STATUS_OK == (status = tick42blpBridgeMamaPublisher_getDictionaryMessage(blpPublisherBridge->transport_, &reply)))
		{
			// Create the dictionary part of the dictionary reply message
			mamaBridgeImpl* bridgeImpl = mamaTransportImpl_getBridgeImpl(blpPublisherBridge->transport_);
			if (!bridgeImpl) {
				mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeCreateDictionary(): Could not get bridge");
				return MAMA_STATUS_PLATFORM;
			}
    
			// Prepare mandatory fields needed for the reply
			mamaMsg_updateString(reply, SUBJECT_FIELD_NAME, 0, blpPublisher(publisher)->subject_);
			status = mamaMsg_updateString(reply, INBOX_FIELD_NAME, 0, replyAddr);
			mamaMsg_addI32(reply, "MdMsgType", 1, MAMA_MSG_TYPE_INITIAL);
			mamaMsg_addI32(reply, "MdMsgStatus", 2, MAMA_MSG_STATUS_OK);


			// Enqueue the reply 
			dictionaryReplyClosure *queueClosure = (dictionaryReplyClosure*)malloc(sizeof(dictionaryReplyClosure));
			queueClosure->inbox = inbox;
			queueClosure->msg = reply;

			mamaQueue theBridgeQueue = NULL;
			tick42blpBridge_getTheMamaQueue (&theBridgeQueue);

			return mamaQueue_enqueueEvent(theBridgeQueue, dictionaryReplyCb, queueClosure);
		}
	}

    return tick42blpBridgeMamaPublisher_send(publisher, msg);
}

mama_status
tick42blpBridgeMamaPublisher_sendFromInbox (publisherBridge publisher,
                                        mamaInbox     inbox,
                                        mamaMsg       msg)
{
    return tick42blpBridgeMamaPublisher_sendFromInboxByIndex (publisher, 0, inbox, msg);
}

mama_status
tick42blpBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge publisher,
                                               void *          inbox,
                                               mamaMsg         reply)
{
    mama_status status;
	CHECK_PUBLISHER(publisher);

	status = mamaMsg_updateString(reply, SUBJECT_FIELD_NAME, 0, (const char*) inbox);
	if (status != MAMA_STATUS_OK)
		return status;

	return  tick42blpBridgeMamaPublisher_sendFromInboxByIndex(publisher, 0, (mamaInbox) inbox, reply);
}
