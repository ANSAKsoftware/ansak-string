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
// 2017.04.04 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// mock_file_handle_exception.cxx -- implementation of a mock to FileHandleException
//
///////////////////////////////////////////////////////////////////////////

#include <mock_file_handle_exception.hxx>
#include <gmock/gmock.h>

#include <sstream>

using namespace std;

namespace ansak
{

FileHandleExceptionMock* FileHandleExceptionMock::m_currentMock = nullptr;

FileHandleException::FileHandleException
(
    const FileSystemPath&   problem,        // I - file system path in play
    unsigned int            errorCode,      // I - OS-specific error code, if any
    const string&           message         // I - a message describing the problem
) noexcept :
    m_what(),
    m_code(errorCode)
{
    ostringstream os;
    os << "Mocked up exception: " << message << "; file = " << problem.asUtf8String();
    m_what = os.str();
}

FileHandleException::FileHandleException
(
    const FileHandleException&  src,            // I - a source FileHandleException
    size_t                      inProgress      // I - a progress report around something that failed
) noexcept :
    m_what(),
    m_code(src.m_code)
{
    ostringstream os;
    os << "Mocked up progressive exception: " << src.m_what << "; progress = " << inProgress;
    m_what = os.str();
    m_inProgress = inProgress;
}

FileHandleException::~FileHandleException() noexcept
{
}

const char* FileHandleException::what() const noexcept
{
    return FileHandleExceptionMock::getMock()->mockWhat(this);
}

}
