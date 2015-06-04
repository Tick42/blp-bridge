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

#include <bridge.h>
#include <payloadbridge.h>
#include <mama/mama.h>
#include <msgfieldimpl.h>

#ifdef str
#undef str
#endif

#include "blpmsgimpl.h"
#include "blppayload.h"

#define blpPayload(msg) ((BlpPayload*)msg)


//
// There are a whole bunch of interesting issues associate with data dictionaries in OM !
// One that we need to deal with here is that the set of fields delivered by bloomberg is disjoint to the set of fields defined in the wombat dictionary.
// Mostly this doesnt matter and we can just deliver bloomberg fields by name but there is set of fields whose values are requested by fid.
// So in the short term we work around this with a small hardcoded dictionary to lookup these fields

typedef std::map<int, std::string> fid2nameMap_t;

static fid2nameMap_t fid2names;

static bool fidMapIsInit = false;

void InitFidMap()
{
	if(fidMapIsInit)
	{ 
		// no need to do it more than once
		return;
	}

	fid2names.insert(fid2nameMap_t::value_type(1 , "MdMsgType")); 
	fid2names.insert(fid2nameMap_t::value_type(2 , "MdMsgStatus")); 
	fid2names.insert(fid2nameMap_t::value_type(7 , "MdMsgNum")); 
	fid2names.insert(fid2nameMap_t::value_type(8 , "MdMsgTotal")); 
	fid2names.insert(fid2nameMap_t::value_type(10 , "MdSeqNum")); 
	fid2names.insert(fid2nameMap_t::value_type(11 , "MdFeedName"));
	fid2names.insert(fid2nameMap_t::value_type(12 , "MdFeedHost"));
	fid2names.insert(fid2nameMap_t::value_type(13 , "MdFeedGroup"));
	fid2names.insert(fid2nameMap_t::value_type(15 , "MdItemSeq"));
	fid2names.insert(fid2nameMap_t::value_type(16 , "MamaSendTime"));
	fid2names.insert(fid2nameMap_t::value_type(17 , "MamaAppDataType"));
	fid2names.insert(fid2nameMap_t::value_type(18 , "MamaAppMsgType"));
	fid2names.insert(fid2nameMap_t::value_type(20 , "MamaSenderId"));
	fid2names.insert(fid2nameMap_t::value_type(21 , "wMsgQual"));
	fid2names.insert(fid2nameMap_t::value_type(22 , "wConflateCount"));
	fid2names.insert(fid2nameMap_t::value_type(23 , "wConflateQuoteCount"));
	fid2names.insert(fid2nameMap_t::value_type(24 , "wConflateTradeCount"));
	fid2names.insert(fid2nameMap_t::value_type(305 , "wIssueSymbol"));

	return;

}

bool LookupFid(int fid, std::string & fieldname)
{
	bool found = false;
	fid2nameMap_t::iterator it = fid2names.find(fid);
	if(it == fid2names.end())
	{
		fieldname = "";
		found = false;
		mama_log(MAMA_LOG_LEVEL_WARN, "blpmsgimpl: failed to look up field name for fid %d", fid);
	}
	else
	{
		fieldname = it->second;
		found = true;
	}

	return found;
}


// if there is no fieldname and only a fid look up the fieldname from the fid
#define CHECK_LOOKUPFID(ID, FIELDNAME)\
	do\
{\
	if(ID == 0 && fid != 0)\
{\
	if (LookupFid(fid, FIELDNAME))\
{\
	ID = FIELDNAME.c_str();\
}\
	else\
{\
	return MAMA_STATUS_NOT_FOUND;\
}\
}\
}while(0);

#define CHECK_NAME(NAME)\
do\
{\
	if (!NAME)\
		return MAMA_STATUS_NOT_FOUND;\
}while(0);

namespace {
	static const char empty_buf[] = "";
}

