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
#ifndef BLP_PAYLOAD_H
#define BLP_PAYLOAD_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "blpfieldpayload.h"
#include "blpmsgutils.h"
#include <algorithm>
class BlpPayloadFieldIterator;

class BlpPayload
{
public:
	friend class BlpPayloadFieldIterator;
	typedef std::map<mama_fid_t, BlpFieldPayload> FieldsMap_t;
	typedef FieldsMap_t::const_iterator FieldIterator_t;

	BlpPayload(mamaMsg parent) :
		parent_ (parent)
	{}

	BlpPayload() :
		parent_ (0)
    {}

	BlpPayload(const BlpPayload& payload) :
		parent_ (payload.parent_),
		fields_ (payload.fields_)
    {}

	BlpPayload& operator=(const BlpPayload& payload)
	{
		parent_ = payload.parent_;
		fields_ = payload.fields_;

		return *this;
	}

	void setParent(mamaMsg parent)
	{
		parent_ = parent;
	}

	void set(mama_fid_t fid, const std::string& name, const std::string& value)
	{
		fields_[fid] = BlpFieldPayload(fid, name, value);
	}

	template <typename T>
	void set(mama_fid_t fid, const std::string& name, T value)
	{
		fields_[fid] = BlpFieldPayload(fid, name, value);
	}

	// Support fields lookup by name. has O(n) complexity
	inline BlpPayload::FieldsMap_t::const_iterator FindNameInFields(const std::string &name) const
	{

		BlpPayload::FieldsMap_t::const_iterator cit = fields_.begin();
		for(; cit != fields_.end() && cit->second.name_ != name; ++cit);
		return cit;
	}
	
	// get with name has O(n) complexity
	mama_status get(const std::string& name, const char** value/*out*/) const
	{
		auto found = FindNameInFields(name);
		if (found == fields_.end())
		{
			return MAMA_STATUS_NOT_FOUND;
		}

		mama_status ret = MAMA_STATUS_OK;
		try 
		{
			*value = boost::get<std::string>(found->second.data_).c_str();
		}
		catch (boost::bad_get&)
		{
			ret = MAMA_STATUS_WRONG_FIELD_TYPE;
		}

		return ret;
	}


	mama_status get(const std::string& name, int16_t value/*out*/) const
	{
		auto found = FindNameInFields(name);

		if (found == fields_.end())
		{
			return MAMA_STATUS_NOT_FOUND;
		}

		mama_status ret = MAMA_STATUS_OK;

		if((found->second.data_).which() == 4)
		{
			value = (int16_t)boost::get<int32_t>(found->second.data_);
		}
		else
		{
			value = boost::get<int16_t>(found->second.data_);
		}

		return ret;
	}

	// for others
	template <typename T>
	mama_status get(const std::string& name, T& value/*out*/) const
	{
		auto found = FindNameInFields(name);
		if (found == fields_.end())
		{
			return MAMA_STATUS_NOT_FOUND;
		}

		mama_status ret = MAMA_STATUS_OK;
		try 
		{
			value = boost::get<T>(found->second.data_);
		}
		catch (boost::bad_get&)
		{
			ret = MAMA_STATUS_WRONG_FIELD_TYPE;
		}

		return ret;
	}

	mama_status getAsString(const std::string& name, std::string& value) const
	{
		auto found = FindNameInFields(name);
		if (found == fields_.end())
		{
			return MAMA_STATUS_NOT_FOUND;
		}

		mama_status ret = MAMA_STATUS_OK;
		if (found->second.type_ == MAMA_FIELD_TYPE_STRING)
		{
			try 
			{
				value = boost::get<std::string>(found->second.data_);
			}
			catch (boost::bad_get&)
			{
				ret = MAMA_STATUS_WRONG_FIELD_TYPE;
			}
		}
		else if (found->second.type_ == MAMA_FIELD_TYPE_TIME)
		{
			try 
			{
				uint64_t ms = boost::get<uint64_t>(found->second.data_);
				value = epochTimeToString(ms);
			}
			catch (boost::bad_get&)
			{
				ret = MAMA_STATUS_WRONG_FIELD_TYPE;
			}
		}
		else
		{
			std::ostringstream oss;

			oss << found->second.data_;
			value = oss.str();
		}

		return ret;
	}

