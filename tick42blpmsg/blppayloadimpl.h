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

#ifndef BLP_MSGBRIDGE_H__
#define BLP_MSGBRIDGE_H__

#include <mama/types.h>
#include "payloadbridge.h"

extern "C" {

MAMAExpBridgeDLL
extern mama_status
tick42blpmsgPayload_destroyImpl       (mamaPayloadBridge mamaPayloadBridge);

MAMAExpBridgeDLL
extern mama_status
tick42blpmsgPayload_createImpl        (mamaPayloadBridge* result, char* identifier);

extern mamaPayloadType
tick42blpmsgPayload_getType           (void);

extern mama_status
tick42blpmsgPayload_create            (msgPayload*         msg);

extern mama_status
tick42blpmsgPayload_createForTemplate (msgPayload*         msg,
                                mamaPayloadBridge       bridge,
                                mama_u32_t          templateId);
extern mama_status
tick42blpmsgPayload_copy              (const msgPayload    msg,
                                msgPayload*         copy);
extern mama_status
tick42blpmsgPayload_clear             (msgPayload          msg);
extern mama_status
tick42blpmsgPayload_destroy           (msgPayload          msg);
extern mama_status
tick42blpmsgPayload_setParent         (msgPayload          msg,
                               const mamaMsg       parent);
extern mama_status
tick42blpmsgPayload_getByteSize       (const msgPayload    msg,
                                mama_size_t*        size);

extern mama_status
tick42blpmsgPayload_getNumFields      (const msgPayload    msg,
                                mama_size_t*        numFields);

extern mama_status
tick42blpmsgPayload_getSendSubject      (const msgPayload    msg,
                                const char **        subject);

extern const char*
tick42blpmsgPayload_toString          (const msgPayload    msg);
extern mama_status
tick42blpmsgPayload_iterateFields     (const msgPayload    msg,
                                const mamaMsg       parent,
                                mamaMsgField        field,
                                mamaMsgIteratorCb   cb,
                                void*               closure);
extern mama_status
tick42blpmsgPayload_getFieldAsString  (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               buffer,
                                mama_size_t         len);

extern mama_status
tick42blpmsgPayload_serialize (const msgPayload    payload,
                           const void**        buffer,
                           mama_size_t*        bufferLength);

extern mama_status
tick42blpmsgPayload_unSerialize (const msgPayload    payload,
                           const void**        buffer,
                           mama_size_t        bufferLength);


extern mama_status
tick42blpmsgPayload_getByteBuffer     (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength);
extern mama_status
tick42blpmsgPayload_setByteBuffer     (const msgPayload    msg,
                                mamaPayloadBridge       bridge,
                                const void*         buffer,
                                mama_size_t         bufferLength);

extern mama_status
tick42blpmsgPayload_createFromByteBuffer (msgPayload*          msg,
                                   mamaPayloadBridge        bridge,
                                   const void*          buffer,
                                   mama_size_t          bufferLength);
extern mama_status
tick42blpmsgPayload_apply             (msgPayload          dest,
                                const msgPayload    src);
extern mama_status
tick42blpmsgPayload_getNativeMsg     (const msgPayload    msg,
                               void**              nativeMsg);
extern mama_status
tick42blpmsgPayload_addBool           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value);
extern mama_status
tick42blpmsgPayload_addChar           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value);
extern mama_status
tick42blpmsgPayload_addI8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value);
extern mama_status
tick42blpmsgPayload_addU8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value);
extern mama_status
tick42blpmsgPayload_addI16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value);
extern mama_status
tick42blpmsgPayload_addU16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value);
extern mama_status
tick42blpmsgPayload_addI32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value);
extern mama_status
tick42blpmsgPayload_addU32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value);
extern mama_status
tick42blpmsgPayload_addI64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value);
extern mama_status
tick42blpmsgPayload_addU64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value);
extern mama_status
tick42blpmsgPayload_addF32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value);
extern mama_status
tick42blpmsgPayload_addF64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value);
extern mama_status
tick42blpmsgPayload_addString         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value);
extern mama_status
tick42blpmsgPayload_addOpaque         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addDateTime       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value);
extern mama_status
tick42blpmsgPayload_addPrice          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value);
extern mama_status
tick42blpmsgPayload_addMsg            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload          value);
extern mama_status
tick42blpmsgPayload_addVectorBool     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t   value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorChar     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorI8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorU8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorI16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorU16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorI32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorU32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorI64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorU64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorF32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorF64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorString   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorMsg      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorDateTime (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_addVectorPrice    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateBool        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value);
extern mama_status
tick42blpmsgPayload_updateChar        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value);
extern mama_status
tick42blpmsgPayload_updateU8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value);
extern mama_status
tick42blpmsgPayload_updateI8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value);
extern mama_status
tick42blpmsgPayload_updateI16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value);
extern mama_status
tick42blpmsgPayload_updateU16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value);
extern mama_status
tick42blpmsgPayload_updateI32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value);
extern mama_status
tick42blpmsgPayload_updateU32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value);
extern mama_status
tick42blpmsgPayload_updateI64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value);
extern mama_status
tick42blpmsgPayload_updateU64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value);
extern mama_status
tick42blpmsgPayload_updateF32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value);
extern mama_status
tick42blpmsgPayload_updateF64
                               (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value);
