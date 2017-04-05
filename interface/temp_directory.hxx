///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, Arthur N. Klassen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////
//
// 2016.01.02 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// temp_directory.hxx -- Class that wraps platform-dependant notions of temp
//                       file/directory use. Creates a directory in the temp
//                       folder on construction, hands out FileSystemPath
//                       specs for sub-files/folders as needed. Destroys them
//                       all (by default) on destruction.
//
//                       Includes a "detach" parameter in "asUtf8StringPath()"
//                       to disconnect ownership.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "file_system_path.hxx"
#include "string.hxx"
#include <memory>

namespace ansak {

class TempDirectory;
using TempDirectoryPtr = std::unique_ptr<TempDirectory>;

///////////////////////////////////////////////////////////////////////
// class TempDirectory -- utility class interface to manage a temporary
// directory life-time specific to this process, or even to one sub-
// system within this process.

class TempDirectory {

    friend TempDirectoryPtr createTempDirectory();

    ///////////////////////////////////////////////////////////////////
    // Constructor -- Begin the life-cycle of the temporary directory.

    TempDirectory
    (
        const FileSystemPath&   parent      // I - the parent temp dir
    );

    static std::string m_tempDirectoryMarker;

public:

    static const bool detachIt = true;
    static const bool dontDetachIt = false;

    ///////////////////////////////////////////////////////////////////
    // Move Constructor -- Begin the life-cycle of the temporary directory.
    // but don't copy/assign

    TempDirectory(const TempDirectory& src) = delete;
    TempDirectory& operator=(const TempDirectory& src) = delete;

    ///////////////////////////////////////////////////////////////////
    // Destructor -- On exit, delete the directory and all its contents,
    // if the directory remains "attached" to the object.

    ~TempDirectory();

    ///////////////////////////////////////////////////////////////////
    // Change the default name of the temp directory to PID-yourTextHere-N
    // where PID is the platform-specific process ID as a decimal number,
    // and N is a natural number in sequence since temporary directories
    // were being created.

    static void setTempDirectoryMarker(const std::string& marker);

    ///////////////////////////////////////////////////////////////////
    // child -- create a sub-path from the temporary directory, for a
    // file, a directory or whatever is required and available on the
    // current platform

    FileSystemPath child
    (
        const utf8String&       subPath     // I - a dir sub-element to create
    ) const;

    ///////////////////////////////////////////////////////////////////
    // asFileSystemPath -- If you need the path to the temp directory,
    // here's how to get it -- and to detach it from this object if so
    // needed.

    FileSystemPath asFileSystemPath
    (
        bool        detach = false              // I - suppress delete on dtor
    );

private:
    FileSystemPath      m_path;
    bool                m_deleteOnExit = true;
};

///////////////////////////////////////////////////////////////////////
// Create a directory in the temp directory and a TempDirectory object
// around it to hand out FileSystemPaths within it, and to delete the
// whole thing on exit.

TempDirectoryPtr createTempDirectory();

}
