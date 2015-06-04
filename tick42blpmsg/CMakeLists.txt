cmake_minimum_required(VERSION 2.6)

project (tick42blpmsg)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${OPENMAMA_INCLUDE_DIR} 
	${OPENMAMA_COMMON_INCLUDE_DIR} 
	${Boost_INCLUDE_DIRS}
	${BLPAPI_CPP_INCLUDE_DIR}
	${WOMBAT_OS_DEPENDENT_INCLUDE_DIR}
	)

add_library(mamatick42blpmsgimpl SHARED	
	blpmsgimpl.cpp
	blpmsgutils.cpp
	blppayloadimpl.cpp
	
	blpfieldpayload.h
	blpmsgimpl.h
	blpmsgutils.h
	blppayload.h
	blppayloadimpl.h
	blpvaluetype.h
	)

target_link_libraries(	mamatick42blpmsgimpl 
	${OPENMAMA_LIBRARIES} 
	${Boost_FILESYSTEM_LIBRARY} 
	${Boost_DATE_TIME_LIBRARY}
	${OPENMAMA_COMMON_LIBRARIES}
	${BLPAPI_CPP_LIBRARIES}						
	)

#install (TARGETS mamatick42blpmsgimpl LIBRARY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} lib)
