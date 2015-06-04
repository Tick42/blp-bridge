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
#ifndef __BLPSESSION_H__
#define __BLPSESSION_H__

#include "blpapi_service.h"

// the BlpSession class provides a simple wrapper around the blpapi session object
//
// The derived classes deal with the differences between subscribing to market data and requesting reference data. We normalise these to a common Open / Close model

class BlpSession
{
public:

	// TODO may not need this
	typedef enum
	{
		eSessionUnkown = 0,
		eSessionMktData, 
		eSessionRefData

	} SessionType;

public:
	BlpSession();
	~BlpSession(void);


	virtual bool Start(blpapi::SessionOptions & options, blpapi::EventHandler * pEvtHandler) = 0;
	virtual bool Stop() = 0;
	

	virtual bool OpenItem(blpItemPtr_t item) = 0;
	virtual bool CloseItem(blpItemPtr_t item) = 0;
	bool hasSession() {return blpapiSession_ != 0;}


protected:



	string name_;	// track whether the blp api session is started 
	bool started_;
	bool stopped_;
	// The blpapi service
	blpapi::Service blpapiService_;
	// the blpapi session
	blpapi::Session * blpapiSession_;

};

class BlpMktDataSession : public BlpSession
{
public:
	BlpMktDataSession();

	virtual ~BlpMktDataSession();

	virtual bool Start(blpapi::SessionOptions & options, blpapi::EventHandler * pEvtHandler);
	virtual bool Stop();


	virtual bool OpenItem(blpItemPtr_t item);
	virtual bool CloseItem(blpItemPtr_t item);

	// dump the //blp/mktdata cshema to a file
	void DumpSchema(char * filename);

private:

};

class BlpRefDataSession : public BlpSession
{
public:
	BlpRefDataSession();

	virtual ~BlpRefDataSession();

	virtual bool Start(blpapi::SessionOptions & options, blpapi::EventHandler * pEvtHandler);
	virtual bool Stop();


	virtual bool OpenItem(blpItemPtr_t item);
	virtual bool CloseItem(blpItemPtr_t item);


private:

};

#endif //__BLPSESSION_H__