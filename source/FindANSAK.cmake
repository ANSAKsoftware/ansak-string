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
#                                                                               +--RelWithDebInfo
# synonyms for Win32: x86, i386, X86
# synonyms for x64: Win64, x86_64, x86-64, X64, X86_64, X86-64, amd64
# synonyms for ARM: Arm, arm
# synonyms for ARM64: Arm64, arm64
############################################################################

############################################################################
# announce settings if there were any
if(ANSAK_DIR)
    message(STATUS "var ANSAK_DIR is defined: ${ANSAK_DIR}")
endif()

if(NOT "$ENV{ANSAK_DIR}" STREQUAL "")
    message(STATUS "environment ANSAK_DIR is defined: $ENV{ANSAK_DIR}")
endif()

if(PROGRAM_DATA)
    message(STATUS "var PROGRAM_DATA is defined: ${PROGRAM_DATA}")
endif()

if(NOT "$ENV{PROGRAM_DATA}" STREQUAL "")
    message(STATUS "environment PROGRAM_DATA is defined: $ENV{PROGRAM_DATA}")
endif()
#
############################################################################

set(win32Dirs Win32 x86 i386 X86)
set(win64Dirs x64 Win64 x86-64 x86_64 X64 X86_64 X86-64 amd64)
set(armDirs ARM Arm arm)
set(arm64Dirs ARM64 Arm64 arm64)
function(platform_matches_dirname dirname result)
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM")
        set(theList ${armDirs})
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(theList ${arm64Dirs})
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(theList ${win32Dirs})
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(theList ${win64Dirs})
    else()
        message(WARNING "Did not find ${dirname} -- do you need to pass in its contents (dollar-curly-brace)?")
        set(${result} False PARENT_SCOPE)
    endif()

    list(FIND theList ${dirname} index)
    if(index GREATER_EQUAL 0)
        set(${result} True PARENT_SCOPE)
    else()
        set(${result} False PARENT_SCOPE)
    endif()
endfunction()

function(mirror_value var value doc)
    string(TOUPPER "${var}" upper_var)
    set(${upper_var} ${value} CACHE FILEPATH "${doc}")
endfunction()

