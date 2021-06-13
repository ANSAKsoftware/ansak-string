###########################################################################
#
# Copyright (c) 2020, Arthur N. Klassen # All rights reserved.
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

message( "Looking for ANSAK components..." )
if( SQLite3_FOUND )
    message( " ... SQLite3 found, including SQLite interface components" )
endif()

#[[
FindANSAK
---------

Find ANSAK library elements ansak-string, ansak-lib, ansak-sqlite

For Linux or MacOS:
  * search will be by command-line variable, then environment variable, then /usr/include and /usr/lib
  * these files will be called libansakString.a, libansak.a and libansakSqlite.a

For Windows:
  * search will be by command-line variable, then environment variable
  * the files will be ansakStringXXX.lib, ansakXXX.lib and ansakSqliteXXX.lib

The environment variable or command line variable referred to is ANSAK_DIR

ANSAK_FOUND -- set true if any ANSAK components are found; only the string parts are standalone
               if false other variables listed here will be meaningless

ANSAK_LIB_FOUND -- set true if ANSAK string and generic library components are found
ANSAK_SQLITE_FOUND -- set true if ANSAK SQLite interface components are found

ANSAK_INCLUDE -- set to path containing an ansak/ sub-directory with ansak namespace headers inside it

Full paths of libraries:
ANSAK_STRING_LIB -- ANSAK string library (static link), where one library does it all
ANSAK_LIB -- ANSAK library (static link), where one library does it all
ANSAK_SQLITE_LIB -- ANSAK SQLite library (static link), where one library does it all

For DevStudio Generators, the picture is slightly murkier. See following comments.
]]#

############################################################################
# FOR WINDOWS:
#   * if a build-type tag is used as a sub-directory of lib, use values of the
#     form, ANSAK_RELEASE_LIB, ANSAK_DEBUG_LIB and ANSAK_RELWITHDEBINFO_LIB to
#     choose the appropriate library.
#     ANSAK_LIB will be set to match ANSAK_DEBUG_LIB
#     (this can become a generator expression in CMakeLists.txt files that use
#     ANSAK components)
#   * ANSAK_LIB will be set to match ANSAK_DEBUG_LIB on Unix-like systems
#   * if an architecture tag is used as a sub-directory of lib, the values
#     Win32, x64, ARM and ARM64 and selected synonyms will be recognized and
#     the libraries will be searched for by architecture according to
#     CMAKE_GENERATOR_PLATFORM
#
# FOR NON-WINDOWS:
#   * If CMAKE_BUILD_TYPE is defined, ANSAK_${CMAKE_BUILD_TYPE}_LIB will be
#     set to the same value as ANSAK_LIB. Other "build types" will be undefined.
#
# The expected directory structure is one of :
# Case I  <ANSAK_DIR>     Case II <ANSAK_DIR> (Windows)  Case III <ANSAK_DIR> (Windows)
#              +--include              +--include                      +--include       <==== ANSAK_INCLUDE
#              |     +--ansak          |     +--ansak                  |     +--ansak
#              +--lib                  +--lib                          +--lib
#                                          +--Debug                        +--Win32 or x64 or ARM or ARM64
#                                          +--Release                           +--Debug
#                                          +--RelWithDebInfo                    +--Release
#                                          +--MinSizeRel                        +--RelWithDebInfo
#                                                                               +--MinSizeRel
# synonyms for Win32: x86, i386, X86
# synonyms for x64: Win64, x86_64, x86-64, X64, X86_64, X86-64, amd64
# synonyms for ARM: Arm, arm
# synonyms for ARM64: Arm64, arm64
#
# _LIB values will be generated for Visual Studio unless ANSAK_NO_GENERATOR_LIBS is
# set either in the environment, or in a CMake variable.
############################################################################

############################################################################
# announce settings if there were any
if( ANSAK_DIR )
    message( STATUS "var ANSAK_DIR is defined: ${ANSAK_DIR}" )
endif()

if( NOT "$ENV{ANSAK_DIR}" STREQUAL "" )
    message( STATUS "environment ANSAK_DIR is defined: $ENV{ANSAK_DIR}" )
