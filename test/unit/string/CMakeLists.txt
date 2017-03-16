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

add_custom_command( OUTPUT "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
          COMMAND "$<TARGET_FILE:mkUnicodeTestData>"
                  "${absUnicodeSubDir}/UnicodeData.txt"
                  "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
                  COMMENT "Generating test data for charIsUnicode test"
                  DEPENDS "${absUnicodeSubDir}/UnicodeData.txt" mkUnicodeTestData
                  VERBATIM )

add_executable( test-ansakString string_test.cxx
                              string_decode_utf8_test.cxx
                              encode_predicate_test.cxx
                              string_splitjoin_test.cxx
                              string_tolower_test${ANSAK_UNICODE_SUPPORT}.cxx
                              string_trim_test.cxx
                              string_with_predicate_test.cxx
                              char_is_unicode_test.cxx
                              "${PROJECT_BINARY_DIR}/char_is_unicode_test_data.hxx"
            )
target_include_directories( test-ansakString PRIVATE "${PROJECT_BINARY_DIR}"
                                                     ../../../source/include )
target_link_libraries( test-ansakString PRIVATE ansakString gtest_main )
SETUP_TO_EXECUTE_GTEST( test-ansakString )

add_test( NAME ansakStringTest COMMAND test-ansakString )