extern mama_status
tick42blpmsgPayload_updateString      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value);
extern mama_status
tick42blpmsgPayload_updateOpaque      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateDateTime    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value);
extern mama_status
tick42blpmsgPayload_updatePrice       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value);
extern mama_status
tick42blpmsgPayload_getBool           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t*        result);
extern mama_status
tick42blpmsgPayload_updateSubMsg      (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const msgPayload    subMsg);
extern mama_status
tick42blpmsgPayload_updateVectorMsg   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorString (msgPayload         msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorBool  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_bool_t   value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorChar  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorI8    (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorU8    (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorI16   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorU16   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorI32   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorU32   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorI64   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i64_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorU64   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorF32   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorF64   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorPrice (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaPrice*    value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_updateVectorTime  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size);
extern mama_status
tick42blpmsgPayload_getChar           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               result);
extern mama_status
tick42blpmsgPayload_getI8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t*          result);
extern mama_status
tick42blpmsgPayload_getU8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t*          result);
extern mama_status
tick42blpmsgPayload_getI16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t*         result);
extern mama_status
tick42blpmsgPayload_getU16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t*         result);
extern mama_status
tick42blpmsgPayload_getI32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t*         result);
extern mama_status
tick42blpmsgPayload_getU32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t*         result);
extern mama_status
tick42blpmsgPayload_getI64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t*         result);
extern mama_status
tick42blpmsgPayload_getU64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t*         result);
extern mama_status
tick42blpmsgPayload_getF32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t*         result);
extern mama_status
tick42blpmsgPayload_getF64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t*         result);
extern mama_status
tick42blpmsgPayload_getString         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result);
extern mama_status
tick42blpmsgPayload_getOpaque         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void**        result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getField          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgFieldPayload*    result);
extern mama_status
tick42blpmsgPayload_getDateTime       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaDateTime        result);
extern mama_status
tick42blpmsgPayload_getPrice          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaPrice           result);
extern mama_status
tick42blpmsgPayload_getMsg            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload*         result);
extern mama_status
tick42blpmsgPayload_getVectorBool     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t** result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorChar     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorI8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t**   result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorU8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t**   result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorI16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorU16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorI32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorU32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorI64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorU64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorF32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorF64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorString   (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char***       result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorDateTime (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime* result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorPrice    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice*    result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayload_getVectorMsg      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const msgPayload**  result,
                                mama_size_t*        size);
extern mama_status
tick42blpmsgPayloadIter_create        (msgPayloadIter*     iter,
                                msgPayload          msg);
extern msgFieldPayload
tick42blpmsgPayloadIter_next          (msgPayloadIter      iter,
                                msgFieldPayload     field,
                                msgPayload          msg);
extern mama_bool_t
tick42blpmsgPayloadIter_hasNext       (msgPayloadIter      iter,
                                msgPayload          msg);
extern msgFieldPayload
tick42blpmsgPayloadIter_begin         (msgPayloadIter      iter,
                                msgFieldPayload     field,
                                msgPayload          msg);
extern msgFieldPayload
tick42blpmsgPayloadIter_end           (msgPayloadIter      iter,
                                msgPayload          msg);
extern mama_status
tick42blpmsgPayloadIter_associate     (msgPayloadIter      iter,
                                msgPayload          msg);
extern mama_status
tick42blpmsgPayloadIter_destroy       (msgPayloadIter      iter);

extern mama_status
tick42blpmsgFieldPayload_create       (msgFieldPayload*    field);

extern mama_status
tick42blpmsgFieldPayload_destroy      (msgFieldPayload     field);

extern mama_status
tick42blpmsgFieldPayload_getName      (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                const char**            result);
extern mama_status
tick42blpmsgFieldPayload_getFid       (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                uint16_t*               result);

extern mama_status
tick42blpmsgFieldPayload_getDescriptor(const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor*    result);
extern mama_status
tick42blpmsgFieldPayload_getType      (const msgFieldPayload   field,
                                mamaFieldType*          result);
extern mama_status
tick42blpmsgFieldPayload_updateBool   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_bool_t             value);
extern mama_status
tick42blpmsgFieldPayload_updateChar   (msgFieldPayload         field,
                                msgPayload              msg,
                                char                    value);