endif()

if( PROGRAM_DATA )
    message( STATUS "var PROGRAM_DATA is defined: ${PROGRAM_DATA}" )
endif()

if( NOT "$ENV{PROGRAM_DATA}" STREQUAL "" )
    message( STATUS "environment PROGRAM_DATA is defined: $ENV{PROGRAM_DATA}" )
endif()

if( NOT "$ENV{ANSAK_NO_GENERATOR_LIBS}" STREQUAL "" OR ANSAK_NO_GENERATOR_LIBS )
    set( _no_generator_libs True )
else()
    set( _no_generator_libs False )
endif()

if ( ANSAK_DEBUG )
    message( "ANSAK_DIR=\"$ENV{ANSAK_DIR}/${ANSAK_DIR}\"" )
    message( "PROGRAM_DATA=\"$ENV{PROGRAM_DATA}/${PROGRAM_DATA}\"" )
    message( "ANSAK_NO_GENERATOR_LIBS=$ENV{ANSAK_NO_GENERATOR_LIBS}/${ANSAK_NO_GENERATOR_LIBS}" )
endif()
#
############################################################################

set( win32Dirs Win32 x86 i386 X86 )
set( win64Dirs x64 Win64 x86-64 x86_64 X64 X86_64 X86-64 amd64 )
set( armDirs ARM Arm arm )
set( arm64Dirs ARM64 Arm64 arm64 )
function( platform_matches_dirname dirname result )
    if( CMAKE_GENERATOR_PLATFORM STREQUAL "ARM" )
        set( theList ${armDirs} )
    elseif( CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64" )
        set( theList ${arm64Dirs} )
    elseif( CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" )
        set( theList ${win32Dirs} )
    elseif( CMAKE_GENERATOR_PLATFORM STREQUAL "x64" )
        set( theList ${win64Dirs} )
    else()
        message( WARNING "Did not find ${dirname} -- do you need to pass in its contents (dollar-curly-brace)?" )
        set( ${result} False PARENT_SCOPE )
    endif()

    list( FIND theList ${dirname} index )
    if( index GREATER_EQUAL 0 )
        set( ${result} True PARENT_SCOPE )
    else()
        set( ${result} False PARENT_SCOPE )
    endif()
endfunction()

function( mirror_value var value doc )
    string( TOUPPER "${var}" upper_var )
    set( ${upper_var} ${value} CACHE FILEPATH "${doc}" )
endfunction()

function( find_win_lib find_var signal_library doc_string )
    if( ARGN )
        list( GET ARGN 0 cand )
        if ( ANSAK_DEBUG )
            message( "Candidate, ${cand}, to be searched for ${signal_library}, to be noted in ${find_var}" )
        endif()
        find_library( ${find_var} ${signal_library}
                                 HINTS "${cand}/lib"
                                       "${cand}/lib/Debug"
                                   DOC "${doc_string}"
                      REQUIRED NO_DEFAULT_PATH )
        if( CMAKE_GENERATOR_PLATFORM STREQUAL "ARM" )
            find_library( ${find_var} ${signal_library}
                                     HINTS "${cand}/lib/ARM/Debug"      # other options are case insensitive
                                       DOC "${doc_string}"
                           REQUIRED NO_DEFAULT_PATH )
        elseif( CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64" )
            find_library( ${find_var} ${signal_library}
                                     HINTS "${cand}/lib/ARM64/Debug"    # other options are case insensitive
                                       DOC "${doc_string}"
                          REQUIRED NO_DEFAULT_PATH )
        elseif( CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" )
            find_library( ${find_var} ${signal_library}
                                     HINTS "${cand}/lib/Win32/Debug"
                                           "${cand}/lib/x86/Debug"      # windows, matches X86 as well
                                           "${cand}/lib/i386/Debug"
                                       DOC "${doc_string}"
                          REQUIRED NO_DEFAULT_PATH )
        elseif( CMAKE_GENERATOR_PLATFORM STREQUAL "x64" )
            find_library( ${find_var} ${signal_library}
                                     HINTS "${cand}/lib/x64/Debug"      # windows, matches X64 as well
                                           "${cand}/lib/Win64/Debug"
                                           "${cand}/lib/x86_64/Debug"   # windows, matches X86_64 as well
                                           "${cand}/lib/x86-64/Debug"   # windows, matches X86-64 as well
                                           "${cand}/lib/amd64/Debug"
                                       DOC "${doc_string}"
                          REQUIRED NO_DEFAULT_PATH )
        endif()
        if ( ANSAK_DEBUG )
            message( "${find_var} searched for in ${cand} sub-directories. Result: ${${find_var}}" )
        endif()
    endif()
endfunction()

############################################################################
# search for ansak-string library, by variable, by environment by default places
if( UNIX OR MINGW OR CYGWIN )
    set( _srch_string_lib libansakString.a )
    if ( ANSAK_DEBUG )
        message( "looking for ${_srch_string_lib}" )
    endif()
    if( ANSAK_DIR )
        find_library( _locator ${_srch_string_lib} HINTS "${ANSAK_DIR}/lib"
                                                   DOC "ANSAK string library"
                                                   REQUIRED )
    elseif( NOT "$ENV{ANSAK_DIR}" STREQUAL "" )
        find_library( _loctaor ${_srch_string_lib} HINTS "$ENV{ANSAK_DIR}/lib"
                                                   DOC "ANSAK string library"
                                                   REQUIRED )
    else()
        # searches in places like /usr/... and /usr/local/... as last resort
        find_library( _locator ${_srch_string_lib} DOC "ANSAK string library" REQUIRED )
    endif()
    set( _srch_lib libansak.a )
    set( _srch_sqlite_lib libansakSqlite.a )
else()
    set( _srch_string_lib ansakString.lib )
    if ( ANSAK_DEBUG )
        message( "looking for ${_srch_string_lib}" )
    endif()
    find_win_lib( _locator ${_srch_string_lib} "ANSAK string library" "${ANSAK_DIR}" )
    find_win_lib( _locator ${_srch_string_lib} "ANSAK string library" "$ENV{ANSAK_DIR}" )
    find_win_lib( _locator ${_srch_string_lib} "ANSAK string library" "${PROGRAM_DATA}" )
    find_win_lib( _locator ${_srch_string_lib} "ANSAK string library" "$ENV{PROGRAM_DATA}" )
    find_win_lib( _locator ${_srch_string_lib} "ANSAK string library" C:/ProgramData )

    if ( NOT _locator AND NOT CMAKE_GENERATOR_PLATFORM )
        message( SEND_ERROR "${_srch_string_lib} was not found with no platform specified. Should you have set the platform? -A ..." )
    endif()

    set( _srch_lib ansak.lib )
    set( _srch_sqlite_lib ansakSqlite.lib )
endif()

if( _locator STREQUAL "_locator-NOTFOUND" )
    message( SEND_ERROR "ANSAK_STRING_LIB not found." )
    return()
endif()
#
############################################################################

############################################################################
# develop the location of _ansak_root -- it'll be some parent of the locator

get_filename_component( _locator_dir "${_locator}" DIRECTORY )
set( _dirpath "${_locator_dir}" )
if( ANSAK_DEBUG )
    message( "location development: _locator='${_locator}';_locator_dir='${_locator_dir}'" )
    message( "location development: path='${_dirpath}'" )
endif()

# Case I, II, III?
get_filename_component( _dirname "${_dirpath}" NAME )
if( ANSAK_DEBUG )
    message( "location development: path='${_dirpath}';name='${_dirname}'" )
endif()

if( NOT _dirname STREQUAL "Debug" )
    # Case I, regardless of platform, if not './lib', mention that
    set( _split_libs 0 )
    # traverse up level
    get_filename_component( _dirpath "${_dirpath}" DIRECTORY )
    if( NOT _dirname STREQUAL "lib" )
        message( STATUS "ANSAK component location may be wrong: libraries are not in a ./lib directory: ${_locator}" )
        message( STATUS "using ${_dirpath} to search for include-headers" )
    endif()
    set( ANSAK_STRING_LIB "${_locator}" CACHE FILEPATH "ANSAK String Library" )
    set( _ansak_root "${_dirpath}" )
    if( ANSAK_DEBUG )
        message( "location development (I): root='${_ansak_root}'" )
    endif()
else() # _dirname STREQUAL "Debug"
    # Case II or III -- directory of _locator is .../lib/Debug or .../lib/<arch>/Debug
    set( _split_libs 1 )
    string( REGEX MATCH "^Visual Studio" _vs_match ${CMAKE_GENERATOR} )
    if ( _vs_match AND NOT _no_generator_libs )
        set( ANSAK_LIB_BY_GENERATOR True
            CACHE BOOL "ANSAK library variables will be set by generator expressions." )
        if( ANSAK_DEBUG )
            message( "Matching for visual studio." )
        endif()
    endif()

    # traverse up level, save that point for finding libraries later
    get_filename_component( _dirpath "${_dirpath}" DIRECTORY )
    set( _locator_parent_dir "${_dirpath}" )
    get_filename_component( _locator_parentdir_name "${_locator_parent_dir}" NAME )
    if( _vs_match )
        set( _gen_root "${_locator_parent_dir}" )
    endif()

    # _dirpath is .../lib or .../lib/<arch>
    # Windows: is it lib
    if( _locator_parentdir_name STREQUAL "lib" )
        # Case II
        # _dirpath is .../lib, one more parent gives us _ansak_root
        # traverse up one more for _ansak_root
        get_filename_component( _ansak_root "${_locator_parent_dir}" DIRECTORY )
        if( ANSAK_DEBUG )
            message( "location development (II): path='${_dirpath}'" )
        endif()
    # Windows: it'll be conditioned to the generator architecture
    else()
        # _up_one_lib_name is an <arch> tag, maybe?
        platform_matches_dirname( ${_locator_parentdir_name} itsMyPlatform )
        if( itsMyPlatform )
            # yes it is
            # Case III -- _locator_dir points to ${_ansak_root}/../<arch>/Debug
            set( _arch_tag ${_locator_parentdir_name} )
            get_filename_component( _dirpath "${_dirpath}" DIRECTORY )
            set( _locator_grandparent_dir "${_dirpath}" )
            get_filename_component( _locator_grandparentdir_name "${_locator_grandparent_dir}" NAME )
            # _locator_dir is .../lib/<arch>/Debug, so _locator_grandparentdir_name is "lib", right?
            if( NOT _locator_grandparentdir_name STREQUAL "lib" )
                # grudgingly accepting the ambiguity
                message( STATUS "ANSAK build-type component doubtful from the found library: ${_locator}" )
            endif()
            # traverse up one more for _ansak_root
            get_filename_component( _ansak_root "${_dirpath}" DIRECTORY )
            if( ANSAK_DEBUG )
                message( "location development (III): root='${_ansak_root}'" )
            endif()
        else()
            # Case II (mis-named)
            # grudgingly accepting the ambiguity
            set( _ansak_root "${_locator_parent_dir}" )
            if( ANSAK_DEBUG )
                message( "location development (mis-named II): root='${_ansak_root}'" )
            endif()
            message( STATUS "ANSAK component location in doubt, libraries are not in a ./lib, ./lib/Debug, ./lib/<arch>/Debug Directory" )
            message( STATUS "using ${_ansak_root} to search for include-headers" )
        endif()
    endif()
    if (_vs_match)
        set( ANSAK_STRING_LIB "${_locator_parent_dir}/$<CONFIG>/ansakString.lib" CACHE FILEPATH "ANSAK String Library" )
    else()
        set( ANSAK_STRING_LIB "${_locator}" CACHE FILEPATH "ANSAK String Library" )
    endif()
endif()
# _ansak_root points to ${ANSAK_DIR}, $ENV{ANSAK_DIR}, /usr, /usr/local on Linux
# _ansak_root points to ${ANSAK_DIR}, $ENV{ANSAK_DIR}, ${PROGRAM_DATA}, $ENV{PROGRAM_DATA} or C:/ProgramData on Windows
#      .... single value wherever it started
# ANSAK_STRING_LIB points to libansakString.a / ansakString.lib in its directory
# _locator_dir points to ${_ansak_root}/lib on Linux
# _locator_dir points to whatever dir ANSAK_STRING_LIB is in, probably
#      ${_ansak_root}/lib on Linux, some Windows
#      ${_ansak_root}/lib/Debug, ${_ansak_root}/lib/${_arch tag}/Debug on Windows
#      or some other variant
############################################################################

if( ANSAK_DEBUG )
    message( "location development done, _ansak_root='${_ansak_root}'" )
endif()

############################################################################
# how many "ansak" components to we have?
#     string.hxx            --> ansak-string
#     runtime_exception.hxx --> ansak-lib (with or without SQLite3
#     sqlite_exception.hxx  --> ansak-lib-sqlite3 (must Sqlite3 must be found)

# find signal include files
set( _include_dir "${_ansak_root}/include/ansak" )
find_file( ANSAK_STRING_HEADER "string.hxx" HINTS "${_include_dir}" )
find_file( ANSAK_LIB_HEADER "runtime_exception.hxx" HINTS "${_include_dir}" )
find_file( ANSAK_SQLITE_HEADER "sqlite_exception.hxx" HINTS "${_include_dir}" )

# if no string.hxx file, there's nothing more to find
if( ANSAK_STRING_HEADER STREQUAL "ANSAK_STRING_HEADER-NOTFOUND" )
    set( ANSAK_FOUND False CACHE BOOL "ANSAK components not found." )
    message( SEND_ERROR "ANSAK string header not found. Giving up." )
    return()
    # early exit
    ########################################################################
endif()

# we have the string library and the header -- ready to compile/link something
set( ANSAK_FOUND True CACHE BOOL "ANSAK components found." )
get_filename_component( _file_dir "${ANSAK_STRING_HEADER}" DIRECTORY )  # ".../ansak/string.hxx --> .../ansak
get_filename_component( _include_path "${_file_dir}" DIRECTORY )        # location that has ansak dir in it
# for standard Unix-style locations, ANSAK_INCLUDE is blank and won't add -I elements
if( _include_path STREQUAL "/usr/include" OR _include_path STREQUAL "/usr/local/include" )
    if( APPLE )
        message( WARNING "Derived path, ${_include_path}, will be assumed by macOS-clang to be from a MacOSX SDK." )
    else()
        message( VERBOSE "Derived include path, ${_include_path}, is a standard location, leaving ANSAK_INCLUDE unset" )
    endif()
else()
    message( VERBOSE "Derived include path is a non-standard location, setting ANSAK_INCLUDE to \"${_include_path}\"" )
    set( ANSAK_INCLUDE "${_include_path}" CACHE PATH "ANSAK include root" )
endif()

# we assume that runtime_exception.hxx and sqlite_exception.hxx, if they exist,
# are in the same directory

#  we have the main library's header, do we have the lib?
if( NOT ANSAK_LIB_HEADER STREQUAL "ANSAK_LIB_HEADER-NOTFOUND" )
    find_library( _platform_ansak_lib ${_srch_lib} HINTS "${_locator_dir}" DOC "ANSAK general library" )
    if( _platform_ansak_lib STREQUAL "_platform_ansak_lib-NOTFOUND" )
        set( ANSAK_LIB_HEADER "ANSAK_LIB_HEADER-NOTFOUND" CACHE STRING "Library not present, ignoring header" FORCE )
        set( ANSAK_LIB_FOUND False CACHE BOOL "ANSAK library not found." )
        # if we have the library but not the header, no point...
        ####################################################################
    else()
        set( ANSAK_LIB_FOUND True CACHE BOOL "ANSAK library found." )
        if ( _vs_match AND _split_libs )
            set( ANSAK_LIB "${_locator_parent_dir}/$<CONFIG>/ansak.lib" CACHE FILEPATH "ANSAK general Library" )
        else()
            set( ANSAK_LIB "${_platform_ansak_lib}" CACHE FILEPATH "ANSAK general Library" )
        endif()
    endif()
else()
    set( ANSAK_LIB_FOUND False CACHE BOOL "ANSAK library not found." )
endif()

# if we have the sqlite library header, do we have SQLite3 and the lib?
if( SQLite3_FOUND )
    if( NOT ANSAK_SQLITE_HEADER STREQUAL "ANSAK_SQLITE_HEADER-NOTFOUND" )
        find_library( _platform_ansak_sqlite_lib ${_srch_sqlite_lib} HINTS "${_locator_dir}" DOC "ANSAK SQLite library" )
        if( _platform_ansak_sqlite_lib STREQUAL "_platform_ansak_sqlite_lib-NOTFOUND" )
            set( ANSAK_SQLITE_HEADER "ANSAK_SQLITE_HEADER-NOTFOUND" CACHE STRING "SQLite library not present, ignoring header" FORCE )
            set( ANSAK_SQLITE_FOUND False CACHE  BOOL "ANSAK SQLite library not found." )
            # if we have the library but not the header, no point...
            ################################################################
        else()
            set( ANSAK_SQLITE_FOUND True CACHE  BOOL "ANSAK SQLite library found." )
            if ( _vs_match AND _split_libs )
              set( ANSAK_SQLITE_LIB "${_locator_parent_dir}/$<CONFIG>/ansakSqlite.lib" CACHE FILEPATH "ANSAK SQLite Library" )
            else()
                set( ANSAK_SQLITE_LIB "${_platform_ansak_sqlite_lib}" CACHE FILEPATH "ANSAK SQLite Library" )
            endif()
        endif()
    endif()
elseif( NOT ANSAK_SQLITE_HEADER STREQUAL "ANSAK_SQLITE_HEADER-NOTFOUND" )
    message( STATUS "SQLite3 not found, ignoring ANSAK sqlite feature." )
    set( ANSAK_SQLITE_HEADER "ANSAK_SQLITE_HEADER-NOTFOUND" CACHE STRING "SQLite3 not found, ignoring header" FORCE )
    set( ANSAK_SQLITE_FOUND False CACHE  BOOL "SQLite3 is not found, ANSAK SQLite library is irrelevant." )
    # if we have the header, but SQLite3 is not found, no point
    ########################################################################
else()
    set( ANSAK_SQLITE_FOUND False CACHE  BOOL "Neither ANSAK SQLite library nor SQLite3 found." )
    # we have no header and SQLite3 is not found, no point -- except to document
    ########################################################################
endif()
# unless string.hxx is in /usr/include or /usr/local/include
#     ANSAK_INCLUDE points to _ansak_root
#     maybe someday, if string.hxx is in c:/ProgramData/include/ansak/string.hxx or
#                                        ${PROGRAM_DATA}/include/ansak/string.hxx or
#                                        $ENV{PROGRAM_DATA}/include/ansak/string.hxx or
#     we'll be able to do the same to ANSAK_INCLUDE in those cases?
# ANSAK_STRING_LIB, ANSAK_LIB, ANSAK_SQLITE_LIB determined
# ANSAK_FOUND determined
############################################################################

message( "Looking for ANSAK components... done" )
if ( ANSAK_DEBUG )
    set( _comp_found "" )
    if( ANSAK_FOUND )
        list( APPEND _comp "ansak-string" )
        message( "ANSAK_STRING_LIB is ${ANSAK_STRING_LIB}" )
    endif()
    if( ANSAK_LIB_FOUND )
        list( APPEND _comp "ansak-lib" )
        message( "ANSAK_LIB is ${ANSAK_LIB}" )
    endif()
    if( ANSAK_SQLITE_FOUND )
        list( APPEND _comp "ansak-sqlite" )
        message( "ANSAK_SQLITE_LIB is ${ANSAK_SQLITE_LIB}" )
    endif()
    message( STATUS "ANSAK components found: ${_comp}" )
endif()
