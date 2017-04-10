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
// 2016.11.28 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines_exception.hxx -- Exception for file-as-vector<string>
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "file_path.hxx"

#include <string>
#include <exception>

namespace ansak
{

class FileHandleException;

//===========================================================================
// std::exception-derived exception when the arguments are problematic

class FileOfLinesException : public std::exception
{
public:

    /////////////////////////////////////////////////////////////////////////
    // Constructors
    FileOfLinesException
    (
        const char*                 message,        // I - a message about a FileOfLines failure
        const FilePath&             fileName        // I - the file name being parsed
                                            = FilePath::invalidPath(),
        unsigned long long          fileOffset      // I - an offset into the file, if available
                                            = ~static_cast<unsigned long long>(0)
    ) noexcept;

    FileOfLinesException
    (
        const char*                 message,        // I - a message about a FileOfLines failure
        const FilePath&             fileName,       // I - the file name being parsed
        const FileHandleException&  ioProblem,      // I - a FileHandle problem being wrapped
        unsigned long long          fileOffset      // I - an offset into the file, if available
                                            = ~static_cast<unsigned long long>(0)
    ) noexcept;

    /////////////////////////////////////////////////////////////////////////
    // Destructors
    virtual ~FileOfLinesException() noexcept override;

    /////////////////////////////////////////////////////////////////////////
    // Return information about the exception
    virtual const char* what() const noexcept override;

private:
    std::string         m_what = std::string();
};

}
