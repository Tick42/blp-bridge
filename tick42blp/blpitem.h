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

#ifndef __BLPITEM_H__
#define __BLPITEM_H__


// BlpItem and its derived classed provide a place where we can implement functionality specific to the request type. 
//
// In this implementation we differentiate between real time mktdata requests and reference data requests. In both cases we implement setup of default fields.
//
// This also hooks the updates returned by Bloomberg to provide the ability to do request type specific processing - we dont make use of this inthis version







class BlpSubscription;

class BlpItem : public boost::enable_shared_from_this<BlpItem>
{
public:
	BlpItem(const string & name);
	virtual ~BlpItem(void);

	virtual bool Initialize(blpServicePtr_t service, BlpSubscription * pOwner);

	virtual bool Open()
	{return false;}
	virtual bool Close(){return false;}

	virtual void OnUpdate(blpapi::Message& msg)
	{
		return;
	}

	virtual void OnInvalidSecurity(blpapi::Message& msg)
	{
		return;
	}

	// has the object been created successfully
	virtual bool IsValid() const
	{
		return isValid_;
	}

	string SecurityName();

	// TODO typedef the field set
	std::set<std::string> Fields();

protected:
	string name_;

	string security_;
	string fieldList_;

	FieldSet_t fields_;
	bool filterFields_;

	bool isValid_;

	bool ParseName(const string & name);
	bool ParseFieldList(const string &);

	blpServicePtr_t service_;

	BlpSubscription * owner_;
};



class BlpMktDataItem :
	public BlpItem
{
public:

	static blpItemPtr_t CreateItem(const string & name, bool delayed = false);

	BlpMktDataItem(const string & name, bool delayed = false);

	virtual ~BlpMktDataItem(void);

	virtual bool Initialize(blpServicePtr_t service, BlpSubscription * pOwner);

	virtual bool Open();
	virtual bool Close();

	virtual void OnUpdate(blpapi::Message& msg);
	virtual void OnInvalidSecurity(blpapi::Message& msg);


	static const std::set<std::string> DEFAULT_MKTDATA_FIELDS;

	bool Isdelayed() const
	{
		return delayed_;
	}

private:
	bool delayed_;

};

class BlpRefDataItem : public BlpItem
{
public:

	static blpItemPtr_t CreateItem(const string & name);
	BlpRefDataItem(const string & name);
	~BlpRefDataItem(void);

	virtual bool Initialize(blpServicePtr_t service, BlpSubscription * pOwner);


	virtual void OnUpdate(blpapi::Message & msg);
	virtual bool Open();

	virtual bool Close();

	static const std::set<std::string> DEFAULT_REFDATA_FIELDS;
};

#endif //__BLPITEM_H__