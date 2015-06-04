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
#ifndef BLP_BRIDGE_IMPL_H
#define BLP_BRIDGE_IMPL_H

#include <boost/shared_ptr.hpp>

class BlpTransportBridge;


// Simple wrapper around a BlpTranspoortBridge shared_ptr

class BlpBridgeImpl
{
public:
	
	void setTransportBridge(const boost::shared_ptr<BlpTransportBridge>& transportBridge = boost::shared_ptr<BlpTransportBridge>())
	{
		transportBridge_ = transportBridge;
	}

	boost::shared_ptr<BlpTransportBridge> getTransportBridge() const
	{
		return transportBridge_;
	}

	bool hasTransportBridge() const
	{
		return transportBridge_ != 0;
	}

private:
	boost::shared_ptr<BlpTransportBridge> transportBridge_;
};

#endif