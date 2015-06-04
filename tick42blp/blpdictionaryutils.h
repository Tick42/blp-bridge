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
#ifndef __BLPDICTIONARYUTILS_H__
#define __BLPDICTIONARYUTILS_H__

#include <mama/mama.h>
#include <blpapi_element.h>

#include <string>
#include <map>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <functional>


enum blpapi_DataType_t;
enum mamaFieldType_;
/*
 *  BlpToWombatField_t encapsulate one OpenMAMA field information (name, FID & type)
 *	It reflects the columns "wombat_field_name","wombat_fid","wombat_field_type" in the fieldmap.csv
 */
struct BlpToWombatField_t
{
	std::string  		wombat_field_name;
	mama_fid_t 			wombat_fid;
	mamaFieldType_ 		wombat_field_type;

	BlpToWombatField_t() : wombat_field_name(), wombat_fid(), wombat_field_type() {}
	BlpToWombatField_t(const BlpToWombatField_t &rhs) { *this = rhs;};
	BlpToWombatField_t &operator=(const BlpToWombatField_t &rhs)
	{
		if (this != &rhs)
		{
			wombat_field_name	= rhs.wombat_field_name;
			wombat_fid			= rhs.wombat_fid;
			wombat_field_type 	= rhs.wombat_field_type; 
		}
		return *this;
	}
};


/* Maps Bloomberg field name and type to MAMA
 * The key reflects the "field_name" column in fieldmap.csv 
 * The value reflects the rest of the columns "wombat_field_name","wombat_fid","wombat_field_type"
 */
typedef std::map<std::string, BlpToWombatField_t> BlpToWombatFieldsMap_t;

/*
 * SourceWombatTypeKey_t is used as key for type conversion between any source type parameter
 * to OpenMAMA destination type. The key supports any map/set and any unordered map/set 
 * counterparts
 */
template <typename SourceType> 
struct SourceWombatTypeKey_t
{
	SourceType from;
	mamaFieldType_ to;
	SourceWombatTypeKey_t() : from(), to() {}
	SourceWombatTypeKey_t(SourceType from, mamaFieldType_ to) : from(from), to(to) {}
	SourceWombatTypeKey_t(const SourceWombatTypeKey_t &rhs) {*this = rhs;}
	SourceWombatTypeKey_t & operator=(const SourceWombatTypeKey_t &rhs)
	{
		if (this != &rhs)
		{
			from = rhs.from;
			to = rhs.to;
		}
		return *this;
	}
	bool operator <(const SourceWombatTypeKey_t &rhs) const
	{
		if (this->from == rhs.from)
			return (this->to < rhs.to);
		else
			return (this->from < rhs.from);
	}

	bool operator ==(const SourceWombatTypeKey_t &rhs) const
	{
		return (this->from == rhs.from) && (this->to == rhs.to);
	}

};

/*
 * BlpWombatTypeKey_t is a specialization of SourceWombatTypeKey_t<> to support spcific Bloomberg type conversion
 */
typedef SourceWombatTypeKey_t<blpapi_DataType_t> BlpWombatTypeKey_t;

/*
 * createMessageFunc_t is used later on along to prepare a message according to conversion specified 
 * in the key of the map OperationOnTypesMap_t. 
 */
typedef std::function<mama_status (mamaMsg &msg_, const std::string &fieldName, mama_fid_t fid, const BloombergLP::blpapi::Element &field)> createMessageFunc_t;
/*
 * Map of special operations (creating special messages) from a specific source type to OpenMAMA, Thus support 
 * non-trivial type conversions and then creating a message for them. See the lambda implementation of  
 * createMessageFunc_t in order to understand what kind of operations are done.
 */
typedef std::map<BlpWombatTypeKey_t, createMessageFunc_t> OperationOnTypesMap_t;

/*
 * BlpFieldDictionary holds all the information and operations needed for field-name and type conversion between 
 * the source feed (currently Bloomberg) and OpenMAMA
 */
class BlpFieldDictionary {
public:
	BlpToWombatFieldsMap_t BlpToWombatFieldsMap_;
private:
	OperationOnTypesMap_t OperationOnTypesMap_;
	const mama_fid_t NonTranslatedFieldFid_Start;
	mama_fid_t NonTranslatedFieldFid_CurrentValue_; //should be incremented each time a non translated field is added
	std::map<std::string, mama_fid_t> NonTranslatedFieldFidMap_;
public:
	/* Initialize maps 
	 * @param path - path for the fieldmap.csv
	 * @param nonTranslatedFieldFid_Start - the first FID number for non-translated fields
	 */
	BlpFieldDictionary(std::string path, mama_fid_t nonTranslatedFieldFid_Start);
	/* Initialize maps without the fieldmap.csv dictionary 
	 * @param nonTranslatedFieldFid_Start - the first FID number for non-translated fields
	 */
	BlpFieldDictionary(mama_fid_t nonTranslatedFieldFid_Start);

	typedef BlpToWombatFieldsMap_t::mapped_type fieldValue_t;
	typedef OperationOnTypesMap_t::mapped_type operationOnType_t;

	typedef OperationOnTypesMap_t::key_type typesKey_t;
	/* Find field in the fields map.
	 * @param[in] key - field key.
	 * @param[out] value - the result value is succeed to find.
	 * @return true if field is found.
	 */
	bool FindField(const BlpToWombatFieldsMap_t::key_type &key, fieldValue_t &value) const;

	/* Find what operation (prepare a message) to create on a given key that consist of the source
	 * type (Bloomberg) and the destination type (OpenMAMA)
	 * @param[in] key - a given source and destination types pair.
	 * @param[out] value -  the result functor is succeed to find.
	 */
	bool FindOperationForTypes(const typesKey_t &key, operationOnType_t &value) const;
	/* Generate an arbitrary and unique key FID for each non-translated field, i.e. that does no exist 
	 * in the fieldmap.csv. The given field number is serial according to order of fields in the feed 
	 * and valid only per session.
	 * @param[in] key - the field name.
	 * @return arbitrary unique FID
	 */
	mama_fid_t GetNonTranslatedFieldFid(const std::string &key);
private:
	/* Initialize the operations to be done on special type conversion 
	 * see OperationOnTypesMap_t for more information.
	 */
	void InitOperationOnTypesMap();
	/**
	 * Create a predefined dictionary from.csv dictionary file 
	 * Logfile prefix LoadPredefinedBlpFieldsDictionary
	 *
	 * @param in_dict_file: File object for the .csv file
	 * @return true if succeed
	 */
	bool LoadPredefinedBlpFieldsDictionary(std::ifstream &in_dict_file); 
	
private:
	// --- this object should not be copied. use a reference/pointer or a shared_ptr instead
	BlpFieldDictionary(const BlpFieldDictionary&);
	BlpFieldDictionary &operator=(const BlpFieldDictionary&);
};

#endif