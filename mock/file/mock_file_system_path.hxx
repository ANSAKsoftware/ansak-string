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
// 2017.03.28 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// mock_file_system_path.hxx -- declaration of a mock to FileSystemPath
//
///////////////////////////////////////////////////////////////////////////

#include <file_system_path.hxx>
#include <gmock/gmock.h>

namespace ansak
{

class FileSystemPathMock
{
public:
    static FileSystemPathMock* getMock() { return m_currentMock; }

    FileSystemPathMock();
    ~FileSystemPathMock();

    MOCK_METHOD1(createDirectory, bool(FileSystemPath*));
    MOCK_METHOD2(remove, bool(FileSystemPath*, bool));
    MOCK_METHOD1(exists, bool(const FileSystemPath*));
    MOCK_METHOD1(isFile, bool(const FileSystemPath*));
    MOCK_METHOD1(isDir, bool(const FileSystemPath*));
    MOCK_METHOD1(size, uint64_t(const FileSystemPath*));
    MOCK_METHOD1(parent, FileSystemPath(const FileSystemPath*));

private:

    static FileSystemPathMock* m_currentMock;
};

}
