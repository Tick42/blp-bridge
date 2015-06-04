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

*Distributed under the Boost Software License, Version 1.0.
*    (See accompanying file LICENSE_1_0.txt or copy at
*         http://www.boost.org/LICENSE_1_0.txt)

*/

#include "blpdictionaryutils.h"
#include <mama/msg.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/scope_exit.hpp>
#include <boost/format.hpp>
#include "blpfilesystem.h"
#include "blpenvironment.h"
#include "common.h"  

static const std::string ENV_VAR_WOMBAT_PATH("WOMBAT_PATH");

static bool inline To_mamaFieldType(const std::string &from, mamaFieldType_ &to)
{
	/** Sub message*/
	if 		(from == "MAMA_FIELD_TYPE_MSG") to = MAMA_FIELD_TYPE_MSG;

	/** Opaque binary */
	else if (from == "MAMA_FIELD_TYPE_OPAQUE") to = MAMA_FIELD_TYPE_OPAQUE;

	/** String */
	else if (from == "MAMA_FIELD_TYPE_STRING") to = MAMA_FIELD_TYPE_STRING;

	/** Boolean */
	else if (from == "MAMA_FIELD_TYPE_BOOL") to = MAMA_FIELD_TYPE_BOOL;

	/** Character */
	else if (from == "MAMA_FIELD_TYPE_CHAR") to = MAMA_FIELD_TYPE_CHAR;

	/** Signed 8 bit integer */
	else if (from == "MAMA_FIELD_TYPE_I8") to = MAMA_FIELD_TYPE_I8;

	/** Unsigned byte */
	else if (from == "MAMA_FIELD_TYPE_U8") to = MAMA_FIELD_TYPE_U8;

	/** Signed 16 bit integer */
	else if (from == "MAMA_FIELD_TYPE_I16") to = MAMA_FIELD_TYPE_I16;

	/** Unsigned 16 bit integer */
	else if (from == "MAMA_FIELD_TYPE_U16") to = MAMA_FIELD_TYPE_U16;

	/** Signed 32 bit integer */
	else if (from == "MAMA_FIELD_TYPE_I32") to = MAMA_FIELD_TYPE_I32;

	/** Unsigned 32 bit integer */
	else if (from == "MAMA_FIELD_TYPE_U32") to = MAMA_FIELD_TYPE_U32;

	/** Signed 64 bit integer */
	else if (from == "MAMA_FIELD_TYPE_I64") to = MAMA_FIELD_TYPE_I64;

	/** Unsigned 64 bit integer */
	else if (from == "MAMA_FIELD_TYPE_U64") to = MAMA_FIELD_TYPE_U64;

	/** 32 bit float */
	else if (from == "MAMA_FIELD_TYPE_F32") to = MAMA_FIELD_TYPE_F32;

	/** 64 bit float */
	else if (from == "MAMA_FIELD_TYPE_F64") to = MAMA_FIELD_TYPE_F64;

	/** 64 bit MAMA time */
	else if (from == "MAMA_FIELD_TYPE_TIME") to = MAMA_FIELD_TYPE_TIME;

	/** MAMA price */
	else if (from == "MAMA_FIELD_TYPE_PRICE") to = MAMA_FIELD_TYPE_PRICE;

	/** Array type support */
	else if (from == "MAMA_FIELD_TYPE_VECTOR_I8") to = MAMA_FIELD_TYPE_VECTOR_I8;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_U8") to = MAMA_FIELD_TYPE_VECTOR_U8;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_I16") to = MAMA_FIELD_TYPE_VECTOR_I16;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_U16") to = MAMA_FIELD_TYPE_VECTOR_U16;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_I32") to = MAMA_FIELD_TYPE_VECTOR_I32;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_U32") to = MAMA_FIELD_TYPE_VECTOR_U32;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_I64") to = MAMA_FIELD_TYPE_VECTOR_I64;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_U64") to = MAMA_FIELD_TYPE_VECTOR_U64;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_F32") to = MAMA_FIELD_TYPE_VECTOR_F32;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_F64") to = MAMA_FIELD_TYPE_VECTOR_F64;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_STRING") to = MAMA_FIELD_TYPE_VECTOR_STRING;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_MSG") to = MAMA_FIELD_TYPE_VECTOR_MSG;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_TIME") to = MAMA_FIELD_TYPE_VECTOR_TIME;
	else if (from == "MAMA_FIELD_TYPE_VECTOR_PRICE") to = MAMA_FIELD_TYPE_VECTOR_PRICE;
	else if (from == "MAMA_FIELD_TYPE_QUANTITY") to = MAMA_FIELD_TYPE_QUANTITY;

	/** Collection */
	else if (from == "MAMA_FIELD_TYPE_COLLECTION") to = MAMA_FIELD_TYPE_COLLECTION;

	/** Unknown */
	else if (from == "MAMA_FIELD_TYPE_UNKNOWN") to = MAMA_FIELD_TYPE_UNKNOWN;
	else {
		//do assert
		return false;
	}
	return true;
}

