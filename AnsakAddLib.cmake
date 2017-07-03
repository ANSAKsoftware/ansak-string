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
# as adding -DANSAK_LIB_TYPE=Static -DANSAK_CRUNTIME=Dynamic to the CMake
# invocation.
#
# Adding -DANSAK_LIB_TYPE=Dynamic to the CMake invocation produces .DLL / .so
# libraries.
#
# On Windows, adding -DANSAK_CRUNTIME=Static to the CMake invocation, produces
# libraries that are compatible with the statically-linked C-Runtime.
#
# Selecting -DANSAK_CRUNTIME=Static on Linux has no effect.
# Selecting -DANSAK_CRUNTIME=Static with -DANSAK_LIB_TYPE=Dynamic is an error.
# Using other values than Dynamic and Static with these defines is an error.
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

if( WIN32 )
    set( _ansak_runtime "dynamic" )
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
    elseif( _ansakRuntime STREQUAL dynamic )
        set( _ansak_runtime_is_static false )
    else()
        message( FATAL ": -DANSAK_CRUNTIME=${ANSAK_CRUNTIME} is invalid" )
    endif()

    # Dynamic v. Static on Windows
    include( WindowsCRuntime )
    if( _ansak_runtime_is_static )
        SETUP_C_RUNTIME( "STATIC" )
    else()
        SETUP_C_RUNTIME( "DYNAMIC" )
    endif()
elseif( ANSAK_CRUNTIME )
    message( FATAL ": -DANSAK_CRUNTIME=${ANSAK_CRUNTIME} is valid only for Windows." )
endif()

#set( _ansak_library_bits )
#
#function( _add_library _targetName _targetSource )
#    if( _targetName STREQUAL ansak )
#        add_library( ansak ${ansak_lib_predicate} ${_ansak_library_bits} )
#    else()
#        add_library( ${_targetName}-o OBJECT "${_targetSource}" )
#        set( _targetNameObjects $<TARGET_OBJECTS:${_targetName}-o> )
#        add_library( ${_targetName} ${ansak_lib_predicate} ${_targetNameObjects} )
#        list( APPEND _ansak_library_bits ${_targetNameObjects} )
#    endif()
#    if (ANSAK_EFFECTIVE_CPP)
#        target_compile_options( ${_targetName} PRIVATE -Weffc++ )
#    endif()
#endfunction()
#
function( ansak_add_library )
    set( _options )
    set( _oneValue TARGET )
    set( _multiValue SOURCES )
    cmake_parse_arguments( _ADD_LIB "${_options}" "${_oneValue}" "${_multiValue}" "${ARGN}" )
    add_library( ${_ADD_LIB_TARGET} ${ansak_lib_predicate} ${_ADD_LIB_SOURCES} )
    if (ANSAK_EFFECTIVE_CPP)
        target_compile_options( ${_ADD_LIB_TARGET} PRIVATE -Weffc++ )
    endif()
endfunction()

#    message( "ansak_add_library is ${ansak_add_library}" )
#    message( "TARGETNAME is ${TARGETNAME}" )
#    message( "TARGETSOURCE is ${TARGETSOURCE}" )
#    if( ansak STREQUAL TARGETNAME )
#        message( "Got the mudder-ship" )
#        _add_library( ansak objects )
#        # foreach member of _ansak_library_bits
#        # 
#    elseif( ( ansakConfig STREQUAL TARGETNAME ) OR
#            ( ansakString STREQUAL TARGETNAME ) OR
#            ( ansakFileTools STREQUAL TARGETNAME ) OR
#            ( ansakFile STREQUAL TARGETNAME ) OR
#            ( ansakPrimitives STREQUAL TARGETNAME ) OR
#            ( ansakOSLayer STREQUAL TARGETNAME ) )
#        _add_library( ${TARGETNAME} ${ansak_lib_predicate} "${TARGETSOURCE}" )
#    elseif( ansakSqlite STREQUAL TARGETNAME )
#        message( "Got the latest and greatest piece." )
#    elseif( NOT "" STREQUAL TARGETNAME )
#        message( FATAL ": Got unknown parameter to ansak_add_library: '${TARGETNAME}. Error, aborting." )
#    endif()
#endfunction()
#
#function( ansak_add_overall_library _targetName )
#    _add_library( _targetName, _ansak_library_bits )
#endfunction()

function( ansak_add_mock_library )
    set( _options )
    set( _oneValue TARGET )
    set( _multiValue SOURCES )
    cmake_parse_arguments( _ADD_MOCK_LIB "${_options}" "${_oneValue}" "${_multiValue}" "${ARGN}" )
    add_library( ${_ADD_MOCK_LIB_TARGET} STATIC ${_ADD_MOCK_LIB_SOURCES} )
endfunction()
