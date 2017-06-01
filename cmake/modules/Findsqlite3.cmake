###########################################################################
#
# Copyright (c) 2017, Arthur N. Klassen
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
###########################################################################
#
#    May you do good and not evil.
#    May you find forgiveness for yourself and forgive others.
#    May you share freely, never taking more than you give.
#
###########################################################################
#
# Source information:
#   Derived from spectrum2 source tree
#       at: https://github.com/hanzz/spectrum2/tree/master/cmake_modules
#       on 2017.05.31 (c5edfd19b1aac670a820114621243aacc1abf633)
#  at which time no copyright was asserted on it. I modernized its CMake
#  dialect and am making it compatible with Win32 as well.

##########################################################################
# Resulting values are:
#   SQLITE3_INCLUDE_DIR - directory containing Sqlite 3 header files
#   SQLITE3_LIBRARIES - Sqlite 3 libraries by name
#   SQLITE3_LIBRARY_RELEASE - directory containing the release libraries
#   SQLITE3_LIBRARY_DEBUG - directory containing the release libraries
#   SQLITE3_FOUND - Set to 'TRUE' if library and includes were found

if( SQLITE3_INCLUDE_DIR AND
    SQLITE3_LIBRARY_RELEASE AND
    SQLITE3_LIBRARY_DEBUG )
    set(SQLITE3_FIND_QUIETLY TRUE)
endif()

find_path( SQLITE3_INCLUDE_DIR sqlite3.h  )

find_library(SQLITE3_LIBRARY_RELEASE NAMES sqlite3 )

if (WIN32)
else()
    find_library(SQLITE3_LIBRARY_DEBUG
                    NAMES sqlite3 sqlite3d
                    HINTS /usr/lib/debug/usr/lib/ )
endif()

if( SQLITE3_LIBRARY_RELEASE OR SQLITE3_LIBRARY_DEBUG AND SQLITE3_INCLUDE_DIR )
	set( SQLITE3_FOUND TRUE )
endif()

if( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )
	# if the generator supports configuration types then set
	# optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
	if( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
		set( SQLITE3_LIBRARIES optimized ${SQLITE3_LIBRARY_RELEASE}
                                   debug ${SQLITE3_LIBRARY_DEBUG} )
	else()
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
		set( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
	endif()
elseif( SQLITE3_LIBRARY_RELEASE )
	set( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
else()
	set( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_DEBUG} )
endif()

if( SQLITE3_FOUND )
	if( NOT SQLITE3_FIND_QUIETLY )
		message( STATUS "Found Sqlite3 header file in ${SQLITE3_INCLUDE_DIR}")
		message( STATUS "Found Sqlite3 libraries: ${SQLITE3_LIBRARIES}")
	endif()
else()
	if(SQLITE3_FIND_REQUIRED)
		message( FATAL_ERROR "Could not find Sqlite3" )
	ELSE()
		message( STATUS "Optional package Sqlite3 was not found" )
	endif()
endif()
