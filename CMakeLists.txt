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

# For the poor sods who are stuck on earlier cmakes comment out "3.1" and
# uncomment the other comments below this one.
cmake_minimum_required(VERSION 3.1)
# cmake_minimum_required(VERSION 2.8)

project(ansakString)

include( CTest )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

# target_compile_options( ansakString PRIVATE "-std=c++11" )
# target_compile_options( ansakString PRIVATE "-Wall" "-Wextra" "-Werror" )

add_executable( mkUnicodeTestData mkUnicodeTestData/mkUnicodeTestData.cxx )
target_link_libraries( mkUnicodeTestData PRIVATE ansakString )

add_custom_command( OUTPUT "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
            COMMAND ./mkUnicodeTestData "${PROJECT_SOURCE_DIR}/src/unitTests/Unicode8Data.txt" char_is_unicode_test_data.hxx
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                    COMMENT "Generating test data for charIsUnicode test"
                    DEPENDS "${PROJECT_SOURCE_DIR}/src/unitTests/Unicode8Data.txt"
                    VERBATIM )

add_executable( ansakStringTest src/unitTests/UnitTest.cxx
                                src/unitTests/encode_predicate_test.cxx
                                src/unitTests/string_splitjoin_test.cxx
                                src/unitTests/string_tolower_test.cxx
                                src/unitTests/string_test.cxx
                                src/unitTests/string_with_predicate_test.cxx
                                src/unitTests/char_is_unicode_test.cxx
                                "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                                )
add_dependencies( ansakStringTest mkUnicodeTestData )
target_include_directories( ansakStringTest PRIVATE "${PROJECT_BINARY_DIR}" )

# target_compile_options( ansakStringTest PRIVATE "-std=c++11" )
target_link_libraries( ansakStringTest PRIVATE ansakString
                                               cppunit )
add_test( NAME ansakStringTest COMMAND ansakStringTest )
