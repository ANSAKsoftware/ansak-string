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
// file_handle_exception.hxx -- Implementation of exceptions around platform-
//                              dependant file handles. Exceptions are only
//                              thrown if the client chooses them over error
//                              returns.
//
///////////////////////////////////////////////////////////////////////////

#include <file_handle_exception.hxx>
#include <operating_system_primitives.hxx>

#include <sstream>

using namespace std;

namespace ansak
{

//============================================================================
// public, constructor

FileHandleException::FileHandleException
(
    const FileSystemPath&   problem,
    unsigned int            errorCode,
    const std::string&      message
) noexcept :
    m_code(errorCode)
{
    try
    {
        ostringstream os;
        os << "FileHandleException: " << message <<
              "; file = \"" << problem.asUtf8String() << "\"; ";
        if (errorCode != 0)
        {
            os << " code = " << errorCode <<
                  "; code explanation: " << getOperatingSystemPrimitives()->errorAsString(errorCode) << "; ";
        }
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//============================================================================
// public, constructor (almost copy)

FileHandleException::FileHandleException
(
    const FileHandleException&  src,            // I - a source FileHandleException
    size_t                      inProgress      // I - a progress report around something that failed
) noexcept :
    m_what(),
    m_code(src.m_code),
    m_inProgress(inProgress)
{
    try
    {
        ostringstream os;
        os << src.m_what << " (progress: " << inProgress << ')';
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//============================================================================
// public, destructor

FileHandleException::~FileHandleException() noexcept
{
}

//============================================================================
// virtual, public

const char* FileHandleException::what() const noexcept
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
        static const char exceptedWhat[] = "FileHandleException - throw on what()";
        return exceptedWhat;
    }
    static const char emptyWhat[] = "FileHandleExcpetion: no details are available.";
    return emptyWhat;
}

}
