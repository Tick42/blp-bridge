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
#ifndef BLP_DEFS_H
#define BLP_DEFS_H

#define CHECK_BLP(blp) \
    do {  \
       if (blp == 0) return MAMA_STATUS_NULL_ARG; \
     } while(0)

#define SUBJECT_FIELD_NAME  "__subj"
#define INBOX_FIELD_NAME    "__inbx"
#define MAX_SUBJECT_LENGTH    256

#define blpBridge(bridgeImpl) ((BlpBridgeImpl*) bridgeImpl)

#define BLP_VERBOSE

#if defined BLP_VERBOSE
#define BLP_PRINT fprintf
#else
#define BLP_PRINT(...)
#endif

#ifdef ENABLE_TESTING
	#ifdef MAMA_DLL
	#define BLPExpDll __declspec( dllexport )
	#else
	#define BLPExpDll __declspec( dllimport )
	#endif
#else
	#define BLPExpDll 
#endif

#endif /* BLP_DEFS_H */
