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
#  configure.py -- Implement in Python3 what is unavailable on Windows
#                  (outside of MinGW / Cygwin), the minimal autoconf/automake-
#                  like behaviour of the config shell script provided for use
#                  in Linux / MinGW/Cygwin, macOS. For more, see
#
#                  python3 configure.py --help
#
# #########################################################################

import sys
import os
import os.path
import argparse
import pathlib
import re
from subprocess import STDOUT, PIPE
import subprocess

called_by = 'python'
compiler = None
v = False

DEFAULT_PREFIX = 'C:\\ProgramData'
DEFAULT_MAKENSIS_LOCATION = 'C:\\Program Files (x86)\\NSIS\\makensis.exe'
DEFAULT_UNICODE_VERSION = 15


def run_it(*args):
    try:
        p = subprocess.Popen(args, stdin=PIPE, stdout=PIPE, stderr=STDOUT)
        return [line.decode('utf-8') for line in p.stdout.readlines()]
    except FileNotFoundError:
        return None


def get_cmake_version():
    with open('CMakeLists.txt', 'r') as f:
        cmake_project_line = [line for line in f.readlines()
                              if line.startswith('project')][0]
    cmake_version_re = re.compile(r'project\([a-zA-Z]+ VERSION (.*)\)')
    cmake_version = cmake_version_re.findall(cmake_project_line)[0]
    return cmake_version


def find_generator():
    global v
    cmake_help_lines = run_it('cmake', '--help')
    if v:
        print("Read {} lines from cmake --help".format(len(cmake_help_lines)))
    studio_lines = [line.strip() for line in cmake_help_lines
                    if "Visual Studio" in line]
    if v:
        print("Read {} Visual Studio generator lines from cmake --help".format(
              len(studio_lines)))
    starred = [line for line in studio_lines if line.startswith('*')]
    if v:
        print("Read {} starred generator line(s) from cmake --help".format(
              len(starred)))
        for s in starred:
            print("   {}".format(s))

    if len(starred) == 0:
        print("No default Visual Studio generator. Install Visual Studio")
        sys.exit(1)
    elif len(starred) > 1:
        print("{} default Visual Studio generators? Something is wrong".format(
              len(starred)))
        sys.exit(1)
    else:
        star_line = starred[0][1:]
        star_left_half = star_line.split('=')[0].strip()
        star_before_bracket = star_left_half.split('[')[0].strip()
        if v:
            print("Default Visual Studio generator found: {}".format(
                  star_before_bracket))
        return star_before_bracket


def find_make_nsis(loc):
    nsis_lines = run_it(loc, '/VERSION')
    if not nsis_lines:
        if v:
            print("""No makensis was found at {}, 'make package' will not be
available.

MakeNSIS can be downloaded from https://nsis.sourceforge.io/Download -- choose
version 3 or later""".format(loc))
        return None
    nsis_output = nsis_lines[0].strip()
    if nsis_output[0] != 'v':
        print("Unrecognized output from makensis /VERSION: {}".format(
              nsis_output))
        return None
    version_string = nsis_output[1:]
    version_parts = version_string.split('.')
    try:
        if int(version_parts[0]) < 3:
            print("Earlier version of makensis, may not work.")
    except ValueError:
        print("Unrecognized version from makensis /VERSION: {}".format(
              version_string))
        return None
    if v:
        print("Makensis was found at {}, 'make package' will be available.".
              format(loc))
    return loc


def main(argv=sys.argv):
    global v
    run_it('git', 'submodule', 'update', '--init')

    parser = argparse.ArgumentParser(
            description="Configure script for ansak-string on Windows")
    parser.add_argument('--prefix',
                        help='non-default location to install files (does not '
                             'affect \'make package\')',
                        type=str,
                        default=DEFAULT_PREFIX)
    parser.add_argument('--compiler',
                        help='alternative C++ compiler to use',
                        type=str)
    parser.add_argument('--unicode-version',
                        help='Unicode Version to compile support for',
                        default=DEFAULT_UNICODE_VERSION,
                        type=int)
    parser.add_argument('--make-nsis',
                        help='location of package builder, NullSoft '
                             'Installation System',
                        type=str, default=DEFAULT_MAKENSIS_LOCATION)
    parser.add_argument('-v', '--verbose',
                        help='more detailed progress messages',
                        action='store_true')

    args = parser.parse_args()
    v = bool(args.verbose)

    # get PACKAGE_VERSION for make.py from CMakeLists.txt directive
    # 'project(ansakString VERSION xxxx)'
    package_version = get_cmake_version()

    # determine ... prefix, compiler, MSDev generator
    prefix = os.path.realpath(args.prefix)
    if not os.path.isdir(prefix):
        pathlib.Path(prefix).mkdir(parents=True, exist_ok=True)
    if not os.path.isdir(prefix):
        raise Exception("Prefix is not an available directory: {}".format(
                        prefix))
    if args.compiler:
        compiler = repr(args.compiler)
    else:
        compiler = None

    generator = find_generator()

    make_nsis = find_make_nsis(args.make_nsis)

    # write configvars.py with generator, prefix, compiler values
    with open('configvars.py', 'w') as configs:
        print('GENERATOR = {}'.format(repr(generator)), file=configs)
        print('PREFIX = {}'.format(repr(prefix)), file=configs)
        print('COMPILER = {}'.format(repr(compiler)), file=configs)
        print('MAKE_NSIS = {}'.format(repr(make_nsis)), file=configs)
        print('UNICODE_VERSION = {}'.format(args.unicode_version),
              file=configs)
        print('PACKAGE_VERSION = {}'.format(repr(package_version)),
              file=configs)
    if v:
        print('Created configvars.py file with values:')
        print('    GENERATOR = {}'.format(repr(generator)))
        print('    PREFIX = {}'.format(repr(prefix)))
        print('    COMPILER = {}'.format(repr(compiler)))
        print('    MAKE_NSIS = {}'.format(repr(make_nsis)))
        print('    UNICODE_VERSION = {}'.format(args.unicode_version))
        print('    PACKAGE_VERSION = {}'.format(repr(package_version)))

    # write make.cmd running python make.py %*
    with open('make.cmd', 'w') as makebat:
        print('@{} make.py %*'.format(called_by), file=makebat)


if __name__ == '__main__':
    if len(sys.argv) == 1:
        sys.argv.append('--help')
    elif not sys.argv[1].startswith('-'):
        called_by = sys.argv[1]
        replacement = [sys.argv[0]]
        replacement = replacement + sys.argv[2:]
        sys.argv = replacement
    main()
