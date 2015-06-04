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
#include <mama/datetime.h>
#include <subscriptionimpl.h>
#include <transportimpl.h>
#include <msgimpl.h>
#include <queueimpl.h>
#include <wombat/queue.h>

#ifdef str
#undef str
#endif

#include "blpbridgefunctions.h"
#include "transportbridge.h"


#include "blpsubscription.h"
#include "blpsystemglobal.h"
#include <boost/scope_exit.hpp>


#define blpSub(subscriber) ((BlpSubscription*)(subscriber))
#define CHECK_SUBSCRIBER(subscriber) \
        do {  \
           if (blpSub(subscriber) == 0) return MAMA_STATUS_NULL_ARG; \
        } while(0)


static void MAMACALLTYPE
destroy_callback(void* subscriber, void* closure)
{
	BlpSubscription* blpSubscription = blpSub(subscriber);
    // cant do anything without a subscriber
    if (!blpSubscription) 
	{
        mama_log (MAMA_LOG_LEVEL_ERROR, "blp_callback(): called with NULL subscriber!");
        return;
    }

    blpSubscription->GetMsgCallback().onDestroy(blpSubscription->GetSubscription(), blpSubscription->GetClosure());

	blpSubscription->Release();
}

mama_status
tick42blpBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                    const char*         source,
                                    const char*         symbol,
                                    mamaTransport       transport,
                                    mamaQueue           queue,
                                    mamaMsgCallbacks    callback,
                                    mamaSubscription    subscription,
                                    void*               closure)
{
    if (!subscriber || !subscription || !transport )
        return MAMA_STATUS_NULL_ARG;

	BlpTransportBridge* blpTransportBridge = BlpTransportBridge::GetTransport(transport);
	if (!blpTransportBridge)
	{
		return MAMA_STATUS_INVALID_ARG;
	}


	// Create a BlpSubscription object
	BlpSubscription* impl = new (std::nothrow) BlpSubscription((source) ? source : "", 
															   (symbol) ? symbol : "", 
															   transport, queue, callback, subscription, closure);
    if (!impl)
	{
		return MAMA_STATUS_NOMEM;
	}

	if(!impl->IsValid())
	{
		return MAMA_STATUS_INVALID_ARG;
	}

	*subscriber = (subscriptionBridge)impl;

	// and initiate the subscription
	if (!impl->Subscribe(*blpTransportBridge))
	{
		return MAMA_STATUS_NO_SUBSCRIBERS;
	}

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaSubscription_createWildCard (subscriptionBridge* subscriber,
										const char*         source,
										const char*         symbol,
										mamaTransport       transport,
										mamaQueue           queue,
										mamaMsgCallbacks    callback,
										mamaSubscription    subscription,
										void*               closure )
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaSubscription_destroy (subscriptionBridge subscriber)
{
    mama_status status = MAMA_STATUS_OK;
    wombatQueue   queue = NULL;
    CHECK_SUBSCRIBER(subscriber);

	// Get the BlpSubscription object 
	BlpSubscription* blpSubscription = blpSub(subscriber);
	BlpTransportBridge* blpTransportBridge = BlpTransportBridge::GetTransport(blpSubscription->GetTransport());

	if (!blpTransportBridge)
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeMamaSubscription_destroy(): " "Could not get blp transport bridge.");
        return MAMA_STATUS_PLATFORM;
	}

	// and unsubscribe
	blpSubscription->Unsubscribe(*blpTransportBridge);


	//there seem to be some locking issues in the queue shutdown if we queue the destroy callback
	//
	// in this bridge there's no reason why we cant do this synchronously. (but leave the code here commented out so we can revert if the async issues are resoolved)

	//mamaQueue_getNativeHandle(blpSubscription->GetQueue(), &queue);
	//if (!queue)
	//{
	//	mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blp_callback(): "
	//		"Could not get event queue.");
	//	return MAMA_STATUS_PLATFORM;
	//}

	//wombatQueue_enqueue (queue, destroy_callback,
	//	(void*)subscriber, NULL);

	// synchronously shutdown the blp subscription
	//lock and release end-of-scope

	blpSubscription->GetMsgCallback().onDestroy(blpSubscription->GetSubscription(), blpSubscription->GetClosure());
	blpSubscription->Release();


    return status;
}


mama_status
tick42blpBridgeMamaSubscription_mute (subscriptionBridge subscriber)
{
    CHECK_SUBSCRIBER(subscriber);

	blpSub(subscriber)->Mute();

    return MAMA_STATUS_OK;
}


int
tick42blpBridgeMamaSubscription_isValid (subscriptionBridge subscriber)
{
    if (!subscriber) return 0;
    return 1;
}

mama_status
tick42blpBridgeMamaSubscription_getPlatformError (subscriptionBridge subscriber,
                                              void**             error)
{
    CHECK_SUBSCRIBER(subscriber);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaSubscription_setTopicClosure (subscriptionBridge subscriber,
                                             void* closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

int
tick42blpBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber)
{
    return 0;
}

int
tick42blpBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber)
{
    CHECK_SUBSCRIBER(subscriber);
    return 0;
}

