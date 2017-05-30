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
// runtime_exception.hxx -- debug-assert-like things (used for file-of-lines,
//                          especially).
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <exception>
#include <string>

namespace ansak
{

///////////////////////////////////////////////////////////////////////////
// class RuntimeException -- a std::exception-derived friendly assertion
//                           exception

class RuntimeException : public std::exception
{
public:

    ///////////////////////////////////////////////////////////////////////
    // Constructor -- expected to be called from ansak::enforce()

    RuntimeException
    (
        const char*         complaint,      // I - text describing the violation
        const char*         fileName,       // I - __FILE__ if you will
        unsigned int        lineNumber      // I - __LINE__ if you will
    ) noexcept;

    virtual ~RuntimeException() noexcept override { }
    virtual const char* what() const noexcept override;

    ///////////////////////////////////////////////////////////////////////
    // Accessors for inner bits of a runtime exception.

    const char* fileName() const noexcept { return m_fileName.c_str(); }
    unsigned int lineNumber() const noexcept { return m_lineNumber; }

private:
    std::string             m_what = std::string();
    std::string             m_fileName = std::string();
    unsigned int            m_lineNumber = 0u;
};

///////////////////////////////////////////////////////////////////////////
// A friendlier way to assert your interfaces (called "enforce", not to
// collide with assert.h)

inline void enforce
(
    bool                condition,              // I - condition to test
    const char*         complaint = nullptr,    // I - a description of the condition
    const char*         fileName = nullptr,     // I - __FILE__ if you will
    unsigned int        lineNumber = 0          // I - __LINE__ if you will
)
{
    if (!condition) throw RuntimeException(complaint, fileName, lineNumber);
}

}
