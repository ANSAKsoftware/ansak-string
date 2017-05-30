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
// 2016.02.01 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// runtime_exception.cxx -- debug-assert-like things (used for file-of-lines,
//                          especially).
//
///////////////////////////////////////////////////////////////////////////

#include <runtime_exception.hxx>

#include <string>
#include <sstream>

using namespace std;

namespace ansak
{

//============================================================================
// public, constructor

RuntimeException::RuntimeException
(
    const char*         complaint,      // I - text describing the violation
    const char*         fileName,       // I - __FILE__ if you will
    unsigned int        lineNumber      // I - __LINE__ if you will
) noexcept
{
    try
    {
        const char* realComplaint = (complaint == nullptr || *complaint == '\0') ?
                        "<No complaint message specified>" : complaint;
        ostringstream os;
        os << "Runtime Exception: " << realComplaint;
        if (fileName != nullptr && *fileName != '\0')
        {
            m_fileName = fileName;
            os << " in file, " << fileName;
            if (lineNumber != 0)
            {
                m_lineNumber = lineNumber;
                os << " at line number#" << lineNumber;
            }
            else
            {
                m_lineNumber = 0u;
            }
        }
        else
        {
            m_fileName = fileName == nullptr ? "<no filename given>" : "<empty-string filename given>";
            m_lineNumber = 0u;
        }
        os << '.';
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//============================================================================
// public, virtual

const char* RuntimeException::what() const noexcept
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
        static const char exceptedWhat[] = "RuntimeException - throw on what()";
        return exceptedWhat;
    }

    static const char emptyWhat[] = "RuntimeException - no info available.";
    return emptyWhat;
}

}
