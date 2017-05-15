ansak-lib
=========

Copyright 2017, Arthur N. Klassen; see LICENSE for usage guidelines (BSD 2-clause)

This library is the second piece of a larger project that was stalled. It started as a simple string library. With the release of version 2.0, I am returning to work on the larger project now, and the results will be available under the **ansak-lib** branch of this repository. String changes will be merged into the master branch and pushed down to **ansak-lib**.

**ansak-lib** includes **ansak-string** but expands on it, using C++11 concepts and built with a *Python-like* mentality, i.e. just make simple things straightforward and hide the details from most users. It includes:

* A simple **TimeStamp** class used to receive platform-specific time stamp data and provide it in a unified fashion using ordinary years; months 1..12; days 1..28,29,30 or 31; hours 0..23; minutes and seconds 0..59. It just stores these values can compare them. It makes no effort (yet) to do calculations with them.
* A **ParseArgs** class that takes in an argc/argv pair and organizes them as flags (known only if present -- no associated value), settings (known only if present, include a string value) and inputs (other parameters that aren't flags or settings). The values present can be changed, added to, deleted from and a new argc/argv pair can be generated for consumption, for instance by graphical libraries that want to consume an argc/argv.
* A **RuntimeException** class and an associated **enforce** method that throws a **RuntimeException** if some condition is not satisfied. (Also a **NullPtrException** for when a method *must* receive a pointer and did not.)
* A **FilePath** to handle file names and file-name processing without reference to a file system. Regardless of platform, this class will handle both Windows and non-Windows paths.
* A **FileSystemPath** for when a **FilePath** needs to refer to a real file in the system. Closely related is **FileHandle** that provides platform independent file I/O. Errors can be optionally reported through **FileHandleException** being thrown. Except as regards the file paths being manipulated, **FileSystemPath** and **FileHandle** are as platform independent as they can be.
* A **TempDirectory** object which will create a uniquely-named directory in the platform-appropriate temporary storage area, hand out paths to locations within that directory, and then clean up and destroy the whole directory and sub tree when the object drops out of scope -- this can be prevented for testing and other purposes by using the **TempDirectory::asFileSystemPath** method with **TempDirectory::detachIt** as a parameter.
* **FileOfLines** which will present (just as independently of platform as **FileSystemPath**) the contents of any file of unformatted text, whether UTF-8, UTF-16/UCS-2 BE or LE, UCS-4 BE or LE through const\_iterator objects that behave in most ways as though they pointed into an STL **std::vector&lt;std::string&gt;**.
* **Config**, a flexible way of managing configuration settings and how internal defaults, system-wide settings and user settings might interact with one another. This includes platform-independent ways of retrieving system-wide and user-specific settings, and of writing user-specific settings. Under the hood, the Linux implementation looks in **/etc** for system-wide settings and in hidden files under **$HOME** for user-specific settings. On Windows, the same APIs manipulate Windows Registry values under **HKLM\Software** for system-wide settings and **HKCU\Software** for user-specific ones. For both files and registry, there are arbitrary APIs to read and write any file for settings (a privileged program could use this API to write system-wide settings in Linux) and on Windows, any valid registry path that the running program has the necessary privileges to use.

Commercial Support:
-------------------

Available on request, subject to reasonable terms we can agree on. Please contact me via anklassen (at) onepost dot net.

Requests for more conversions from Windows Code Pages and/or ISO/ECMA 8859 encoding standards will be considered individually.

Long-term Goal:
---------------

The ultimate goal of these libraries is to support a simple household expense tracking program that I plan to release under an appropriate open source license, using Qt for User Interface, sqlite3 for storage and implemented with the best that C++11/14/7 can supply.

Additional stuff:
-----------------

Regarding **ansak-string**:

* unicodeLength gives number of UCS-4 codes (regardless of composite/composable points) in a string
* split and join, like python's string and array methods
* all-Unicode-sensitive "tolower". It can operate in "Turkic" mode if you pass in an (optional) constant C-string for a Turkic language's ISO-639 two or three character code. In this mode, I-dot and dotless-i are handled correctly for those languages.

ISO-639 codes for Turkic languages:

* Azerbaizhani: "az", "azb", "aze", "azj"
* Kazakh: "kk", "kaz"
* Tatar: "tt", "tat"
* Turkish "tr", "tur"

Regarding **ansak-lib**:
* With **FileHandle**, call **FileHandle::throwErrors** early on to report all file I/O errors through throwing **FileHandleException**. This is a one-shot and cannot be reversed.
