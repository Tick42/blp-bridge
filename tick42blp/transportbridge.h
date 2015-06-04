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

#ifndef BLP_TRANPSORT_BRIDGE_H
#define BLP_TRANPSORT_BRIDGE_H

#include "blpdefs.h"

#include <vector>

#include <memory>
#include "blpdictionaryutils.h"

class BlpSubscription;
class BlpSubscriptionEventHandler;



//
// Provides a wrapper around the tick42 blp classes
//

class BlpTransportBridge
{
public:

	BLPExpDll BlpTransportBridge(const std::string& host, int port);

	~BlpTransportBridge();


	// set the mama transport handle
	void SetTransport(mamaTransport transport)
	{
		if (!stopped_)
			transport_ = transport;
		else
			transport_=0;
	}


	//get  MAMA transport handle
		mamaTransport GetTransport() const
	{
		if (!stopped_)
			return transport_;
		else
			return 0;
	}

	
	 // Starts BLP Session and services
	BLPExpDll mama_status Start();

	
	 // Stops BLP Session.
	BLPExpDll mama_status Stop();


public:
	blpServicePtr_t GetService(const string & name );

public:
	static BlpTransportBridge* GetTransport(mamaTransport transport);

private:
	void ProcessSubscriptions();

private:
	mamaTransport transport_;

	// map service names to services so we can route subscriptions to the right service
	typedef std::map<string, blpServicePtr_t> ServicesMap_t;
	ServicesMap_t servicesMap_;

	bool started_;
	bool stopped_;
public:
	 std::unique_ptr<BlpFieldDictionary> BlpWombatDictionary_; // map of predefined Bloomberg fields to correspondent WOMBAT(OpenMAMA) fields (names/types)
	 bool dictIncludeUnmappedFields_;
};

#endif
