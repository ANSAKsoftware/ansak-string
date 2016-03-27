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
# On MS-Windows, this project requires VS2012 or later to use it, requires VS2015
# or later to run the Unit Tests (If you use CLANG your mileage will CERTAINLY vary.

# On MS-Windows, in order to run CPPUNIT, install the binaries compatible with your
# compiler and architecture and include definitions for CPPUNIT_INCLUDE_DIRS to point
# to your CPPUnit header files and CPPUNIT_LIBRARY_DIRS to point to your CPPUnit binary
# files on the cmake command line, like this:
#    -DCPPUNIT_INCLUDE_DIRS=<path to your cppunit install's include files>
#    -DCPPUNIT_LIBRARY_DIRS=<path to your cppunit install's lib files>

# If you have gcov and lcov installed on a Linux system, code coverage is provided via
#   cmake -DCMAKE_BUILD_TYPE=Coverage for this target to work properly

cmake_minimum_required(VERSION 3.1)

list( INSERT CMAKE_MODULE_PATH
      0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

project(ansakString)

include( CTest )
if (CMAKE_BUILD_TYPE STREQUAL "Coverage" )
    if (NOT UNIX)
        message("Trying to build code coverage on a non-Linux system isn't going to end well.")
    endif()
    include( CodeCoverage )
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
target_include_directories( ansakString PRIVATE ${ansakString_privIncludes}
                                        PUBLIC interface )

if (WIN32)
    find_package( CPPUnit )
else()
    set(CPPUNIT_LIBRARY cppunit)
    set(CPPUNIT_FOUND TRUE)
endif()

if (CPPUNIT_FOUND)
    add_executable( mkUnicodeTestData mkUnicodeTestData/mkUnicodeTestData.cxx )
    target_link_libraries( mkUnicodeTestData PRIVATE ansakString )

    add_custom_command( OUTPUT "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                COMMAND "${CMAKE_CFG_INTDIR}/mkUnicodeTestData"
                        "${absBitsDir}/UnicodeData.txt"
                        "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                        COMMENT "Generating test data for charIsUnicode test"
                        DEPENDS "${absBitsDir}/UnicodeData.txt" mkUnicodeTestData
                        VERBATIM )

    add_executable( ansakStringTest source/unitTests/UnitTest.cxx
                                    source/unitTests/encode_predicate_test.cxx
                                    source/unitTests/string_splitjoin_test.cxx
                                    source/unitTests/string_tolower_test.cxx
                                    source/unitTests/string_test.cxx
                                    source/unitTests/string_with_predicate_test.cxx
                                    source/unitTests/char_is_unicode_test.cxx
                                    "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx")
    target_include_directories( ansakStringTest PRIVATE "${absBitsDir}" "${PROJECT_SOURCE_DIR}/source" "${PROJECT_BINARY_DIR}")
    target_link_libraries( ansakStringTest PRIVATE ansakString "${CPPUNIT_LIBRARY}" )
    add_test( NAME ansakStringTest COMMAND ansakStringTest )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Coverage" )
    SETUP_TARGET_FOR_COVERAGE(coverage ansakStringTest ansakStringCoverage)
endif()
