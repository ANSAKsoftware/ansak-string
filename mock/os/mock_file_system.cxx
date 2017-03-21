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
// mock_file_system.cxx -- declaration of a mock to the os primitives used by
//                         FileSystemPath and FileHandle
//
///////////////////////////////////////////////////////////////////////////

#include "mock_file_system.hxx"

using namespace testing;

namespace ansak {

FileSystemMock::~FileSystemMock()
{
}

bool FileSystemMock::pathExists(const FilePath& filePath) const
{
    return mockPathExists(filePath);
}

bool FileSystemMock::pathIsFile(const FilePath& filePath) const
{
    return mockPathIsFile(filePath);
}

bool FileSystemMock::pathIsDir(const FilePath& filePath) const
{
    return mockPathIsDir(filePath);
}

uint64_t FileSystemMock::fileSize(const FilePath& filePath) const
{
    return mockFileSize(filePath);
}

TimeStamp FileSystemMock::lastModTime(const FilePath& filePath) const
{
    return mockLastModTime(filePath);
}

bool FileSystemMock::createDirectory(const FilePath& filePath) const
{
    return mockCreateDirectory(filePath);
}

bool FileSystemMock::createFile(const FilePath& filePath) const
{
    return mockCreateFile(filePath);
}

bool FileSystemMock::rename(const FilePath& filePath, const utf8String& newName) const
{
    return mockRename(filePath, newName);
}

bool FileSystemMock::remove(const FilePath& filePath) const
{
    return mockRemove(filePath);
}

unsigned long long FileSystemMock::create(const FilePath& path, int permissions, unsigned int& errorID) const
{
    return mockCreate(path, permissions, errorID);
}

unsigned long long FileSystemMock::open(const FilePath& path, OpenMode mode, int permissions, unsigned int& errorID) const
{
    return mockOpen(path, mode, permissions, errorID);
}


uint64_t FileSystemMock::fileSize(unsigned long long handle) const
{
    return mockFileSize(handle);
}

void FileSystemMock::close(unsigned long long handle) const
{
    mockClose(handle);
}

void FileSystemMock::seek(unsigned long long handle, off_t position) const
{
    mockSeek(handle, position);
}

size_t FileSystemMock::read(unsigned long long handle, char* destination, size_t destSize) const
{
    return mockRead(handle, destination, destSize);
}

size_t FileSystemMock::write(unsigned long long handle, const char* source, size_t srcSize) const
{
    return mockWrite(handle, source, srcSize);
}

DirectoryListMock::DirectoryListMock(const FilePath& parentDir)
 : DirectoryListPrimitive(),
   m_parentDir(parentDir)
{
}

void DirectoryListMock::expectOneElement(const char* element)
{
    EXPECT_CALL(*this, mockInvocation()).WillOnce(Return(m_parentDir.child(element)));
}

void DirectoryListMock::expectDone()
{
    EXPECT_CALL(*this, mockInvocation()).WillOnce(Return(FilePath()));
}

void DirectoryListMock::setupSimpleMock()
{
#if defined(WIN32)
    m_parentDir = FilePath("C:\\Users\\me");
#else
    m_parentDir = FilePath("/home/me");
#endif

    expectOneElement("alpha");
    expectOneElement("bravo");
    expectOneElement("charlie");
    expectOneElement("delta");
    expectDone();
}

FilePath DirectoryListMock::operator()()
{
    return mockInvocation();
}

}
