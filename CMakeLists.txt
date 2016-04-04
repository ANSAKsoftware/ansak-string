# The author (Arthur N. Klassen) disclaims all copyright to this source code.
# In place of a legal notice, here is a blessing:
#
#    May you do good and not evil.
#    May you find forgiveness for yourself and forgive others.
#    May you share freely, never taking more than you give.
#
# The above text with a date will be included in headings for all the files in
# this repository. It would be considered a kindness if you would retain it in
# whatever environment you use it. Thank you.

# This project requires CMake 3.1 or later with a C++11-compatible compiler.

# Select your desired version of unicode with the ANSAK_UNICODE_SUPPORT variable, e.g.:
#   cmake -DANSAK_UNICODE_SUPPORT=9      # to use the 9.0.0d2 version of Unicode
# ANSAK_UNICODE_SUPPORT defualts to8.

# Be sure to update the gtest sub-module so the unit tests will work!

# If you have gcov and lcov installed on a Linux system, code coverage is provided via
#   cmake -DCMAKE_BUILD_TYPE=Coverage for this target to work properly

# On MS-Windows, this project requires VS2012 or later to use it, requires VS2015
# or later to run the Unit Tests (If you use CLANG your mileage will CERTAINLY vary.
# Define ANSAK_SUPPRESS_UNIT_TESTS=1 for VS2012

cmake_minimum_required(VERSION 3.1)

list( INSERT CMAKE_MODULE_PATH
      0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

project(ansakString)

if(ANSAK_SUPPRESS_UNIT_TESTS)
else()
    include( CTest )
    if (CMAKE_BUILD_TYPE STREQUAL "Coverage" )
        if (NOT UNIX)
            message("Trying to build code coverage on a non-Linux system isn't going to end well.")
        endif()
        include( CodeCoverage )
    endif()

    add_subdirectory( test/googletest )
endif()

set( DEFAULT_SUPPORTED_UNICODE_VERSION "8" )
if(ANSAK_UNICODE_SUPPORT)
    if ( NOT ( ( ANSAK_UNICODE_SUPPORT STREQUAL "7" ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL "8" ) OR
               ( ANSAK_UNICODE_SUPPORT STREQUAL "9" ) ) )
        message( "Unicode version, ${ANSAK_UNICODE_SUPPORT}, is not supported. Reverting to ${DEFAULT_SUPPORTED_UNICODE_VERSION}" )
        set( ANSAK_UNICODE_SUPPORT ${DEFAULT_SUPPORTED_UNICODE_VERSION} )
    endif()
else()
    message( "No Unicode version selected. Defaulting to ${DEFAULT_SUPPORTED_UNICODE_VERSION}" )
    set( ANSAK_UNICODE_SUPPORT ${DEFAULT_SUPPORTED_UNICODE_VERSION} )
endif()

if (UNIX)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    add_compile_options(-Wall -Wextra -Werror)
                        # -Wno-multichar -Wno-attributes -Wno-unknown-pragmas -Wno-switch )
    if (COVERAGE)
        add_compile_options(-O0 -fprofile-arcs -ftest-coverage)
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
    endif()
    if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
        add_compile_options(-Wno-missing-field-initializers)
    endif()
endif()

set( ansakString_intfc )
list( APPEND ansakString_intfc
                             interface/string.hxx
                             interface/string_splitjoin.hxx
    )

set( bitsDir source/bits${ANSAK_UNICODE_SUPPORT} )
set( absBitsDir "${PROJECT_SOURCE_DIR}/${bitsDir}" )

set( ansakString_src )
list( APPEND ansakString_src source/string.cxx
                             source/string_tolower.cxx
                             source/string_toutf8.cxx
                             source/encoding_check_predicate.cxx
                             source/string_internal.hxx
                             ${bitsDir}/char_to_lower.cxx
                             ${bitsDir}/char_is_unicode.cxx
    )

add_library( ansakString STATIC ${ansakString_src} )

set( ansakString_privIncludes )
list( APPEND ansakString_privIncludes ${bitsDir} source )
target_include_directories( ansakString PRIVATE ${ansakString_privIncludes} PUBLIC interface )

if(ANSAK_SUPPRESS_UNIT_TESTS)
else()
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
                                test/unit/encode_predicate_test.cxx
                                test/unit/string_splitjoin_test.cxx
                                test/unit/string_tolower_test${ANSAK_UNICODE_SUPPORT}.cxx
                                test/unit/string_with_predicate_test.cxx
                                test/unit/char_is_unicode_test.cxx
                                "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
              )
target_link_libraries( ansakStringTest PRIVATE ansakString gtest_main )
target_include_directories( ansakStringTest PRIVATE "$<TARGET_PROPERTY:ansakString,INCLUDE_DIRECTORIES>"
                                                    "${PROJECT_BINARY_DIR}"
                          )

add_test( NAME ansakStringTest COMMAND ansakStringTest )

if (CMAKE_BUILD_TYPE STREQUAL "Coverage" )
    SETUP_TARGET_FOR_COVERAGE(coverage ansakStringTest ansakStringCoverage)
endif()
endif()
