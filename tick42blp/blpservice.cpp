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
#include <string.h>
#include "blpsession.h"
#include "blpservice.h" 
#include "blpsubscriptioneventhandler.h" 
#include "blpresponseeventhandler.h"
#include "blpitem.h"




BlpService::BlpService( string name )
	: name_(name)
{
	//thisPtr_ =  blpServicePtr(this);
}

BlpService::~BlpService(void)
{
	
}

bool BlpService::Initialize(SessionOptions opt)
{
	return false;
}

void BlpService::Shutdown()
{

	if(eventHandler_)
	{
		delete eventHandler_;
		eventHandler_ = 0;
	}

	if(session_)
	{
		session_->Stop();
		delete session_;
		session_ = 0;
	}

	//thisPtr_.reset();

}

bool BlpService::Start()
{
	if(!session_)
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "blpService::Start: Attempt to start blpSession before initialize");
		return false;
	}

	// TODO should check state of session here.... is it already started?
	return session_->Start(options_, eventHandler_);

}

bool BlpService::Stop()
{
	eventHandler_->isShutdown_=true;
	if(!session_)
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "blpService::Stop: Attempt to stop blpSession before initialize");
		return false;
	}

	
	return session_->Stop();
}

BlpSession * BlpService::GetSession()
{
	return session_;
}


////////////////////
//
// blpMktDataService
//
////////////////////

blpServicePtr_t BlpMktDataService::CreateService( const string & name )
{
	return blpServicePtr_t(new BlpMktDataService(name));

}

BlpMktDataService::BlpMktDataService( string name )
	:BlpService(name), delayed_(false)
{
}


BlpMktDataService::~BlpMktDataService(void)
{
}





// Set up internal state and create the blp session



bool BlpMktDataService::Initialize(SessionOptions opt)
{

	eventHandler_ = new BlpSubscriptionEventHandler();
	session_ = new BlpMktDataSession();
	options_ = opt;


	return true;
}


void BlpMktDataService::SetDelayed( bool delayed /*= true*/ )
{
	delayed_= delayed;
}

blpItemPtr_t BlpMktDataService::CreateItem( const string & name, BlpSubscription * pOwner )
{
	blpItemPtr_t p = BlpMktDataItem::CreateItem(name, delayed_);

	p->Initialize( BlpService::shared_from_this(), pOwner);


	return p;
}



////////////////////
//
// blpRefDataService
//
////////////////////

blpServicePtr_t BlpRefDataService::CreateService( const string & name )
{
	return blpServicePtr_t(new BlpRefDataService(name));

}

BlpRefDataService::BlpRefDataService( string name )
	:BlpService(name)
{
}


BlpRefDataService::~BlpRefDataService(void)
{
}





// Set up internal state and create the blp session



bool BlpRefDataService::Initialize(SessionOptions opt)
{

	eventHandler_ = new BlpResponseEventHandler();
	session_ = new BlpRefDataSession();
	options_ = opt;


	return true;
}




blpItemPtr_t BlpRefDataService::CreateItem( const string & name, BlpSubscription * pOwner )
{
	blpItemPtr_t p = BlpRefDataItem::CreateItem(name);

	p->Initialize( BlpService::shared_from_this(), pOwner);


	return p;
}