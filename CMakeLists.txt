###########################################################################
#
# Copyright (c) 2015, Arthur N. Klassen
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

# This project requires CMake 3.1 or later with a C++11-compatible compiler.

# Select your desired version of unicode with the ANSAK_UNICODE_SUPPORT variable, e.g.:
#   cmake -DANSAK_UNICODE_SUPPORT=9      # to use the 9.0.0 version of Unicode
# ANSAK_UNICODE_SUPPORT defualts to 9.

# Be sure to update the gtest sub-module so the tests will work!

# If you have gcov and lcov installed on a Linux system, code coverage is provided via
#   cmake -DCMAKE_BUILD_TYPE=Coverage for this target to work properly

# On MS-Windows, this project requires VS2012 or later to use it, requires VS2015
# or later to run the Tests (If you use CLANG your mileage will CERTAINLY vary.)
# Tests will be suppressed for VS2012 (some C++11 features used for tests are
# incompatible with the VS2012 compiler).

cmake_minimum_required(VERSION 3.1)

project(ansakLib)

## part of ansak multi-part project? ####

set( _ansakRoot 0 )
if( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
    set( _ansakRoot 1 )
endif()

if( ${_ansakRoot} )
    list( INSERT CMAKE_MODULE_PATH
          0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )
endif()

# Choose Unicode version to support
set( DEFAULT_SUPPORTED_UNICODE_VERSION "9" )
if( ANSAK_UNICODE_SUPPORT )
    if( NOT ( ( ANSAK_UNICODE_SUPPORT STREQUAL "7" ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL "8" ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL "9" ) ) )
        message( "Unicode version, ${ANSAK_UNICODE_SUPPORT}, is not supported. Reverting to ${DEFAULT_SUPPORTED_UNICODE_VERSION}" )
        set( ANSAK_UNICODE_SUPPORT ${DEFAULT_SUPPORTED_UNICODE_VERSION} )
    endif()
else()
    message( "No Unicode version selected. Defaulting to ${DEFAULT_SUPPORTED_UNICODE_VERSION}" )
    set( ANSAK_UNICODE_SUPPORT ${DEFAULT_SUPPORTED_UNICODE_VERSION} )
endif()
set( unicodeSubDir bits${ANSAK_UNICODE_SUPPORT} )

# Choose unit-tests or not (default on), Coverage or not, based on target platform
set( _useTests 1 )

set( _ansakBuildType "notCoverage")
if( CMAKE_BUILD_TYPE )
    set(_ansakBuildType "${CMAKE_BUILD_TYPE}")
endif()

if( CMAKE_GENERATOR MATCHES "Visual Studio" )
    if( _ansakBuildType STREQUAL "Coverage" )
        message( FATAL_ERROR "Coverage is not available in Visual Studio Builds." )
    endif()
    if( _useTests AND ${CMAKE_GENERATOR} STRLESS "Visual Studio 14 2015" )
        message( "Tests require Visual Studio 2015 or later. Suppressing..." )
        set( _useTests 0 )
    endif()
endif()

include( WindowsCRuntime )
SETUP_C_RUNTIME( DYNAMIC )

# Choose basic compiler options
if( UNIX )
    set( CMAKE_CXX_STANDARD 11 )
    set( CMAKE_CXX_STANDARD_REQUIRED ON )
    add_compile_options( -Wall -Wextra -Werror )
    if( COVERAGE )
        add_compile_options( -O0 -fprofile-arcs -ftest-coverage )
        SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov" )
    endif()
    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
        add_compile_options( -Wno-missing-field-initializers )
    endif()
endif()

if( ${_ansakBuildType} STREQUAL "Coverage" )
    if( ${_useTests} EQUAL 0 )
        message( "Coverage builds require tests to be active." )
        set( _useTests 1 )
    endif()
endif()

if( _useTests )
    if( ${_ansakRoot} ) 
        include( CTest )
        if( _ansakBuildType STREQUAL "Coverage" )
            if( NOT UNIX )
                message( "Trying to build code coverage on a non-Linux system isn't going to end well." )
            endif()
            include( CodeCoverage )
        endif()
        add_subdirectory( submods )
    endif()
endif()

# Bring in the source and test directories
add_subdirectory( source )
if( _useTests )
    add_subdirectory( mock )
    add_subdirectory( test )
endif()
