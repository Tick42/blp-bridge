
find_path (OPENMAMA_INCLUDE_DIR 
	NAMES
		bridge.h
#		lookup2.h
	PATH_SUFFIXES 
		mama
		include
	PATHS
		$ENV{TICK42_OPENMAMA}
		$ENV{TICK42_OPENMAMA}/mama/c_cpp/src/c
		$ENV{OPENMAMA_API}/include
		/usr/local/include
		/usr/include
		/opt/local/include
		/opt/include
	)

message( "OPENMAMA_INCLUDE_DIR=${OPENMAMA_INCLUDE_DIR}")

find_path (OPENMAMA_COMMON_INCLUDE_DIR 
	NAMES
		lookup2.h
	HINTS
		${OPENMAMA_INCLUDE_DIR}
	PATH_SUFFIXES 
		wombat
		common
		include
	PATHS
		$ENV{TICK42_OPENMAMA}
		$ENV{TICK42_OPENMAMA}/common/c_cpp/src/c
		$ENV{OPENMAMA_API}
	)

message( "OPENMAMA_COMMON_INCLUDE_DIR=${OPENMAMA_COMMON_INCLUDE_DIR}")

# OpenMama Wombat libraries OS dependent
IF(WIN32 AND NOT CYGWIN)
find_path (WOMBAT_OS_DEPENDENT_INCLUDE_DIR 
	NAMES
		lock.h
	PATH_SUFFIXES 
		windows
	PATHS
		$ENV{TICK42_OPENMAMA}/common/c_cpp/src/c
		$ENV{OPENMAMA_API}/include
		/usr/local/include
		/usr/include
		/opt/local/include
		/opt/include
	)
ELSE (WIN32 AND NOT CYGWIN)
find_path (WOMBAT_OS_DEPENDENT_INCLUDE_DIR
	NAMES
		port.h
	PATH_SUFFIXES 
		wombat
		common
		linux
	PATHS
		$ENV{TICK42_OPENMAMA}/common/c_cpp/src/c
		OPENMAMA_INCLUDE_DIR/common/c_cpp/src/c
		OPENMAMA_INCLUDE_DIR
		$ENV{TICK42_OPENMAMA}/include
		$ENV{OPENMAMA_API}/include
		/usr/local/include
		/usr/include
		/opt/local/include
		/opt/include
	)
ENDIF(WIN32 AND NOT CYGWIN)
message( "WOMBAT_OS_DEPENDENT_INCLUDE_DIR=${WOMBAT_OS_DEPENDENT_INCLUDE_DIR}")	

IF(WIN32 AND NOT CYGWIN)
	#in windows
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set( MAMA_LIBS_SUFFIX "Debug" )
		set( WOMBAT_COMMON "libwombatcommonmd.dll")
	else (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set( MAMA_LIBS_SUFFIX "Release" )
		set( WOMBAT_COMMON "libwombatcommonmdd.dll")
	endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
	
ELSE (WIN32 AND NOT CYGWIN)
	set( MAMA_LIBS_SUFFIX "" )
ENDIF(WIN32 AND NOT CYGWIN)

find_library (OPENMAMA_LIBRARIES
	NAMES
		mamac
		mama
	PATH_SUFFIXES 
		${MAMA_LIBS_SUFFIX}
		lib64 
		lib
	PATHS
		$ENV{TICK42_OPENMAMA}
		$ENV{OPENMAMA_API}
		/usr/local
		/usr
		/opt/local
		/opt
	)
message( "OPENMAMA_LIBRARIES=${OPENMAMA_LIBRARIES}")

find_library (OPENMAMA_COMMON_LIBRARIES
	NAMES
		commonc
		wombatcommon
	PATH_SUFFIXES 
	${MAMA_LIBS_SUFFIX}
		lib64 
		lib
		PATHS
		$ENV{TICK42_OPENMAMA}
		$ENV{OPENMAMA_API}
		/usr/local
		/usr
		/opt/local
		/opt
	)
message( "OPENMAMA_COMMON_LIBRARIES=${OPENMAMA_COMMON_LIBRARIES}")

include (FindPackageHandleStandardArgs)
mark_as_advanced (CLEAR OPENMAMA_LIBRARIES, OPENMAMA_INCLUDE_DIR)
find_package_handle_standard_args (OpenMAMA DEFAULT_MSG OPENMAMA_LIBRARIES OPENMAMA_INCLUDE_DIR)

