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
#ifndef BLP_FIELD_PAYLOAD_H
#define BLP_FIELD_PAYLOAD_H

#include <string>

#include "blpvaluetype.h"

template <typename T>
struct MamaFieldType
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_UNKNOWN;
};

template <>
struct MamaFieldType<int8_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_I8;
};

template <>
struct MamaFieldType<uint8_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_U8;
};

template <>
struct MamaFieldType<int16_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_I16;
};

template <>
struct MamaFieldType<uint16_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_U16;
};


template <>
struct MamaFieldType<int32_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_I32;
};

template <>
struct MamaFieldType<uint32_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_U32;
};

template <>
struct MamaFieldType<int64_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_I64;
};

template <>
struct MamaFieldType<uint64_t>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_U64;
};

template <>
struct MamaFieldType<mama_datetime>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_TIME;
};

template <>
struct MamaFieldType<float>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_F32;
};

template <>
struct MamaFieldType<double>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_F64;
};

template <>
struct MamaFieldType<std::string>
{
	static const mamaFieldType Value = MAMA_FIELD_TYPE_STRING;
};

struct BlpFieldPayload
{
	mama_fid_t fid_;
	std::string name_;
	mamaFieldType type_;
	ValueType_t data_;

	BlpFieldPayload() 
		: fid_(0)
		, name_ ()
		, type_ (MAMA_FIELD_TYPE_UNKNOWN)
		, data_ ()
	{}

	BlpFieldPayload(const mama_fid_t fid, const std::string &name, const char* value) 
		: fid_	(fid)
		, name_ (name)
		, type_ (MAMA_FIELD_TYPE_STRING)
		, data_ (std::string(value))
	{}


	BlpFieldPayload(const mama_fid_t fid, const std::string &name, const mama_datetime & value) 
		: fid_	(fid)
		, name_ (name)
		, type_ (MAMA_FIELD_TYPE_TIME)
		, data_ (value)
		{}

	template <typename T>
	BlpFieldPayload(const mama_fid_t fid, const std::string &name, T value) 
		: fid_	(fid)
		, name_ (name)
		, type_ (MamaFieldType<T>::Value)
		, data_ (value)
	{}

	mama_status get(const char*& value/*out*/) const
	{
		mama_status ret = MAMA_STATUS_OK;
		try 
		{
			value = boost::get<std::string>(data_).c_str();
		}
		catch (boost::bad_get&)
		{
			ret = MAMA_STATUS_WRONG_FIELD_TYPE;
		}

		return ret;
	}

	mama_status get(int32_t & value)
	{
		mama_status ret = MAMA_STATUS_OK;

		if(data_.which() == 4)
		{
			value = boost::get<int32_t>(data_);
		}
		

		return ret;
	}

	template <typename T>
	mama_status get(T& value/*out*/) const
	{
		mama_status ret = MAMA_STATUS_OK;
		try 
		{
			value = boost::get<T>(data_);
		}
		catch (boost::bad_get&)
		{
			ret = MAMA_STATUS_WRONG_FIELD_TYPE;
		}

		return ret;
	}

	template <typename T>
	mama_status set(T value)
	{
		data_ = value;
		return MAMA_STATUS_OK;
	}
};

#endif