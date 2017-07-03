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

# googletest and cmake, each in their wisdom have chosen incompatible default
# behaviours in the area of C-Runtime-as-DLL.
# CMake: "Use C-Runtime-via-DLL no matter what!"
# GoogleTest: "Use C-Runtime-as-static when you're statically linked, -via-DLL otherwise"
#
# It's inconvenient that Google would choose that behaviour but at least it's a
# default that can be over-ridden. To me, it makes sense to have the choice
# explicit at both points.

set( C_RUNTIME_CHOICE ambiguous )

if( WIN32 )

MACRO( SETUP_C_RUNTIME CHOICE )
        if ( ${CHOICE} STREQUAL "STATIC" )
            set( C_RUNTIME_CHOICE static )
            set( _src1 "/MD" )
            set( _src2 "-MD" )
            set( _dest "/MT" )
            option(
              gtest_force_shared_crt
              "Use shared (DLL) run-time lib even when Google Test is built as static lib."
              OFF)
            set(CompilerFlags
                    CMAKE_CXX_FLAGS
                    CMAKE_CXX_FLAGS_DEBUG
                    CMAKE_CXX_FLAGS_RELEASE
                    CMAKE_C_FLAGS
                    CMAKE_C_FLAGS_DEBUG
                    CMAKE_C_FLAGS_RELEASE
                    )
            foreach(CompilerFlag ${CompilerFlags})
              string(REPLACE ${_src1} ${_dest} ${CompilerFlag} "${${CompilerFlag}}")
              string(REPLACE ${_src2} ${_dest} ${CompilerFlag} "${${CompilerFlag}}")
            endforeach()
        elseif ( ${CHOICE} STREQUAL "DYNAMIC" )
            set( C_RUNTIME_CHOICE dynamic )
            option(
              gtest_force_shared_crt
              "Use shared (DLL) run-time lib even when Google Test is built as static lib."
              ON)
        else()
            message( FATAL_ERROR "Call SETUP_RUNTIME with DYNAMIC or STATIC, only." )
        endif()
ENDMACRO()

endif()