/* enum_bridge_to_wombat_csv_fields_t - columns offsets of columns in the fieldmap.csv file */
enum enum_bridge_to_wombat_csv_fields_t
{
	ebmfc_remark =0,
	ebmfc_source_field_name = 1, 
	ebmfc_wombat_field_name=2,
	ebmfc_wombat_fid=3,
	ebmfc_wombat_field_type=4,
	ebmfc_source_field_description=5,
	ebmfc_max_fields,
};

bool BlpFieldDictionary::LoadPredefinedBlpFieldsDictionary(std::ifstream &in_dict_file) 
{
	int line_offset=0;

	//Try to tokenize the fieldmap.csv file as long as it is opened and no other failure happened
	try {
		using namespace std;
		using namespace boost;

		// nothing to tokenize
		if (!in_dict_file.is_open()) return false;

		typedef tokenizer< escaped_list_separator<char> > tokenizer_t;

		string line;
		const int number_of_fields = ebmfc_max_fields;
		BlpToWombatFieldsMap_t blp_to_wombat_map;

		// logged_items - items used later on for logging needs. 
		struct {
			string				source_field_name;
			string				wombat_field_name;
			string		  		wombat_fid;
			string		 		wombat_field_type ;
			void clear() {source_field_name.clear(); wombat_field_name.clear(); wombat_fid.clear(); wombat_field_type.clear();}
		} logged_items;

		// tokenize the file line by line
		
		for (;std::getline(in_dict_file,line); ++line_offset)
		{
			// Init
			logged_items.clear();
			tokenizer_t tokens(line);

			string tok;

			int i=0;
			string key;
			BlpToWombatField_t blp_to_wombat_field;

			bool should_skip_this_item =false;

			// Tokenize line
			for (tokenizer_t::const_iterator tok_cit = tokens.begin(); i < number_of_fields && tok_cit != tokens.end(); ++tok_cit)
			{
				tok = *tok_cit;
				trim(tok); 

				// skip any lines with a comment symbol
				if ((tok.size() && tok[0] == '#') || tok == "remark" && tok_cit == tokens.begin() ) 
				{
					should_skip_this_item=true;
					mama_log (MAMA_LOG_LEVEL_FINEST, "LoadPredefinedBlpFieldsDictionary skipped line %d", line_offset, tok.c_str());
					break; 
				}

				// Get the column to be worked on
				enum_bridge_to_wombat_csv_fields_t field = enum_bridge_to_wombat_csv_fields_t(i);

				switch(field)
				{
				default:
					mama_log (MAMA_LOG_LEVEL_WARN, "LoadPredefinedBlpFieldsDictionary Unknown field %d, for line %d", i, line_offset);
				case ebmfc_source_field_description:
					break;
				case ebmfc_remark:
					// here the remarked line is due to a first hush symbol in the line as it APPEARS in the spreadsheet application (like Excel or Open Office Calc)
					if ((tok.size() && tok[0] == '#') || tok == "remark" && tok_cit == tokens.begin() ) 
					{
						should_skip_this_item=true;
						mama_log (MAMA_LOG_LEVEL_FINEST, "LoadPredefinedBlpFieldsDictionary skipped line %d", line_offset, tok.c_str());
					}
					break;
				case ebmfc_source_field_name:
					logged_items.source_field_name = tok;
					key = tok;
					break;
				case ebmfc_wombat_field_name:
					logged_items.wombat_field_name = tok;
					blp_to_wombat_field.wombat_field_name = tok;
					break;
				case ebmfc_wombat_fid:
					logged_items.wombat_fid = tok;
					try {
						int val = boost::lexical_cast<int>(tok);
						blp_to_wombat_field.wombat_fid = val;
					} catch( boost::bad_lexical_cast const& ) {
						should_skip_this_item=true;
						if (should_skip_this_item) mama_log (MAMA_LOG_LEVEL_WARN, "LoadPredefinedBlpFieldsDictionary couldn't translate line %d, MAMA FID [%s]", line_offset, tok.c_str());
					}
					break;
				case ebmfc_wombat_field_type: 
					logged_items.wombat_field_type = tok;
					should_skip_this_item = !To_mamaFieldType(tok, blp_to_wombat_field.wombat_field_type);
					if (should_skip_this_item) mama_log (MAMA_LOG_LEVEL_WARN, "LoadPredefinedBlpFieldsDictionary couldn't translate line %d, MAMA Field Type [%s]", line_offset, tok.c_str());
					break;
				}

				if (should_skip_this_item)
					break;
				//----------------
				++i;
			}

			if (should_skip_this_item)
				continue;

			// if everything is OK so far, then add field to the map
			if (BlpToWombatFieldsMap_.insert(BlpToWombatFieldsMap_t::value_type(key, blp_to_wombat_field)).second)
				mama_log (MAMA_LOG_LEVEL_FINEST, "LoadPredefinedBlpFieldsDictionary New field from line [%05d]: %s -> %s[%s] as %s", line_offset, logged_items.source_field_name.c_str(), logged_items.wombat_field_name.c_str(), logged_items.wombat_fid.c_str(), logged_items.wombat_field_type.c_str());

		}
	}
	catch(...){
		return false;
		mama_log (MAMA_LOG_LEVEL_WARN, "LoadPredefinedBlpFieldsDictionary couldn't translate line [%d]", line_offset);
	}
	return true;
}


