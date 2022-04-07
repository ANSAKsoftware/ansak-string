@echo off
REM ########################################################################
REM
REM Copyright (c) 2022, Arthur N. Klassen
REM All rights reserved.
REM
REM Redistribution and use in source and binary forms, with or without
REM modification, are permitted provided that the following conditions are met:
REM
REM 1. Redistributions of source code must retain the above copyright
REM    notice, this list of conditions and the following disclaimer.
REM
REM 2. Redistributions in binary form must reproduce the above copyright
REM    notice, this list of conditions and the following disclaimer in the
REM    documentation and/or other materials provided with the distribution.
REM
REM THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
REM AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
REM IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
REM ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
REM LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
REM CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
REM SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
REM INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
REM CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
REM ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
REM POSSIBILITY OF SUCH DAMAGE.
REM
REM ########################################################################
REM
REM 2022.02.20 - First version
REM
REM    May you do good and not evil.
REM    May you find forgiveness for yourself and forgive others.
REM    May you share freely, never taking more than you give.
REM
REM ###########################################################################
REM
REM configure.cmd -- A Windows .cmd file to call configure.pyImplement in Python3 what is unavailable on Windows
REM                  (outside of MinGW / Cygwin), the minimal autoconf/automake-
REM                  like behaviour of the config shell script
REM
REM ###########################################################################
REM determine location if any of python3

if "%1"=="--help" goto help
if "%1"=="-h" goto help

REM blows up if cmake is not installed, called out below
call cmake.exe --version 2>NUL:

REM find python -- manually? over-ride with that
if "%1"=="--python" goto neither

REM try python to see if it's version 3 (blows up if absent)
python choosep3.py
if not errorlevel 3 goto tryp3
python configure.py python %*
goto done

:tryp3
python3 choosep3.py
if not errorlevel 3 goto noThreeIn3
python3 configure.py python3 %*
goto done

:noThreeIn3
python choosep3.py
echo python selects version %ERRORLEVEL%
python3 choosep3.py
echo python3 selects version %ERRORLEVEL%
echo Have we moved to Python4?
goto done

:neither
shift
if "%1"=="" goto help
%1 configure.py %*
goto done

:help
echo configure.cmd looks for Python 3 and uses it to run configure.py.
echo.
echo Parameters passed to configure.py are
echo    * optionally, --python followed by the command that runs python 3
echo    * all the parameters you might pass in to configure.cmd,
echo          try python3 configure.py --help to see them
echo.
echo What those additinoal parameters might be are explained by running
echo python3 configure.py --help
echo.
echo If you get a 'not recognized' error message about python, you need to
echo install a python3 implementation; at time of writing, one option is:
echo https://www.python.org/downloads/release/python-3102/
echo.
echo If you get a 'not recognized' error message about cmake, you need to
echo install a cmake implementation; at time of writing, one option is:
echo https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-windows-x86_64.msi

:done
