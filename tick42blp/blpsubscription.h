/** BlpBridge: The Bloomberg v3 API Bridge for OpenMama
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

#include <string>
#include <set>

#include <blpapi_session.h>
#include "blpthreadutil.h"

class BlpTransportBridge;


//BlpSubscription subscription class represents a single OpenMAMA subscription. The methods map the OM programming model onto the tick42 blp programming model, so for example the OM subscription_create calls the Subscribe
// method which will result in an BlpItem being created on a BlpService and opened
//


class BlpSubscription
{
public:
	/*
	 * Constructor
	 *
	 * @param[in] source Source id
	 * @param[in] symbol Symbol string
	 * @param[in] transport MAMA transport handle
	 * @param[in] queue MAMA queue handle
	 * @param[in] callback MAMA message callback
	 * @param[in] subscription MAMA subscription handle
	 * @param[in] closure Closure pointer
	 */
	BLPExpDll BlpSubscription(const std::string& source,
					const std::string& symbol,
					mamaTransport transport,
					mamaQueue queue,
					mamaMsgCallbacks callback,
					mamaSubscription subscription,
					void* closure);

	/*
	 * Destructor
	 */
	BLPExpDll ~BlpSubscription();

	/*
	 * Source setter method
	 *
	 * @param[in] source Source id
	 */
	void SetSource(const std::string& source) 
	{
		source_ = source;
	}

	/*
	 * Symbol setter method
	 *
	 * @param[in] symbol Symbol string
	 */
	void SetSymbol(const std::string& symbol)
	{
		symbol_ = symbol;
	}

	/*
	 * MAMA message callback setter method
	 *
	 * @param[in] callback MAMA message callback
	 */
	void SetMsgCallback(mamaMsgCallbacks callback)
	{
		callback_ = callback;
	}

	/*
	 * Symbol setter method
	 *
	 * @param[in] symbol Symbol string
	 */
	void SetSubscription(mamaSubscription subscription)
	{
		subscription_ = subscription;
	}

	/*
	 * MAMA queue setter method
	 *
	 * @param[in] queue MAMA queue handle
	 */
	void SetQueue(mamaQueue queue)
	{
		queue_ = queue;
	}

	/*
	 * MAMA transport setter method
	 *
	 * @param[in] transport MAMA transport handle
	 */
	void SetTransport(mamaTransport transport)
	{
		transport_ = transport;
	}

	/*
	 * Closure setter method
	 *
	 * @param[in] closure Closure pointer
	 */
	void SetClosure(void* closure)
	{
		closure_ = closure;
	}

	/*
	 * MAMA transport getter method
	 *
	 * @return MAMA transport handle
	 */
	mamaTransport GetTransport() const
	{
		return transport_;
	}

	/*
	 * MAMA subscription getter method
	 *
	 * @return MAMA subscription handle
	 */
	mamaSubscription GetSubscription() const
	{
		return subscription_;
	}

	/*
	 * MAMA queue getter method
	 *
	 * @return MAMA queue handle
	 */
	mamaQueue GetQueue() const
	{
		return queue_;
	}

	/*
	 * Closure getter method
	 *
	 * @return Closure pointer
	 */
	void* GetClosure() const
	{
		return closure_;
	}

	/*
	 * MAMA message callback getter method
	 *
	 * @return MAMA message callback handle
	 */
	mamaMsgCallbacks GetMsgCallback() const 
	{
		return callback_;
	}

	/*
	 * Source id getter method
	 *
	 * @return source id
	 */
	const std::string& GetSource
		() const
	{
		return source_;
	}

	/*
	 * Symbol string getter method
	 *
	 * @return symbol string
	 */
	const std::string& GetSymbol() const
	{
		return symbol_;
	}

	/*
	 * Sends subscription request via Blp session object
	 *
	 * @param[in] transport Valid session
	 * @return subscription status (success/fail)
	 */
	BLPExpDll bool Subscribe(BlpTransportBridge& transport);

	bool isubscribed;
	/*
	 * Sends unsubscription request via Blp session object
	 *
	 * @param[in] transport Valid session
	 */
	BLPExpDll void Unsubscribe(BlpTransportBridge& transport);

	/*
	 * Blp session callback function.
	 *
	 * @param[in] msg Message to be processed
	 */
	//void onUpdate(blpapi::Message& msg);

	void OnUpdate(blpapi::Message& msg, FieldSet_t & fields, bool filterFields);

	void OnInvalidSec(blpapi::Message& msg);

	void ProcessResponse(blpapi::Message& msg, FieldSet_t & fields, bool filterFields);


	bool IsValid() const
	{
		return isValid_;
	}

	void Mute()
	{
		isNotMuted_ = false;
	}

	void AddRef();
	void Release();

public:
	static const std::set<std::string> DEFAULT_MKTDATA_FIELDS;
	static const std::set<std::string> DEFAULT_REFDATA_FIELDS;

private:
	void setMsgType(const blpapi::Message& blpmsg);
	int setMsgFields(const blpapi::Element& blpmsg, FieldSet_t & fields, bool filterFields);

	void setMsgNum();

private:


	blpItemPtr_t blpItem_;

	std::string source_;
	std::string symbol_;

	int msgTotal_;
	int msgNum_;

	mamaTransport	  transport_;
	mamaQueue         queue_;
	mamaMsgCallbacks  callback_;
    mamaSubscription  subscription_;
	void*			  closure_;
	mamaMsg			  msg_;

	bool              isNotMuted_;
    bool              isValid_;

	bool isSubscribed_;
	//blpapi::SubscriptionList subscriptions_;
	interlockedInt_t  refCount_;


	enum { MKTDATA_SUBSCRIPTION, MKTDATA_DELAYED_SUBSCRIPTION, REFDATA_SUBSCRIPTION, UNKNOWN_SUBSCRIPTION }  type_;

#ifdef ENABLE_TESTING
public:
	void (*onUpdateTest)(const BlpSubscription & sub);
	void (*onResponseTest)(const BlpSubscription & sub);
#endif
};

