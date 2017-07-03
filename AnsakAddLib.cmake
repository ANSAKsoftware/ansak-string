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
# ansakString / ansak / ansakSqlite use ansak_add_library, defined here,
# which conditions the build based on .DLL / .so output, (on Windows, also
# between /MT and /MD builds) and partitioning the output into three libraries
# ansakString -- contains only ansakString
# ansak -- contains ansakConfig, ansakFileTools, ansakFile, ansakPrimitives,
#          ansakString, ansakOSLayer
# ansakSqlite -- contains only ansakSqlite
#
###########################################################################
#
# root CMake calls ansak_add_library for "ansak" after add_subdirectory(src)
# completes
#
###########################################################################
#
# Calling root CMake file with no parameters produces static-linked libraries,
# that on Windows are compatible with a DLL-based C-RunTime. This is the same
# as adding -DANSAK_LIB_TYPE=Static -DANSAK_CRUNTIME=Shared to the CMake
# invocation.
#
# Adding -DANSAK_LIB_TYPE=Shared to the CMake invocation produces .DLL / .so
# libraries.
#
# On Windows, adding -DANSAK_CRUNTIME=Static to the CMake invocation, produces
# libraries that are compatible with the statically-linked C-Runtime.
#
# Selecting -DANSAK_CRUNTIME=Static on Linux has no effect.
# Selecting -DANSAK_CRUNTIME=Static with -DANSAK_LIB_TYPE=Shared is an error.
# Using other values than Shared and Static with these defines is an error.
#
###########################################################################

include( CMakeParseArguments )

set( _ansak_lib_type "static" )
set( ansak_lib_predicate )

if( ANSAK_LIB_TYPE )
    string( TOLOWER "${ANSAK_LIB_TYPE}" _ansak_lib_type )
endif()
if( _ansak_lib_type STREQUAL static )
    set( ansak_lib_predicate "STATIC" )
elseif( _ansak_lib_type STREQUAL shared )
    set( ansak_lib_predicate "SHARED" )
else()
    message( FATAL ": -DANSAK_LIB_TYPE=${ANSAK_LIB_TYPE} is invalid" )
endif()
if( ansak_lib_predicate STREQUAL "SHARED" )
    set( CMAKE_POSITION_INDEPENDENT_CODE ON )
endif()

set( _ansak_lib_objects CACHE INTERNAL "libs in libansak.xx" FORCE )

if( WIN32 )
    set( _ansak_runtime "shared" )
    set( _ansak_runtime_is_static false )
    if( ANSAK_CRUNTIME )
        string( TOLOWER "${ANSAK_RUNTIME}" _ansakRuntime )
    endif()
    if( _ansakRuntime STREQUAL static )
        if ( ansak_lib_predicate STREQUAL "SHARED" )
            message( FATAL ": -DANSAK_CRUNTIME=${ANSAK_CRUNTIME} is invalid when -DANSAK_LIB_TYPE=${ANSAK_LIB_TYPE}" )
        else()
            set( _ansak_runtime_is_static true )
        endif()
    elseif( _ansakRuntime STREQUAL shared )
        set( _ansak_runtime_is_static false )
    else()
        message( FATAL ": -DANSAK_CRUNTIME=${ANSAK_CRUNTIME} is invalid" )
    endif()

    # Dynamic ("shared") v. Static on Windows
    include( WindowsCRuntime )
    if( _ansak_runtime_is_static )
        SETUP_C_RUNTIME( "STATIC" )
    else()
        SETUP_C_RUNTIME( "DYNAMIC" )
    endif()
elseif( ANSAK_CRUNTIME )
    message( FATAL ": -DANSAK_CRUNTIME=${ANSAK_CRUNTIME} is valid only for Windows." )
endif()

macro( ansak_add_library )
    set( _local_ansak_lib_objects "${_ansak_lib_objects}" )
    set( _options )
    set( _oneValue TARGET )
    set( _multiValue SOURCES PRIVATE_INCLUDE PUBLIC_INCLUDE )
    cmake_parse_arguments( _ADD_LIB "${_options}" "${_oneValue}" "${_multiValue}" "${ARGN}" )
    set( _ADD_LIB_OBJ_TARGET "${_ADD_LIB_TARGET}-o" )

    add_library( ${_ADD_LIB_OBJ_TARGET} OBJECT ${_ADD_LIB_SOURCES} )
    if( _ADD_LIB_PRIVATE_INCLUDE AND _ADD_LIB_PUBLIC_INCLUDE )
        target_include_directories( ${_ADD_LIB_OBJ_TARGET} PRIVATE ${_ADD_LIB_PRIVATE_INCLUDE}
                                                           PUBLIC ${_ADD_LIB_PUBLIC_INCLUDE} )
    elseif( _ADD_LIB_PUBLIC_INCLUDE )
        target_include_directories( ${_ADD_LIB_OBJ_TARGET} PUBLIC ${_ADD_LIB_PUBLIC_INCLUDE} )
    elseif( _ADD_LIB_PRIVATE_INCLUDE )
        target_include_directories( ${_ADD_LIB_OBJ_TARGET} PRIVATE ${_ADD_LIB_PRIVATE_INCLUDE} )
    endif()
    if( ANSAK_EFFECTIVE_CPP )
        target_compile_options( ${_ADD_LIB_OBJ_TARGET} PRIVATE -Weffc++ )
    endif()

    if( NOT _ADD_LIB_TARGET MATCHES Sql )
        list( APPEND _local_ansak_lib_objects "$<TARGET_OBJECTS:${_ADD_LIB_OBJ_TARGET}>" )
    endif()

    add_library( ${_ADD_LIB_TARGET} ${ansak_lib_predicate} $<TARGET_OBJECTS:${_ADD_LIB_OBJ_TARGET}> )
    if( _ADD_LIB_PUBLIC_INCLUDE )
        target_include_directories( ${_ADD_LIB_TARGET} PUBLIC ${_ADD_LIB_PUBLIC_INCLUDE} )
    endif()
    set( _ansak_lib_objects "${_local_ansak_lib_objects}" CACHE INTERNAL "libs in libansak.xx" FORCE )
endmacro()

macro( add_ansak_lib )
    add_library( ansakLib ${ansak_lib_predicate} ${_ansak_lib_objects} )
endmacro()

function( ansak_add_mock_library )
    set( _options )
    set( _oneValue TARGET )
    set( _multiValue SOURCES )
    cmake_parse_arguments( _ADD_MOCK_LIB "${_options}" "${_oneValue}" "${_multiValue}" "${ARGN}" )
    add_library( ${_ADD_MOCK_LIB_TARGET} STATIC ${_ADD_MOCK_LIB_SOURCES} )
endfunction()
