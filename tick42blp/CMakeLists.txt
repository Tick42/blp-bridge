cmake_minimum_required(VERSION 2.6)

project (tick42blp)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

IF(WIN32 AND NOT CYGWIN)
# For Windows just fake the need for uuid library
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENMAMA_INCLUDE_DIR}
    ${OPENMAMA_COMMON_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${BLPAPI_CPP_INCLUDE_DIR}
    ${WOMBAT_OS_DEPENDENT_INCLUDE_DIR}
    )
ELSE (WIN32 AND NOT CYGWIN)
# For other systems but native Windows
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENMAMA_INCLUDE_DIR}
    ${OPENMAMA_COMMON_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${BLPAPI_CPP_INCLUDE_DIR}
    ${WOMBAT_OS_DEPENDENT_INCLUDE_DIR}
    ${UUID_INCLUDE_DIR}
    )
ENDIF(WIN32 AND NOT CYGWIN)


add_library(mamatick42blpimpl SHARED
    blpenvironment.cpp
    blpbridgesubscription.cpp
    blpdictionaryutils.cpp
    blpfilesystem.cpp
    blpitem.cpp
    blpresponseeventhandler.cpp
    blpservice.cpp
    blpsession.cpp
    BlpSubscription.cpp
    blpsubscriptioneventhandler.cpp
    blpsystemglobal.cpp
    blpthreadutil.cpp
    blputils.cpp
    bridge.cpp
    io.cpp
    msg.cpp
    publisher.cpp
    queue.cpp
    stdafx.cpp
    subinitial.cpp
    timer.cpp
    transportbridge.cpp
    blptimeutils.cpp

    blpenvironment.h
    blpbridgefunctions.h
    blpbridgeimpl.h
    blpdefs.h
    blpdictionaryutils.h
    blpfilesystem.h
    blpitem.h
    blpresponseeventhandler.h
    blpservice.h
    blpsession.h
    blpsubscription.h
    blpsubscriptioneventhandler.h
    blpsystemglobal.h
    blpthreadutil.h
    blputils.h
    common.h
    subinitial.h
    transportbridge.h
    blptimeutils.h
    )

target_link_libraries(mamatick42blpimpl
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${OPENMAMA_LIBRARIES}
    ${OPENMAMA_COMMON_LIBRARIES}
    ${BLPAPI_CPP_LIBRARIES}
    )

#install (TARGETS mamatick42blpimpl LIBRARY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} lib)
