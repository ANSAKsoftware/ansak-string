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
#  make.py -- Implement in Python3 what is unavailable on Windows (outside of
#             MinGW / Cygwin), the minimal Makefile-like behaviour of all
#             install, uninstall and a few other things. For more, see
#
#             python3 make.py help
#
# #########################################################################

import sys
import os
import os.path
import argparse
import itertools
from pathlib import Path
import shutil
import subprocess

from project import BUILD_TARGETS, BUILD_TEST_TARGETS
from project import HEADERS_TO_INSTALL, LIBS_TO_INSTALL, PACKAGE_NAME

from configvars import GENERATOR, PREFIX, COMPILER, MAKE_NSIS
from configvars import UNICODE_VERSION, PACKAGE_VERSION


CMD = 'c:\\windows\\system32\\cmd.exe'
C = '/c'
CMAKE = 'cmake'
BUILD = '--build'
GRANDPARENT = '..\\..'
CONF = '-C'
GEN = '-G'
ARCH = '-A'


class Proc:

    def __init__(self, *args, consume=False, env=None, cwd=None):
        try:
            self.lines_ = []
            self.rc_ = None
            self.consume_ = consume
            extras = {'stdin': subprocess.PIPE, 'stdout': subprocess.PIPE,
                      'stderr': subprocess.STDOUT} if consume else {}
            if cwd:
                extras['cwd'] = cwd
            if env:
                env.update(os.environ)
                extras['env'] = env
            self.p_ = subprocess.Popen(args, **extras)
        except FileNotFoundError:
            self.p_ = None
            self.rc_ = 9009

    def rc(self):
        if self.rc_ is not None:
            return self.rc_
        if self.consume_:
            self.lines_ += self.p_.stdout.readlines()
        self.rc_ = self.p_.wait()
        return self.rc_

    def lines(self):
        if self.rc_ is None and self.consume_:
            self.lines_ += self.p_.stdout.readlines()
        return self.lines_

    def run(self):
        return self.rc()


def source_is_newer(than_file, other=None):
    if not os.path.isfile(than_file):
        return True

    # get changed-date of than_file
    than_file_stamp = os.path.getmtime(than_file)

    def newer(f):
        return os.path.getmtime(f) > than_file_stamp

    iter_these = [('configvars.py',), Path('.').rglob('CMakeLists.txt'),
                  Path('.').rglob('*[hc]xx')]
    if other is not None:
        iter_these.append(other)
    scan_these = itertools.chain(*iter_these)

    return any(newer(p) for p in scan_these
               if not any(str(p).startswith(deriv)
               for deriv in ['submods', 'build']))


def run_or_die(action):
    r = action()
    if r != 0:
        sys.exit(r)


def create_dirs(dirs):
    for d in dirs:
        try:
            os.mkdir(d)
        except FileExistsError as efef:
            if not os.path.isdir(d):
                return efef.args[0]
        except PermissionError as epe:
            return epe.args[0]
    return 0


def cmake_modules_path():
    cmake_root_line = [rl.decode('utf-8') for rl in
                       Proc(CMD, C, 'cmake', '--system-information',
                            consume=True).lines() if
                       rl.decode('utf-8').startswith('CMAKE_ROOT')][0].strip()
    cmake_root = ' '.join(cmake_root_line.split()[1:])
    cmake_root = cmake_root.split('"')[1]
    cmake_root = '\\'.join(cmake_root.split('/'))
    return os.path.join(cmake_root, "Modules")


def rm_f(path):
    if os.path.isdir(os.path.dirname(path)) and os.path.isfile(path):
        os.unlink(path)


