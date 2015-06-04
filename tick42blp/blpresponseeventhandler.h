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

#ifndef __BLPRESPONSEEVENTHANDLER_H__
#define __BLPRESPONSEEVENTHANDLER_H__

// Handler for Bloomberg response events. These are one-off replies to requests for reference data
#include "blpsubscriptioneventhandler.h"
class Tick42BlpEventHandlerBase;
class BlpResponseEventHandler : public Tick42BlpEventHandlerBase
{
public:
	BlpResponseEventHandler(void);
	~BlpResponseEventHandler(void);

	// implement override from blp base class
	bool processEvent(const Event &event, Session *session);

private:
	// event type specific handlers

	bool ProcessResponseEvent(const Event &event);

	bool ProcessMiscEvents(const Event &event);

	bool ProcessSessionStatusEvent(const Event & event, Session * session);


};

#endif //__BLPRESPONSEEVENTHANDLER_H__