/*-----------------------------------------------------------------------------------------------------------------------------------*
 * MsgConverterItem_t: a helper class that associate each message converted with a pair of source type and message target type
 *-----------------------------------------------------------------------------------------------------------------------------------*/
template <blpapi_DataType_t SourceParam, mamaFieldType_ TargetParam>
class MsgConverterItem_t
{
	static mama_status CreateMessage(mamaMsg &msg_, const std::string &fieldName, mama_fid_t fid, const BloombergLP::blpapi::Element &field); //Left unimplemented to make sure the link will fail not implemented
public:
	static const OperationOnTypesMap_t::value_type &GetItem()
	{
		static const BlpWombatTypeKey_t key = BlpWombatTypeKey_t(SourceParam, TargetParam);
		static const OperationOnTypesMap_t::value_type item = OperationOnTypesMap_t::value_type(key,&CreateMessage);
		return item;
	}
};

template<>
mama_status MsgConverterItem_t<BLPAPI_DATATYPE_FLOAT32,MAMA_FIELD_TYPE_F64>::CreateMessage(mamaMsg &msg_, const std::string &fieldName, mama_fid_t fid, const BloombergLP::blpapi::Element &field) 
{
	return mamaMsg_addF64(msg_, fieldName.c_str(), fid, (mama_f64_t)field.getValueAsFloat32());
}

