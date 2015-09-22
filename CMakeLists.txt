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
                                src/string.hxx
                                src/string_splitjoin.hxx
                                src/bits/string_internal.hxx )

target_include_directories( ansakString PRIVATE src/bits 
                                        PUBLIC src )

# target_compile_options( ansakString PRIVATE "-std=c++11" )
# target_compile_options( ansakString PRIVATE "-Wall" "-Wextra" "-Werror" )

add_executable( ansakStringTest src/unitTests/UnitTest.cxx
                                src/unitTests/string_splitjoin_test.cxx
                                src/unitTests/string_test.cxx )
# target_compile_options( ansakStringTest PRIVATE "-std=c++11" )
target_link_libraries( ansakStringTest PRIVATE ansakString 
                                               cppunit )
add_test( NAME ansakStringTest COMMAND ansakStringTest )
