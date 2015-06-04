cmake_minimum_required (VERSION 2.6)

#--------------------------------------------------------------------------------------------------
# cmake source path (used for the includes and find packages.
#--------------------------------------------------------------------------------------------------

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#--------------------------------------------------------------------------------------------------
# Packages Prerequisites
#--------------------------------------------------------------------------------------------------

set (BOOST_ROOT $ENV{TICK42_BOOST})

#--------------------------------------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------------------------------------

include (CheckCXXCompilerFlag)
include (FindBoost)
include (FindOpenMAMA)
include (FindBlpAPI)
include (FindUuid)

find_package (OpenMAMA)
find_package (BlpAPI)
find_package (Uuid)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

#--------------------------------------------------------------------------------------------------
# Boost Libraries
#--------------------------------------------------------------------------------------------------

set (Boost_DIR "$ENV{TICK42_BOOST}")
message ("Boost root is " ${Boost_DIR})

set (Boost_MULTITHREADED TRUE)
set (Boost_USE_STATIC_LIBS FALSE)

find_package (Boost 1.54 REQUIRED date_time filesystem  system thread)

add_definitions (-DBOOST_FILESYSTEM_VERSION=3 -DBOOST_FILESYSTEM_NO_DEPRECATED)

include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

#--------------------------------------------------------------------------------------------------
# Compiler C++11/0x support
#--------------------------------------------------------------------------------------------------

if(CMAKE_COMPILER_IS_GNUCXX)
   SET(ENABLE_CXX11 "-std=c++11")

   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_LESS 4.7)
      SET(ENABLE_CXX11 "-std=c++0x")
   endif()

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
endif()

#--------------------------------------------------------------------------------------------------
# Compiler
#--------------------------------------------------------------------------------------------------

function(add_compiler_flag_if_supported flag name)
  check_cxx_compiler_flag (${flag} ${name})
  if (${name})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
  endif ()
endfunction()

set (PEDANTIC OFF CACHE BOOL "$Enable lots of warnings")

add_compiler_flag_if_supported(-Werror werror)
add_compiler_flag_if_supported(-Wwrite-strings wwrite_strings)
add_compiler_flag_if_supported(-Wreturn-type wreturn_type)
add_compiler_flag_if_supported(-Wunused-value wunused_value)
add_compiler_flag_if_supported(-Wtrigraphs wtrigraphs)
add_compiler_flag_if_supported(-Wmissing-braces wmissing_braces)
add_compiler_flag_if_supported(-Wcomment wcomment)
add_compiler_flag_if_supported(-Wtype-limits type_limits)
add_compiler_flag_if_supported(-Wempty-body empty_body)
add_compiler_flag_if_supported(-Waddress  waddress)

if (PEDANTIC)
  add_compiler_flag_if_supported(-Wall wall)
  #add_compiler_flag_if_supported(-Wextra wextra)
  add_compiler_flag_if_supported(-Wuninitialized wuninitialized)
  add_compiler_flag_if_supported(-Wsign-compare  wsign_compare)
  add_compiler_flag_if_supported(-Wchar-subscripts char_subscipts)
endif()

#--------------------------------------------------------------------------------------------------
# Dump all variables - use it when you need some generated variables from packages
#--------------------------------------------------------------------------------------------------

set (DUMP_VARIABLES OFF CACHE BOOL "Dump all variables")
if (DUMP_VARIABLES)
	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
	    message("${_variableName}=${${_variableName}}")
	endforeach()
endif()
#--------------------------------------------------------------------------------------------------
# Sub projects
#--------------------------------------------------------------------------------------------------
                                        
add_subdirectory(tick42blpmsg)
add_subdirectory(tick42blp)

