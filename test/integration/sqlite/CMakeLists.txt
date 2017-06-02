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

set( _target integrate-ansakSqlite )
set( _targetAsTest ansakSqliteIntegrationTest )
set( _targetPrivateLibs ansakSqlite ansakFileTools ansakFile ansakPrimitives ansakString ansakOSLayer )
set( _targetSource sqliteDBTest.cxx
                   sqlitePragmaTest.cxx
                   sqliteStmtTest.cxx
   )

add_executable( ${_target} ${_targetSource} )
target_link_libraries( ${_target}
               PRIVATE ${SQLITE3_LIBRARIES} ${_targetPrivateLibs} gtest_main gmock )
target_include_directories( ${_target} PRIVATE ../../../source/include ${SQLITE3_INCLUDE_DIR} )

SETUP_TO_EXECUTE_GTEST( ${_target} )
if( WIN32 )
  add_custom_command( TARGET ${_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${SQLITE3_DLL}"      ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
endif()

add_test( NAME ${_targetAsTest} COMMAND ${_target} )