template<>
mama_status MsgConverterItem_t<BLPAPI_DATATYPE_DATE,MAMA_FIELD_TYPE_TIME>::CreateMessage(mamaMsg &msg_, const std::string &fieldName, mama_fid_t fid, const BloombergLP::blpapi::Element &field) 
{
	mama_status res = mama_status();
	Datetime dateTime = field.getValueAsDatetime();
	mamaDateTime dt;

	mamaDateTime_create(&dt);
	BOOST_SCOPE_EXIT( (&dt) )
	{
		mamaDateTime_destroy(dt);
	} BOOST_SCOPE_EXIT_END;

	mamaDateTime_setToMidnightToday(dt,	NULL);
	mamaDateTime_setDate(dt, dateTime.year(), dateTime.month(), dateTime.day());
	res = mamaMsg_addDateTime(msg_, fieldName.c_str(), fid, dt);

	//log as FINEST the date time conversion, due to the differences in convention between Bloomberg and OpenMAMA
	char dtBuf[50] = {0};
	mamaDateTime_getAsString(dt, dtBuf, sizeof(dtBuf));
	mama_log (MAMA_LOG_LEVEL_FINEST, "Convert Date/Time BLPAPI_DATATYPE_DATE->MAMA_FIELD_TYPE_TIME field %s = [%s]", fieldName.c_str(),dtBuf);
	return res;
}

template<>
mama_status MsgConverterItem_t<BLPAPI_DATATYPE_TIME,MAMA_FIELD_TYPE_TIME>::CreateMessage(mamaMsg &msg_, const std::string &fieldName, mama_fid_t fid, const BloombergLP::blpapi::Element &field) 
{
	mama_status res = mama_status();
	Datetime dateTime = field.getValueAsDatetime();
	mamaDateTime dt;

	mamaDateTime_create(&dt);
	BOOST_SCOPE_EXIT( (&dt) )
	{
		mamaDateTime_destroy(dt);
	} BOOST_SCOPE_EXIT_END;

	mamaDateTime_setToMidnightToday(dt,	NULL);
	mamaDateTime_setTime(dt, dateTime.hours(), dateTime.minutes(), dateTime.seconds(), dateTime.milliSeconds() * 1000);
	res = mamaMsg_addDateTime(msg_, fieldName.c_str(), fid, dt);

	//log as FINEST the date time conversion, due to the differences in convention between Bloomberg and OpenMAMA
	char dtBuf[50] = {0};
	mamaDateTime_getAsString(dt, dtBuf, sizeof(dtBuf));
	mama_log (MAMA_LOG_LEVEL_FINEST, "Convert Date/Time BLPAPI_DATATYPE_TIME->MAMA_FIELD_TYPE_TIME field %s = [%s]",fieldName.c_str(), dtBuf);
	return res;
}

template<>
mama_status MsgConverterItem_t<BLPAPI_DATATYPE_INT32,MAMA_FIELD_TYPE_I64>::CreateMessage(mamaMsg &msg_, const std::string &fieldName, mama_fid_t fid, const BloombergLP::blpapi::Element &field) 
{
	return mamaMsg_addI64(msg_, fieldName.c_str(), fid, (mama_i32_t)field.getValueAsInt32());
}

void BlpFieldDictionary::InitOperationOnTypesMap()
{
    /* In the first item it is easy to understand that:
     * The key is made of two values:
     * 1.  BLPAPI_DATATYPE_FLOAT32 - is the source type of the Bloomberg field
     * 2.  MAMA_FIELD_TYPE_F64 - is the destination type of the OpenMAMA field (in the message)
	 *
     * The value of the map is a functor that in here is made of an implementation of the function 
	 *     MsgConverterItem_t<BLPAPI_DATATYPE_FLOAT32,MAMA_FIELD_TYPE_F64> that creates a MAMA message
     *     (see mamaMsg_addF64...)
	 * Use MsgConverterItem_t<BLPAPI_DATATYPE_FLOAT32,MAMA_FIELD_TYPE_F64>::GetItem() to insert a new 
	 * item to the map. If the function is not implemented a link error will happen.
     */
	OperationOnTypesMap_.insert(MsgConverterItem_t<BLPAPI_DATATYPE_FLOAT32,MAMA_FIELD_TYPE_F64>::GetItem());
	OperationOnTypesMap_.insert(MsgConverterItem_t<BLPAPI_DATATYPE_DATE,MAMA_FIELD_TYPE_TIME>::GetItem());
	OperationOnTypesMap_.insert(MsgConverterItem_t<BLPAPI_DATATYPE_TIME,MAMA_FIELD_TYPE_TIME>::GetItem());
	OperationOnTypesMap_.insert(MsgConverterItem_t<BLPAPI_DATATYPE_INT32,MAMA_FIELD_TYPE_I64>::GetItem());	
}

