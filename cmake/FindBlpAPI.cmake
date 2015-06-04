find_path (BLPAPI_CPP_INCLUDE_DIR 
		   NAMES blpapi_session.h
           HINTS $ENV{TICK42_BLPAPI_CPP}
           PATH_SUFFIXES include
           PATHS /usr/local/include
                 /usr/include
                 /opt/local/include
                 /opt/include
          )
		  
# create a variable that depend on the os called BLPAPI_LIB
# libblpapi3_32.so
# libblpapi3_64.so    
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( BLPAPI_LIB_SO "blpapi3_64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( BLPAPI_LIB_SO "blpapi3_32" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  
set( CMAKE_EXE_LINKER_FLAGS ${BLPAPI_LIB_SO} )

message( "BLPAPI_LIB_SO=${BLPAPI_LIB_SO}")
message( "CMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}")
 
find_library (BLPAPI_CPP_LIBRARIES 
			  NAMES ${BLPAPI_LIB_SO} 
              HINTS $ENV{TICK42_BLPAPI_CPP}
              PATH_SUFFIXES lib lib64 Linux
              PATHS
              /usr/local
              /usr
              /opt/local
              /option
             )
             
message(  "BLPAPI_CPP_LIBRARIES=${BLPAPI_CPP_LIBRARIES}")
message(  "BLPAPI_CPP_INCLUDE_DIR=${BLPAPI_CPP_INCLUDE_DIR}")

include (FindPackageHandleStandardArgs)

mark_as_advanced (CLEAR BLPAPI_CPP_LIBRARIES, BLPAPI_CPP_INCLUDE_DIR)
find_package_handle_standard_args (BlpAPI DEFAULT_MSG BLPAPI_CPP_LIBRARIES BLPAPI_CPP_INCLUDE_DIR)    
