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

#ifndef BLP_VALUE_TYPE_H
#define BLP_VALUE_TYPE_H

#include <boost/variant.hpp>

struct mama_datetime
{

	uint64_t dt;
	friend std::ostream & operator<<(std::ostream& out, const mama_datetime & val);
	
};





typedef boost::variant<int8_t, 
					   uint8_t, 
					   int16_t, 
					   uint16_t, 
					   int32_t, 
					   uint32_t, 
					   int64_t, 
					   uint64_t, 
					   float, 
					   double,
					   mama_datetime,
					   std::string> ValueType_t;


#endif