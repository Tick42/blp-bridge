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

#ifndef BLPMSGIMPL_H
#define BLPMSGIMPL_H

#include "bridge.h"
#include "payloadbridge.h"

#ifdef str
#undef str
#endif

#if defined(__cplusplus)
extern "C" {
#endif


// fid lookup
	void InitFidMap();

typedef struct blpPayload
{
    struct blpFieldPayload*    blpField_;
    mamaMsg                     parent_;
} blpPayloadImpl_t;


typedef struct blpIterator_t
{
    blpFieldPayload*   blpField_;
} blpIterator_t;


mama_status
blpMsg_setBool(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        mama_bool_t     value);


mama_status
blpMsg_setChar(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        char            value);

mama_status
blpMsg_setI8(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        int8_t          value);

mama_status
blpMsg_setU8(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        uint8_t         value);

mama_status
blpMsg_setI16(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        int16_t         value);

mama_status
blpMsg_setU16(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        uint16_t        value);

mama_status
blpMsg_setI32(
    const msgPayload    msg,
    const char*         name,
    mama_fid_t          fid,
    int32_t             value);

mama_status
blpMsg_setU32(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        uint32_t        value);

mama_status
blpMsg_setI64(
    const msgPayload    msg,
    const char*         name,
    mama_fid_t          fid,
    int64_t             value);

mama_status
blpMsg_setU64(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        uint64_t        value);

mama_status
blpMsg_setF32(
   const msgPayload    msg,
   const char*     name,
   mama_fid_t      fid,
   mama_f32_t      value);

mama_status
blpMsg_setF64(
    const msgPayload    msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t   value);

mama_status
blpMsg_setString(
    const msgPayload    msg,
    const char*  name,
    mama_fid_t   fid,
    const char*  value);

mama_status
blpMsg_setOpaque(
        const msgPayload    msg,
        const char*  name,
        mama_fid_t   fid,
        const void*  value,
        size_t       size);

mama_status
blpMsg_setVectorBool(
    const msgPayload    msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_bool_t  value[],
    size_t             numElements);

mama_status
blpMsg_setVectorChar (
    const msgPayload    msg,
    const char*        name,
    mama_fid_t         fid,
    const char         value[],
    size_t             numElements);

mama_status
blpMsg_setVectorI8 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const int8_t       value[],
        size_t             numElements);

mama_status
blpMsg_setVectorU8 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const uint8_t      value[],
        size_t             numElements);

mama_status
blpMsg_setVectorI16 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const int16_t      value[],
        size_t             numElements);

mama_status
blpMsg_setVectorU16 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const uint16_t     value[],
        size_t             numElements);

mama_status
blpMsg_setVectorI32 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const int32_t      value[],
        size_t             numElements);


mama_status
blpMsg_setVectorU32 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const uint32_t     value[],
        size_t             numElements);


mama_status
blpMsg_setVectorI64 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const int64_t      value[],
        size_t             numElements);


mama_status
blpMsg_setVectorU64 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const uint64_t     value[],
        size_t             numElements);


mama_status
blpMsg_setVectorF32 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_f32_t   value[],
        size_t             numElements);


mama_status
blpMsg_setVectorF64 (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_f64_t   value[],
        size_t             numElements);


mama_status
blpMsg_setVectorString (
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const char*        value[],
        size_t             numElements);


mama_status
blpMsg_setVectorDateTime (
    const msgPayload    msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value[],
    size_t              numElements);

mama_status
blpMsg_setMsg(
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const msgPayload   value);


mama_status
blpMsg_setVectorMsg(
        const msgPayload    msg,
        const char*        name,
        mama_fid_t         fid,
        const mamaMsg      value[],
        size_t             numElements);


mama_status
blpMsg_getBool(
        const msgPayload    msg,
        const char*   name,
        mama_fid_t    fid,
        mama_bool_t*  result);

mama_status
blpMsg_getChar(
        const msgPayload    msg,
        const char*   name,
        mama_fid_t    fid,
        char*         result);