extern mama_status
tick42blpmsgFieldPayload_updateU8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u8_t               value);
extern mama_status
tick42blpmsgFieldPayload_updateI8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i8_t               value);
extern mama_status
tick42blpmsgFieldPayload_updateI16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i16_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateU16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u16_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateI32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i32_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateU32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u32_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateI64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i64_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateU64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u64_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateF32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f32_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateF64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f64_t              value);
extern mama_status
tick42blpmsgFieldPayload_updateString (msgFieldPayload         field,
	                            msgPayload              msg,
	                            const char*             value);

extern mama_status
tick42blpmsgFieldPayload_updateDateTime
                               (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaDateTime      value);
extern mama_status
tick42blpmsgFieldPayload_updatePrice  (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaPrice         value);
extern mama_status
tick42blpmsgFieldPayload_getBool      (const msgFieldPayload   field,
                                mama_bool_t*            result);
extern mama_status
tick42blpmsgFieldPayload_getChar      (const msgFieldPayload   field,
                                char*                   result);
extern mama_status
tick42blpmsgFieldPayload_getI8        (const msgFieldPayload   field,
                                mama_i8_t*              result);
extern mama_status
tick42blpmsgFieldPayload_getU8        (const msgFieldPayload   field,
                                mama_u8_t*              result);
extern mama_status
tick42blpmsgFieldPayload_getI16       (const msgFieldPayload   field,
                                mama_i16_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getU16       (const msgFieldPayload   field,
                                mama_u16_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getI32       (const msgFieldPayload   field,
                                mama_i32_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getU32       (const msgFieldPayload   field,
                                mama_u32_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getI64       (const msgFieldPayload   field,
                                mama_i64_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getU64       (const msgFieldPayload   field,
                                mama_u64_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getF32       (const msgFieldPayload   field,
                                mama_f32_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getF64       (const msgFieldPayload   field,
                                mama_f64_t*             result);
extern mama_status
tick42blpmsgFieldPayload_getString    (const msgFieldPayload   field,
                                const char**            result);
extern mama_status
tick42blpmsgFieldPayload_getOpaque    (const msgFieldPayload   field,
                                const void**            result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getField     (const msgFieldPayload   field,
                                mamaMsgField*           result);
extern mama_status
tick42blpmsgFieldPayload_getDateTime  (const msgFieldPayload   field,
                                mamaDateTime            result);
extern mama_status
tick42blpmsgFieldPayload_getPrice     (const msgFieldPayload   field,
                                mamaPrice               result);
extern mama_status
tick42blpmsgFieldPayload_getMsg       (const msgFieldPayload   field,
                                msgPayload*             result);
extern mama_status
tick42blpmsgFieldPayload_getVectorBool
                               (const msgFieldPayload   field,
                                const mama_bool_t**     result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorChar
                               (const msgFieldPayload   field,
                                const char**            result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorI8
                               (const msgFieldPayload   field,
                                const mama_i8_t**       result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorU8  (const msgFieldPayload   field,
                                const mama_u8_t**       result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorI16 (const msgFieldPayload   field,
                                const mama_i16_t**      result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorU16 (const msgFieldPayload   field,
                                const mama_u16_t**      result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorI32 (const msgFieldPayload   field,
                                const mama_i32_t**      result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorU32 (const msgFieldPayload   field,
                                const mama_u32_t**      result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorI64 (const msgFieldPayload   field,
                                const mama_i64_t**      result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorU64 (const msgFieldPayload   field,
                                const mama_u64_t**      result,
                                mama_size_t*            size);
 extern mama_status
tick42blpmsgFieldPayload_getVectorF32 (const msgFieldPayload   field,
                                const mama_f32_t**      result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorF64 (const msgFieldPayload   field,
                                const mama_f64_t**      result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorString
                               (const msgFieldPayload   field,
                                const char***           result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorDateTime
                               (const msgFieldPayload   field,
                                const mamaDateTime*     result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorPrice
                               (const msgFieldPayload   field,
                                const mamaPrice*        result,
                                mama_size_t*            size);
extern mama_status
tick42blpmsgFieldPayload_getVectorMsg (const msgFieldPayload   field,
                                const msgPayload**      result,
                                mama_size_t*            size);

extern mama_status
tick42blpmsgFieldPayload_getAsString (const msgFieldPayload   field,
								const msgPayload   msg,
								char*         buf,
								mama_size_t   len);

}

#endif /* BLP_MSG_IMPL_H__*/
