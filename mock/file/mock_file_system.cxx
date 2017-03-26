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

FilePath FileSystemMock::getTempFilePath() const
{
    return mockGetTempFilePath();
}

utf8String FileSystemMock::getEnvironmentVariable(const char* variableName) const
{
    return mockGetEnvironmentVariable(variableName);
}

unsigned long FileSystemMock::getProcessID() const
{
    return mockGetProcessID();
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

FilePath FileSystemMock::getCurrentWorkingDirectory() const
{
    return mockGetCwd();
}

bool FileSystemMock::createDirectory(const FilePath& filePath) const
{
    return mockCreateDirectory(filePath);
}

bool FileSystemMock::removeDirectory(const FilePath& filePath) const
{
    return mockRemoveDirectory(filePath);
}

bool FileSystemMock::createFile(const FilePath& filePath) const
{
    return mockCreateFile(filePath);
}

bool FileSystemMock::move(const FilePath& filePath, const FilePath& newName) const
{
    return mockMove(filePath, newName);
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


uint64_t FileSystemMock::fileSize(unsigned long long handle, unsigned int& errorID) const
{
    return mockFileSize(handle, errorID);
}

void FileSystemMock::close(unsigned long long handle, unsigned int& errorID) const
{
    mockClose(handle, errorID);
}

void FileSystemMock::seek(unsigned long long handle, off_t position, unsigned int& errorID) const
{
    mockSeek(handle, position, errorID);
}

size_t FileSystemMock::read(unsigned long long handle, char* destination, size_t destSize, unsigned int& errorID) const
{
    return mockRead(handle, destination, destSize, errorID);
}

size_t FileSystemMock::write(unsigned long long handle, const char* source, size_t srcSize, unsigned int& errorID) const
{
    return mockWrite(handle, source, srcSize, errorID);
}

utf8String FileSystemMock::errorAsString(unsigned int errorID) const
{
    return mockErrorAsString(errorID);
}

DirectoryListPrimitive* FileSystemMock::newPathIterator(const FilePath& directory) const
{
    auto r = new DirectoryListMock(directory);
    auto iMocker = m_dirMockers.find(directory.asUtf8String());
    if (iMocker != m_dirMockers.end())
    {
        iMocker->second(*r);
    }
    else
    {
        m_lister = r;
    }
    return r;
}

void FileSystemMock::registerPathIteratorMocker(const FilePath& dirToIterate, PathIteratorMocker function)
{
    m_dirMockers[dirToIterate.asUtf8String()] = function;
}

DirectoryListMock::DirectoryListMock(const FilePath& parentDir)
 : DirectoryListPrimitive(),
   m_parentDir(parentDir)
{
}

FilePath DirectoryListMock::operator()()
{
    return mockInvocation();
}

}
