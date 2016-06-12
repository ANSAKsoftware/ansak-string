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
#   cmake -DANSAK_UNICODE_SUPPORT=9      # to use the 9.0.0d2 version of Unicode
# ANSAK_UNICODE_SUPPORT defualts to8.

# Be sure to update the gtest sub-module so the unit tests will work!

# If you have gcov and lcov installed on a Linux system, code coverage is provided via
#   cmake -DCMAKE_BUILD_TYPE=Coverage for this target to work properly

# On MS-Windows, this project requires VS2012 or later to use it, requires VS2015
# or later to run the Unit Tests (If you use CLANG your mileage will CERTAINLY vary.)
# Unit tests will be suppressed for VS2012 (some C++11 features used for those unit tests
# are incompatible with the VS2012 compiler).

# Unit tests are enabled but may be disabled by setting ANSAK_SUPPRESS_UNIT_TESTS=1 before
# calling add_subdirectory() to include it.

cmake_minimum_required(VERSION 3.1)

project(ansakString)

set( _ansakRoot 0 )
if( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
    set( _ansakRoot 1 )
endif()

if( ${_ansakRoot} )
    list( INSERT CMAKE_MODULE_PATH
          0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

    set( _useUnitTests 1 )
    if( ANSAK_SUPPRESS_UNIT_TESTS )
        set( _useUnitTests 0 )
    endif()
endif()

set( _ansakBuildType "notCoverage")
if( CMAKE_BUILD_TYPE )
    set(_ansakBuildType "${CMAKE_BUILD_TYPE}")
endif()

if( CMAKE_GENERATOR MATCHES "Visual Studio" )
    if( _ansakBuildType STREQUAL "Coverage" )
        message( FATAL_ERROR "Coverage is not available in Visual Studio Builds." )
    endif()
    if( _useUnitTests AND ${CMAKE_GENERATOR} STRLESS "Visual Studio 14 2015" )
        message( "Unit tests require Visual Studio 2015 or later. Suppressing..." )
        set( _useUnitTests 0 )
    endif()
endif()

if( ${_ansakBuildType} STREQUAL "Coverage" )
    if( ${_useUnitTests} EQUAL 0 )
        message( "Coverage builds require unit tests to be active." )
        set( _useUnitTests 1 )
    endif()
endif()

if( _useUnitTests )
    if( ${_ansakRoot} ) 
        include( CTest )
        if( _ansakBuildType STREQUAL "Coverage" )
            if( NOT UNIX )
                message( "Trying to build code coverage on a non-Linux system isn't going to end well." )
            endif()
            include( CodeCoverage )
        endif()

        if(NOT DEFINED ANSAK_HAVE_GOOGLE_TEST)
            add_subdirectory( test/googletest )
            set( ANSAK_HAVE_GOOGLE_TEST on )
        endif()
    endif()
endif()

set( DEFAULT_SUPPORTED_UNICODE_VERSION "8" )
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

if( UNIX )
    set( CMAKE_CXX_STANDARD 11 )
    set( CMAKE_CXX_STANDARD_REQUIRED ON )
    add_compile_options( -Wall -Wextra -Werror )
                        # -Wno-multichar -Wno-attributes -Wno-unknown-pragmas -Wno-switch )
    if( COVERAGE )
        add_compile_options( -O0 -fprofile-arcs -ftest-coverage )
        SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov" )
    endif()
    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
        add_compile_options( -Wno-missing-field-initializers )
    endif()
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

set( ansakString_privIncludes )
list( APPEND ansakString_privIncludes ${bitsDir} source )
target_include_directories( ansakString PRIVATE ${ansakString_privIncludes} PUBLIC interface )

if( _useUnitTests )
  add_executable( mkUnicodeTestData mkUnicodeTestData/mkUnicodeTestData.cxx )
  target_link_libraries( mkUnicodeTestData PRIVATE ansakString )

  add_custom_command( OUTPUT "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
              COMMAND "${CMAKE_CFG_INTDIR}/mkUnicodeTestData"
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
                                  test/unit/string_with_predicate_test.cxx
                                  test/unit/char_is_unicode_test.cxx
                                  "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                )
  target_include_directories( ansakStringTest PRIVATE "$<TARGET_PROPERTY:ansakString,INCLUDE_DIRECTORIES>"
                                                      "${PROJECT_BINARY_DIR}")
  target_link_libraries( ansakStringTest PRIVATE ansakString gtest_main )

  add_test( NAME ansakStringTest COMMAND ansakStringTest )

  if( _ansakBuildType STREQUAL "Coverage" )
      SETUP_TARGET_FOR_COVERAGE( coverage ansakStringTest ansakStringCoverage )
  endif()
endif()
