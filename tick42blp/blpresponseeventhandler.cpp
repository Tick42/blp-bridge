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

#include <boost/scope_exit.hpp>

#include "blpresponseeventhandler.h"

#include "blpsystemglobal.h"
#include "blpitem.h"
#include "blptimeutils.h"
//#include "sub.h"

#include <time.h>

BlpResponseEventHandler::BlpResponseEventHandler(void) :Tick42BlpEventHandlerBase ()
{
}


BlpResponseEventHandler::~BlpResponseEventHandler(void)
{
}

bool BlpResponseEventHandler::processEvent( const Event &event, Session *session )
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

	try
	{
		switch (event.eventType())
		{
		case Event::PARTIAL_RESPONSE:
			// TODO: Implement PARTIAL_RESPONSE

		case Event::RESPONSE:
			return ProcessResponseEvent(event);
			break;

			// Note that we don t expect to see any subscription events in a response event handler
			// they will be logged as masc events if they occur

		default:
			return ProcessMiscEvents(event);
			break;
		}
	} 
	catch (Exception&)
	{
		mama_log(MAMA_LOG_LEVEL_WARN,"Caught exception in response event handler");
	}

	return false;

}

bool BlpResponseEventHandler::ProcessResponseEvent( const Event &event )
{

	MessageIterator msgIter(event);
	while (msgIter.next()) 
	{
		Message msg = msgIter.message();
		BlpItem* pItem = reinterpret_cast<BlpItem*>(msg.correlationId().asPointer());

		if (pItem)
		{
			pItem->OnUpdate(msg);
		}
	}

	return true;

}



bool BlpResponseEventHandler::ProcessSessionStatusEvent( const Event & event, Session * session )
{
	return true;
}

// just log any other events we see
bool BlpResponseEventHandler::ProcessMiscEvents(const Event &event)
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
