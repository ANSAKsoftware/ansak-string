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

cmake_minimum_required(VERSION 3.1)

list( INSERT CMAKE_MODULE_PATH
      0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

project(ansakString)

include( CTest )

if (UNIX)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_library( ansakString STATIC src/string.cxx
                                src/string_tolower.cxx
                                src/string_toutf8.cxx
                                src/encoding_check_predicate.cxx
                                src/bits/char_to_lower.cxx
                                src/bits/char_is_unicode.cxx
                                src/string.hxx
                                src/string_splitjoin.hxx
                                src/bits/string_internal.hxx )

target_include_directories( ansakString PRIVATE src/bits
                                        PUBLIC src )

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
                        "${PROJECT_SOURCE_DIR}/src/unitTests/Unicode8Data.txt"
                        "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                        COMMENT "Generating test data for charIsUnicode test"
                        DEPENDS "${PROJECT_SOURCE_DIR}/src/unitTests/Unicode8Data.txt" mkUnicodeTestData
                        VERBATIM )

    add_executable( ansakStringTest src/unitTests/UnitTest.cxx
                                    src/unitTests/encode_predicate_test.cxx
                                    src/unitTests/string_splitjoin_test.cxx
                                    src/unitTests/string_tolower_test.cxx
                                    src/unitTests/string_test.cxx
                                    src/unitTests/string_with_predicate_test.cxx
                                    src/unitTests/char_is_unicode_test.cxx
                                    "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx")
    target_include_directories( ansakStringTest PRIVATE "${CPPUNIT_INCLUDE_DIR}" "${PROJECT_BINARY_DIR}")
    target_link_libraries( ansakStringTest PRIVATE ansakString "${CPPUNIT_LIBRARY}" )
    add_test( NAME ansakStringTest COMMAND ansakStringTest )
endif()

