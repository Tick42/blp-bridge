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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bridge.h>
#include <payloadbridge.h>
#include <mama/mama.h>
#include <msgfieldimpl.h>
#include <mama/reservedfields.h>
#include <mama/msg.h>

#ifdef str
#undef str
#endif

#include "blppayloadimpl.h"
#include "blpmsgimpl.h"
#include "blppayload.h"
#include "blpvaluetype.h"

std::ostream & operator<<(std::ostream& out, const mama_datetime & val){return out << val.dt;}

#define blpPayload(msg) ((BlpPayload*)msg)

#define CHECK_PAYLOAD(msg) \
        do {  \
           if ((blpPayload(msg)) == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)

#define CHECK_NAME(name,fid) \
        do {  \
           if ((fid == 0) && (name == 0)) return MAMA_STATUS_NULL_ARG; \
           if ((fid == 0) && (strlen(name)== 0)) return MAMA_STATUS_INVALID_ARG; \
         } while(0)


#define CHECK_ITER(iter) \
        do {  \
           if (((blpMsgIteratorImpl*)(iter)) == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)

#define blpField(field) ((BlpFieldPayload*)(field))

#define CHECK_FIELD(field) \
        do {  \
           if (blpField(field) == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)


msgFieldPayload
tick42blpmsgPayloadIter_get          (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg);
/******************************************************************************
* bridge functions
*******************************************************************************/
extern mama_status
tick42blpmsgPayload_destroyImpl (mamaPayloadBridge mamaPayloadBridge)
{
	/* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != mamaPayloadBridge)
    {
        /* Get the impl. */
        mamaPayloadBridgeImpl *impl = (mamaPayloadBridgeImpl *)mamaPayloadBridge;

        /* Free the impl. */
        free(impl);
    }

    return ret;
}

extern mama_status
tick42blpmsgPayload_createImpl (mamaPayloadBridge* result, char* identifier)
{
	mamaPayloadBridgeImpl*       impl    = NULL;
    mama_status             resultStatus = MAMA_STATUS_OK;

    if (!result) return MAMA_STATUS_NULL_ARG;

    impl = (mamaPayloadBridgeImpl*)calloc (1, sizeof (mamaPayloadBridgeImpl));
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE, "tick42blpmsgPayload_createImpl(): "
                  "Could not allocate memory for payload impl.");
        return MAMA_STATUS_NULL_ARG;
    }

    INITIALIZE_PAYLOAD_BRIDGE (impl, tick42blpmsg);
	impl->msgPayloadCreate = tick42blpmsgPayload_create; 

    impl->mClosure = NULL;

    *result     = (mamaPayloadBridge)impl;
    *identifier = MAMA_PAYLOAD_TICK42BLP;

    return resultStatus;
}

mamaPayloadType
tick42blpmsgPayload_getType ()
{
    return MAMA_PAYLOAD_TICK42BLP;
}

/******************************************************************************
* general functions
*******************************************************************************/
mama_status
tick42blpmsgPayload_create (msgPayload* msg)