BlpFieldDictionary::BlpFieldDictionary(std::string path, mama_fid_t nonTranslatedFieldFid_Start) 
	: NonTranslatedFieldFid_Start(nonTranslatedFieldFid_Start)  
	, NonTranslatedFieldFid_CurrentValue_(NonTranslatedFieldFid_Start)
{ 
	std::string actual_path=path;

	if (has_relative_path(path))
	{
		std::string abs_path = absolute_path(path);
		if (path_exists(abs_path))
		{
			actual_path = abs_path;
		}
		else
		{
			std::string env_prefix = blpenvironment_getVariable(ENV_VAR_WOMBAT_PATH);
			abs_path = complete_path(env_prefix, path);
			if (path_exists(abs_path))
			{
				actual_path = abs_path;
			}
		}
		mama_log (MAMA_LOG_LEVEL_NORMAL, "BlpFieldDictionary: The file [%s] is a relative path.",path.c_str());
	}
	
	try {
		using namespace boost;

		if (path_exists(actual_path))
		{
			ifstream in_dict_file(actual_path);
			BOOST_SCOPE_EXIT( (&in_dict_file) )
			{
				in_dict_file.close();
			} BOOST_SCOPE_EXIT_END;
			if (LoadPredefinedBlpFieldsDictionary(in_dict_file))
				mama_log (MAMA_LOG_LEVEL_FINER, "BlpFieldDictionary: Successfully loaded from file [%s].",actual_path.c_str());
			else
				mama_log (MAMA_LOG_LEVEL_WARN, "BlpFieldDictionary: Could not load Predefined Fields into Dictionary from the path [%s].",actual_path.c_str());
		}
		else
		{
			mama_log (MAMA_LOG_LEVEL_WARN, "BlpFieldDictionary: The file [%s] could not be found. Dictionary is not supported.",actual_path.c_str());
		}
	}
	catch(...)
	{
		mama_log (MAMA_LOG_LEVEL_WARN, "BlpFieldDictionary: Could not load Predefined Fields into Dictionary from file [%s].",actual_path.c_str());
		// Since We're not sure about the state of the map, clean the map and fall back to use the original fields names/types
	}
	InitOperationOnTypesMap(); 
}

BlpFieldDictionary::BlpFieldDictionary(mama_fid_t nonTranslatedFieldFid_Start)  
	: NonTranslatedFieldFid_Start(nonTranslatedFieldFid_Start)
	, NonTranslatedFieldFid_CurrentValue_(NonTranslatedFieldFid_Start)
{ 
	// This constructor should be run in case there is no valid dictionary file
	InitOperationOnTypesMap(); 
}

bool BlpFieldDictionary::FindField(const BlpToWombatFieldsMap_t::key_type &key, fieldValue_t &value) const
{
	auto cit = BlpToWombatFieldsMap_.find(key);
	if (cit != BlpToWombatFieldsMap_.end()) value = cit->second;
	return cit != BlpToWombatFieldsMap_.end();
}
bool BlpFieldDictionary::FindOperationForTypes(const typesKey_t &key, operationOnType_t &value) const
{
	auto cit = OperationOnTypesMap_.find(key);
	if (cit != OperationOnTypesMap_.end()) value = cit->second;
	return cit != OperationOnTypesMap_.end();
}

mama_fid_t BlpFieldDictionary::GetNonTranslatedFieldFid( const std::string &key )
{
	mama_fid_t value=0; 

	auto cit = NonTranslatedFieldFidMap_.find(key);
	if (cit != NonTranslatedFieldFidMap_.end()) 
		value = cit->second;
	else
	{
		auto res = NonTranslatedFieldFidMap_.insert(std::map<std::string, mama_fid_t>::value_type(key,NonTranslatedFieldFid_CurrentValue_));
		if (res.second)
			value = NonTranslatedFieldFid_CurrentValue_++;
		else
			mama_log (MAMA_LOG_LEVEL_WARN, "BlpFieldDictionary::NonTranslatedFieldFid key[%s] conflicts with other key[%s] for FID [%d]", key.c_str(), res.first->first.c_str(), res.first->second);
	}
	return value;
	
}