class MakerDirs:

    def touch_files():
        return [os.path.join('build', 'all.touch'),
                os.path.join('build', 'test.touch')]

    def install_dests():
        include_root = os.path.join(PREFIX, 'include')
        lib_root = os.path.join(PREFIX, 'lib')
        lib_win32_root = os.path.join(lib_root, 'Win32')
        lib_x64_root = os.path.join(lib_root, 'x64')
        return {'include_root': include_root,
                'include_ansak': os.path.join(include_root, 'ansak'),
                'lib_root': lib_root,
                'lib_win32_root': lib_win32_root,
                'lib_x64_root': lib_x64_root,
                'lib_win32_release': os.path.join(lib_win32_root, 'Release'),
                'lib_win32_debug': os.path.join(lib_win32_root, 'Debug'),
                'lib_win32_relwithdebinfo': os.path.join(lib_win32_root,
                                                         'RelWithDebInfo'),
                'lib_win32_minsizerel': os.path.join(lib_win32_root,
                                                     'MinSizeRel'),
                'lib_x64_release': os.path.join(lib_x64_root, 'Release'),
                'lib_x64_debug': os.path.join(lib_x64_root, 'Debug'),
                'lib_x64_relwithdebinfo': os.path.join(lib_x64_root,
                                                       'RelWithDebInfo'),
                'lib_x64_minsizerel': os.path.join(lib_x64_root, 'MinSizeRel')}

    def nsis_dests():
        if MAKE_NSIS:
            nsis_root = os.path.join('build', 'nsis')
            nsis_include = os.path.join(nsis_root, 'include')
            nsis_lib_root = os.path.join(nsis_root, 'lib')
            nsis_lib_win32_root = os.path.join(nsis_lib_root, 'Win32')
            nsis_lib_x64_root = os.path.join(nsis_lib_root, 'x64')
            return {'nsis': nsis_root,
                    'include': nsis_include,
                    'include_ansak': os.path.join(nsis_include, 'ansak'),
                    'lib': nsis_lib_root,
                    'lib_win32': nsis_lib_win32_root,
                    'lib_x64': nsis_lib_x64_root,
                    'lib_win32_release': os.path.join(nsis_lib_win32_root,
                                                      'Release'),
                    'lib_win32_debug': os.path.join(nsis_lib_win32_root,
                                                    'Debug'),
                    'lib_win32_relwithdebinfo':
                        os.path.join(nsis_lib_win32_root, 'RelWithDebInfo'),
                    'lib_win32_minsizerel': os.path.join(nsis_lib_win32_root,
                                                         'MinSizeRel'),
                    'lib_x64_release': os.path.join(nsis_lib_x64_root,
                                                    'Release'),
                    'lib_x64_debug': os.path.join(nsis_lib_x64_root, 'Debug'),
                    'lib_x64_relwithdebinfo': os.path.join(nsis_lib_x64_root,
                                                           'RelWithDebInfo'),
                    'lib_x64_minsizerel': os.path.join(nsis_lib_x64_root,
                                                       'MinSizeRel')}
        else:
            return {}


