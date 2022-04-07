#!/usr/bin/env python3

# #########################################################################
#
#  Copyright (c) 2022, Arthur N. Klassen
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
# #########################################################################
#
#  2022.02.20 - First version
#
#     May you do good and not evil.
#     May you find forgiveness for yourself and forgive others.
#     May you share freely, never taking more than you give.
#
# #########################################################################
#
#  project.py -- Project settings for ansakString in the configure.py and
#                make.py build scripting
#
# #########################################################################

import os

# test for configvars existence
try:
    import configvars
except ModuleNotFoundError:
    import configure
    configure.main(['configure.py'])
    import configvars

BUILD_TARGETS = ['ansakString']

BUILD_TEST_TARGETS = ['ansakSTringTest']

HEADERS_TO_INSTALL = [
    (os.path.join('interface', 'string.hxx'), 'include_ansak'),
    (os.path.join('interface', 'string_splitjoin.hxx'), 'include_ansak'),
    (os.path.join('interface', 'string_trim.hxx'), 'include_ansak')]

LIBS_TO_INSTALL = ['ansakString.lib']

PACKAGE_NAME = 'ansak-string-u{}-setup.exe'.format(configvars.UNICODE_VERSION)