mama_status
blpMsg_setBool(msgPayload      msg,
			   const char*     name,
			   mama_fid_t      fid,
			   mama_bool_t     value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

	return MAMA_STATUS_OK;
}

mama_status
blpMsg_setChar(msgPayload      msg,
			   const char*     name,
			   mama_fid_t      fid,
			   char            value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setI8(msgPayload     msg,
			 const char*     name,
			 mama_fid_t      fid,
			 int8_t          value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setU8(msgPayload     msg,
			 const char*     name,
			 mama_fid_t      fid,
			 uint8_t         value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setI16(msgPayload     msg,
			  const char*     name,
			  mama_fid_t      fid,
			  int16_t         value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

	return MAMA_STATUS_OK;

}

mama_status
blpMsg_setU16(msgPayload     msg,
			  const char*     name,
			  mama_fid_t      fid,
			  uint16_t        value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setI32(msgPayload     msg,
			  const char*         name,
			  mama_fid_t          fid,
			  int32_t             value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setU32(msgPayload     msg,
			  const char*     name,
			  mama_fid_t      fid,
			  uint32_t        value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setI64(msgPayload     msg,
			  const char*         name,
			  mama_fid_t          fid,
			  int64_t             value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setU64(msgPayload     msg,
			  const char*     name,
			  mama_fid_t      fid,
			  uint64_t        value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setF32(msgPayload     msg,
			  const char*     name,
			  mama_fid_t      fid,
			  mama_f32_t      value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setF64(msgPayload     msg,
			  const char*  name,
			  mama_fid_t   fid,
			  mama_f64_t   value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setString(msgPayload     msg,
				 const char*  name,
				 mama_fid_t   fid,
				 const char*  value)
{
	CHECK_NAME(name);
	if (!name) 
		name = empty_buf;

	blpPayload(msg)->set(fid, name, value);

    return MAMA_STATUS_OK;
}

mama_status
blpMsg_setOpaque(msgPayload     msg,
				 const char*  name,
				 mama_fid_t   fid,
				 const void*  value,
				 size_t       size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorBool (
    msgPayload     msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_bool_t  value[],
    size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorChar (
    msgPayload     msg,
    const char*        name,
    mama_fid_t         fid,
    const char         value[],
    size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorI8 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const int8_t       value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorU8 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const uint8_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorI16 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const int16_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorU16 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const uint16_t     value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorI32 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const int32_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorU32 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const uint32_t     value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorI64 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const int64_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorU64 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const uint64_t     value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorF32 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_f32_t   value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorF64 (
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_f64_t   value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorString (
        msgPayload     msg,
        const char*  name,
        mama_fid_t   fid,
        const char*  value[],
        size_t       numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorDateTime (
        msgPayload     msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value[],
    size_t              numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setMsg(
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const msgPayload   value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorMsg(
        msgPayload     msg,
        const char*        name,
        mama_fid_t         fid,
        const mamaMsg      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getBool(msgPayload     msg,
			   const char*   name,
			   mama_fid_t    fid,
			   mama_bool_t*  result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}


mama_status
blpMsg_getChar(msgPayload     msg,
			   const char*   name,
			   mama_fid_t    fid,
			   char*         result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getI8(msgPayload     msg,
			 const char*   name,
			 mama_fid_t    fid,
			 int8_t*       result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getU8(msgPayload     msg,
			 const char*   name,
			 mama_fid_t    fid,
			 uint8_t*      result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getI16(msgPayload     msg,
			  const char*     name,
			  mama_fid_t      fid,
			  int16_t*        result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)

		return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getU16(msgPayload     msg,
			  const char*     name,
			  mama_fid_t      fid,
			  uint16_t*       result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)

		return blpPayload(msg)->get(fid, *result);
}




mama_status
blpMsg_getI32(msgPayload     msg,
			  const char*  name,
			  mama_fid_t   fid,
			  int32_t*     result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)

	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getU32(msgPayload     msg,
			  const char*    name,
			  mama_fid_t     fid,
			  uint32_t*      result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getI64(msgPayload     msg,
			  const char*  name,
			  mama_fid_t   fid,
			  int64_t*     result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getU64(msgPayload     msg,
			  const char*    name,
			  mama_fid_t     fid,
			  uint64_t*      result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getF32(msgPayload     msg,
			  const char*    name,
			  mama_fid_t     fid,
			  mama_f32_t*    result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getF64(msgPayload     msg,
			  const char*  name,
			  mama_fid_t   fid,
			  mama_f64_t*  result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	return blpPayload(msg)->get(fid, *result);
}

mama_status
blpMsg_getString(msgPayload     msg,
				 const char*  name,
				 mama_fid_t   fid,
				 const char** result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)

	return blpPayload(msg)->get(fid, *result);
}


mama_status
blpMsg_getOpaque(
        msgPayload     msg,
        const char*    name,
        mama_fid_t     fid,
        const void**   result,
        size_t*        size)
{
	return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getDateTime(
        msgPayload     msg,
        const char*    name,
        mama_fid_t     fid,
        mamaDateTime   result)
{
	const char* id = name;
	std::string fieldname;
	CHECK_LOOKUPFID(id, fieldname)


	char tempName[64];
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}

	mama_datetime tempdt;

	mama_status st = blpPayload(msg)->get(fid, tempdt);
	if (st == MAMA_STATUS_OK)
	{
		mamaDateTime_setEpochTimeMicroseconds(result, tempdt.dt);
	}

	return st;
}

mama_status
blpMsg_setDateTime(
        msgPayload     msg,
        const char*         name,
        mama_fid_t          fid,
        const mamaDateTime  value)
{
	uint64_t tempu64;
	mamaDateTime_getEpochTimeMicroseconds(value, &tempu64);

	mama_datetime tempdt;
	tempdt.dt = tempu64;
	blpPayload(msg)->set(fid, name, tempdt);
	return MAMA_STATUS_OK;
}

mama_status
blpMsg_getVectorDateTime (
        msgPayload     msg,
        const char*           name,
        mama_fid_t            fid,
        const mamaDateTime*   result,
        size_t*               resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorPrice (
        msgPayload     msg,
        const char*           name,
        mama_fid_t            fid,
        const mamaPrice*      result,
        size_t*               resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setPrice(
    msgPayload     msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value)
{
	return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_setVectorPrice (
    msgPayload     msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice    value[],
    size_t              numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getPrice(
        msgPayload     msg,
        const char*    name,
        mama_fid_t     fid,
        mamaPrice      result)
{
	return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getMsg (
    msgPayload     msg,
    const char*         name,
    mama_fid_t          fid,
    msgPayload*         result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorBool (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_bool_t**  result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorChar (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const char**         result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorI8 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const int8_t**       result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorU8 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const uint8_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorI16 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const int16_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorU16 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const uint16_t**     result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorI32 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const int32_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorU32 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const uint32_t**     result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorI64 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const int64_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorU64 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const uint64_t**     result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorF32 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_f32_t**   result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorF64 (
        msgPayload     msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_f64_t**   result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorString (
        msgPayload     msg,
        const char*    name,
        mama_fid_t     fid,
        const char***  result,
        size_t*        resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getVectorMsg (
    msgPayload     msg,
    const char*         name,
    mama_fid_t          fid,
    const msgPayload**  result,
    size_t*             resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
blpMsg_getFieldAsString(msgPayload     msg,
						const char*  name,
						mama_fid_t   fid,
						char*        buf,
						size_t       len)
{
	std::string value;
	mama_status ret = blpPayload(msg)->getAsString(fid, value);

	if (ret == MAMA_STATUS_OK)
	{
		strncpy(buf, value.c_str(), value.size());
		buf[value.length()] = 0;
	}

	return ret;
}

