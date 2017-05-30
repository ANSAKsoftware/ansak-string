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
// 2017.03.28 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// os_layer_windows.hxx -- class for Windows-specific implementations of operating
//                         system primitives
//
///////////////////////////////////////////////////////////////////////////

#include <operating_system_primitives.hxx>
#include <file_path.hxx>
#include <string.hxx>
#include <time_stamp.hxx>

namespace ansak {

/////////////////////////////////////////////////////////////////////////////
// Class to wrap the Linux primitives

class WindowsPrimitives : public OperatingSystemPrimitives
{
public:

    virtual ~WindowsPrimitives() = default;

    bool pathExists(const FilePath& filePath) const override;
    bool pathIsFile(const FilePath& filePath) const override;
    bool pathIsDir(const FilePath& filePath) const override;
    uint64_t fileSize(const FilePath& filePath) const override;
    TimeStamp lastModTime(const FilePath& filePath) const override;
    FilePath getCurrentWorkingDirectory() const override;
    bool createDirectory(const FilePath& filePath) const override;
    bool removeDirectory(const FilePath& filePath) const override;
    bool createFile(const FilePath& filePath) const override;
    bool move(const FilePath& filePath, const FilePath& newName) const override;
    bool remove(const FilePath& filePath) const override;

    unsigned long long create(const FilePath& path, int permissions, unsigned int& errorID) const override;
    unsigned long long open(const FilePath& path, OpenMode mode, int permissions, unsigned int& errorID) const override;
    uint64_t fileSize(unsigned long long handle, unsigned int& errorID) const override;
    void close(unsigned long long handle, unsigned int& errorID) const override;
    void seek(unsigned long long handle, off_t position, unsigned int& errorID) const override;
    size_t read(unsigned long long handle, char* destination, size_t destSize, unsigned int& errorID) const override;
    size_t write(unsigned long long handle, const char* source, size_t srcSize, unsigned int& errorID) const override;

    FilePath getTempFilePath() const override;
    utf8String getEnvironmentVariable(const char* variableName) const override;
    unsigned long getProcessID() const override;

    utf8String errorAsString(unsigned int errorID) const override;

    DirectoryListPrimitive* newPathIterator(const FilePath& directory) const override;
};

}
