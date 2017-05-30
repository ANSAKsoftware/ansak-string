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
// nullptr_exception.cxx -- Class that derives std::exception for a nullptr.
//
///////////////////////////////////////////////////////////////////////////

#include <nullptr_exception.hxx>
#include <sstream>

using namespace std;

namespace ansak
{

//============================================================================
// public, constructor

NullPtrException::NullPtrException
(
    const char*     function,       // I - __FILE__ if desired, def nulllptr
    unsigned int    line            // I - __LINE__ if desired, def 0
) noexcept
{
    try
    {
        ostringstream os;

        os << "Unexpected Nullptr";
        if (function != nullptr) os << " in function, " << function;
        if (line != 0) os << ", at line #" << line;
        os << '.';
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//============================================================================
// public, destructor

NullPtrException::~NullPtrException() noexcept { }

//============================================================================
// virtual, public

const char* NullPtrException::what() const noexcept
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
        static const char exceptedWhat[] = "NullPtrException - throw on what()";
        return exceptedWhat;
    }

    static const char emptyWhat[] = "NullPtrException - no info available.";
    return emptyWhat;
}

}
