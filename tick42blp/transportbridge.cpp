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
//#include "wombat/port.h"

#include <property.h>
#include <mama/mama.h>
#include <mama/types.h>
#include <mama/status.h>
#include <transportimpl.h>
#include <timers.h>
#include <errno.h>

#ifdef str
#undef str
#endif

#include "transportbridge.h"
#include "blpbridgefunctions.h"
#include "blpdefs.h"
#include "blpbridgeimpl.h"

#include <string>
#include <fstream>

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/scope_exit.hpp>
#include <boost/make_shared.hpp>

#include "blpservice.h"
#include "blpsystemglobal.h"
#include "blpfilesystem.h"

static const std::string TPORT_PREFIX("mama.tick42blp.transport");
static const std::string TPORT_PARAM("url");

static const std::string DEFAULT_URL("localhost");
static const int         DEFAULT_PORT(8194);

static const std::string DICT_FIELDMAPFILE_PARAM("fieldmap");
static const std::string DICT_INCL_MAPPED_FIELDS_PARAM("unmappedflds");//=true
static const std::string DICT_UNMAPPED_FIELDS_FID_OFFSET("newfidsoffset");// =20000

static const std::string DEFAULT_FIELDMAPFILE("fieldmap.csv");
static const std::string DEFAULT_INCL_MAPPED_FIELDS("true");
static const std::string DEFAULT_UNMAPPED_FIELDS_FID_OFFSET("5000");

#define blpTransport(transport) (reinterpret_cast<BlpTransportBridge*>(transport))
#define CHECK_TRANSPORT(transport) \
        do {  \
           if (blpTransport(transport) == 0) return MAMA_STATUS_NULL_ARG; \
        } while(0)

static std::string getURL(const std::string& name)
{
	std::string url;

	if (name.empty())
	{
		return url;
	}

	std::string propertyName = boost::str(boost::format("%1%.%2%.%3%") % TPORT_PREFIX % name % TPORT_PARAM);

	const char* property = properties_Get( mamaInternal_getProperties(), propertyName.c_str());
    if (!property)
	{
		return DEFAULT_URL;
	}

	mama_log (MAMA_LOG_LEVEL_FINER, "Blp transport (%s) main connection: %s", name.c_str(), property);
    return std::string(property);
}
static std::string getProperty(const std::string &transport_name, const std::string property_name, const std::string property_default)
{
	std::string fieldMapFile;

	if (transport_name.empty())
	{
		return fieldMapFile;
		mama_log (MAMA_LOG_LEVEL_WARN, "getFieldMapFile: Blp transport name is empty!"); 
	}

	std::string propertyName = TPORT_PREFIX + "." + transport_name + "." + property_name;

	const char* property = properties_Get( mamaInternal_getProperties(), propertyName.c_str());
	if (!property)
	{
		mama_log (MAMA_LOG_LEVEL_WARN, "getFieldMapFile: Could not find property %s for transport %s, default to: %s!", propertyName.c_str(), transport_name.c_str(), property_default.c_str()); 
		return property_default;
	}

	mama_log (MAMA_LOG_LEVEL_FINER, "Blp transport (%s) dictionary: %s", transport_name.c_str(), property); 
	return std::string(property);
}

/*
 * getFieldMapFile
 * Get the Field Map (Dictionary csv file) file name
 * Parameters:
 * @param transport_name: Transport name
 * @return: path of the fieldmap.csv
 */
static std::string getFieldMapFile(const std::string& transport_name)
{
	string path = getProperty(transport_name, DICT_FIELDMAPFILE_PARAM, DEFAULT_FIELDMAPFILE);
	return path;
}

static bool to_bool(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}

static inline bool getProperty_DictIncludeUnmappedFields(const std::string &transport_name) 
{ 
	return to_bool(getProperty(transport_name, DICT_INCL_MAPPED_FIELDS_PARAM, DEFAULT_INCL_MAPPED_FIELDS));
}

static inline mama_fid_t getProperty_DictUnmappedFieldsFidOffset(const std::string &transport_name) 
{ 
	try {
		return boost::lexical_cast<mama_fid_t>(getProperty(transport_name, DICT_UNMAPPED_FIELDS_FID_OFFSET, DEFAULT_UNMAPPED_FIELDS_FID_OFFSET));
	}catch(...){
		mama_log (MAMA_LOG_LEVEL_WARN, "getProperty_DictUnmappedFieldsFidOffset couldn't convert value to number.");
	}
	return 0;
}

/*=========================================================================
  =                    Functions for the mamaTransport                    =
  =========================================================================*/

