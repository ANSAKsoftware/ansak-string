#pragma once

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
// 2016.03.16 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_system_primitives.hxx -- Definition of file system abstraction layer
//          interfaces
//
///////////////////////////////////////////////////////////////////////////

#include <string.hxx>
#include <time_stamp.hxx>

namespace ansak
{

class DirectoryListPrimitive;

class FileSystemPrimitives
{
public:

    virtual ~FileSystemPrimitives() = default;

    virtual bool pathExists(const FilePath& filePath) const = 0;
    virtual bool pathIsFile(const FilePath& filePath) const = 0;
    virtual bool pathIsDir(const FilePath& filePath) const = 0;
    virtual uint64_t fileSize(const FilePath& filePath) const = 0;
    virtual TimeStamp lastModTime(const FilePath& filePath) const = 0;
    virtual FilePath getCurrentWorkingDirectory() const = 0;
    virtual bool createDirectory(const FilePath& filePath) const = 0;
    virtual bool createFile(const FilePath& filePath) const = 0;
    virtual bool rename(const FilePath& filePath, const utf8String& newName) const = 0;
    virtual bool remove(const FilePath& filePath) const = 0;

    enum OpenMode : int { kForReading, kForAppending, kForReadingAndWriting };

    virtual unsigned long long create(const FilePath& path, int permissions, unsigned int& errorID) const = 0;
    virtual unsigned long long open(const FilePath& path, OpenMode mode, int permissions, unsigned int& errorID) const = 0;
    virtual uint64_t fileSize(unsigned long long handle, unsigned int& errorID) const = 0;
    virtual void close(unsigned long long handle, unsigned int& errorID) const = 0;
    virtual void seek(unsigned long long handle, off_t position, unsigned int& errorID) const = 0;
    virtual size_t read(unsigned long long handle, char* destination, size_t destSize, unsigned int& errorID) const = 0;
    virtual size_t write(unsigned long long handle, const char* source, size_t srcSize, unsigned int& errorID) const = 0;

    virtual ansak::utf8String errorAsString(unsigned int errorID) const = 0;

    virtual DirectoryListPrimitive* newPathIterator(const FilePath& directory) const = 0;
};

class DirectoryListPrimitive
{
public:

    DirectoryListPrimitive() = default;
    virtual ~DirectoryListPrimitive() = default;

    virtual FilePath operator()() = 0;
};

const FileSystemPrimitives* getPrimitives();

}