	mama_status getField(const std::string& name, msgFieldPayload* result) const
	{
		auto found = FindNameInFields(name);
		if (found == fields_.end())
		{
			*result = 0;
			return MAMA_STATUS_NOT_FOUND;
		}

		*result = (msgFieldPayload)&found->second;

		return MAMA_STATUS_OK;
	}

// get with fid has log(n) complexity
mama_status get(mama_fid_t fid, const char** value/*out*/) const 
{
	auto found = fields_.find(fid);
	if (found == fields_.end())
	{
		return MAMA_STATUS_NOT_FOUND;
	}

	mama_status ret = MAMA_STATUS_OK;
	try 
	{
		*value = boost::get<std::string>(found->second.data_).c_str();
	}
	catch (boost::bad_get&)
	{
		ret = MAMA_STATUS_WRONG_FIELD_TYPE;
	}

	return ret;
}


mama_status get(mama_fid_t fid, int16_t value/*out*/) const
{
	auto found = fields_.find(fid);

	if (found == fields_.end())
	{
		return MAMA_STATUS_NOT_FOUND;
	}

	mama_status ret = MAMA_STATUS_OK;

	if((found->second.data_).which() == 4)
	{
		value = (int16_t)boost::get<int32_t>(found->second.data_);
	}
	else
	{
		value = boost::get<int16_t>(found->second.data_);
	}

	return ret;
}

// for others
template <typename T>
mama_status get(mama_fid_t fid, T& value/*out*/) const
{
	auto found = fields_.find(fid);
	if (found == fields_.end())
		return MAMA_STATUS_NOT_FOUND;

	mama_status ret = MAMA_STATUS_OK;
	try 
	{
		value = boost::get<T>(found->second.data_);
	}
	catch (boost::bad_get&)
	{
		ret = MAMA_STATUS_WRONG_FIELD_TYPE;
	}

	return ret;
}

mama_status getAsString(mama_fid_t fid, std::string& value) const
{
	auto found = fields_.find(fid);
	if (found == fields_.end())
		return MAMA_STATUS_NOT_FOUND;

	mama_status ret = MAMA_STATUS_OK;
	if (found->second.type_ == MAMA_FIELD_TYPE_STRING)
	{
		try 
		{
			value = boost::get<std::string>(found->second.data_);
		}
		catch (boost::bad_get&)
		{
			ret = MAMA_STATUS_WRONG_FIELD_TYPE;
		}
	}
	else if (found->second.type_ == MAMA_FIELD_TYPE_TIME)
	{
		try 
		{
			uint64_t ms = boost::get<uint64_t>(found->second.data_);
			value = epochTimeToString(ms);
		}
		catch (boost::bad_get&)
		{
			ret = MAMA_STATUS_WRONG_FIELD_TYPE;
		}
	}
	else
	{
		std::ostringstream oss;

		oss << found->second.data_;
		value = oss.str();
	}

	return ret;
}

mama_status getField(mama_fid_t fid, msgFieldPayload* result) const
{
	auto found = fields_.find(fid);
	if (found == fields_.end())
	{
		*result = 0;
		return MAMA_STATUS_NOT_FOUND;
	}

	*result = (msgFieldPayload)&found->second;

	return MAMA_STATUS_OK;
}

	mama_status iterateFields(mamaMsgField            field,
                              mamaMsgIteratorCb       cb,
                              void*                   closure) const
	{
		for (FieldIterator_t fld = fields_.begin(); fld != fields_.end(); ++fld)
		{
			mamaMsgFieldImpl_setPayload (field, (msgFieldPayload)&fld->second);
			(cb)(parent_, field, closure);
		}

		return MAMA_STATUS_OK;
	}

	void clear()
	{
		fields_.clear();
	}

	size_t numFields() const
	{
		return fields_.size();
	}

private:
	mamaMsg parent_;

	FieldsMap_t fields_;
};



class BlpPayloadFieldIterator
{
public:
	BlpPayloadFieldIterator(const BlpPayload& payload) :
		current_ (payload.fields_.begin()),
		end_ (payload.fields_.begin())
	{
	}


private:
	BlpPayload::FieldsMap_t::const_iterator current_;
	const BlpPayload::FieldsMap_t::const_iterator end_;
};

#endif