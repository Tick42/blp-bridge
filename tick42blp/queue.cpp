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
#include <bridge.h>
#include "queueimpl.h"
#include "blpbridgefunctions.h"
#include <wombat/queue.h>
#include "blpsystemglobal.h"
#include "boost/scope_exit.hpp"

typedef struct blpQueueBridge_t 
{
	mamaQueue          parent_;
    wombatQueue        queue_;
    uint8_t            isNative_;
} blpQueueBridge_t;

typedef struct blpQueueClosure_t 
{
	blpQueueBridge_t* impl_;
    mamaQueueEventCB cb_;
    void*            userClosure_;
} blpQueueClosure_t;

#define blpQueue(queue) ((blpQueueBridge_t*) queue)
#define CHECK_QUEUE(queue) \
        do {  \
           if (blpQueue(queue) == 0) return MAMA_STATUS_NULL_ARG; \
		   if (blpQueue(queue)->queue_ == NULL) return MAMA_STATUS_NULL_ARG; \
        } while(0)


mama_status
tick42blpBridgeMamaQueue_create (queueBridge* queue,
                            mamaQueue    parent)
{
    blpQueueBridge_t* blpQueue = NULL;
    if (queue == NULL)
        return MAMA_STATUS_NULL_ARG;
    *queue = NULL;

    blpQueue = (blpQueueBridge_t*)calloc (1, sizeof (blpQueueBridge_t));
    if (blpQueue == NULL)
        return MAMA_STATUS_NOMEM;

    blpQueue->parent_  = parent;

    wombatQueue_allocate (&blpQueue->queue_);
    wombatQueue_create (blpQueue->queue_, 0, 0, 0);

    *queue = (queueBridge) blpQueue;

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_create_usingNative (queueBridge* queue,
                                        mamaQueue    parent,
                                        void*        nativeQueue)
{
    blpQueueBridge_t* blpQueue = NULL;
    if (queue == NULL)
        return MAMA_STATUS_NULL_ARG;
    *queue = NULL;

    blpQueue = (blpQueueBridge_t*)calloc (1, sizeof (blpQueueBridge_t));
    if (blpQueue == NULL)
        return MAMA_STATUS_NOMEM;

    blpQueue->parent_  = parent;
    blpQueue->queue_   = (wombatQueue)nativeQueue;
    blpQueue->isNative_ = 1;

    *queue = (queueBridge) blpQueue;

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_destroy (queueBridge queue)
{
	CHECK_QUEUE(queue);
	if (blpQueue(queue)->isNative_)
		wombatQueue_destroy (blpQueue(queue)->queue_);
	free(blpQueue(queue));
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_dispatch (queueBridge queue)
{
    wombatQueueStatus status;

    CHECK_QUEUE(queue);
    do
    {
        /* 500 is .5 seconds */
        status = wombatQueue_timedDispatch (blpQueue(queue)->queue_,
                     NULL, NULL, 500);
    }
    while ((status == WOMBAT_QUEUE_OK ||
            status == WOMBAT_QUEUE_TIMEOUT) &&
            mamaQueueImpl_isDispatching (blpQueue(queue)->parent_));

    if (status != WOMBAT_QUEUE_OK && status != WOMBAT_QUEUE_TIMEOUT)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to dispatch Blp Middleware queue. %d",
                  "mamaQueue_dispatch ():",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_timedDispatch (queueBridge queue, uint64_t timeout)
{
    wombatQueueStatus status;
    CHECK_QUEUE(queue);

    status = wombatQueue_timedDispatch (blpQueue(queue)->queue_,
                     NULL, NULL, timeout);
    if (status == WOMBAT_QUEUE_TIMEOUT)
		return MAMA_STATUS_TIMEOUT;

    if (status != WOMBAT_QUEUE_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to dispatch Blp Middleware queue. %d",
                  "mamaQueue_dispatch ():",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_dispatchEvent (queueBridge queue)
{
    wombatQueueStatus status;
    CHECK_QUEUE(queue);

    status = wombatQueue_dispatch (blpQueue(queue)->queue_,
                     NULL, NULL);

    if (status != WOMBAT_QUEUE_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to dispatch Blp Middleware queue. %d",
                  "mamaQueue_dispatch ():",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

static void MAMACALLTYPE queueCb (void *ignored, void* closure)
{
    blpQueueClosure_t* cl = (blpQueueClosure_t*)closure;
    if (NULL ==cl) return;
    cl->cb_ (cl->impl_->parent_, cl->userClosure_);
    free (cl);
}

mama_status
tick42blpBridgeMamaQueue_enqueueEvent (queueBridge        queue,
                                  mamaQueueEventCB   callback,
                                  void*              closure)
{
    wombatQueueStatus status;
    blpQueueClosure_t* cl = NULL;
    CHECK_QUEUE(queue);

    cl = (blpQueueClosure_t*)calloc(1, sizeof(blpQueueClosure_t));
    if (NULL == cl) return MAMA_STATUS_NOMEM;

    cl->impl_ = blpQueue(queue);
    cl->cb_    = callback;
    cl->userClosure_ = closure;

    status = wombatQueue_enqueue (blpQueue(queue)->queue_,
                queueCb, NULL, cl);

    if (status != WOMBAT_QUEUE_OK)
        return MAMA_STATUS_PLATFORM;

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_stopDispatch (queueBridge queue)
{
    wombatQueueStatus status;
    CHECK_QUEUE(queue);

    if (queue == NULL)
        return MAMA_STATUS_NULL_ARG;

    status = wombatQueue_unblock (blpQueue(queue)->queue_);
    if (status != WOMBAT_QUEUE_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  " Failed to stop dispatching Blp Middleware queue.",
                  "wmwMamaQueue_stopDispatch ():");
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_setEnqueueCallback (queueBridge        queue,
                                        mamaQueueEnqueueCB callback,
                                        void*              closure)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaQueue_removeEnqueueCallback (queueBridge queue)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaQueue_getNativeHandle (queueBridge queue,
                                     void**      result)
{
    CHECK_QUEUE(queue);
	*result = blpQueue(queue)->queue_;
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaQueue_setHighWatermark (queueBridge queue,
                                      size_t      highWatermark)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaQueue_setLowWatermark (queueBridge queue,
                                     size_t lowWatermark)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaQueue_getEventCount (queueBridge queue, size_t* count)
{
    CHECK_QUEUE(queue);
    *count = 0;
    wombatQueue_getSize (blpQueue(queue)->queue_, (int*)count);
    return MAMA_STATUS_OK;
}