mama_status
blpMsg_getI8(
        const msgPayload    msg,
        const char*   name,
        mama_fid_t    fid,
        int8_t*       result);

mama_status
blpMsg_getU8(
        const msgPayload    msg,
        const char*   name,
        mama_fid_t    fid,
        uint8_t*      result);

mama_status
blpMsg_getI16(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        int16_t*        result);


mama_status
blpMsg_getU16(
        const msgPayload    msg,
        const char*     name,
        mama_fid_t      fid,
        uint16_t*       result);

mama_status
blpMsg_getI32(
    const msgPayload    msg,
    const char*  name,
    mama_fid_t   fid,
    int32_t*     result);

mama_status
blpMsg_getU32(
        const msgPayload    msg,
        const char*    name,
        mama_fid_t     fid,
        uint32_t*      result);


mama_status
blpMsg_getI64(
    const msgPayload    msg,
    const char*  name,
    mama_fid_t   fid,
    int64_t*     result);


mama_status
blpMsg_getU64(
        const msgPayload    msg,
        const char*    name,
        mama_fid_t     fid,
        uint64_t*      result);

mama_status
blpMsg_getF32(
        const msgPayload    msg,
        const char*    name,
        mama_fid_t     fid,
        mama_f32_t*    result);


mama_status
blpMsg_getF64(
    const msgPayload    msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t*  result);


mama_status
blpMsg_getString(
    const msgPayload    msg,
    const char*  name,
    mama_fid_t   fid,
    const char** result);


mama_status
blpMsg_getOpaque(
        const msgPayload    msg,
        const char*    name,
        mama_fid_t     fid,
        const void**   result,
        size_t*        size);


mama_status
blpMsg_getDateTime(
        const msgPayload    msg,
        const char*    name,
        mama_fid_t     fid,
        mamaDateTime   result);


mama_status
blpMsg_setDateTime(
        const msgPayload    msg,
        const char*         name,
        mama_fid_t          fid,
        const mamaDateTime  value);

mama_status
blpMsg_getVectorDateTime (
        const msgPayload    msg,
        const char*           name,
        mama_fid_t            fid,
        const mamaDateTime*   result,
        size_t*               resultLen);

mama_status
blpMsg_getVectorPrice (
        const msgPayload    msg,
        const char*           name,
        mama_fid_t            fid,
        const mamaPrice*      result,
        size_t*               resultLen);

mama_status
blpMsg_setPrice(
    const msgPayload    msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value);

mama_status
blpMsg_setVectorPrice (
    const msgPayload    msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value[],
    size_t              numElements);

mama_status
blpMsg_getPrice(
    const msgPayload    msg,
    const char*    name,
    mama_fid_t     fid,
    mamaPrice      result);

mama_status
blpMsg_getMsg (
    const msgPayload    msg,
    const char*      name,
    mama_fid_t       fid,
    msgPayload*      result);

mama_status
blpMsg_getVectorBool (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_bool_t**  result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorChar (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const char**         result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorI8 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const int8_t**       result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorU8 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const uint8_t**      result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorI16 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const int16_t**      result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorU16 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const uint16_t**     result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorI32 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const int32_t**      result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorU32 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const uint32_t**     result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorI64 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const int64_t**      result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorU64 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const uint64_t**     result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorF32 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_f32_t**   result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorF64 (
        const msgPayload    msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_f64_t**   result,
        size_t*              resultLen);


mama_status
blpMsg_getVectorString (
        const msgPayload    msg,
        const char*    name,
        mama_fid_t     fid,
        const char***  result,
        size_t*        resultLen);


mama_status
blpMsg_getVectorMsg (
    const msgPayload    msg,
    const char*         name,
    mama_fid_t          fid,
    const msgPayload**  result,
    size_t*             resultLen);

mama_status
blpMsg_getFieldAsString(const msgPayload    msg,
    const char*  name,
    mama_fid_t   fid,
    char*        buf,
    size_t       len);



#if defined(__cplusplus)
}
#endif

#endif
