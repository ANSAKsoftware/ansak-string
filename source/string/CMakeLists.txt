###########################################################################
#
# Copyright (c) 2017, Arthur N. Klassen
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

cmake_minimum_required(VERSION 3.1)

set( _target ansakString )
set( _interface ../../interface )
set( _privIncludes )
list( APPEND _privIncludes ${unicodeSubDir} ../include )
set( absUnicodeSubDir "${CMAKE_CURRENT_SOURCE_DIR}/${unicodeSubDir}"
         CACHE STRING "Sub-directory of unicode-version-specific sources" )

set( ansakString_src )
list( APPEND ansakString_src string.cxx
                             string_tolower.cxx
                             string_toutf8.cxx
                             string_decode_utf8.cxx
                             encoding_check_predicate.cxx
                             ${unicodeSubDir}/char_to_lower.cxx
                             ${unicodeSubDir}/char_is_unicode.cxx
    )

add_library( ${_target} STATIC ${ansakString_src} )
if (UNIX)
    target_compile_options( ${_target} PRIVATE -Weffc++ )
endif()
target_include_directories( ${_target} PRIVATE ${_privIncludes} PUBLIC ${_interface} )

