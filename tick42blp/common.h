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

#ifndef __COMMON_H__
#define __COMMON_H__

//#include <string>
#include <map>
#include <set>

//#include <vector>
//#include <list>
//#include <queue>
using namespace std;

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//#include <boost/algorithm/string.hpp>

#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_element.h>
#include <blpapi_name.h>
#include <blpapi_request.h>
#include <blpapi_subscriptionlist.h>
#include <blpapi_defs.h>
#include <blpapi_exception.h>
#include <blpapi_session.h>
using namespace BloombergLP;
using namespace blpapi;

#include "blpdefs.h"

typedef std::set<string> FieldSet_t;

// forward ref and define shared ptr types
class BlpService;
typedef boost::shared_ptr<BlpService> blpServicePtr_t;

class BlpItem;
typedef boost::shared_ptr<BlpItem> blpItemPtr_t;
#include <mama/mama.h>

#endif //__COMMON_H__