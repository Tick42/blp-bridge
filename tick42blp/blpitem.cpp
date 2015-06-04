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

#include "blpitem.h"
#include "blputils.h"
#include <boost/assign/list_of.hpp>
#include "blpservice.h"
#include "blpsession.h"
#include "blpsubscription.h"
#include <boost/tokenizer.hpp>
//#include "sub.h"



const std::set<std::string> BlpMktDataItem::DEFAULT_MKTDATA_FIELDS = boost::assign::list_of ("LAST_PRICE") 
	("LAST_SIZE")
	("BID")
	("BID_SIZE")
	("ASK")
	("ASK_SIZE")
	("HIGH")
	("LOW")
	("VOLUME") 
	("TIME");

const std::set<std::string> BlpRefDataItem::DEFAULT_REFDATA_FIELDS = boost::assign::list_of ("NAME")
	("PX_YEST_CLOSE")
	("PX_CLOSE_DT")
	("PX_BID")
	("PX_ASK")
	("PX_LAST")
	("ID_ISIN")
	("VOLUME_AVG_5D")
	("VOLUME_AVG_10D");

BlpItem::BlpItem(const string & name)
	:name_(name), isValid_(false), filterFields_(false)
{
}


BlpItem::~BlpItem(void)
{
}


bool BlpItem::ParseName(const string & name)
{


	// The generic bloomberg name is formed as 
	//
	// secname <ticker type> [fields] - in this version the ticker type is ignored
	//
	// Fields is list of fields

	//
	// secname includes an asset class (yellow-key) which will be the last whitespace delimited token in
	// secname.
	// 

	// before we run the state machine parser just validate pairing of ticker and field list delimiters
	// as this makes the SM much simpler

	bool failed = false;
	std::string secString = name;

	// make sure the <> match
	if(::strchr(secString.c_str(),'<')!= 0)
	{
		// has left so better have right
		if(::strchr(secString.c_str(),'>') == 0)
		{
			failed = true;
		}
	}

	// ... and the []
	if(!failed)
	{
		if(::strchr(secString.c_str(),'[')!= 0)
		{
			// has left so better have right
			if(::strchr(secString.c_str(),']') == 0)
			{
				failed = true;
			}
		}
	}

	if(failed)
	{
		return false;
	}



	// copy the string into a  buffer for tokenisation
	char * pBuff = new char[secString.size()+1];

	// and allocate additional buffers for results of parse
	// make them all the same size
	char * pSecNameBuff = new char[secString.size()+1];
	char * pTickerBuff = new char[secString.size()+1];
	char * pFieldListBuff = new char[secString.size()+1];

	::strcpy(pBuff, secString.c_str());

	bool hasTicker = false;
	bool hasFieldList = false;


	// state machine parser
	enum state{secName, ticker, fieldList,done};
	char* pNextChar = pBuff;
	char* pNextSecName = pSecNameBuff;
	char* pNextTicker = pTickerBuff;
	char* pNextFieldList = pFieldListBuff;

	state curState = secName;
	// get the sec name.... its up until a , < [ or ,
	while(*pNextChar)
	{
		switch(curState)
		{
		case secName:
			{
				if(*pNextChar == '<' )
				{
					*pNextSecName = 0;
					curState = ticker;
				}
				else if(*pNextChar == ',')
				{
					*pNextSecName = 0;
					curState = fieldList;
				}
				else if(*pNextChar == '[')
				{
					*pNextSecName = 0;
					curState = fieldList;
				}
				else
				{
					*pNextSecName++ = *pNextChar;
				}
				break;
			}
		case ticker:
			{
				if(*pNextChar == '>')
				{
					// found end of ticker
					curState = fieldList;
					*pNextTicker = 0;
				}
				else
				{
					*pNextTicker++ = *pNextChar;
				}
				break;
			}
		case fieldList:
			{
				if(*pNextChar == '[')
				{
					//was because there was a comma

				}
				else if (*pNextChar == ']')
				{
					*pNextFieldList = 0;
					curState = done;
				}
				else
				{
					*pNextFieldList++ = *pNextChar;
				}
				break;
			}
		case done:
			break;
		}

		pNextChar++;
	}

	//just make sure all the buffers are properly nulled
	*pNextSecName = 0;
	*pNextFieldList = 0;
	*pNextTicker = 0;


	// stringize the char buffers and clean up
	string strSecName = pSecNameBuff;
	security_ = trim(strSecName);
	delete[] pSecNameBuff;

	//m_TickerType = pTickerBuff;
	//m_TickerType = trim(m_TickerType);
	delete[] pTickerBuff;

	fieldList_.assign(pFieldListBuff);
	fieldList_ = trim(fieldList_);
	delete [] pFieldListBuff;

	delete [] pBuff;

	return true;

}

