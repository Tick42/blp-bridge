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

#ifndef __BLPSYSTEMGLOBAL_H__
#define __BLPSYSTEMGLOBAL_H__

#include "blpthreadutil.h"
#include <memory>
#include <wlock.h>

/* This class holds all the globals that are needed to be used among multiple threads.
 * Most likely the globals below will be thread synchronization objects
 * The class should hold only static independent objects, that are able to construct and 
 * destruct safely out of the application/library context
 */
class BlpSystemGlobal
{
public:
	static interlockedInt_t gSystemShutdown; //interlocked system state flag which states that system is shutting down. Used along with gMsgCbLock.
	static semaphore_t gMsgCbLock; // semaphore that states whether any message is processed now through any Bloomberg subscription. When locked on system close it'll never be released.
};
#endif //__BLPSYSTEMGLOBAL_H__