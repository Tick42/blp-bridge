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

#ifndef BLP_BRIDGE_FUNCTIONS__
#define BLP_BRIDGE_FUNCTIONS__

#include <mama/mama.h>
#include <bridge.h>

#if defined(__cplusplus)
extern "C" {
#endif



// These are the Open Mama C entry points for the Tick42BLP Bridge

 /*=========================================================================
  =                    Functions for the bridge                           =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status
tick42blpBridge_init(mamaBridge bridgeImpl);

MAMAExpBridgeDLL
extern const char*
tick42blpBridge_getVersion (void);

MAMAExpBridgeDLL
mama_status
tick42blpBridge_getDefaultPayloadId (char*** name, char** id);

/**
 * Get the underlying default mamaQueue of the bridge. the queue is referred as
 * impl->mDefaultEventQueue of mamaBridgeImpl
 *
 * @param queue the requested queue
 */
MAMAExpBridgeDLL
mama_status
tick42blpBridge_getTheMamaQueue (mamaQueue* queue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridge_open (mamaBridge bridgeImpl);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridge_close (mamaBridge bridgeImpl);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridge_start (mamaQueue defaultEventQueue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridge_stop (mamaQueue defaultEventQueue);

MAMAExpBridgeDLL
extern const char*
tick42blpBridge_getName (void);


/*=========================================================================
  =                    Functions for the mamaQueue                        =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_GetThePublisherQueue (queueBridge* queue, mamaQueue parent);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_create (queueBridge *queue, mamaQueue parent);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_create_usingNative (queueBridge *queue, mamaQueue parent, void* nativeQueue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_destroy (queueBridge queue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_getEventCount (queueBridge queue, size_t* count);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_dispatch (queueBridge queue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_timedDispatch (queueBridge queue, uint64_t timeout);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_dispatchEvent (queueBridge queue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_enqueueEvent (queueBridge        queue,
                                   mamaQueueEnqueueCB callback,
                                   void*              closure);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_stopDispatch (queueBridge queue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_setEnqueueCallback (queueBridge        queue,
                                         mamaQueueEnqueueCB callback,
                                         void*              closure);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_removeEnqueueCallback (queueBridge queue);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_getNativeHandle (queueBridge queue,
                                    void**      result);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_setLowWatermark (queueBridge queue,
                                    size_t      lowWatermark);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaQueue_setHighWatermark (queueBridge queue,
                                     size_t      highWatermark);
/*=========================================================================
  =                    Functions for the mamaTransport                    =
  =========================================================================*/
MAMAExpBridgeDLL
extern int
tick42blpBridgeMamaTransport_isValid (transportBridge transport);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_destroy (transportBridge transport);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_create (transportBridge* result,
                                 const char*      name,
                                 mamaTransport    parent);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_forceClientDisconnect (transportBridge* transports,
                                              int              numTransports,
                                              const char*      ipAddress,
                                              uint16_t         port);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_findConnection (transportBridge* transports,
                                       int              numTransports,
                                       mamaConnection*  result,
                                       const char*      ipAddress,
                                       uint16_t         port);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getAllConnections (transportBridge* transports,
                                          int              numTransports,
                                          mamaConnection** result,
                                          uint32_t*        len);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getAllConnectionsForTopic (transportBridge* transports,
                                                    int              numTransports,
                                                    const char*      topic,
                                                    mamaConnection** result,
                                                    uint32_t*        len);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_freeAllConnections (transportBridge* transports,
                                           int              numTransports,
                                           mamaConnection*  connections,
                                           uint32_t         len);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getAllServerConnections (
                                        transportBridge*       transports,
                                        int                    numTransports,
                                        mamaServerConnection** result,
                                        uint32_t*              len);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_freeAllServerConnections (
                                        transportBridge*        transports,
                                        int                     numTransports,
                                        mamaServerConnection*   connections,
                                        uint32_t                len);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getNumLoadBalanceAttributes (
                                    const char* name,
                                    int*        numLoadBalanceAttributes);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getLoadBalanceSharedObjectName (
                                    const char*  name,
                                    const char** loadBalanceSharedObjectName);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getLoadBalanceScheme (const char*    name,
                                             tportLbScheme* scheme);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_sendMsgToConnection (
                                    transportBridge transport,
                                    mamaConnection  connection,
                                    mamaMsg         msg,
                                    const char*     topic);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_isConnectionIntercepted (
                                    mamaConnection connection,
                                    uint8_t* result);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_installConnectConflateMgr (
                                    transportBridge       transport,
                                    mamaConflationManager mgr,
                                    mamaConnection        connection,
                                    conflateProcessCb     processCb,
                                    conflateGetMsgCb      msgCb);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_uninstallConnectConflateMgr (
                                    transportBridge       transport,
                                    mamaConflationManager mgr,
                                    mamaConnection        connection);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_startConnectionConflation (
                                    transportBridge        transport,
                                    mamaConflationManager  mgr,
                                    mamaConnection         connection);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_requestConflation (transportBridge* transports,
                                            int              numTransports);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_requestEndConflation (transportBridge* transports,
                                               int              numTransports);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getNativeTransport (transportBridge transport,
                                             void**          result);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaTransport_getNativeTransportNamingCtx (transportBridge transport,
                                                      void**          result);

/*=========================================================================
  =                    Functions for the mamaSubscription                 =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaSubscription_create
                               (subscriptionBridge* subsc_,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure );

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaSubscription_createWildCard (
                                subscriptionBridge* subsc_,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure );

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaSubscription_mute
                                (subscriptionBridge subscriber);

MAMAExpBridgeDLL
extern  mama_status tick42blpBridgeMamaSubscription_destroy
                                (subscriptionBridge subscriber);

MAMAExpBridgeDLL
extern int tick42blpBridgeMamaSubscription_isValid
                                (subscriptionBridge bridge);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaSubscription_getSubject
                                (subscriptionBridge subscriber,
                                 const char**       subject);

MAMAExpBridgeDLL
extern int tick42blpBridgeMamaSubscription_hasWildcards
                                (subscriptionBridge subscriber);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaSubscription_getPlatformError
                                (subscriptionBridge subsc, void** error);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaSubscription_setTopicClosure
                                (subscriptionBridge subsc, void* closure);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaSubscription_muteCurrentTopic
                                (subscriptionBridge subsc);

MAMAExpBridgeDLL
extern int tick42blpBridgeMamaSubscription_isTportDisconnected
                                (subscriptionBridge subsc);

/*=========================================================================
  =                    Functions for the mamaTimer                        =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaTimer_create (timerBridge* timer,
                                              void*        nativeQueueHandle,
                                              mamaTimerCb  action,
                                              mamaTimerCb  onTimerDestroyed,
                                              mama_f64_t   interval,
                                              mamaTimer    parent,
                                              void*        closure);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaTimer_destroy (timerBridge timer);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaTimer_reset (timerBridge timer);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaTimer_setInterval (timerBridge timer,
                                                   mama_f64_t  interval);

MAMAExpBridgeDLL
extern mama_status tick42blpBridgeMamaTimer_getInterval (timerBridge timer,
                                                   mama_f64_t* interval);

/*=========================================================================
  =                    Functions for the mamaIo                           =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaIo_create (ioBridge*       result,
                        void*           nativeQueueHandle,
                        uint32_t        descriptor,
                        mamaIoCb        action,
                        mamaIoType      ioType,
                        mamaIo          parent,
                        void*           closure);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaIo_destroy (ioBridge io);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaIo_getDescriptor (ioBridge io, uint32_t* result);


/*=========================================================================
  =                    Functions for the mamaPublisher                    =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_createByIndex (
                               publisherBridge*  result,
                               mamaTransport     tport,
                               int               tportIndex,
                               const char*       topic,
                               const char*       source,
                               const char*       root,
                               void*             nativeQueueHandle,
                               mamaPublisher     parent);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_create (publisherBridge*  result,
                               mamaTransport     tport,
                               const char*       topic,
                               const char*       source,
                               const char*       root,
                               void*             nativeQueueHandle,
                               mamaPublisher     parent);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_destroy (publisherBridge publisher);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_sendReplyToInbox (publisherBridge publisher,
                                         void*         request,
                                         mamaMsg         reply);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_sendFromInbox (publisherBridge publisher,
                                      mamaInbox       inbox,
                                      mamaMsg         msg);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge publisher,
                                             int           tportIndex,
                                             mamaInbox     inbox,
                                             mamaMsg       msg);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge publisher,
                                               void *          wmwReply,
                                               mamaMsg         reply);

/*=========================================================================
  =                    Functions for the mamaInbox                        =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaInbox_create (
            inboxBridge*           bridge,
            mamaTransport          tport,
            mamaQueue              queue,
            mamaInboxMsgCallback   msgCB,
            mamaInboxErrorCallback errorCB,
            mamaInboxDestroyCallback onInboxDestroyed,
            void*                  closure,
            mamaInbox              parent);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaInbox_createByIndex (
            inboxBridge*           bridge,
            mamaTransport          tport,
            int                    tportIndex,
            mamaQueue              queue,
            mamaInboxMsgCallback   msgCB,
            mamaInboxErrorCallback errorCB,
            mamaInboxDestroyCallback onInboxDestroyed,
            void*                  closure,
            mamaInbox              parent);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaInbox_destroy (inboxBridge inbox);

/*=========================================================================
  =                    Functions for the mamaMsg                           =
  =========================================================================*/
MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_create (msgBridge* msg, mamaMsg parent);

MAMAExpBridgeDLL
extern int
tick42blpBridgeMamaMsg_isFromInbox (msgBridge msg);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_destroy (msgBridge msg, int destroyMsg);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_destroyMiddlewareMsg (msgBridge msg);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_detach (msgBridge msg);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_getPlatformError (msgBridge msg, void** error);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_setSendSubject (msgBridge   msg,
                                   const char* symbol,
                                   const char* subject);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_getNativeHandle (msgBridge msg, void** result);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_duplicateReplyHandle (msgBridge msg, void** result);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_copyReplyHandle (void* src, void** dest);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsg_destroyReplyHandle (void* result);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsgImpl_setReplyHandle (msgBridge msg, void* result);

MAMAExpBridgeDLL
extern mama_status
tick42blpBridgeMamaMsgImpl_setReplyHandleAndIncrement (msgBridge msg, void* result);

MAMAExpBridgeDLL
extern mama_status
blpBridgeMamaMsgImpl_setAttributesAndSecure (msgBridge msg, void* attributes, uint8_t secure);

#if defined(__cplusplus)
}
#endif

#endif /*BLP_BRIDGE_FUNCTIONS__*/

