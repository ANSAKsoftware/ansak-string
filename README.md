ansak-string
============

Copyright 2015-2022 Arthur N. Klassen; see LICENSE for usage guidelines (BSD 2-clause)

This library started as the first piece of a larger project. It's a simple
string library. With the release of version 2.0, I am returning to work on the
larger project now, and the results will be available under an **ansak-lib**
repository.

**ansak-string** is mostly about purifying input and performing conversions
between different string encodings more simply than other available APIs.

If your string (whether it's 7-bit ASCII, UTF-8, UTF-16, UCS-2 or UTF-32/UCS-4)
makes it through my APIs (into 7-bit ASCII, UTF-8 etc.), then that's what it
is. No more, no less. This includes filtering for publicly assigned Unicode
values (currently following Unicode 13 by default, other versions supported
through a cmake switch every major version back to 7.0).

A simple trim-string and a pair of split and join templates are also present.
Split and join are based on character types (not strings as in their python
analogue) and all of trim, split and join basic\_string for their processing
are also present. Trim trims all blank code-points defined in Unicode.

Commercial Support:
-------------------

Available on request, subject to reasonable terms we can agree on -- and which
don't violate other agreements I am party to. Please contact me via anklassen
(at) onepost dot net.

Requests for more conversions from Windows Code Pages and/or ISO/ECMA 8859
encoding standards will be considered individually.

Additional stuff:
-----------------

* unicodeLength gives number of UCS-4 codes (regardless of composite/composable
  points) in a string
* all-Unicode-sensitive "tolower". It can operate in "Turkic" mode if you pass
  in an (optional) constant C-string for a Turkic language's ISO-639 two or
  three character code. In this mode, I-dot and dotless-i are handled correctly
  for those languages.

ISO-639 codes for Turkic languages:

* Azerbaizhani: "az", "azb", "aze", "azj"
* Kazakh: "kk", "kaz"
* Tatar: "tt", "tat"
* Turkish "tr", "tur"

Deploying:
----------

### **On Linux, Cygwin / MingW, macOS**
  I'm not using `autoconf` / `automake` but... exeucte
```
./configure
make
make install
```
  If you don't have CMake installed, `./configure` will complain about that
  fact. Otherwise, those three commands will put three headers in a directory,
  `.../include/ansak`, and `libansakString.a` in a `.../lib` directory.

  `--help` on configure, the `help` target on `make` exposes more options,
  including `--prefix`.

  This will be enough in order to start using **ansak-string** directly.
  If you want to use CMake with **ansak-string**, you may want to use
  `make cmake-install` as well, especially for macOS.

  `uninstall` and `cmake-uninstall` are also targets.

#### **Platform differences: Linux**

  On Linux, prefix defaults to `/usr/local` and you will be asked to raise
  privilege in order to `install`, `uninstall`, `cmake-install` or
  `cmake-uninstall`.

  On Linux, package, deb, rpm and arch are also availble as targets. Package
  will work based on your platform, the others may be available based on what
  tools are installed

#### **Platform differences: macOS**

  On macOS, prefix defaults to `$HOME`. `/usr` and `/usr/local` are more
  difficult to write into. Other teams write 3rd party components into...
```
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/local
```
  You could select that location using the `--prefix` location if you wish.
  You will be asked to provide a password to raise privilege if you do.

TODO: Make sure FindANSAK.cmake can find files even buried deep inside the Xcode.app

#### **Platform differences: MinGW/Cygwin**

  On MinGW/Cygwin prefix defaults to `/usr/local` and you will not be asked to
  raise privilege for any operation.

### **On Windows (Microsoft Compilers)**
`autoconf` / `automake` are not, to my knowledge, available on Windows without
Cygwin or MinGW but I didn't use them for Linux. Why would I use them here?

I would not. But the pattern of `configure` / `make` is a functional one and
I've provided a simulation for them on Windows, dependant on CMake as for
Unix-likes, but also on Python3 (I just couldn't be bothered to learn enough
PowerShell to do it more natively). If Python3 is available as `python` or as
`python3`, just run:
```
.\configure
.\make
.\make install
```

The 3rd step needs to be run in a cmd.exe window with Administrator Privileges
unless you over-ride the default prefix which is `C:\\ProgramData`. Actually,
unless you need to over-ride any of the defaults offered (see `python configure.py --help`),
you can combine the first two steps by running `python make.py` instead.

If Python3 is not available on your path, consider re-installing it, or you can
try to tell configure where to find your Python3:
```
.\configure --python <your path to Python3>
```

`configure.py` tries to find the default Visual Studio build system generator
and writes the name of that generator, together with the prefix and other
settings into a `configvars.py` file, consumed by `make.py`. If no Visual
Studio generator is found as the default, `configure.py` fails.

TODO: add parameter to configure to over-ride the generator? Maybe not.
Non-default generators may not have compilers on the system, either.

* `.\configure --help` documents what configure can do.
* `python configure.py --help` documents what the python script can do.
* `.\make help` (or `python make.py help`) will document what the make script can
do.

`.\make install` deploys files thus (assuming default prefix):
* header files to `C:\\ProgramData\\include\\ansak`
* `ansakString.lib` (Win32-Debug) to `C:\\ProgramData\\lib\\Win32\\Debug`
* `ansakString.lib` (Win32-Release) to `C:\\ProgramData\\lib\\Win32\\Release`
* `ansakString.lib` (Win32-RelWithDebInfo) to `C:\\ProgramData\\lib\\Win32\\RelWithDebInfo`
* `ansakString.lib` (x64-Debug) to `C:\\ProgramData\\lib\\x64\\Debug`
* `ansakString.lib` (x64-Release) to `C:\\ProgramData\\lib\\x64\\Release`
* `ansakString.lib` (x64-RelWithDebInfo) to `C:\\ProgramData\\lib\\x64\\RelWithDebInfo`

This is where `FindANSAK.cmake` expects to find things under native Win32.

TODO: add a way to build ARM-Debug/Release/RelWithDebInfo binaries?

If `configure.py` finds makensis installed and on the path, `.\make package`
will create an NSIS-based installer package, defaulted to the prefix above.

`cmake-install` and `cmake-uninstall` targets exist for Windows as well and
will be needed if you are not already using C:\\ProgramData-subdirectories for
3rd party libraries.

Using:
------

If you've done `cmake-install`, you need do no more than add add
`find_package(ANSAK)` to your root `CMakeLists.txt` file. Use the variables you
see in your `CMakeCache.txt` file as you would for any other dependant
component.

When using the headers, for instance the string splitter template, write

`#include <ansak/string_split.hxx>`

into your source file. And, in some cases, you may also consider adding:

`using namespace ansak;`

Long-Term Goal:
---------------

The ultimate goal of these libraries is to support a simple household expense
tracking program that I plan to release under an appropriate open source
license, probably using Qt for User Interface, sqlite3 for storage and
implemented with the best that C++11/14/7 can supply.

I have added `FindANSAK.cmake` to **ansak-string**, to be mirrored and kept up
to date in on-going releases of **ansak-lib**.

The contents of the former `ansak-lib` branch of **ansak-string** have been
pulled out into their own repository,
[ansak-lib](https://github.com/ANSAKsoftware/ansak-lib), which allows their
release cycle to be completely independent of that of **ansak-string**,
appropriate because Unicode release cycles are on their own time-table, and
outside of changes there, **ansak-string** doesn't have a lot of outstanding
tasks waiting for attention.
