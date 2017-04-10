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
// 2017.04.05 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// mock_file_handle.cxx -- implementation of a mock to FileHandle
//
///////////////////////////////////////////////////////////////////////////

#include "mock_file_handle.hxx"
#include <gmock/gmock.h>

#include <sstream>

using namespace testing;

namespace ansak
{

FileHandleMock* FileHandleMock::m_currentMock = nullptr;

FileHandle::~FileHandle()
{
}

FileHandle::FileHandle(FileHandle&& src) : m_path(FileSystemPath())
{
    m_path = src.m_path;    src.m_path = FileSystemPath();
    m_fh   = src.m_fh;      src.m_fh = nullHandle;
}

FileHandle FileHandle::open(const FileSystemPath& path, FileHandle::OpenType)
{
    FileHandle result(path);
    result.m_fh = 35u;
    return result;
}

bool FileHandle::operator==(const FileHandle& rhs) const
{
    if (this == &rhs)   return true;
    return m_path == rhs.m_path && m_fh == rhs.m_fh;
}

size_t FileHandle::read(char* dest, size_t destSize)
{
    return FileHandleMock::getMock()->mockRead(this, dest, destSize);
}

}
