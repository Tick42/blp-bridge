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
#include <mama/mama.h>
#include "blpsystemglobal.h"
#include <boost/scope_exit.hpp>

extern "C" {
#include <timers.h>
}

#include <queueimpl.h>

#include "blpbridgefunctions.h"
#include "blpdefs.h"
#include "blpbridgeimpl.h"
#include "transportbridge.h"
#include "blpsystemglobal.h"
timerHeap gTimerHeap;

/*Responsible for creating the bridge impl structure*/

// this function *must* have the same name as is enumnerated for the middleware in the mama middleware.c because there is 
//     snprintf (initFuncName, 256, "%sBridge_createImpl",  middlewareName); in mama_loadBridgeWithPathInternal  in mama.c
void tick42blpBridge_createImpl (mamaBridge* result)
{
    mamaBridgeImpl* impl = NULL;
    if (!result) return;
    *result = NULL;

    impl = (mamaBridgeImpl*)calloc (1, sizeof (mamaBridgeImpl));
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE, "tick42blpBridge_createImpl(): "
                "Could not allocate mem for impl.");
        return;
    }

	BlpBridgeImpl* blpBridge = new BlpBridgeImpl();

    /*Populate the bridge impl structure with the function pointers*/
    INITIALIZE_BRIDGE (impl, tick42blp);

    mamaBridgeImpl_setClosure((mamaBridge) impl, blpBridge);

    *result = (mamaBridge)impl;
}

const char*
tick42blpBridge_getVersion (void)
{
    return (const char*) "Unable to get version number";
}

const char*
tick42blpBridge_getName (void)
{
    return "blp";
}

// This is non constant array of char that holds the name of the bridge
// That is because tick42blpBridge_getDefaultPayloadId (char** *name, char** id)
// insist of haveing non-const strings as parameters (which might be wrong)
// However it is a MAMA bridge definition (see OpenMAMA documentation) and also:
// typedef mama_status (*bridge_getDefaultPayloadId)(char***name, char** id);

static char tick42PayloadName[] = "tick42blpmsg"; 
static char * PAYLOAD_NAMES[] = {tick42PayloadName, NULL};
static char PAYLOAD_IDS[] = {MAMA_PAYLOAD_TICK42BLP, NULL};

mama_status
tick42blpBridge_getDefaultPayloadId (char** *name, char** id)
{
	*name = PAYLOAD_NAMES;
	*id = PAYLOAD_IDS;
    
	return MAMA_STATUS_OK;
}

static mamaQueue gPublisher_MamaQueue= NULL;

mama_status
	tick42blpBridge_getTheMamaQueue (mamaQueue* queue)
{
	if (!gPublisher_MamaQueue)
		return MAMA_STATUS_NOT_INITIALISED;

	*queue = gPublisher_MamaQueue;

	return MAMA_STATUS_OK;
}

