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

#ifndef __BLPSERVICE_H__
#define __BLPSERVICE_H__

#include "blpsubscriptioneventhandler.h"
// Base class for service.
//
// The tick42 service is keyed to the OM source and encapsulates the session configurations and event handling for the different blp services


class BlpSession;
class BlpSubscription;

class BlpService : public boost::enable_shared_from_this<BlpService>
{
protected:
	BlpService(string name);
	virtual ~BlpService(void);

private:
	string name_;

protected:

	BlpSession * session_;

	Tick42BlpEventHandlerBase * eventHandler_;

	blpapi::SessionOptions options_;

public:
	virtual bool Initialize(SessionOptions opt);

	virtual void Shutdown();

	virtual bool Start();
	virtual bool Stop();



	string  Name()
	{
		return name_;
	}

	virtual blpItemPtr_t CreateItem(const string & name, BlpSubscription * pOwner) = 0;

	// TODO  refactor this as boost shared_ptr
	BlpSession * GetSession();
};


// Service class for handling subscription requests on the blp MktData service

class BlpMktDataService : public BlpService
{
public:

	static blpServicePtr_t CreateService(const string & name);
	BlpMktDataService(string name);
	virtual ~BlpMktDataService(void);
	// Set up internal state and create the blp session
	virtual bool Initialize(SessionOptions opt);

	virtual void SetDelayed(bool delayed = true);

	virtual blpItemPtr_t CreateItem(const string & name, BlpSubscription * pOwner);



private:
	bool delayed_;
};


// Service class for handling subscription requests on the blp RefData service

class BlpRefDataService : public BlpService
{
public:

	static blpServicePtr_t CreateService(const string & name);

	BlpRefDataService(string name);
	virtual ~BlpRefDataService(void);
	// Set up internal state and create the blp session
	virtual bool Initialize(SessionOptions opt);

	virtual blpItemPtr_t CreateItem(const string & name, BlpSubscription * pOwner);



private:
	
};




#endif //__BLPSERVICE_H__