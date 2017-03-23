///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, Arthur N. Klassen
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
// file_system_path.hxx -- Class that wraps platform-dependant file operations
//                         on files as file-system objects. Constructed from
//                         a valid, native FilePath object, it checks for its
//                         existence, date (last modified where that distinction
//                         matters), file-or-directory (or other), size (if it's
//                         a file).
//
//                         Constructed from an invalid or non-native FilePath
//                         object, this object is labelled invalid.
//
//                         parent-of and child-of operations are forwarded
//                         through FilePath and back (on valid, native objects)
//                         to create a new FileSystemPath object.
//
//                         children() returns a vector of FileSystemPath objects
//                         for the entries of a FileSystemObject that is a
//                         directory.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "file_path.hxx"
#include "time_stamp.hxx"

#include <functional>
#include <memory>

namespace ansak {

class DirectoryListPrimitive;

///////////////////////////////////////////////////////////////////////////
// class FileSystemPath -- things you can do in a file system with a valid
//                         path. Not just string operations anymore.

class FileSystemPath {

public:

    static const bool failIfThere = true;
    static const bool recursively = true;

    ///////////////////////////////////////////////////////////////////////
    // Constructor -- stores it, based on a file path
    //
    // If path is valid, tries to realize it (by derelativizing it if
    // necessary). If the path cannot be realized, the FileSystemPath will be
    // marked invalid.

    FileSystemPath
    (
        const FilePath&     path = FilePath()
    );

    ///////////////////////////////////////////////////////////////////////
    // Constructor -- stores it, based on a file path built from a string.
    //
    // If path is valid, tries to realize it (by derelativizing it if
    // necessary). If the path cannot be realized, the FileSystemPath will be
    // marked invalid.

    FileSystemPath
    (
        const std::string&  pathString
    );

    ///////////////////////////////////////////////////////////////////////
    // Copy, Assign -- default implementations are good.

    FileSystemPath(const FileSystemPath& src) = default;
    FileSystemPath& operator=(const FileSystemPath& src) = default;

    ///////////////////////////////////////////////////////////////////////
    // class TempDirectory -- utility class interface to manage a temporary
    // directory life-time specific to this process, or even to one sub-
    // system within this process.

#if 0
    class TempDirectory {
    public:
        static const bool detachIt = true;
        static const bool dontDetachIt = false;

        ///////////////////////////////////////////////////////////////////
        // Constructor -- Begin the life-cycle of the temporary directory.

        TempDirectory();

        ///////////////////////////////////////////////////////////////////
        // Destructor -- On exit, delete the directory and all its contents,
        // if the directory remains "attached" to the object.

        virtual ~TempDirectory();

        ///////////////////////////////////////////////////////////////////
        // child -- create a sub-path from the temporary directory, for a
        // file, a directory or whatever is required and available on the
        // current platform

        virtual FileSystemPath child(const utf8String& subPath) const = 0;

        ///////////////////////////////////////////////////////////////////
        // asFileSystemPath -- If you need the path to the temp directory,
        // here's how to get it -- and to detach it from this object if so
        // needed.

        virtual FileSystemPath asFileSystemPath(bool detach = dontDetachIt) = 0;
    };

    ///////////////////////////////////////////////////////////////////////
    // createTemporaryDirectory -- Instantiate an object that implements the
    // TempDirectory interface.

    using TempDirectoryPtr = std::unique_ptr<TempDirectory>;
    static TempDirectoryPtr createTemporaryDirectory();
#endif

    ///////////////////////////////////////////////////////////////////////
    // parent -- Like the FilePath version, but produces a parent
    // FileSystemPath.

    FileSystemPath parent() const;

    ///////////////////////////////////////////////////////////////////////
    // class ChildrenRetriever -- utility class to iterate through contents
    // of a sub-directory

    class ChildrenRetriever {
    public:
        ///////////////////////////////////////////////////////////////////
        // Constructor -- Create a retriever from a FileSystemPath. If path
        // is not a real directory, the ChildrenRetreiver does no work.

        ChildrenRetriever(const FileSystemPath& path);
        ChildrenRetriever(ChildrenRetriever&& src);
        ~ChildrenRetriever();

        ///////////////////////////////////////////////////////////////////
        // operator() -- For a functional ChildrenRetriever, finds the next
        // sub item.

        FilePath operator()();

    private:
        FilePath                                        m_path;
        mutable std::unique_ptr<DirectoryListPrimitive> m_directoryReader;
    };

    ///////////////////////////////////////////////////////////////////////
    // children -- start iterating through the children of a given path, if
    // possible.

    ChildrenRetriever children() const;

    ///////////////////////////////////////////////////////////////////////
    // retrieve the underlying FilePath, explicitly or implicitly.

    operator FilePath() const { return m_path; }
    FilePath asFilePath() const { return m_path; }

    ///////////////////////////////////////////////////////////////////////
    // retrieve the underlying FilePath, as a string (as your implementation
    // best understands it)

    utf8String asUtf8String() const { return m_path.asUtf8String(); }
    utf16String asUtf16String() const { return m_path.asUtf16String(); }

    ///////////////////////////////////////////////////////////////////////
    // child -- generate a FilePath for a (potentially existent) sub-object
    // of this FileSystemPath.

    FilePath child(const std::string& child) { return m_path.child(child); }

    ///////////////////////////////////////////////////////////////////////
    // Simple comparisons that (given the implementation of FilePath) will
    // optimize to string comparisons of the inner string value.
    bool operator==(const FileSystemPath& other) const { return m_path == other.m_path; }
    bool operator!=(const FileSystemPath& other) const { return m_path != other.m_path; }
    bool operator<(const FileSystemPath& other) const { return m_path < other.m_path; }
    bool operator<=(const FileSystemPath& other) const { return m_path <= other.m_path; }
    bool operator>(const FileSystemPath& other) const { return m_path > other.m_path; }
    bool operator>=(const FileSystemPath& other) const { return m_path >= other.m_path; }

    ///////////////////////////////////////////////////////////////////////
    // Create this FileSystemPath as a directory of a file.

    bool createDirectory(bool recursively = false);
    bool createFile(bool failIfThere = false);

    ///////////////////////////////////////////////////////////////////////
    // Copy other file (as FileSystemPath) to a file at this FileSystemPath.

    bool copyFromFile(const FileSystemPath& other);

    ///////////////////////////////////////////////////////////////////////
    // Rename this file to a name in an other file (as FileSystemPath).

    bool renameTo(const FileSystemPath& other) const;

    ///////////////////////////////////////////////////////////////////////
    // Remove this file or directory -- if directory, and if recursive,
    // delete all its contents

    bool remove(bool recursive = false);

    ///////////////////////////////////////////////////////////////////////
    // Accessors: is it valid? is it relative? is it a "root" path?
    // And then, does it exist? is it a file? a directory? how big is it?
    // and when was it last changed?

    bool isValid() const { return m_isValid; }
    bool isRelative() const { return m_isValid && m_path.isRelative(); }
    bool isRoot() const { return m_path.isRoot(); }
    bool exists() const;
    bool isFile() const;
    bool isDir() const;
    uint64_t size() const;
    TimeStamp lastModTime() const;

private:

    ///////////////////////////////////////////////////////////////////////
    // realize -- expects the embedded path is valid. checks if a path is
    // relative; if relative, tries to make it absolute; if that succeeds,
    // m_path becomes an absolute path; otherwise the path is marked invalid

    void realize();

    FilePath                m_path;
    bool                    m_isValid;
};

}