mama_status
tick42blpBridge_open (mamaBridge bridgeImpl)
{
	// Initialize sockets. Create queues and timers for the Bridge
	mama_status status = MAMA_STATUS_OK;
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;

    wsocketstartup();
    mama_log (MAMA_LOG_LEVEL_FINEST, "tick42blpBridge_open(): Entering.");

    if (MAMA_STATUS_OK !=
       (status =  mamaQueue_create (&impl->mDefaultEventQueue, bridgeImpl)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_open():"
            "Failed to create Blp queue.");
        return status;
    }

    mamaQueue_setQueueName (impl->mDefaultEventQueue,
                            "BLP_DEFAULT_MAMA_QUEUE");


	gPublisher_MamaQueue = impl->mDefaultEventQueue;
    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "tick42blpBridge_open(): Successfully created tick42blp queue");

    if (0 != createTimerHeap (&gTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                "tick42blpBridge_open(): Failed to initialize timers.");
        return MAMA_STATUS_PLATFORM;
    }

    if (0 != startDispatchTimerHeap (gTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                "tick42blpBridge_open(): Failed to start timer thread.");
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridge_close (mamaBridge bridgeImpl)
{
	
	BlpSystemGlobal::gSystemShutdown.set(1); // Change the system state to shutdown 
	BlpSystemGlobal::gMsgCbLock.wait(); //Once locked here, the gMsgCbLock is never released (post).

	mama_status     status = MAMA_STATUS_OK;
	mamaBridgeImpl* impl   = NULL;

	mama_log (MAMA_LOG_LEVEL_FINEST, "tick42blpBridge_close(): Entering.");

	BlpBridgeImpl* blpBridge = (BlpBridgeImpl*)bridgeImpl;

	//mama_log (MAMA_LOG_LEVEL_FINER, "tick42blpBridge_stop(): Stopping bridge.");
	//if (MAMA_STATUS_OK != (status = mamaBridgeImpl_getClosure((mamaBridge) mamaQueueImpl_getBridgeImpl(defaultEventQueue), (void**) &blpBridge))) {
	//	mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_stop(): Could not get Blp bridge object");
	//	return status;
	//}


	//if (MAMA_STATUS_OK != (status = blpBridge->getTransportBridge()->Stop())) {
	//	mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_stop(): Could not stop dispatching on Blp %d", status);
	//	return status;
	//}

	
	// Destroy queues and timers. Cleanup sockets
	impl =  (mamaBridgeImpl*)bridgeImpl;

	//BlpBridgeImpl* blpBridge = NULL;

	if (0 != destroyHeap (gTimerHeap))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_close():"
				"Failed to destroy Blp timer heap.");
		status = MAMA_STATUS_PLATFORM;
	}
	
	gPublisher_MamaQueue = NULL;
	mamaQueue_destroyWait(impl->mDefaultEventQueue);
	
	free (impl);
	wsocketcleanup();
	
	return status;
}


mama_status
tick42blpBridge_start(mamaQueue defaultEventQueue)
{
	mama_status status = MAMA_STATUS_OK;
    BlpBridgeImpl* blpBridge = NULL;

    mama_log (MAMA_LOG_LEVEL_FINER, "tick42blpBridge_start(): Start dispatching on default event queue.");


    if (MAMA_STATUS_OK != (status = mamaBridgeImpl_getClosure((mamaBridge) mamaQueueImpl_getBridgeImpl(defaultEventQueue), (void**) &blpBridge))) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_start(): Could not get Blp bridge object");
        return status;
    }

	if(blpBridge->getTransportBridge()== 0)
	{
		// just return for the moment and the transport bridge create will call start
		    return mamaQueue_dispatch(defaultEventQueue);
		//return MAMA_STATUS_OK;
	}

	if (MAMA_STATUS_OK != (status = blpBridge->getTransportBridge()->Start()))
	{
		//can just wait and start on transport create
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_start(): Could not start dispatching on Blp");
		return status;

    }

    // start Mama event loop
    return mamaQueue_dispatch(defaultEventQueue);
}

mama_status
tick42blpBridge_stop(mamaQueue defaultEventQueue)
{

    mama_status status = MAMA_STATUS_OK;
    BlpBridgeImpl* blpBridge = NULL;

    mama_log (MAMA_LOG_LEVEL_FINER, "tick42blpBridge_stop(): Stopping bridge.");
    if (MAMA_STATUS_OK != (status = mamaBridgeImpl_getClosure((mamaBridge) mamaQueueImpl_getBridgeImpl(defaultEventQueue), (void**) &blpBridge))) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_stop(): Could not get Blp bridge object");
        return status;
	}


	 //if (MAMA_STATUS_OK != (status = blpBridge->getTransportBridge()->Stop())) {
	 //mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_stop(): Could not stop dispatching on Blp %d", status);
	 //return status;
	 //}
	 
    // stop Mama event loop
    status = mamaQueue_stopDispatch (defaultEventQueue);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridge_stop(): Failed to unblock  queue.");
        return status;
    }

    return MAMA_STATUS_OK;
}
