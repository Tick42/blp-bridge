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
#include "blpsubscriptioneventhandler.h"

#include "blpitem.h"

//#include "sub.h"

#include <time.h>
#include "blpsystemglobal.h"
#include <iostream>
#include <boost/scope_exit.hpp>
#include "blptimeutils.h"

Name SUBSCRIPTIONTERMINATED("SubscriptionTerminated");


BlpSubscriptionEventHandler::BlpSubscriptionEventHandler(void) 
	:Tick42BlpEventHandlerBase ()
{
}


BlpSubscriptionEventHandler::~BlpSubscriptionEventHandler(void)
{
}



// Process the base blp event
// exctract the type and call the appropriate handler

bool BlpSubscriptionEventHandler::processEvent(const Event &event, Session *session)
{
	// Try to acquire lock that describes that the bridge is processing an event right now
	// If the system is shutting down do not process the event 
	if(!BlpSystemGlobal::gMsgCbLock.trywait())
	{
		if (BlpSystemGlobal::gSystemShutdown.read() != 0)
			return true; //fake the fact that the event is handled
	}
	int dummy=0;
	using namespace boost;
	BOOST_SCOPE_EXIT( (&dummy) )
	{
		BlpSystemGlobal::gMsgCbLock.post();
	} BOOST_SCOPE_EXIT_END;

	try
	{
		switch (event.eventType())
		{
			// Note that we don t expect to see any response or partial response events in a subscription event handler
			// If they do occur they will be logged as misc events
		case Event::SUBSCRIPTION_DATA:
			return ProcessSubscriptionDataEvent(event);
			
			break;
		case Event::SUBSCRIPTION_STATUS:
			return ProcessSubscriptionStatus(event);
			break;
		case Event::SESSION_STATUS:
			return ProcessSessionStatusEvent(event, session);
			break;
		default:
			return ProcessMiscEvents(event);
			break;
		}
	} 
	catch (Exception&)
	{
		mama_log(MAMA_LOG_LEVEL_WARN,"Caught exception in subscription event handler");

	}
	catch(...)
	{	
		mama_log(MAMA_LOG_LEVEL_WARN,"Caught exception in subscription event handler");
	}
	return false;
}

bool BlpSubscriptionEventHandler::ProcessSubscriptionDataEvent(const Event &event)
{
	// Try to acquire lock that describes that the bridge is processing an event right now
	// If the system is shutting down do not process the event 
	if(!BlpSystemGlobal::gMsgCbLock.trywait())
	{
		if (BlpSystemGlobal::gSystemShutdown.read() != 0)
			return true;
	}
	int dummy=0;
	using namespace boost;
	BOOST_SCOPE_EXIT( (&dummy) )
	{
		BlpSystemGlobal::gMsgCbLock.post();
	} BOOST_SCOPE_EXIT_END;

	BLP_PRINT(stdout, "\nProcessing SUBSCRIPTION_DATA\n");

	MessageIterator msgIter(event);
	while (msgIter.next()) 
	{
		Message msg = msgIter.message();
		BLP_PRINT(stdout, "%s: %s\n", GetTimeStamp().c_str(), msg.messageType().string());

		BlpItem * item = reinterpret_cast<BlpItem*>(msg.correlationId().asPointer());

		if (item)
		{
			item->OnUpdate(msg);
		}

	}
	return true;
}

// just log any other events we see
bool BlpSubscriptionEventHandler::ProcessMiscEvents(const Event &event)
{
	MessageIterator msgIter(event);
	while (msgIter.next())
	{
		Message msg = msgIter.message();
		BLP_PRINT(stdout, "%s: %s\n",
			GetTimeStamp().c_str(), msg.messageType().string());
	}
	return true;
}

bool BlpSubscriptionEventHandler::ProcessSubscriptionStatus(const Event &event)
{
	// Try to acquire lock that describes that the bridge is processing an event right now
	// If the system is shutting down do not process the event 
	if(!BlpSystemGlobal::gMsgCbLock.trywait())
	{
		if (BlpSystemGlobal::gSystemShutdown.read() != 0)
			return true;
	}
	int dummy=0;
	using namespace boost;
	BOOST_SCOPE_EXIT( (&dummy) )
	{
		BlpSystemGlobal::gMsgCbLock.post();
	} BOOST_SCOPE_EXIT_END;

	
	BLP_PRINT(stdout, "Processing SUBSCRIPTION_STATUS\n");
	MessageIterator msgIter(event);
	while (msgIter.next())
	{
		Message msg = msgIter.message();

		if(msg.messageType()== SUBSCRIPTIONTERMINATED)
		{
			// we dont do anything with this
			continue;
		}

		BlpItem* pItem = reinterpret_cast<BlpItem*>(msg.correlationId().asPointer());	


		BLP_PRINT(stdout, "%s: %s - %s\n", GetTimeStamp().c_str(),
			pItem->SecurityName().c_str(),
			msg.messageType().string());


		// only case we handle here is invalid security
		if (strcmp(msg.messageType().string(), "SubscriptionFailure") == 0)
		{
			BlpItem* pItem = reinterpret_cast<BlpItem*>(msg.correlationId().asPointer());

			if (pItem)
			{
				pItem->OnInvalidSecurity(msg);
			}
		}
	}

	
	return true;
}

bool BlpSubscriptionEventHandler::ProcessSessionStatusEvent( const Event & event, Session * session )
{
	MessageIterator msgIter(event);
	while (msgIter.next())
	{
		Message msg = msgIter.message();

		BLP_PRINT(stdout, "Session Status Event %s: %s\n",
			GetTimeStamp().c_str(), msg.messageType().string());
	}

	return true;

}