mama_status
tick42blpBridgeMamaTransport_getNumLoadBalanceAttributes (
                                          const char* name,
                                          int*        numLoadBalanceAttributes)
{
    *numLoadBalanceAttributes = 0;
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaTransport_getLoadBalanceSharedObjectName (
                                      const char*  name,
                                      const char** loadBalanceSharedObjectName)
{
    *loadBalanceSharedObjectName = NULL;
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaTransport_getLoadBalanceScheme (
                                    const char*    name,
                                    tportLbScheme* scheme)
{
    *scheme = TPORT_LB_SCHEME_STATIC;
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaTransport_create (transportBridge* result,
                               const char*      name,
                               mamaTransport    mamaTport )
{
	mama_status          status;

	// get hold of the bridge implementation
    mamaBridgeImpl* bridgeImpl = mamaTransportImpl_getBridgeImpl(mamaTport);
    if (!bridgeImpl) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeMamaTransport_create(): Could not get bridge");
        return MAMA_STATUS_PLATFORM;
    }

	BlpBridgeImpl*      blpBridge = NULL;
    if (MAMA_STATUS_OK != (status = mamaBridgeImpl_getClosure((mamaBridge) bridgeImpl, (void**) &blpBridge))) 
	{
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeMamaTransport_create(): Could not get Blp bridge object");
        return status;
    }
    if (blpBridge->hasTransportBridge()) 
	{
        mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeMamaTransport_create(): Blp already connected");
        return MAMA_STATUS_PLATFORM;
    }

	// open the server connection - use defaults as necessary
	//
	// for bloomberg this is just the hostname and the port of whichever blp comms service is being used
    std::string url = getURL(name);
    if (url.empty()) 
	{
        mama_log (MAMA_LOG_LEVEL_NORMAL, "No %s property defined for transport : %s", TPORT_PARAM.c_str(), name);
        return MAMA_STATUS_INVALID_ARG;
    }

	std::string host;
	int port;
	std::size_t pos = url.find_first_of(':');

	if (pos != std::string::npos)
	{
		host.assign(url, 0, pos);

		try 
		{
			port = boost::lexical_cast<int>(url.substr(pos + 1));
		}
		catch (boost::bad_lexical_cast&)
		{
			port = DEFAULT_PORT;
		}
	}
	else
	{
		host.assign(url);
		port = DEFAULT_PORT;
	}


	//Now we can create the blp transport
	boost::shared_ptr<BlpTransportBridge> transport(new (std::nothrow) BlpTransportBridge(host, port));

    if (!transport)
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "tick42blpBridgeMamaTransport_create: Could not create blpapi::Session object");
        return MAMA_STATUS_NOMEM;
	}

    transport->SetTransport(mamaTport);
	blpBridge->setTransportBridge(transport);	

	transport->BlpWombatDictionary_ = std::unique_ptr<BlpFieldDictionary>(new BlpFieldDictionary(getFieldMapFile(name),getProperty_DictUnmappedFieldsFidOffset(name))); //create with field name translation dictionary
	transport->dictIncludeUnmappedFields_ = getProperty_DictIncludeUnmappedFields(name);

	transport->Start();

    *result = (transportBridge) transport.get();

    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaTransport_destroy (transportBridge transport)
{

	blpTransport(transport)->Stop();

	return MAMA_STATUS_OK;
}


int
tick42blpBridgeMamaTransport_isValid (transportBridge transport)
{
	return 1;
}

mama_status
tick42blpBridgeMamaTransport_forceClientDisconnect (transportBridge* transports,
                                              int              numTransports,
                                              const char*      ipAddress,
                                              uint16_t         port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_findConnection (transportBridge* transports,
                            int              numTransports,
                            mamaConnection*  result,
                            const char*      ipAddress,
                            uint16_t         port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_getAllConnections (transportBridge* transports,
                               int              numTransports,
                               mamaConnection** result,
                               uint32_t*        len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_getAllConnectionsForTopic (transportBridge* transports,
                                                    int              numTransports,
                                                    const char*      topic,
                                                    mamaConnection** result,
                                                    uint32_t*        len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_freeAllConnections (
                                        transportBridge* transports,
                                        int              numTransports,
                                        mamaConnection*  result,
                                        uint32_t         len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_getAllServerConnections (
                                        transportBridge*       transports,
                                        int                    numTransports,
                                        mamaServerConnection** result,
                                        uint32_t*              len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_freeAllServerConnections (
                                          transportBridge*      transports,
                                          int                   numTransports,
                                          mamaServerConnection* result,
                                          uint32_t              len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_sendMsgToConnection (transportBridge tport,
                                              mamaConnection  connection,
                                              mamaMsg         msg,
                                              const char*     topic)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_installConnectConflateMgr (
                                           transportBridge       handle,
                                           mamaConflationManager mgr,
                                           mamaConnection        connection,
                                           conflateProcessCb     processCb,
                                           conflateGetMsgCb      msgCb)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


mama_status
tick42blpBridgeMamaTransport_uninstallConnectConflateMgr (
                                            transportBridge       handle,
                                            mamaConflationManager mgr,
                                            mamaConnection        connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_isConnectionIntercepted (mamaConnection connection,
                                                  uint8_t* result)
{
    *result = 0;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_startConnectionConflation (
    transportBridge       tport,
    mamaConflationManager mgr,
    mamaConnection        connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_getNativeTransport (transportBridge transport,
                                             void**          result)
{
    CHECK_TRANSPORT(transport);
    *result = blpTransport(transport);
    return MAMA_STATUS_OK;
}

mama_status
tick42blpBridgeMamaTransport_getNativeTransportNamingCtx (transportBridge transport,
                                                      void** result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_requestConflation (transportBridge* transports,
                                            int              numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
tick42blpBridgeMamaTransport_requestEndConflation (transportBridge* transports,
                                               int              numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}



// BlpTransportBridge implementation
//

BlpTransportBridge* BlpTransportBridge::GetTransport(mamaTransport transport)
{
	BlpTransportBridge* pTransportBridge;
	mama_status status = mamaTransport_getBridgeTransport (transport, (transportBridge*)&pTransportBridge);
	if ((status != MAMA_STATUS_OK) || (pTransportBridge == NULL))
	{
		return 0;
	}

	return pTransportBridge;
}

BlpTransportBridge::BlpTransportBridge(const std::string& host, int port) :
	transport_ (0), started_(false), stopped_(false), dictIncludeUnmappedFields_(false), BlpWombatDictionary_()
{
	blpapi::SessionOptions options;
	options.setServerHost(host.c_str());
	options.setServerPort(port);
	options.setClientMode(BLPAPI_CLIENTMODE_AUTO);

	// create services hard code the names here and key on them

	// In a more sophisticated implementation we would want to read this stuff from some external configuration

	blpServicePtr_t service = BlpMktDataService::CreateService("BlpMktData");
	service->Initialize(options);
	servicesMap_.insert(ServicesMap_t::value_type(service->Name(), service));

	blpServicePtr_t service2 = BlpMktDataService::CreateService("BlpMktDataDelayed");
	service2->Initialize(options);
	((BlpMktDataService*)service2.get())->SetDelayed(true);
	servicesMap_.insert(ServicesMap_t::value_type(service2->Name(), service2));

	blpServicePtr_t service3 = BlpRefDataService::CreateService("BlpRefData");
	service3->Initialize(options);
	servicesMap_.insert(ServicesMap_t::value_type(service3->Name(), service3));

}

BlpTransportBridge::~BlpTransportBridge()
{

}

mama_status BlpTransportBridge::Start()
{
	//should'nt be stopped if haven't started yet!
	if (stopped_)
	{
		mama_log(MAMA_LOG_LEVEL_ERROR, "Trying to access transport that is already stopped!");
		return MAMA_STATUS_NULL_ARG;
	}
 
	if(started_)
	{
		// its probably OK if we are already started 
		return MAMA_STATUS_OK;
	}

	// start each of the services we have registered

	ServicesMap_t::iterator it = servicesMap_.begin();

	// take the approach that so long as we successfully started at least one service then we will continue
	unsigned int NumServicesStarted = 0;
	while(it != servicesMap_.end())
	{
		mama_log(MAMA_LOG_LEVEL_FINE, "Starting blp service %s", it->first.c_str());
		if(!it->second->Start())
		{
			mama_log(MAMA_LOG_LEVEL_WARN, "Failed to start blp service %s", it->first.c_str());
		}
		else
		{
			++NumServicesStarted;
		}
		++it;
	}

	if(NumServicesStarted == 0)
	{
		tick42blpBridgeMamaTransport_destroy((transportBridge)this);
		return MAMA_STATUS_PLATFORM;
	}

	started_ = true;
	return MAMA_STATUS_OK;
	
}

mama_status BlpTransportBridge::Stop()
{
	// stop all the services
	stopped_ = true; //this is wrong. you should check if something else is shutdown and skip this place!

	ServicesMap_t::iterator it = servicesMap_.begin();

	try {
		while(it != servicesMap_.end())
		{
			mama_log(MAMA_LOG_LEVEL_FINE, "stopping blp service %s", it->first.c_str());
			if(!it->second->Stop())
			{
				mama_log(MAMA_LOG_LEVEL_WARN, "Failed to stop blp service %s", it->first.c_str());
			}
			++it;
		}
	}
	catch(...)
	{
		return MAMA_STATUS_SYSTEM_ERROR;
	}

	return MAMA_STATUS_OK;   
}

// Lookup the service, return a null if its not there.
blpServicePtr_t BlpTransportBridge::GetService( const string & name )
{

	blpServicePtr_t ret;
	if (!stopped_)
	{
		ServicesMap_t::iterator it = servicesMap_.find(name);

		if(it != servicesMap_.end())
		{
			ret = it->second;

		}
	}
	return ret;

}
