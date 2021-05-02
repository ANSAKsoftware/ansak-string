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
// 2016.11.18 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_trim.hxx -- defines templates to trim strings
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "string.hxx"

#include <string>
#include <type_traits>

namespace ansak {

template<typename C>
void trim(std::basic_string<C>& victim)
{
    static_assert(std::is_integral<C>::value, "trim needs an integral type.");

    while (!victim.empty() && victim[victim.size() - 1] == static_cast<C>(' '))
    {
        victim.erase(victim.size() - 1);
    }
    while (!victim.empty() && victim[0] == static_cast<C>(' '))
    {
        victim.erase(0, 1);
    }
}

template<typename C>
void trim(std::basic_string<C>& victim, const std::basic_string<C>& spaces)
{
    static_assert(std::is_integral<C>::value, "trim needs an integral type.");

    while (!victim.empty() &&
           spaces.find_first_of(victim[victim.size() - 1]) != std::basic_string<C>::npos)
    {
        victim.erase(victim.size() - 1);
    }
    while (!victim.empty() && spaces.find_first_of(victim[0]) != std::basic_string<C>::npos)
    {
        victim.erase(0, 1);
    }
}

inline void trimAscii7WhiteSpace(std::string& victim)
{
    trim(victim, std::string("\x09\x0a\x0d "));
}

inline void trimWhiteSpace(ucs4String& victim)
{
    trim(victim, std::u32string(
            U"\U00000009\U0000000a\U0000000d \U000000a0\U00002000\U00002001\U00002002\U00002003"
            U"\U00002004\U00002005\U00002006\U00002007\U00002008\U00002009\U0000200a\U0000202f"
            U"\U0000205f\U00003000"));
}

inline void trimWhiteSpace(utf16String& victim)
{
    trim(victim, std::u16string(
            u"\u0009\u000a\u000d \u00a0\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008"
            u"\u2009\u200a\u202f\u205f\u3000"));
}

inline void trimWhiteSpace(utf8String& victim)
{
    auto v16 = toUtf16(victim);
    trimWhiteSpace(v16);
    victim = toUtf8(v16);
}

}
