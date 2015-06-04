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
#include "wombat/port.h"
#include <stdlib.h>
#include <string.h>

#include <mama/mama.h>
#include <msgimpl.h>
#include "blpdefs.h"
#include "blpbridgefunctions.h"

typedef struct blpMsgImpl
{
    void*     blpMsg_;
    mamaMsg   parent_;
    bool      secure_;
} blpMsgImpl;

#define blpMsg(msg) ((blpMsgImpl*)(msg))
#define CHECK_MSG(msg) \
        do {  \
           if ((blpMsg(msg)) == 0) return MAMA_STATUS_NULL_ARG; \
           if ((blpMsg(msg))->blpMsg_ == 0) return MAMA_STATUS_INVALID_ARG; \
         } while(0)


/******************************************************************************
 * Message Creation
 */

mama_status
tick42blpBridgeMamaMsg_create (msgBridge* msg, mamaMsg parent)
{
    blpMsgImpl* impl;
    if (blpMsg(msg) == NULL) return MAMA_STATUS_NULL_ARG;
    *msg = NULL;

    impl = (blpMsgImpl*) calloc(1, sizeof(blpMsgImpl));
    if (!impl) return MAMA_STATUS_NOMEM;

    mamaMsg_getNativeMsg(parent, (void**)&impl->blpMsg_);
    impl->parent_ = parent;
    *msg = (msgBridge) impl;
    return MAMA_STATUS_OK;
}


mama_status
tick42blpBridgeMamaMsg_destroy (msgBridge msg, int destroyMsg)
{
    CHECK_MSG(msg);
    if (destroyMsg)
    {
       tick42blpBridgeMamaMsg_destroyMiddlewareMsg(msg);
    }
    free(blpMsg(msg));

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsg_destroyMiddlewareMsg (msgBridge msg)
{
    CHECK_MSG(msg);
    blpMsg(msg)->blpMsg_ = NULL;

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsg_detach (msgBridge msg)
{

	if ((blpMsg(msg)) == 0) return MAMA_STATUS_NULL_ARG; 
	if ((blpMsg(msg))->blpMsg_ == 0) return MAMA_STATUS_INVALID_ARG; 

	blpMsgImpl * pCopy = new blpMsgImpl();
	pCopy->blpMsg_ = (blpMsg(msg))->blpMsg_;  // might need to clone this
	pCopy->parent_ = (blpMsg(msg))->parent_;
	pCopy->secure_ = (blpMsg(msg))->secure_;



    CHECK_MSG(msg);

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsg_getPlatformError (msgBridge msg, void** error)
{
    if (error) *error  = NULL;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaMsgImpl_setReplyHandle (msgBridge msg, void* result)
{
    mama_status status = MAMA_STATUS_OK;
    CHECK_MSG(msg);
    if (MAMA_STATUS_OK != (status = mamaMsg_updateString(blpMsg(msg)->parent_, INBOX_FIELD_NAME, 0, (const char*) result))) {
        return status;
    }

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsgImpl_setReplyHandleAndIncrement (msgBridge msg, void* result)
{
    // NOTE: reply handles are not reference counted with blp -- they are simply strings
    return tick42blpBridgeMamaMsgImpl_setReplyHandle(msg, result);
}

int
tick42blpBridgeMamaMsg_isFromInbox (msgBridge msg)
{
    const char* dummy;
    return (mamaMsg_getString(blpMsg(msg)->parent_, INBOX_FIELD_NAME, 0, &dummy) == MAMA_STATUS_OK) ? 1 : 0;
}

mama_status
tick42blpBridgeMamaMsg_setSendSubject (msgBridge   msg,
                                   const char* symbol,
                                   const char* subject)
{
    mama_status status = MAMA_STATUS_OK;
    CHECK_MSG(msg);

    if (MAMA_STATUS_OK != (status = mamaMsg_updateString(blpMsg(msg)->parent_, SUBJECT_FIELD_NAME, 0, subject))) {
        return status;
    }

    mamaMsg_updateString (blpMsg(msg)->parent_,
                              MamaFieldSubscSymbol.mName,
                              MamaFieldSubscSymbol.mFid,
                              symbol);

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsg_getNativeHandle (msgBridge msg, void** result)
{
    CHECK_MSG(msg);
    *result = blpMsg(msg)->blpMsg_;
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsg_duplicateReplyHandle (msgBridge msg, void** result)
{
    const char* replyAddr;
    mama_status status = MAMA_STATUS_OK;
    CHECK_MSG(msg);
    if (MAMA_STATUS_OK != (status = mamaMsg_getString(blpMsg(msg)->parent_, INBOX_FIELD_NAME, 0, &replyAddr))) {
        return status;
    }

    *result = (void*) strdup(replyAddr);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsg_copyReplyHandle (void* src, void** dest)
{
    const char* replyAddr = (const char*) src;
    *dest = (void*) strdup(replyAddr);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaMsg_destroyReplyHandle (void* result)
{
    char* replyAddr = (char*) result;
    free(replyAddr);
    return MAMA_STATUS_OK;
}
mama_status
tick42blpBridgeMamaMsgImpl_setAttributesAndSecure (msgBridge msg, void* attributes, uint8_t secure)
{
    blpMsg(msg)->blpMsg_ = attributes;
    blpMsg(msg)->secure_ = (secure) ? true : false;
    return MAMA_STATUS_OK;
}

