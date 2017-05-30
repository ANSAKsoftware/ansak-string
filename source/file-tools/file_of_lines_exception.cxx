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
// file_of_lines_exception.cxx -- Exception for file-as-vector<string>
//
///////////////////////////////////////////////////////////////////////////

#include <file_of_lines_exception.hxx>
#include <file_handle.hxx>
#include <file_handle_exception.hxx>
#include <file_path.hxx>
#include <operating_system_primitives.hxx>

#include <sstream>

using namespace std;

namespace ansak
{

//===========================================================================
// public, constructor

FileOfLinesException::FileOfLinesException
(
    const char*                 message,        // I - a message about a FileOfLines failure
    const FilePath&             fileName,       // I - the file name being parsed, def invalidPath
    unsigned long long          fileOffset      // I - an offset into the file, if available, def ~0
) noexcept :
    m_what()
{
    try
    {
        ostringstream os;
        os << "FileOfLinesException: " << message;
        if (fileName == FilePath::invalidPath())
        {
            os << '.';
        }
        else
        {
            os << "; file = " << fileName.asUtf8String();
            if (fileOffset == ~static_cast<unsigned long long>(0))
            {
                os << '.';
            }
            else
            {
                os << "; error occurred at or near offset " << fileOffset << '.';
            }
        }
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//===========================================================================
// public, constructor

FileOfLinesException::FileOfLinesException
(
    const char*                 message,        // I - a message about a FileOfLines failure
    const FilePath&             fileName,       // I - the file name being parsed
    const FileHandleException&  ioProblem,      // I - a FileHandle problem being wrapped
    unsigned long long          fileOffset      // I - an offset into the file, if available, def ~0
) noexcept :
    m_what()
{
    try
    {
        ostringstream os;
        os << "FileOfLinesException: " << message
           << "; file = " << fileName.asUtf8String()
           << "; exception = (" << ioProblem.what() << ')';
        if (fileOffset == ~static_cast<unsigned long long>(0))
        {
            os << '.';
        }
        else
        {
            os << "; error occurred at or near offset " << fileOffset << '.';
        }

        m_what = os.str();
    }
    catch (...)
    {
    }
}

//===========================================================================
// public, destructor

FileOfLinesException::~FileOfLinesException() noexcept
{
}

//===========================================================================
// public, virtual

const char* FileOfLinesException::what() const noexcept
{
    try
    {
        if (!m_what.empty())
        {
            return m_what.c_str();
        }
    }
    catch (...)
    {
        static const char exceptedWhat[] = "FileOfLinesException - throw on what()";
        return exceptedWhat;
    }

    static const char emptyWhat[] = "FileOfLinesException - no info available.";
    return emptyWhat;
}

}
