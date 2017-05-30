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
// 2016.01.23 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_handle.hxx -- Class that wraps platform-dependant file handle. I'd
//                    use the open/lseek/close library but they won't do
//                    internationalized names in Windows. Gotta abstract
//                    it away...
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <exception>
#include <sys/types.h>
#include <vector>
#include "file_system_path.hxx"

namespace ansak
{

///////////////////////////////////////////////////////////////////////////
// class FileHandle -- operations around a file that generate or require an
//                     OS-specific file handle; wraps them to hide the details

class FileHandle {

    FileHandle(const FileSystemPath& path) : m_path(path) { }

    static const unsigned long long nullHandle = ~static_cast<unsigned long long>(0);
    static const size_t bufferSize = 4096;

public:

    static const off_t          currentSpot = static_cast<off_t>(-1);
    static const size_t         toFileEnd = static_cast<size_t>(~0);

    enum CreateType : int { kFailIfThere, kOpenIfThere };
    enum OpenType : int { kFailIfNotThere, kCreateIfNotThere };

    FileHandle() : m_path() { }
    ~FileHandle();

    ///////////////////////////////////////////////////////////////////////==
    // Disable assign but allow move

    FileHandle(FileHandle& src) = delete;
    FileHandle(FileHandle&& src);
    FileHandle& operator=(const FileHandle& src) = delete;
    FileHandle& operator=(FileHandle&& src);

    static FileHandle create(const FileSystemPath& path, CreateType mode = kFailIfThere);
    static FileHandle open(const FileSystemPath& path, OpenType type = kFailIfNotThere);
    static FileHandle openForReading(const FileSystemPath& path);
    static FileHandle openForAppending(const FileSystemPath& path, OpenType type = kCreateIfNotThere);

    uint64_t size();

    void close();
    bool isValid() const { return m_path.isValid(); }
    bool isOpen() const { return m_fh != nullHandle; }

    void seek(off_t pos);
    size_t read(char* dest, size_t destSize);
    size_t write(const char* src, size_t srcSize);
    size_t copyFrom(FileHandle& otherFile, off_t start = currentSpot, size_t count = toFileEnd);

    bool operator==(const FileHandle& rhs) const;
    bool operator!=(const FileHandle& rhs) const { return !(*this == rhs); }

    static void setPermission(int permissionConstant);

    static void throwErrors();

private:

    bool create();
    bool open(int mode);

    FileSystemPath      m_path;
    unsigned long long  m_fh = nullHandle;
    std::vector<char>   m_buffer = std::vector<char>();

    static bool&        m_throwErrors;
};

}