function(find_win_lib find_var signal_library doc_string)
    if(ARGN)
        list(GET ARGN 0 cand)
        message(DEBUG "Provided extra value, ${cand} to be searched for ${signal_library}, to be noted in ${find_var}")
        find_library(${find_var} ${signal_library}
                                HINTS "${cand}/lib"
                                      "${cand}/lib/Debug"
                                  DOC "${doc_string}"
                     REQUIRED NO_DEFAULT_PATH)
        if(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM")
            find_library(${find_var} ${signal_library}
                                    HINTS "${cand}/lib/ARM/Debug"       # other options are case insensitive
                                      DOC "${doc_string}"
                          REQUIRED NO_DEFAULT_PATH)
        elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
            find_library(${find_var} ${signal_library}
                                    HINTS "${cand}/lib/ARM64/Debug"     # other options are case insensitive
                                      DOC "${doc_string}"
                         REQUIRED NO_DEFAULT_PATH)
        elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
            find_library(${find_var} ${signal_library}
                                    HINTS "${cand}/lib/Win32/Debug"
                                          "${cand}/lib/x86/Debug"       # windows, matches X86 as well
                                          "${cand}/lib/i386/Debug"
                                      DOC "${doc_string}"
                         REQUIRED NO_DEFAULT_PATH)
        elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
            find_library(${find_var} ${signal_library}
                                    HINTS "${cand}/lib/x64/Debug"       # windows, matches X64 as well
                                          "${cand}/lib/Win64/Debug"
                                          "${cand}/lib/x86_64/Debug"    # windows, matches X86_64 as well
                                          "${cand}/lib/x86-64/Debug"    # windows, matches X86-64 as well
                                          "${cand}/lib/amd64/Debug"
                                      DOC "${doc_string}"
                         REQUIRED NO_DEFAULT_PATH)
        endif()
        message(VERBOSE "${find_var} searched for in ${cand} sub-directories. Result: ${${find_var}}")
    endif()
endfunction()

############################################################################
# search for ansak-string library, by variable, by environment by default places
if(UNIX OR MINGW OR CYGWIN)
    set(_srch_string_lib libansakString.a)
    if(ANSAK_DIR)
      find_library(ANSAK_STRING_LIB ${_srch_string_lib} HINTS "${ANSAK_DIR}/lib" 
                                                          DOC "ANSAK string library"
                                                          REQUIRED)
    elseif(NOT "$ENV{ANSAK_DIR}" STREQUAL "")
      find_library(ANSAK_STRING_LIB ${_srch_string_lib} HINTS "$ENV{ANSAK_DIR}/lib"
                                                          DOC "ANSAK string library"
                                                          REQUIRED)
    else()
      # searches in places like /usr/... and /usr/local/... as last resort
      find_library(ANSAK_STRING_LIB ${_srch_string_lib} DOC "ANSAK string library" REQUIRED)
    endif()
    set(_srch_lib libansak.a)
    set(_srch_sqlite_lib libansakSqlite.a)
else()
    set(_srch_string_lib ansakString.lib)
    find_win_lib(ANSAK_STRING_LIB ${_srch_string_lib} "ANSAK string library" "${ANSAK_DIR}")
    find_win_lib(ANSAK_STRING_LIB ${_srch_string_lib} "ANSAK string library" "$ENV{ANSAK_DIR}")
    find_win_lib(ANSAK_STRING_LIB ${_srch_string_lib} "ANSAK string library" "${PROGRAM_DATA}")
    find_win_lib(ANSAK_STRING_LIB ${_srch_string_lib} "ANSAK string library" "$ENV{PROGRAM_DATA}")
    find_win_lib(ANSAK_STRING_LIB ${_srch_string_lib} "ANSAK string library" C:/ProgramData)

    set(_srch_lib ansak.lib)
    set(_srch_sqlite_lib ansakSqlite.lib)
endif()

if(_ansak_find_lib STREQUAL "_ansak_find_lib-NOTFOUND")
    message(STATUS "ANSAK_STRING_LIB not found.")
    return()
endif()
#
############################################################################

############################################################################
# develop the location of _ansak_root -- it'll be some parent of the locator
get_filename_component(_ansak_locator_dir "${ANSAK_STRING_LIB}" DIRECTORY)
set(_ansak_root "${_ansak_locator_dir}")

# Case I, II, III?
get_filename_component(_locator_dirname "${_ansak_root}" NAME)
if(_locator_dirname STREQUAL "Debug")
    # Case II or III -- directory of ANSAK_STRING_LIB is .../lib/Debug or .../lib/<arch>/Debug
    set(_split_libs 1)
    # traverse up level, save that point for finding libraries later
    get_filename_component(_ansak_root "${_ansak_root}" DIRECTORY)
    set(_split_root_dir "${_ansak_root}")
    get_filename_component(_locator_parent_dirname "${_ansak_root}" NAME)
    # _ansak_locator_dir is .../lib or .../lib/<arch>
    # Windows: is it lib
    if(_locator_parent_dirname STREQUAL "lib")
        # Case II
        # _ansak_root is .../lib, one more parent gives us _ansak_root
        # traverse up one more for _ansak_root
        get_filename_component(_ansak_root "${_ansak_root}" DIRECTORY)
    # Windows: it'll be conditioned to the generator architecture
    else()
        # _up_one_lib_name is an <arch> tag, maybe?
        platform_matches_dirname(${_locator_parent_dirname} itsMyPlatform)
        if(itsMyPlatform)
            # yes it is
            # Case III -- _ansak_locator_dir points to ${_ansak_root}/../<arch>/Debug
            set(_arch_tag ${_locator_parent_dirname})
            get_filename_component(_ansak_root "${_ansak_root}" DIRECTORY)
            get_filename_component(_locator_grandparent_dirname "${_ansak_root}" NAME)
            # _ansak_locator_dir is now ./lib/<arch>/Debug, right?
            if(NOT _locator_grandparent_dirname STREQUAL "lib")
                # grudgingly accepting the ambiguity
                message(STATUS "ANSAK build-type component doubtful from the found library: ${ANSAK_STRING_LIB}")
            endif()
            # traverse up one more for _ansak_root
            get_filename_component(_ansak_root "${_ansak_root}" DIRECTORY)
        else()
            # _ansak_root is now .../lib, right? if not, warn
            # grudgingly accepting the ambiguity
            message(STATUS "ANSAK component location in doubt, libraries are not in a ./lib, ./lib/Debug, ./lib/<arch>/Debug Directory")
            message(STATUS "using ${_ansak_root} to search for include-headers")
        endif()
    endif()
else()
    # Case I, regardless of platform, if not './lib', mention that
    set(_split_libs 0)
    # traverse up level
    get_filename_component(_ansak_root "${_ansak_root}" DIRECTORY)
    if(NOT _locator_dirname STREQUAL "lib")
        message(STATUS "ANSAK component location may be wrong: libraries are not in a ./lib directory: ${ANSAK_STRING_LIB}")
        message(STATUS "using ${_ansak_root} to search for include-headers")
    endif()
endif()
# _ansak_root points to ${ANSAK_DIR}, $ENV{ANSAK_DIR}, /usr, /usr/local on Linux
# _ansak_root points to ${ANSAK_DIR}, $ENV{ANSAK_DIR}, ${PROGRAM_DATA}, $ENV{PROGRAM_DATA} or C:/ProgramData on Windows
#      .... single value wherever it started
# ANSAK_STRING_LIB points to libansakString.a / ansakString.lib in its directory
# _ansak_locator_dir points to ${_ansak_root}/lib on Linux
# _ansak_locator_dir points to whatever dir ANSAK_STRING_LIB is in, probably
#      ${_ansak_root}/lib on Linux, some Windows
#      ${_ansak_root}/lib/Debug, ${_ansak_root}/lib/${_arch tag}/Debug on Windows
#      or some other variant
############################################################################

############################################################################
# how many "ansak" components to we have?
#     string.hxx            --> ansak-string
#     runtime_exception.hxx --> ansak-lib (with or without SQLite3
#     sqlite_exception.hxx  --> ansak-lib-sqlite3 (must Sqlite3 must be found)

# find signal include files
set(_include_dir "${_ansak_root}/include/ansak")
find_file(ANSAK_STRING_HEADER "string.hxx" HINTS "${_include_dir}")
find_file(ANSAK_LIB_HEADER "runtime_exception.hxx" HINTS "${_include_dir}")
find_file(ANSAK_SQLITE_HEADER "sqlite_exception.hxx" HINTS "${_include_dir}")

# if no string.hxx file, there's nothing more to find
if(ANSAK_STRING_HEADER STREQUAL "ANSAK_STRING_HEADER-NOTFOUND")
    set(ANSAK_FOUND False CACHE BOOL "ANSAK components not found.")
    message(STATUS "ANSAK string header not found. Giving up.")
    return()
    # early exit
    ########################################################################
endif()

# we have the string library and the header -- ready to compile/link something
set(ANSAK_FOUND True CACHE BOOL "ANSAK components found.")
get_filename_component(_file_dir "${ANSAK_STRING_HEADER}" DIRECTORY)  # ".../ansak/string.hxx --> .../ansak
get_filename_component(_include_path "${_file_dir}" DIRECTORY)        # location that has ansak dir in it
# for standard Unix-style locations, ANSAK_INCLUDE is blank and won't add -I elements
if(_include_path STREQUAL "/usr/include" OR _include_path STREQUAL "/usr/local/include")
    message(VERBOSE "Derived include path, ${_include_path}, is a standard location, leaving ANSAK_INCLUDE unset")
else()
    message(VERBOSE "Derived include path is a non-standard location, setting ANSAK_INCLUDE to \"${_include_path}\"")
    set(ANSAK_INCLUDE "${_include_path}" CACHE PATH "ANSAK include root")
endif()

# we assume that runtime_exception.hxx and sqlite_exception.hxx, if they exist,
# are in the same directory

# we have the main library's header, do we have the lib?
if(NOT ANSAK_LIB_HEADER STREQUAL "ANSAK_LIB_HEADER-NOTFOUND")
    find_library(ANSAK_LIB _srch_lib PATHS _ansak_locator_dir DOC "ANSAK general library")
    if(ANSAK_LIB STREQUAL "ANSAK_LIB-NOTFOUND")
        set(ANSAK_LIB_HEADER "ANSAK_LIB_HEADER-NOTFOUND" CACHE STRING "Library not present, ignoring header" FORCE)
        set(ANSAK_LIB_FOUND False CACHE BOOL "ANSAK library not found.")
        # if we have the library but not the header, no point...
        ####################################################################
    else()
        set(ANSAK_LIB_FOUND True CACHE BOOL "ANSAK library found.")
    endif()
else()
    set(ANSAK_LIB_FOUND False CACHE BOOL "ANSAK library not found.")
endif()

# if we have the sqlite library header, do we have SQLite3 and the lib?
if(SQLite3_FOUND)
    if(NOT ANSAK_SQLITE_HEADER STREQUAL "ANSAK_SQLITE_HEADER-NOTFOUND")
        find_library(ANSAK_SQLITE_LIB _srch_sqlite_lib PATHS _ansak_locator_dir DOC "ANSAK SQLite library")
        if(ANSAK_SQLITE_LIB STREQUAL "ANSAK_SQLITE_LIB-NOTFOUND")
            set(ANSAK_SQLITE_HEADER "ANSAK_SQLITE_HEADER-NOTFOUND" CACHE STRING "SQLite library not present, ignoring header" FORCE)
            set(ANSAK_SQLITE_FOUND False CACHE  BOOL "ANSAK SQLite library not found.")
            # if we have the library but not the header, no point...
            ################################################################
        else()
            set(ANSAK_SQLITE_FOUND True CACHE  BOOL "ANSAK SQLite library found.")
        endif()
    endif()
elseif(NOT ANSAK_SQLITE_HEADER STREQUAL "ANSAK_SQLITE_HEADER-NOTFOUND")
    message(STATUS "SQLite3 not found, ignoring ANSAK sqlite feature.")
    set(ANSAK_SQLITE_HEADER "ANSAK_SQLITE_HEADER-NOTFOUND" CACHE STRING "SQLite3 not found, ignoring header" FORCE)
    set(ANSAK_SQLITE_FOUND False CACHE  BOOL "SQLite3 is not found, ANSAK SQLite library is irrelevant.")
    # if we have the header, but SQLite3 is not found, no point
    ########################################################################
else()
    set(ANSAK_SQLITE_FOUND False CACHE  BOOL "Neither ANSAK SQLite library nor SQLite3 found.")
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

set(_comp_found "")
if(ANSAK_FOUND)
    list(APPEND _comp "ansak-string")
    message(DEBUG "ANSAK_STRING_LIB is ${ANSAK_STRING_LIB}")
endif()
if(ANSAK_LIB_FOUND)
    list(APPEND _comp "ansak-lib")
    message(DEBUG "ANSAK_LIB is ${ANSAK_LIB}")
endif()
if(ANSAK_SQLITE_FOUND)
    list(APPEND _comp "ansak-sqlite")
    message(DEBUG "ANSAK_SQLITE_LIB is ${ANSAK_SQLITE_LIB}")
endif()
message(STATUS "ANSAK components found: ${_comp}")

if(UNIX OR MINGW OR CYGWIN)
    ########################################################################
    # link ANSAK_${CMAKE_BUILD_TYPE}_LIB variables to ANSAK_LIB ones
    if (CMAKE_BUILD_TYPE)
        mirror_value("ANSAK_STRING_${CMAKE_BUILD_TYPE}_LIB" "${ANSAK_STRING_LIB}" "ANSAK string library (${CMAKE_BUILD_TYPE}")
        if (ANSAK_LIB_FOUND)
            mirror_value("ANSAK_${CMAKE_BUILD_TYPE}_LIB" "${ANSAK_LIB}" "ANSAK general library (${CMAKE_BUILD_TYPE}")
        endif()
        if (ANSAK_SQLITE_FOUND)
            mirror_value("ANSAK_SQLITE_${CMAKE_BUILD_TYPE}_LIB" "${ANSAK_SQLITE_LIB}" "ANSAK sqlite library (${CMAKE_BUILD_TYPE}")
        endif()
    endif()
elseif(_split_libs)
    ########################################################################
    # set up the other variables for _split_libs case
    if(_arch_tag)
        message(VERBOSE "ANSAK components per DEBUG, RELEASE, RELWITHDEBINFO looked for separately by architecture tag, ${_arch_tag}.")
    else()
        message(VERBOSE "ANSAK components per DEBUG, RELEASE, RELWITHDEBINFO looked for separately.")
    endif()
    # ansak-string
    find_library(ANSAK_STRING_DEBUG_LIB ${_srch_string_lib} HINTS "${_split_root_dir}/Debug" DOC "ANSAK string library (Debug)")
    find_library(ANSAK_STRING_RELEASE_LIB ${_srch_string_lib} HINTS "${_split_root_dir}/Release" DOC "ANSAK string library (Release)")
    find_library(ANSAK_STRING_RELWITHDEBINFO_LIB ${_srch_string_lib} HINTS "${_split_root_dir}/RelWithDebInfo" DOC "ANSAK string library (RelWithDebInfo)")
    message(DEBUG "ANSAK_STRING_(DEBUG|RELEASE|RELWITHDEBINFO)LIB is ${ANSAK_STRING_DEBUG_LIB};${ANSAK_STRING_RELEASE_LIB};${ANSAK_STRING_RELWITHDEBINFO_LIB};")
    # ansak-lib
    if(ANSAK_LIB_FOUND)
        find_library(ANSAK_DEBUG_LIB ${_srch_lib} HINTS "${_split_root_dir}/Debug" DOC "ANSAK general library (Debug)")
        find_library(ANSAK_RELEASE_LIB ${_srch_lib} HINTS "${_split_root_dir}/Release" DOC "ANSAK general library (Release)")
        find_library(ANSAK_RELWITHDEBINFO_LIB ${_srch_lib} HINTS "${_split_root_dir}/RelWithDebInfo" DOC "ANSAK general library (RelWithDebInfo)")
        message(DEBUG "ANSAK_(DEBUG|RELEASE|RELWITHDEBINFO)LIB is ${ANSAK_DEBUG_LIB};${ANSAK_RELEASE_LIB};${ANSAK_RELWITHDEBINFO_LIB};")
    endif()
    if(ANSAK_SQLITE_FOUND)
        # ansak-sqlite
        find_library(ANSAK_SQLITE_DEBUG_LIB ${_srch_sqlite_lib} HINTS "${_split_root_dir}/Debug" DOC "ANSAK sqlite library (Debug)")
        find_library(ANSAK_SQLITE_RELEASE_LIB ${_srch_sqlite_lib} HINTS "${_split_root_dir}/Release" DOC "ANSAK sqlite library (Release)")
        find_library(ANSAK_SQLITE_RELWITHDEBINFO_LIB ${_srch_sqlite_lib} HINTS "${_split_root_dir}/RelWithDebInfo" DOC "ANSAK sqlite library (RelWithDebInfo)")
        message(DEBUG "ANSAK_SQLITE_(DEBUG|RELEASE|RELWITHDEBINFO)LIB is ${ANSAK_SQLITE_DEBUG_LIB};${ANSAK_SQLITE_RELEASE_LIB};${ANSAK_SQLITE_RELWITHDEBINFO_LIB};")
    endif()
endif()
