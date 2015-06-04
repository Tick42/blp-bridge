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

extern "C" {
#include <wombat/wUuid.h>
}

#include <mama/mama.h>
#include <bridge.h>
#include <string.h>
#include "blpbridgefunctions.h"
#include "blpdefs.h"
#include "subinitial.h"

#include "inboximpl.h"
static const size_t uuidStringLen = 36;

typedef struct blpInboxImpl_t
{
	char                      inbox_[MAX_SUBJECT_LENGTH];
    mamaSubscription          _subscription;
    void*                     closure_;
    mamaInboxMsgCallback      msgCB_;
    mamaInboxErrorCallback    errCB_;
    mamaInboxDestroyCallback  inboxDestroyCB_;
    mamaInbox                 parent_;
} blpInboxImpl_t;


#define blpInbox(inbox) ((blpInboxImpl_t*)(inbox))
#define CHECK_INBOX(inbox) \
        do {  \
           if (blpInbox(inbox) == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)


static void MAMACALLTYPE
blpInbox_onMsg(
    mamaSubscription    subscription,
    mamaMsg             msg,
    void*               closure,
    void*               itemClosure)
{
   if (!blpInbox(closure)) return;

   if (blpInbox(closure)->msgCB_)
      (blpInbox(closure)->msgCB_)(msg, blpInbox(closure)->closure_);
}

static void MAMACALLTYPE
blpInbox_onCreate(
    mamaSubscription    subscription,
    void*               closure)
{
}

static void MAMACALLTYPE
blpInbox_onDestroy(
    mamaSubscription    subscription,
    void*               closure)
{
   if (!blpInbox(closure)) return;

   if (blpInbox(closure)->inboxDestroyCB_)
      (blpInbox(closure)->inboxDestroyCB_)(blpInbox(closure)->parent_, blpInbox(closure)->closure_);
}

static void MAMACALLTYPE
blpInbox_onError(
    mamaSubscription    subscription,
    mama_status         status,
    void*               platformError,
    const char*         subject,
    void*               closure)
{
   if (!blpInbox(closure)) return;

   if (blpInbox(closure)->errCB_)
      (blpInbox(closure)->errCB_)(status, blpInbox(closure)->closure_);
}

mama_status
tick42blpBridgeMamaInbox_createByIndex (inboxBridge*           bridge,
                                    mamaTransport          transport,
                                    int                    tportIndex,
                                    mamaQueue              queue,
                                    mamaInboxMsgCallback   msgCB,
                                    mamaInboxErrorCallback errorCB,
                                    mamaInboxDestroyCallback onInboxDestroyed,
                                    void*                  closure,
                                    mamaInbox              parent)
{
	blpInboxImpl_t* impl = NULL;
	mama_status status = MAMA_STATUS_OK;
	if (!bridge || !transport || !queue || !msgCB) return MAMA_STATUS_NULL_ARG;
	impl = (blpInboxImpl_t*)calloc(1, sizeof(blpInboxImpl_t));
	if (!impl)
		return MAMA_STATUS_NOMEM;

	impl->closure_   = closure;
	impl->msgCB_     = msgCB;
	impl->errCB_     = errorCB;
	impl->parent_    = parent;
	impl->inboxDestroyCB_ = onInboxDestroyed;

	*bridge = (inboxBridge) impl;
	return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaInbox_create (inboxBridge*           bridge,
                             mamaTransport          transport,
                             mamaQueue              queue,
                             mamaInboxMsgCallback   msgCB,
                             mamaInboxErrorCallback errorCB,
                             mamaInboxDestroyCallback onInboxDestroyed,
                             void*                  closure,
                             mamaInbox              parent)
{
	return tick42blpBridgeMamaInbox_createByIndex (bridge,
                                             transport,
                                             0,
                                             queue,
                                             msgCB,
                                             errorCB,
                                             onInboxDestroyed,
                                             closure,
                                             parent);
}

mama_status
tick42blpBridgeMamaInbox_destroy (inboxBridge inbox)
{
	CHECK_INBOX(inbox);
	mamaSubscription_destroy(blpInbox(inbox)->_subscription);
	mamaSubscription_deallocate(blpInbox(inbox)->_subscription);
	//free(blpInbox(inbox));
	return MAMA_STATUS_OK;
}


const char*
blpInboxImpl_getReplySubject(inboxBridge inbox)
{
	if (!blpInbox(inbox))
		return NULL;
	return blpInbox(inbox)->inbox_;
}


mama_status
blpMamaInbox_send( mamaInbox inbox, mamaMsg msg )
{
	if (!blpInbox(inbox))
		return MAMA_STATUS_INVALID_ARG;

	inboxBridge ib = mamaInboxImpl_getInboxBridge(inbox);
	blpInbox(ib)->msgCB_(msg, blpInbox(ib)->closure_);

	return MAMA_STATUS_OK;
}
