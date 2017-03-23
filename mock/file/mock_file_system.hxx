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
// 2017.03.17 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// mock_file_system.hxx -- implementation of a mock to the os primitives
//                         used by FileSystemPath and FileHandle
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <file_path.hxx>
#include <file_system_primitives.hxx>

#include <gmock/gmock.h>

namespace ansak
{

class FileSystemMock : public FileSystemPrimitives
{
public:

    ~FileSystemMock() = default;

    bool pathExists(const FilePath& filePath) const override;
    bool pathIsFile(const FilePath& filePath) const override;
    bool pathIsDir(const FilePath& filePath) const override;
    uint64_t fileSize(const FilePath& filePath) const override;
    TimeStamp lastModTime(const FilePath& filePath) const override;
    FilePath getCurrentWorkingDirectory() const override;
    bool createDirectory(const FilePath& filePath) const override;
    bool createFile(const FilePath& filePath) const override;
    bool rename(const FilePath& filePath, const utf8String& newName) const override;
    bool remove(const FilePath& filePath) const override;

    unsigned long long create(const FilePath& path, int permissions, unsigned int& errorID) const override;
    unsigned long long open(const FilePath& path, OpenMode mode, int permissions, unsigned int& errorID) const override;

    uint64_t fileSize(unsigned long long handle, unsigned int& errorID) const override;
    void close(unsigned long long handle, unsigned int& errorID) const override;
    void seek(unsigned long long handle, off_t position, unsigned int& errorID) const override;
    size_t read(unsigned long long handle, char* destination, size_t destSize, unsigned int& errorID) const override;
    size_t write(unsigned long long handle, const char* source, size_t srcSize, unsigned int& errorID) const override;

    virtual ansak::utf8String errorAsString(unsigned int errorID) const override;

    DirectoryListPrimitive* newPathIterator(const FilePath& directory) const override;

    MOCK_CONST_METHOD1(mockPathExists, bool(const FilePath&));
    MOCK_CONST_METHOD1(mockPathIsFile, bool(const FilePath&));
    MOCK_CONST_METHOD1(mockPathIsDir, bool(const FilePath&));
    MOCK_CONST_METHOD1(mockFileSize, uint64_t(const FilePath&));
    MOCK_CONST_METHOD1(mockLastModTime, TimeStamp(const FilePath&));
    MOCK_CONST_METHOD0(mockGetCwd, FilePath());
    MOCK_CONST_METHOD1(mockCreateDirectory, bool(const FilePath&));
    MOCK_CONST_METHOD1(mockCreateFile, bool(const FilePath&));
    MOCK_CONST_METHOD2(mockRename, bool(const FilePath&, const utf8String&));
    MOCK_CONST_METHOD1(mockRemove, bool(const FilePath&));

    MOCK_CONST_METHOD3(mockCreate, unsigned long long(const FilePath&, int, unsigned int&));
    MOCK_CONST_METHOD4(mockOpen, unsigned long long(const FilePath&, OpenMode, int, unsigned int&));

    MOCK_CONST_METHOD2(mockFileSize, uint64_t(unsigned long long, unsigned int&));
    MOCK_CONST_METHOD2(mockClose, void(unsigned long long, unsigned int&));
    MOCK_CONST_METHOD3(mockSeek, void(unsigned long long, off_t, unsigned int&));
    MOCK_CONST_METHOD4(mockRead, size_t(unsigned long long, char*, size_t, unsigned int&));
    MOCK_CONST_METHOD4(mockWrite, size_t(unsigned long long, const char*, size_t, unsigned int&));

    MOCK_CONST_METHOD1(mockErrorAsString, ansak::utf8String(unsigned int));

    MOCK_CONST_METHOD1(mockNewPathIterator, DirectoryListPrimitive*(const FilePath& directory));

    mutable DirectoryListPrimitive* m_lister = nullptr;
};

class DirectoryListMock : public DirectoryListPrimitive
{
public:

    DirectoryListMock(const FilePath& parentDir);
    ~DirectoryListMock() = default;

    FilePath operator()() override;

    void setupSimpleMock();
    void expectOneElement(const char* oneSubDir);
    void expectDone();

    MOCK_METHOD0(mockInvocation, FilePath());

private:

    FilePath m_parentDir;
};

}