class Maker:

    def __init__(self):
        self.build_dir_ = 'build'
        self.win32_dir_ = os.path.join(self.build_dir_, 'Win32')
        self.x64_dir_ = os.path.join(self.build_dir_, 'x64')
        self.done_ = set()
        self.step_performed_ = False
        self.package_path_ = os.path.join('build', PACKAGE_NAME)
        self.v_ = False

    def valid_order(raw_targets):
        valid = []
        if any(t == 'help' for t in raw_targets):
            return ['help']
        if any(t == 'clean' for t in raw_targets):
            valid.append('clean')
        for t in raw_targets:
            if t != 'clean':
                valid.append(t)
        if not valid:
            return ['all']
        return valid

    def make_all(self):
        create_dirs([self.build_dir_, self.win32_dir_, self.x64_dir_])

        if not source_is_newer('build\\all.touch'):
            self.done_.add('all')
            return

        env = {"CXX": COMPILER} if COMPILER else None
        unicode_choice = "-DANSAK_UNICODE_SUPPORT={}".format(UNICODE_VERSION)

        def cmake_gen(build_dir, arch):
            if not os.path.isfile('CMakeCache.txt'):
                return Proc(CMD, C, CMAKE, GRANDPARENT, unicode_choice, GEN,
                            GENERATOR, ARCH, 'Win32', cwd=build_dir, env=env
                            ).run()
            return 0

        run_or_die(lambda: cmake_gen(self.win32_dir_, 'Win32'))
        run_or_die(lambda: cmake_gen(self.x64_dir_, 'x64'))

        def cmake_build_it(build_dir, project, config):
            return lambda: Proc(CMD, C, CMAKE, BUILD, build_dir, '--config',
                                config, '--target', project).run()

        for target in BUILD_TARGETS:
            run_or_die(cmake_build_it(self.win32_dir_, target, 'Release'))
            run_or_die(cmake_build_it(self.x64_dir_, target, 'Release'))
            run_or_die(cmake_build_it(self.win32_dir_, target, 'Debug'))
            run_or_die(cmake_build_it(self.x64_dir_, target, 'Debug'))
            run_or_die(cmake_build_it(self.win32_dir_, target,
                                      'RelWithDebInfo'))
            run_or_die(cmake_build_it(self.x64_dir_, target, 'RelWithDebInfo'))
            run_or_die(cmake_build_it(self.win32_dir_, target, 'MinSizeRel'))
            run_or_die(cmake_build_it(self.x64_dir_, target, 'MinSizeRel'))
        self.done_.add('all')
        self.step_performed_ = True
        with open('build\\all.touch', 'w') as f:
            print("Done!", file=f)

    def test(self):
        if 'all' not in self.done_:
            self.make_all()

        if not source_is_newer('build\\test.touch'):
            self.done_.add('test')
            return

        def cmake_build_test(target, build_dir):
            return Proc(CMD, C, CMAKE, BUILD, build_dir, '--config', 'Release',
                        '--target', target).run()

        def cmake_run_test(build_dir):
            return Proc(CMD, C, 'ctest', CONF, 'Release', cwd=build_dir).run()

        for target in BUILD_TEST_TARGETS:
            run_or_die(lambda: cmake_build_test(target, self.win32_dir_))
            run_or_die(lambda: cmake_build_test(target, self.x64_dir_))
        run_or_die(lambda: cmake_run_test(self.win32_dir_))
        run_or_die(lambda: cmake_run_test(self.x64_dir_))
        self.done_.add('test')
        self.step_performed_ = True
        with open('build\\test.touch', 'w') as f:
            print("Done!", file=f)

    def install(self):
        if 'all' not in self.done_:
            self.make_all()
        paths = MakerDirs.install_dests()
        dirs_made = create_dirs(paths.values())
        if dirs_made != 0:
            sys.exit(dirs_made)
        try:
            for hpair in HEADERS_TO_INSTALL:
                shutil.copy2(hpair[0], paths[hpair[1]])

            for lib in LIBS_TO_INSTALL:
                pdb = '.'.join([lib.split('.')[0], 'pdb'])
                shutil.copy2(os.path.join('build', 'Win32', 'Release', lib),
                             paths['lib_win32_release'])
                shutil.copy2(os.path.join('build', 'Win32', 'Debug', lib),
                             paths['lib_win32_debug'])
                shutil.copy2(os.path.join('build', 'Win32', 'Debug', pdb),
                             paths['lib_win32_debug'])
                shutil.copy2(os.path.join('build', 'Win32', 'RelWithDebInfo',
                                          lib),
                             paths['lib_win32_relwithdebinfo'])
                shutil.copy2(os.path.join('build', 'Win32', 'RelWithDebInfo',
                                          pdb),
                             paths['lib_win32_relwithdebinfo'])
                shutil.copy2(os.path.join('build', 'Win32', 'Release', lib),
                             paths['lib_win32_release'])
                shutil.copy2(os.path.join('build', 'x64', 'MinSizeRel', lib),
                             paths['lib_win32_minsizerel'])

                shutil.copy2(os.path.join('build', 'x64', 'Release', lib),
                             paths['lib_x64_release'])
                shutil.copy2(os.path.join('build', 'x64', 'Debug', lib),
                             paths['lib_x64_debug'])
                shutil.copy2(os.path.join('build', 'x64', 'Debug', pdb),
                             paths['lib_x64_debug'])
                shutil.copy2(os.path.join('build', 'x64', 'RelWithDebInfo',
                                          lib),
                             paths['lib_x64_relwithdebinfo'])
                shutil.copy2(os.path.join('build', 'x64', 'RelWithDebInfo',
                                          pdb),
                             paths['lib_x64_relwithdebinfo'])
                shutil.copy2(os.path.join('build', 'x64', 'MinSizeRel', lib),
                             paths['lib_x64_minsizerel'])
            self.step_performed_ = True
        except PermissionError as epe:
            print("{} for prefix {} must be run from an {} shell".format(
                  'make install', PREFIX, 'Admin-privilege'))
            sys.exit(epe.args[0])

    def uninstall(self):
        paths = MakerDirs.install_dests()

        def rmdirIfEmpty(empty_dir):
            if os.path.isdir(empty_dir):
                files_in_include = os.listdir(empty_dir)
                if not files_in_include:
                    os.rmdir(empty_dir)
        try:
            for hpair in HEADERS_TO_INSTALL:
                rm_f(os.path.join(paths['include_ansak'],
                                  os.path.basename(hpair[0])))

            rmdirIfEmpty(paths['include_ansak'])
            rmdirIfEmpty(paths['include_root'])

            for lib in LIBS_TO_INSTALL:
                pdb = '.'.join([lib.split('.')[0], 'pdb'])
                rm_f(os.path.join(paths['lib_win32_release'], lib))
                rm_f(os.path.join(paths['lib_win32_debug'], lib))
                rm_f(os.path.join(paths['lib_win32_debug'], pdb))
                rm_f(os.path.join(paths['lib_win32_relwithdebinfo'], lib))
                rm_f(os.path.join(paths['lib_win32_relwithdebinfo'], pdb))
                rm_f(os.path.join(paths['lib_win32_minsizerel'], lib))
                rm_f(os.path.join(paths['lib_x64_release'], lib))
                rm_f(os.path.join(paths['lib_x64_debug'], lib))
                rm_f(os.path.join(paths['lib_x64_debug'], pdb))
                rm_f(os.path.join(paths['lib_x64_relwithdebinfo'], lib))
                rm_f(os.path.join(paths['lib_x64_relwithdebinfo'], pdb))
                rm_f(os.path.join(paths['lib_x64_minsizerel'], lib))
            rmdirIfEmpty(paths['lib_win32_release'])
            rmdirIfEmpty(paths['lib_win32_debug'])
            rmdirIfEmpty(paths['lib_win32_relwithdebinfo'])
            rmdirIfEmpty(paths['lib_win32_minsizerel'])
            rmdirIfEmpty(paths['lib_win32_root'])
            rmdirIfEmpty(paths['lib_x64_release'])
            rmdirIfEmpty(paths['lib_x64_debug'])
            rmdirIfEmpty(paths['lib_x64_relwithdebinfo'])
            rmdirIfEmpty(paths['lib_x64_minsizerel'])
            rmdirIfEmpty(paths['lib_x64_root'])
            rmdirIfEmpty(paths['lib_root'])
            self.step_performed_ = True
        except PermissionError as epe:
            print("{} for prefix {} must be run from an {} shell".format(
                  'make uninstall', PREFIX, 'Admin-privilege'))
            sys.exit(epe.args[0])

    def package(self):
        if not MAKE_NSIS:
            print("makensis could not be located, package target not " +
                  "available.")
            sys.exit(1)
        if 'test' not in self.done_:
            self.test()

        # dependency check of product vs. its sources
        if not source_is_newer(self.package_path_, Path('NSIS').glob('*')):
            return

        # prep the directories
        nsis_dests = MakerDirs.nsis_dests()
        assert nsis_dests
        dir_make = create_dirs(nsis_dests.values())
        if dir_make != 0:
            sys.exit(dir_make)

        # copy things to the directories
        for f in os.listdir('NSIS'):
            shutil.copy2(os.path.join('NSIS', f), nsis_dests['nsis'])
        shutil.copy2('FindANSAK.cmake', nsis_dests['nsis'])

        with open(os.path.join(nsis_dests['nsis'], 'ansakVersion.nsh'),
                  'w') as nsh_header:
            print('!define ANSAK_UNICODE_SUPPORT {}'.format(UNICODE_VERSION),
                  file=nsh_header)
            print('!define ANSAK_LIB_VERSION {}'.format(PACKAGE_VERSION),
                  file=nsh_header)

        for hpair in HEADERS_TO_INSTALL:
            shutil.copy2(hpair[0], nsis_dests[hpair[1]])

        for lib in LIBS_TO_INSTALL:
            pdb = '.'.join([lib.split('.')[0], 'pdb'])

            shutil.copy2(os.path.join('build', 'Win32', 'Release', lib),
                         nsis_dests['lib_win32_release'])
            shutil.copy2(os.path.join('build', 'Win32', 'Debug', lib),
                         nsis_dests['lib_win32_debug'])
            shutil.copy2(os.path.join('build', 'Win32', 'Debug', pdb),
                         nsis_dests['lib_win32_debug'])
            shutil.copy2(os.path.join('build', 'Win32', 'RelWithDebInfo', lib),
                         nsis_dests['lib_win32_relwithdebinfo'])
            src1 = os.path.join('build', 'Win32', 'RelWithDebInfo', pdb)
            src2 = os.path.join('build', 'Win32', 'ansakString.dir',
                                'RelWithDebInfo', pdb)
            shutil.copy2(src1 if os.path.isfile(src1) else src2,
                         nsis_dests['lib_win32_relwithdebinfo'])
            shutil.copy2(os.path.join('build', 'Win32', 'MinSizeRel', lib),
                         nsis_dests['lib_win32_minsizerel'])

            shutil.copy2(os.path.join('build', 'x64', 'Release', lib),
                         nsis_dests['lib_x64_release'])
            shutil.copy2(os.path.join('build', 'x64', 'Debug', lib),
                         nsis_dests['lib_x64_debug'])
            shutil.copy2(os.path.join('build', 'x64', 'Debug', pdb),
                         nsis_dests['lib_x64_debug'])
            shutil.copy2(os.path.join('build', 'x64', 'RelWithDebInfo', lib),
                         nsis_dests['lib_x64_relwithdebinfo'])
            src1 = os.path.join('build', 'x64', 'RelWithDebInfo', pdb)
            src2 = os.path.join('build', 'x64', 'ansakString.dir',
                                'RelWithDebInfo', pdb)
            shutil.copy2(src1 if os.path.isfile(src1) else src2,
                         nsis_dests['lib_x64_relwithdebinfo'])
            shutil.copy2(os.path.join('build', 'x64', 'MinSizeRel', lib),
                         nsis_dests['lib_x64_minsizerel'])

        # create the install set, move it to ./build
        def run_nsis():
            return Proc(CMD, C, MAKE_NSIS, 'ansak-string.nsi',
                        cwd=nsis_dests['nsis']).run()

        run_or_die(run_nsis)
        shutil.copy2(os.path.join(nsis_dests['nsis'], PACKAGE_NAME), 'build')
        self.step_performed_ = True

    def clean(self):
        def deleteThese(paths):
            for path in paths:
                if os.path.isdir(path):
                    shutil.rmtree(path)
                else:
                    rm_f(path)

        deleteThese([self.win32_dir_, self.x64_dir_,
                     MakerDirs.nsis_dests()['nsis'], self.package_path_])
        deleteThese(MakerDirs.touch_files())

        self.step_performed_ = True

    def scrub(self):
        if os.path.isdir('build'):
            Proc(CMD, C, 'rmdir', '/s', '/q', 'build').run()
        rm_f('configvars.py')
        if os.path.isdir('__pycache__'):
            Proc(CMD, C, 'rmdir', '/s', '/q', '__pycache__').run()
        self.step_performed_ = True

    def cmake_install(self):
        cmake_mod_path = cmake_modules_path()
        print("Copying {} to {}".format('FindANSAK.cmake', cmake_mod_path))
        shutil.copy2('FindANSAK.cmake', cmake_mod_path)
        self.step_performed_ = True

    def cmake_uninstall(self):
        cmake_mod_path = cmake_modules_path()
        print("Removing {} from {}".format('FindANSAK.cmake', cmake_mod_path))
        rm_f(os.path.join(cmake_mod_path, 'FindANSAK.cmake'))
        self.step_performed_ = True

    def help(self):
        print("Makefile simluator for ease-of-deployment on Windows in Win32")
        print("  * help: this message")
        print("  * all: (default target) compile of the libraries (Release)")
        print("  * test: compile and run of all tests")
        print("  * install: deploy headers and libraries to prefix")
        print("  * uninstall: remove the headers and libraries at prefix")
        print("  * package: build an installer for this source code, place " +
              "it in .\\build (unaffected by prefix setting)")
        print("  * cmake-install: deploy FindANSAK.cmake to " +
              "CMAKE_ROOT/modules")
        print("  * cmake-uninstall: remove FindANSAK.cmake from CMake modules")
        print("Run .\\configure.cmd before running .\\make. There are some")
        print("important settings to be determined there.")
        self.step_performed_ = True

    targets = {"all": make_all, "test": test, "install": install, "uninstall":
               uninstall, "package": package, "clean": clean, "scrub": scrub,
               "cmake-install": cmake_install, "cmake-uninstall":
               cmake_uninstall, "help": help}

    def process(self, args):
        self.v_ = bool(args.verbose)
        for target in Maker.valid_order(args.targets):
            assert target in Maker.targets
            Maker.targets[target](self)
        if not self.step_performed_:
            print('Nothing to do for targets, {}'.format(repr(args.targets)))


def main():
    parser = argparse.ArgumentParser(
                 description="Make script for ansak-string on Windows")
    parser.add_argument('-v', '--verbose',
                        help='more detailed progress messages',
                        action='store_true')
    targets_prompt = 'Things to build. If nothing specified, "all" '
    targets_prompt += 'is assumed. Possible values are: {}'.format(
                      str(Maker.targets.keys()))
    parser.add_argument('targets', help=targets_prompt, type=str, nargs='*')

    Maker().process(parser.parse_args())


if __name__ == '__main__':
    main()
