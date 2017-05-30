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
// file_handle_exception.hxx -- Class for exceptions around platform-
//                              dependant file handles. Exceptions are only
//                              thrown if the client chooses them over error
//                              returns.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "file_system_path.hxx"

namespace ansak
{

class FileSystemPath;

///////////////////////////////////////////////////////////////////////////
// std::exception-derived exception when the arguments are problematic

class FileHandleException : public std::exception
{
public:
    ///////////////////////////////////////////////////////////////////////
    // Constructor -- includes platform specific error ID (GetLastError() on
    // Windows, errno otherwise) and a string to describe the problem

    FileHandleException
    (
        const FileSystemPath&   problem,        // I - file system path in play
        unsigned int            errorCode,      // I - OS-specific error code, if any
        const std::string&      message         // I - a message describing the problem
    ) noexcept;

    FileHandleException
    (
        const FileHandleException&  src,            // I - a source FileHandleException
        size_t                      inProgress      // I - a progress report around something that failed
    ) noexcept;

    virtual ~FileHandleException() noexcept override;
    virtual const char* what() const noexcept override;
    unsigned int whatCode() const noexcept { return m_code; }

    size_t getInProgress() const noexcept { return m_inProgress; }

private:
    std::string         m_what = std::string();     // what-message
    unsigned int        m_code;                     // OS-specific error code, if any
    size_t              m_inProgress = 0;           // an in-progress marker, if important
};

}