{
    if (!msg)  return MAMA_STATUS_NULL_ARG;

    BlpPayload* newPayload = new (std::nothrow) BlpPayload();

    *msg = (msgPayload)newPayload;

	// init the fid map here
	InitFidMap();

    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_createForTemplate (msgPayload*         msg,
                                 mamaPayloadBridge       bridge,
                                 mama_u32_t          templateId)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgPayload_createFromByteBuffer(msgPayload* msg, 
								   mamaPayloadBridge bridge, 
								   const void* buffer, mama_size_t bufferLength)
{
    if (!msg)  return MAMA_STATUS_NULL_ARG;

    BlpPayload* newPayload = new (std::nothrow) BlpPayload();
	
    *msg = (msgPayload)newPayload;

    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_copy              (const msgPayload    msg,
                                 msgPayload*         copy)
{
	CHECK_PAYLOAD(msg);
	const BlpPayload* payload = reinterpret_cast<BlpPayload*>(msg);

	BlpPayload* newPayload = reinterpret_cast<BlpPayload*>(*copy);

	if (!newPayload)
	{
		newPayload = new (std::nothrow) BlpPayload(*payload);

		if (!newPayload)
		{
			return MAMA_STATUS_PLATFORM;
		}

	}
	else
	{
		*newPayload = *payload;
	}

	*copy = reinterpret_cast<msgPayload*>(newPayload);
	return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_clear             (msgPayload          msg)
{
    CHECK_PAYLOAD(msg);

	BlpPayload* payload = reinterpret_cast<BlpPayload*>(msg);
	if (!payload)
	{
		return MAMA_STATUS_PLATFORM;
	}

	payload->clear();

    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_destroy           (msgPayload          msg)
{
    CHECK_PAYLOAD(msg);

	const BlpPayload* payload = reinterpret_cast<BlpPayload*>(msg);
	if (!payload)
	{
		return MAMA_STATUS_PLATFORM;
	}

	delete payload;

    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_setParent (msgPayload          msg,                           
                        const mamaMsg       parent)
{
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_getByteSize       (const msgPayload    msg,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgPayload_unSerialize (const msgPayload    msg,
                           const void**        buffer,
                           mama_size_t        bufferLength)
{
    return MAMA_STATUS_OK;
}


mama_status
tick42blpmsgPayload_serialize     (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength)
{
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_getByteBuffer     (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength)
{
 	CHECK_PAYLOAD(msg);
	const BlpPayload* payload = reinterpret_cast<BlpPayload*>(msg);

	*buffer = payload;
	*bufferLength = sizeof(BlpPayload);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_setByteBuffer     (const msgPayload    msg,
                                mamaPayloadBridge       bridge,
                                const void*         buffer,
                                mama_size_t         bufferLength)
{
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_getSendSubject     (const msgPayload    msg,
                                const char ** subject)
{
    CHECK_PAYLOAD(msg);
    return MAMA_STATUS_OK;
}


mama_status
tick42blpmsgPayload_getNumFields      (const msgPayload    msg,
                                mama_size_t*        numFields)
{
    CHECK_PAYLOAD(msg);

	const BlpPayload* payload = reinterpret_cast<BlpPayload*>(msg);
	*numFields = payload->numFields();
    return MAMA_STATUS_OK;
}

const char*
tick42blpmsgPayload_toString          (const msgPayload    msg)
{
    return "";
}

mama_status
tick42blpmsgPayload_iterateFields (const msgPayload        msg,
                            const mamaMsg           parent,
                            mamaMsgField            field,
                            mamaMsgIteratorCb       cb,
                            void*                   closure)
{
	CHECK_PAYLOAD(msg);

	const BlpPayload* payload = reinterpret_cast<BlpPayload*>(msg);
	if (!payload)
	{
		return MAMA_STATUS_PLATFORM;
	}

    return payload->iterateFields(field, cb, closure);
}

mama_status
tick42blpmsgPayload_getFieldAsString  (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               buf,
                                mama_size_t         len)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name, fid);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayload_apply            (msgPayload          dest,
                               const msgPayload    src)
{
    CHECK_PAYLOAD(dest);
    CHECK_PAYLOAD(src);

    return MAMA_STATUS_OK;


}

mama_status
tick42blpmsgPayload_getNativeMsg     (const msgPayload    msg,
                               void**              nativeMsg)
{
    CHECK_PAYLOAD(msg);
	    *nativeMsg = msg;
    return MAMA_STATUS_OK;
}

/******************************************************************************
* add functions
*******************************************************************************/

mama_status
tick42blpmsgPayload_addBool           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name, fid);
    return blpMsg_setBool(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addChar(
        msgPayload      msg,
        const char*     name,
        mama_fid_t      fid,
        char            value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setChar(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addI8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setI8(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addU8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setU8(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addI16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setI16(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addU16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setU16(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addI32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setI32(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addU32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setU32(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addI64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setI64(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addU64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setU64(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addF32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setF32(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addF64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setF64(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addString         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setString(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addOpaque         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setOpaque(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addDateTime       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setDateTime(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addPrice          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setPrice(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_addMsg            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setMsg(blpPayload(msg), name, fid, value);
}

/******************************************************************************
 * addVector... functions
 */

mama_status
tick42blpmsgPayload_addVectorBool     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t   value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorBool(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorChar     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorChar(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorI8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI8(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorU8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU8(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorI16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI16(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorU16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU16(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorI32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI32(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorU32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU32(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorI64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const int64_t       value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI64(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorU64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU64(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorF32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorF32(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorF64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorF64(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorString   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorString(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorMsg      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorMsg(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorDateTime (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorDateTime(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_addVectorPrice    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorPrice(blpPayload(msg), name, fid, value, size);
}

/******************************************************************************
* update functions
*******************************************************************************/
mama_status
tick42blpmsgPayload_updateBool        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value)
{
    return blpMsg_setBool(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateChar        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value)
{
    return blpMsg_setChar(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateU8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value)
{
    return blpMsg_setI8(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateI8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value)
{
    return blpMsg_setU8(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateI16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value)
{
    return blpMsg_setI16(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateU16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value)
{
    return blpMsg_setU16(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateI32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value)
{
    return blpMsg_setI32(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateU32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value)
{
    return blpMsg_setU32(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateI64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value)
{
    return blpMsg_setI64(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateU64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value)
{
    return blpMsg_setU64(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateF32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value)
{
    return blpMsg_setF32(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateF64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value)
{
    return blpMsg_setF64(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateString      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value)
{
    return blpMsg_setString(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateOpaque      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size)
{
    return blpMsg_setOpaque(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateDateTime    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setDateTime(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updatePrice       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setPrice(blpPayload(msg), name, fid, value);
}

mama_status
tick42blpmsgPayload_updateSubMsg      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const msgPayload    value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


/******************************************************************************
 * updateVector... functions
 */

mama_status
tick42blpmsgPayload_updateVectorMsg   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorMsg(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorString (msgPayload         msg,
                                 const char*        name,
                                 mama_fid_t         fid,
                                 const char*        strList[],
                                 mama_size_t        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorString(blpPayload(msg), name, fid, strList, size);
}

mama_status
tick42blpmsgPayload_updateVectorBool  (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t   boolList[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorBool(blpPayload(msg), name, fid, boolList, size);
}

mama_status
tick42blpmsgPayload_updateVectorChar  (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorChar(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorI8    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI8(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorU8    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU8(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorI16   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI16(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorU16   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU16(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorI32   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI32(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorU32   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU32(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorI64   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorI64(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorU64   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorU64(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorF32   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorF32(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorF64   (msgPayload         msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorF64(blpPayload(msg), name, fid, value, size);
}

mama_status
tick42blpmsgPayload_updateVectorPrice (msgPayload         msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice*    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorPrice(blpPayload(msg), name, fid, *value, size);
}

mama_status
tick42blpmsgPayload_updateVectorTime  (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_setVectorDateTime(blpPayload(msg), name, fid, value, size);
}

/******************************************************************************
 * get... functions
 */

mama_status
tick42blpmsgPayload_getBool           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t*        mamaResult)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getBool(blpPayload(msg), name, fid, mamaResult);
}

mama_status
tick42blpmsgPayload_getChar           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getChar(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getI8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t*          result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getI8(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getU8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t*          result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getU8(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getI16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getI16(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getU16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getU16(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getI32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getI32(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getU32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getU32(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getI64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t*         mamaResult)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getI64(blpPayload(msg), name, fid, mamaResult);
}

mama_status
tick42blpmsgPayload_getU64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t*         mamaResult)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getU64(blpPayload(msg), name, fid, mamaResult);
}

mama_status
tick42blpmsgPayload_getF32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getF32(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getF64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getF64(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getString         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getString(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getOpaque         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void**        result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getOpaque(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getField          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgFieldPayload*    result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name, fid);

	BlpPayload* payload = reinterpret_cast<BlpPayload*>(msg);

	return payload->getField(fid, result);
}

mama_status
tick42blpmsgPayload_getDateTime       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaDateTime        result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getDateTime(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getPrice          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaPrice           result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getPrice(blpPayload(msg), name, fid, result);
}

mama_status
tick42blpmsgPayload_getMsg            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getMsg(blpPayload(msg), name, fid, result);
}

/******************************************************************************
 * getVector... functions
 */

mama_status
tick42blpmsgPayload_getVectorBool     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t** result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorBool(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorChar     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorChar(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorI8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t**   result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorI8(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorU8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t**   result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorU8(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorI16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorI16(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorU16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorU16(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorI32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorI32(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorU32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorU32(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorI64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorI64(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorU64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorU64(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorF32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorF32(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorF64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorF64(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorString   (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char***       result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorString(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorDateTime (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime* result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorDateTime(blpPayload(msg), name, fid, result, size);
}

mama_status
tick42blpmsgPayload_getVectorPrice    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice*    result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return blpMsg_getVectorPrice(blpPayload(msg), name, fid, result, size);
}


mama_status
tick42blpmsgPayload_getVectorMsg      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const msgPayload**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);

    return blpMsg_getVectorMsg(blpPayload(msg), name, fid, result, size);
}

/******************************************************************************
* iterator functions
*******************************************************************************/
mama_status
tick42blpmsgPayloadIter_create        (msgPayloadIter* iter,
                                msgPayload      msg)
{
    CHECK_PAYLOAD(msg);
	return MAMA_STATUS_OK;
}

msgFieldPayload
tick42blpmsgPayloadIter_get          (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg)
{
    return field;
}

msgFieldPayload
tick42blpmsgPayloadIter_next          (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg)
{
    return field;
}

mama_bool_t
tick42blpmsgPayloadIter_hasNext       (msgPayloadIter iter,
                                msgPayload     msg)
{
    return 0;
}

msgFieldPayload
tick42blpmsgPayloadIter_begin         (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg)
{
    return field;
}

msgFieldPayload
tick42blpmsgPayloadIter_end           (msgPayloadIter iter,
                                msgPayload     msg)
{
    return NULL;
}

mama_status
tick42blpmsgPayloadIter_associate      (msgPayloadIter iter,
                                 msgPayload     msg)
{
    CHECK_PAYLOAD(msg);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgPayloadIter_destroy       (msgPayloadIter iter)
{
    return MAMA_STATUS_OK;
}

/******************************************************************************
* general field functions
*******************************************************************************/
mama_status
tick42blpmsgFieldPayload_create      (msgFieldPayload*   field)
{
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgFieldPayload_destroy      (msgFieldPayload   field)
{
    CHECK_FIELD(field);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgFieldPayload_getName      (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                const char**            result)
{
	mama_fid_t fid =0;
	CHECK_FIELD(field);

	const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);

	fid = atoi(fieldPayload->name_.c_str());

	if (fid!=0)
	{
		if (!desc)
		{
			if (dict)
			{
				mama_status status = MAMA_STATUS_OK;
				if(MAMA_STATUS_OK!=
					(status=mamaDictionary_getFieldDescriptorByFid
					(dict,
					&desc,
					fid)))
				{
					return status;
				}
				*result = mamaFieldDescriptor_getName (desc);
			}
			else
			{
				*result = fieldPayload->name_.c_str();
			}
		}
		else
			*result = mamaFieldDescriptor_getName (desc);
	}
	else
		*result = fieldPayload->name_.c_str();

	if (!*result)
		return MAMA_STATUS_INVALID_ARG;

	return MAMA_STATUS_OK;
}

mama_status
tick42blpmsgFieldPayload_getFid       (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                uint16_t*               result)
{
    CHECK_FIELD(field);
	const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	*result = fieldPayload->fid_;

    return MAMA_STATUS_OK;
}


mama_status
tick42blpmsgFieldPayload_getDescriptor(const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor*    result)
{
    return MAMA_STATUS_OK;
}


mama_status
tick42blpmsgFieldPayload_getType      (msgFieldPayload         field,
                                mamaFieldType*          result)
{
    CHECK_FIELD(field);

	const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	*result = fieldPayload->type_;

    return MAMA_STATUS_OK;
}

/******************************************************************************
* field update functions
*******************************************************************************/
mama_status
tick42blpmsgFieldPayload_updateBool   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_bool_t             value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateChar   (msgFieldPayload         field,
                                msgPayload              msg,
                                char                    value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateU8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u8_t               value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateI8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i8_t               value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateI16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i16_t              value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_updateU16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u16_t              value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_updateI32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i32_t              value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateU32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u32_t              value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateI64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i64_t              value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_updateU64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u64_t              value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_updateF32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f32_t              value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateF64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f64_t              value)
{
    return blpField(field)->set(value);
}

mama_status
tick42blpmsgFieldPayload_updateDateTime
                               (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaDateTime      value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_updatePrice  (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaPrice         value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


/******************************************************************************
* field get functions
*******************************************************************************/
mama_status
tick42blpmsgFieldPayload_getBool      (const msgFieldPayload   field,
                                mama_bool_t*            result)
{
	const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getChar      (const msgFieldPayload   field,
                                char*                   result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getI8        (const msgFieldPayload   field,
                                mama_i8_t*              result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getU8        (const msgFieldPayload   field,
                                mama_u8_t*              result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getI16       (const msgFieldPayload   field,
                                mama_i16_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getU16       (const msgFieldPayload   field,
                                mama_u16_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getI32       (const msgFieldPayload   field,
                                mama_i32_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getU32       (const msgFieldPayload   field,
                                mama_u32_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getI64       (const msgFieldPayload   field,
                                mama_i64_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getU64       (const msgFieldPayload   field,
                                mama_u64_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getF32       (const msgFieldPayload   field,
                                mama_f32_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getF64       (const msgFieldPayload   field,
                                mama_f64_t*             result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getString    (const msgFieldPayload   field,
                                const char**            result)
{
    const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);
	return fieldPayload->get(*result);
}

mama_status
tick42blpmsgFieldPayload_getOpaque    (const msgFieldPayload   field,
                                const void**            result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getDateTime  (const msgFieldPayload   field,
                                mamaDateTime            result)
{
	const BlpFieldPayload* fieldPayload = reinterpret_cast<const BlpFieldPayload*>(field);

	mama_datetime tempdt;

	mama_status st = fieldPayload->get( tempdt);

	if (st == MAMA_STATUS_OK)
	{
		mamaDateTime_setEpochTimeMicroseconds(result, tempdt.dt);
	}

	return st;
}

mama_status
tick42blpmsgFieldPayload_getPrice     (const msgFieldPayload   field,
                                mamaPrice               result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getMsg       (const msgFieldPayload   field,
                                msgPayload*             result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorBool(const msgFieldPayload   field,
                                const mama_bool_t**     result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorChar(const msgFieldPayload   field,
                                const char**            result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorI8  (const msgFieldPayload   field,
                                const mama_i8_t**       result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorU8  (const msgFieldPayload   field,
                                const mama_u8_t**       result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorI16 (const msgFieldPayload   field,
                                const mama_i16_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorU16 (const msgFieldPayload   field,
                                const mama_u16_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorI32 (const msgFieldPayload   field,
                                const mama_i32_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorU32 (const msgFieldPayload   field,
                                const mama_u32_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorI64 (const msgFieldPayload   field,
                                const mama_i64_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorU64 (const msgFieldPayload   field,
                                const mama_u64_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

 mama_status
tick42blpmsgFieldPayload_getVectorF32 (const msgFieldPayload   field,
                                const mama_f32_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorF64 (const msgFieldPayload   field,
                                const mama_f64_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorString
                               (const msgFieldPayload   field,
                                const char***           result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorDateTime
                               (const msgFieldPayload   field,
                                const mamaDateTime*     result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorPrice
                               (const msgFieldPayload   field,
                                const mamaPrice*        result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getVectorMsg (const msgFieldPayload   field,
                                const msgPayload**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpmsgFieldPayload_getAsString (
    const msgFieldPayload   field,
    const msgPayload   msg,
    char*         buf,
    mama_size_t   len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

extern mama_status
	tick42blpmsgFieldPayload_updateString
	(msgFieldPayload         field,
	msgPayload              msg,
	const char*             value)
{
	return blpField(field)->set(std::string(value));
}