// parse the ',' separated list of field names in fieldList into the set fields_
bool BlpItem::ParseFieldList( const string & fieldList )
{

	// tokenize on "," and separate out field names 

	if(trim(fieldList).size()== 0) 
		return false; // nothing to work with;

	using namespace boost;

	typedef tokenizer<boost::char_separator<char> > tokenizer;
	char_separator<char> sep(",");
	tokenizer tokens(fieldList, sep);

	string tok;
	for (tokenizer::const_iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
	{
		tok = *tok_iter;
		trim(tok); 
		fields_.insert(tok);
	}

	return true;
}

bool BlpItem::Initialize( blpServicePtr_t service, BlpSubscription * pOwner )
{
	service_ = service;
	owner_ = pOwner;
	owner_->AddRef();
	return true;
}

string BlpItem::SecurityName()
{
	return security_;
}

FieldSet_t BlpItem::Fields()
{
	return fields_;
}


//
// blpMktDataItem
//
BlpMktDataItem::BlpMktDataItem(const string & name, bool delayed)
	:BlpItem(name), delayed_(delayed)
{
}


BlpMktDataItem::~BlpMktDataItem(void)
{
	if (owner_)
	{
		owner_->Release();
		owner_ = 0;
	}
}

bool BlpMktDataItem::Initialize(blpServicePtr_t service, BlpSubscription * pOwner )
{
	// parse the name and work out what fields we want to use.

	if (!ParseName(name_))
	{
		isValid_ = false;
		return false;
	}

	// set up fields
	if(fieldList_.empty())
	{
		// use the defualt field list
		fields_ = DEFAULT_MKTDATA_FIELDS;
	}
	else
	{
		ParseFieldList(fieldList_);
		// we have received a field list qualifying the security name so we choose to filter the output fields on this list
		filterFields_ = true;
	}
	

	return BlpItem::Initialize(service, pOwner);
	
}


// Start up the blp subscription
bool BlpMktDataItem::Open()
{
	 BlpSession* pSession = service_->GetSession();

	 if (pSession && pSession->hasSession())
	 {
		 if (pSession->OpenItem(BlpItem::shared_from_this()))
			 return true;
	 }
	 else
		 mama_log(MAMA_LOG_LEVEL_WARN, "BlpRefDataItem: Trying to open item [%s] with no valid session", this->name_.c_str());
	 return false;
}

void BlpMktDataItem::OnUpdate( blpapi::Message& msg )
{
	owner_->OnUpdate(msg, fields_, filterFields_);

}

void BlpMktDataItem::OnInvalidSecurity( blpapi::Message& msg )
{
	owner_->OnInvalidSec(msg);
}

bool BlpMktDataItem::Close()
{
	BlpSession* pSession = service_->GetSession();


	if(pSession && pSession->CloseItem(BlpItem::shared_from_this()))
		return true;
	return false;
}

blpItemPtr_t BlpMktDataItem::CreateItem( const string & name, bool delayed)
{
		return blpItemPtr_t(new BlpMktDataItem(name, delayed));
}


BlpRefDataItem::BlpRefDataItem(const string & name)
	:BlpItem(name)
{
}


BlpRefDataItem::~BlpRefDataItem(void)
{
}

void BlpRefDataItem::OnUpdate(blpapi::Message& msg)
{
	owner_->ProcessResponse(msg, fields_, filterFields_);
}


// Start up the blp request
bool BlpRefDataItem::Open()
{
	BlpSession* pSession = service_->GetSession();


	if (pSession && pSession->hasSession())
	{
		if (pSession->OpenItem(BlpItem::shared_from_this()))
			return true;
	}
	else
		mama_log(MAMA_LOG_LEVEL_WARN, "BlpRefDataItem: Trying to open item [%s] with no valid session", this->name_.c_str());
	return false;
}

bool BlpRefDataItem::Initialize(blpServicePtr_t service, BlpSubscription * pOwner )
{
	// parse the name and work out what fields we want to use.

	if (!ParseName(name_))
	{
		isValid_ = false;
		return false;
	}

	// set up fields
	if(fieldList_.empty())
	{
		// use the defualt field list
		fields_ = DEFAULT_REFDATA_FIELDS;
	}
	else
	{
		ParseFieldList(fieldList_);
		// for ref data we will always get just the fields we ask for so no need to filter
		filterFields_ = false;
	}


	return BlpItem::Initialize(service, pOwner);

}

bool BlpRefDataItem::Close()
{
	// for a ref data item there is nothing to do;
	return true;
}

blpItemPtr_t BlpRefDataItem::CreateItem( const string & name )
{
	return blpItemPtr_t(new BlpRefDataItem(name));
}
