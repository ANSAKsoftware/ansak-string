###########################################################################
#
# Copyright (c) 2015-2025, Arthur N. Klassen
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
#   cmake -DANSAK_UNICODE_SUPPORT=11      # to use the 11.0.0 version of Unicode
# ANSAK_UNICODE_SUPPORT defaults to 15 (denoting Unicode 15).

# Be sure to update the gtest sub-module so the unit tests will work!

# If you have gcov and lcov installed on a Linux system, code coverage is provided via
#   cmake -DCMAKE_BUILD_TYPE=Coverage for this target to work properly

# On MS-Windows, this project requires VS2012 or later to use it, requires VS2015
# or later to run the Unit Tests (If you use CLANG your mileage will CERTAINLY vary.)
# Unit tests will be suppressed for VS2012 (some C++11 features used for those unit tests
# are incompatible with the VS2012 compiler).

cmake_minimum_required(VERSION 3.10)

project(ansakString VERSION 2.0.1)

## part of ansak multi-part project? ####

##############################################################################################################
set( _ansakRoot 0 )
if( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
    set( _ansakRoot 1 )
endif()
if(_ansakRoot)
    list( INSERT CMAKE_MODULE_PATH
          0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )
endif()

##############################################################################################################
set( _useUnitTests 0 )
if(TEST_ANSAKSTRING OR _ansakRoot)
  set( _useUnitTests 1 )
endif() 

set( _ansakBuildType "notCoverage")
if( CMAKE_BUILD_TYPE )
    string( TOLOWER "${CMAKE_BUILD_TYPE}" _ansakBuildType )
endif()

if( CMAKE_GENERATOR MATCHES "Visual Studio" )
    if( _ansakBuildType STREQUAL coverage )
        message( FATAL_ERROR "Coverage is not available in Visual Studio Builds." )
    endif()
    if( _useUnitTests AND ${CMAKE_GENERATOR} STRLESS "Visual Studio 14 2015" )
        message( "Unit tests require Visual Studio 2015 or later. Suppressing..." )
        set( _useUnitTests 0 )
    endif()
endif()

#*************************************************************************************************************
if( ${_ansakBuildType} STREQUAL coverage )
    if( NOT _useUnitTests )
        message( "Coverage builds require unit tests to be active." )
        set( _useUnitTests 1 )
    endif()
endif()
##############################################################################################################

##############################################################################################################
if( _ansakRoot )
    if ( ${_ansakBuildType} STREQUAL asan )
        include( FindASan )
    endif()
    if( _useUnitTests )
        include( CTest )
        if( _ansakBuildType STREQUAL coverage )
            if( NOT UNIX )
                message( "Trying to build code coverage on a non-Linux system isn't going to end well." )
            endif()
            include( CodeCoverage )
        endif()
        add_subdirectory( submods )
    endif()
endif()
##############################################################################################################

if( UNIX )
    set( _isGnu 1 )
    if( NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU )
        set( _isGnu 0 )
    endif()
    set( CMAKE_CXX_STANDARD 11 )
    set( CMAKE_CXX_STANDARD_REQUIRED ON )
    add_compile_options( -Wall -Wextra -Werror )
    if( COVERAGE )
        add_compile_options( -O0 -fprofile-arcs -ftest-coverage )
        SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov" )
    endif()
    if( _isGnu )
        add_compile_options( -Wno-missing-field-initializers )
    endif()
endif()

##############################################################################################################
set( DEFAULT_SUPPORTED_UNICODE_VERSION "15" )
if( ANSAK_UNICODE_SUPPORT )
    if( NOT ( ( ANSAK_UNICODE_SUPPORT STREQUAL 7 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 8 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 9 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 10 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 11 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 12 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 13 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 14 ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL 15 ) ) )
        message( "Unicode version, ${ANSAK_UNICODE_SUPPORT}, is not supported. Reverting to ${DEFAULT_SUPPORTED_UNICODE_VERSION}" )
        set( ANSAK_UNICODE_SUPPORT ${DEFAULT_SUPPORTED_UNICODE_VERSION} )
    endif()
else()
    message( "No Unicode version selected. Defaulting to ${DEFAULT_SUPPORTED_UNICODE_VERSION}" )
    set( ANSAK_UNICODE_SUPPORT ${DEFAULT_SUPPORTED_UNICODE_VERSION} )
endif()

set( ansakString_intfc )
list( APPEND ansakString_intfc
             interface/string.hxx
             interface/string_splitjoin.hxx
             interface/internal/string_decode_utf8.hxx )

set( bitsDir source/bits${ANSAK_UNICODE_SUPPORT} )
set( absBitsDir "${PROJECT_SOURCE_DIR}/${bitsDir}" )

set( ansakString_src )
list( APPEND ansakString_src source/string.cxx
                             source/string_tolower.cxx
                             source/string_toutf8.cxx
                             source/string_decode_utf8.cxx
                             source/encoding_check_predicate.cxx
                             source/string_internal.hxx
                             ${bitsDir}/char_to_lower.cxx
                             ${bitsDir}/char_is_unicode.cxx
    )

add_library( ansakString STATIC ${ansakString_src} )
if (UNIX)
    set( _modern 1 )
    if( "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 6.0 )
        set( _modern 0 )
    endif()
    if((NOT _isGnu) OR _modern)
        target_compile_options( ansakString PRIVATE -Weffc++ -fno-operator-names )
    else()
        message("Effective C++ warnings suppressed for older compiler: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_C_COMPILER_VERSION}")
    endif()
endif()

set( ansakString_privIncludes )
list( APPEND ansakString_privIncludes ${bitsDir} source )
target_include_directories( ansakString PRIVATE ${ansakString_privIncludes} PUBLIC interface )

##############################################################################################################
if( _useUnitTests )
    add_executable( mkUnicodeTestData mkUnicodeTestData/mkUnicodeTestData.cxx )
    target_link_libraries( mkUnicodeTestData PRIVATE ansakString )

    add_custom_command( OUTPUT "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                COMMAND "mkUnicodeTestData"
                        "${absBitsDir}/UnicodeData.txt"
                        "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                        COMMENT "Generating test data for charIsUnicode test"
                        DEPENDS "${absBitsDir}/UnicodeData.txt" mkUnicodeTestData
                        VERBATIM )

    add_executable( ansakStringTest test/unit/string_test.cxx
                                    test/unit/string_decode_utf8_test.cxx
                                    test/unit/encode_predicate_test.cxx
                                    test/unit/string_splitjoin_test.cxx
                                    test/unit/string_tolower_test${ANSAK_UNICODE_SUPPORT}.cxx
                                    test/unit/string_trim_test.cxx
                                    test/unit/string_with_predicate_test.cxx
                                    test/unit/string_wchar_tchar.cxx
                                    test/unit/string_wide_tchar.cxx
                                    test/unit/char_is_unicode_test.cxx
                                    "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                  )
    target_include_directories( ansakStringTest PRIVATE "$<TARGET_PROPERTY:ansakString,INCLUDE_DIRECTORIES>"
                                                        "${PROJECT_BINARY_DIR}")
    target_link_libraries( ansakStringTest PRIVATE ansakString gtest_main )
    if( WIN32 )
        add_custom_command( TARGET ansakStringTest POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest>      ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest_main> ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR})
    endif()

    add_test( NAME ansakStringTest COMMAND ansakStringTest )

    if( _ansakBuildType STREQUAL coverage )
        SETUP_TARGET_FOR_COVERAGE( ansak-string-coverage ansakStringTest ansakStringCoverage )
    endif()
endif()
##############################################################################################################

if( _ansakRoot )
    include(packaging.cmake)
endif()
