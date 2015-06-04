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

#ifndef __BLPTHREADUTIL_H__
#define __BLPTHREADUTIL_H__

#include <wombat/wInterlocked.h>
#include <mama/log.h>
#include "wlock.h"
#include <new>
#include <stdexcept>

class interlockedInt_fail_init : public std::runtime_error
{
public:
	interlockedInt_fail_init() : std::runtime_error("Failed to initialize interlocked variable") {}
	virtual ~interlockedInt_fail_init() throw() {}
};

/**
 * A thin wrapper over the wInterlockedInt
 */
//template <int defaultValParam=0>
class interlockedInt_t
{
	wInterlockedInt interlockedInt_;
public:
	interlockedInt_t() : interlockedInt_(0/*defaultValParam*/)  // first clean the value of the interlocked variable
	{
		if(!initialize())
			throw interlockedInt_fail_init();
	}

	~interlockedInt_t()
	{
		destory();
	}
	/**
	 * This function will destroy the internal interlocked integer value.
	 *
	 * @return true on success.
	 */
	bool destory() {return (wInterlocked_destroy(&interlockedInt_) == 0);}
	/**
	 * This function will initialize the internal interlocked integer value.
	 *
	 * @return true on success.
	 */
	bool initialize()
	{ 
		return  (wInterlocked_initialize(&interlockedInt_) == 0);	
	}
	/**
	 * This function will atomically decrement the internal interlocked integer value.
	 *
	 * @return The decremented integer.
	 */
	inline int decrement() { return wInterlocked_decrement(&interlockedInt_);}
	/**
	 * This function will atomically increment the internal interlocked integer value.
	 *
	 * @return The decremented integer.
	 */
	inline int increment() { return wInterlocked_increment(&interlockedInt_); }
	/**
	 * This function will return the value of the interlocked variable.
	 *
	 * @return The value itself.
	 */
	inline int read() {return wInterlocked_read(&interlockedInt_);}
	/**
	 * This function will atomically set the internal interlocked 32-bit integer value.
	 *
	 * @param[in] newValue The new value to set.
	 * @return The updated integer.
	 */
	inline int set(int newValue) { return wInterlocked_set(newValue, &interlockedInt_); }
private:
	//InterlockedIntVariable variable cannot change or copy ownership of it's own internal variable!
	interlockedInt_t(const interlockedInt_t &rhs);
	interlockedInt_t &operator =(const interlockedInt_t &rhs);
};

class semaphore_fail_init : public std::runtime_error
{
public:
	semaphore_fail_init() : std::runtime_error("Failed to initialize semaphore variable") {}
	virtual ~semaphore_fail_init() throw() {}
};

/**
* semaphore_t is a thin wrapper over the POSIX like semaphore as implemented in Wombat library
*/
class semaphore_t
{
	mutable wsem_t semaphore_; // a mutex is always mutable
public:
	semaphore_t() {if (!init (0, 1)) throw semaphore_fail_init();}
	~semaphore_t() {destroy();}
	/*dummy relates to POSIX pshared parameter that doesn't work on linux systems and therefore is not supported here.*/
	semaphore_t(int dummy, int count) {if (!init(dummy, count)) throw semaphore_fail_init();}
	inline bool init(
		int dummy, int count) { return (wsem_init (&semaphore_, dummy, count) == 0);
	}
	inline bool destroy() { return (wsem_destroy (&semaphore_) == 0);}
	inline bool post() { 
		return (wsem_post (&semaphore_) ==0); 
	} //TODO
	inline bool wait() { return (wsem_wait (&semaphore_)==0); }
	inline bool timedwait(unsigned int ts) {return (wsem_timedwait (&semaphore_, ts) ==0);}
	inline bool trywait() {return (wsem_trywait (&semaphore_)==0);}
	inline bool getvalue(int* items) {return  (wsem_getvalue (&semaphore_, items)==0);}
private:
	//semaphore_t variable cannot change or copy ownership of it's own internal mutex!
	semaphore_t(const semaphore_t &rhs);
	semaphore_t&operator =(const semaphore_t &rhs);
};

#endif //__BLPTHREADUTIL_H__