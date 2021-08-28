ansak-string
============

Copyright 2015, 2021 Arthur N. Klassen; see LICENSE for usage guidelines (BSD 2-clause)

This library started as the first piece of a larger project. It's a simple string library. With the release of version 2.0, I am returning to work on the larger project now, and the results will be available under an **ansak-lib** repository.

**ansak-string** is mostly about purifying input and performing conversions more simply than other available APIs. If your string (whether it's 7-bit ASCII, UTF-8, UTF-16, UCS-2 or UTF-32/UCS-4) makes it through my APIs (into 7-bit ASCII, UTF-8 etc.), then that's what it is. No more, no less. This includes filtering for publicly assigned Unicode values, currently following Unicode 13, supporting through cmake switch every major version back to 7.0.

A simple pair of split and join templates, based on character types (not strings as in their python analogue) and using basic\_string for their processing are also present.

Commercial Support:
-------------------

Available on request, subject to reasonable terms we can agree on. Please contact me via anklassen (at) onepost dot net.

Requests for more conversions from Windows Code Pages and/or ISO/ECMA 8859 encoding standards will be considered individually.

Additional stuff:
-----------------

* unicodeLength gives number of UCS-4 codes (regardless of composite/composable points) in a string
* split and join, like python's string and array methods
* all-Unicode-sensitive "tolower". It can operate in "Turkic" mode if you pass in an (optional) constant C-string for a Turkic language's ISO-639 two or three character code. In this mode, I-dot and dotless-i are handled correctly for those languages.

ISO-639 codes for Turkic languages:

* Azerbaizhani: "az", "azb", "aze", "azj"
* Kazakh: "kk", "kaz"
* Tatar: "tt", "tat"
* Turkish "tr", "tur"

Deploying:
----------

* **On Linux** -- use cmake to build the release version and when you're done, copy (not recursively)
  the files from `interface/` to `/usr/include/ansak` or
  `/usr/local/include/ansak` (you'll have to become root to do so); copy
  `libansakstring.a` from the root of your cmake build directory to `/usr/lib`
  or `/usr/local/lib`.  `FindANSAK.cmake` in your project will be able to find them there.
  (TODO: build package for this)

* **On macOS** -- do the same as for Linux

* **On Windows (Cygwin/MINGW)** -- do the same as for Linux

* **On Windows (Microsoft Compilers)** -- For Windows, you will need a
  "Release" version of the library for each of Win32 and x86\_64 (or ARM or ARM64) that you plan
  to use. If you want the resulting executables to be debuggable, you will want
  to build RelWithDebInfo versions, too. And if you plan to debug **your**
  program, you will want to build Debug as well.
  The recommended deployment will be from a "Third Party Software" directory
  containing an `include` directory, and a `lib` directory. The best candidate
  for this is probably `C:\ProgramData` but you can over-ride this.
  Copy (not recursively) the contents of `interface\` from **ansak-string** into the
  `include\ansak` directory. Within the `lib\` directory, create as many platform-
  specific sub-directories as you need. Within each of them, create as many of
  `Release\`, `Debug\` and `RelWithDebInfo\` as  you need. Build as many of
  the libraries as you see fit and copy them to the appropriate directory
  by architecture and build type. (TODO: build NSIS installer for this?)
  By default, ansak libraries will build for use with the DLL-based C/C++
  runtime. If you need to build them for statically-linked C/C++ runtime feel
  free to do so. The single-threaded libraries seem to have vanished and it
  seems that all but a very narrow group of compiler writers prefer the DLL-
  based runtime.
  `FindANSAK.cmake` on Windows will look for ANSAK components
  * by default, as sub-directories of `C:\ProgramData`
  * this can be over-ridden by an environment or CMake variable: `PROGRAM_DATA`
  * this can, in its turn be over-ridden by an environment or CMake variable `ANSAK_DIR`

Using:
------

When you want to use **ansak-string** (or later, other ANSAK Software
components), copy `FindANSAK.cmake` from the `source/` into a `cmake/modules`
directory at the root of your project.

Add a section like this, early, to your root `CMakeLists.txt` file:
```
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")`
    list(INSERT CMAKE_MODULE_PATH` 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)`
endif()
```
Add `find_package(ANSAK)` to your root `CMakeLists.txt` file; use the
variables you see in your `CMakeCache.txt` file as you would for any other
dependant component.

Long-Term Goal:
---------------

The ultimate goal of these libraries is to support a simple household expense
tracking program that I plan to release under an appropriate open source
license, probably using Qt for User Interface, sqlite3 for storage and
implemented with the best that C++11/14/7 can supply.

I have added `FindANSAK.cmake` to **ansak-string**, to be mirrored and kept up
to date in on-going releases of **ansak-lib**.

The contents of the `ansak-lib` branch of **ansak-string** have been pulled out into
their own repository, [ansak-lib](https://github.com/ANSAKsoftware/ansak-lib), which
allows their release cycle to be completely independent of that of **ansak-string**,
appropriate because Unicode release cycles are on their own time-table, and outside
of changes there, **ansak-string** doesn't have a lot of outstanding tasks waiting
for attention.
