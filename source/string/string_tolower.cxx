///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015, Arthur N. Klassen
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
// 2015.10.15 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_tolower.cxx -- a universal "strlwr" function
//
///////////////////////////////////////////////////////////////////////////

#include <string.hxx>
#include <string_internal.hxx>

using namespace std;
using namespace ansak::internal;

namespace ansak {

///////////////////////////////////////////////////////////////////////////
// Local Functions

namespace
{

//=========================================================================
// Is a language identifier for a Turkic language? "tr", "az", "tt", "kk",
// "tur", "aze", "azb", "azj", "tat", or "kaz"
//
// Returns true if lang is not nullptr, and one of the 10 strings above;
// false otherwise (optimized for string length less equal to 2 or 3)

bool isTurkicLang(const char* lang)
{
    if (lang == nullptr || lang[0] == '\0' || lang[1] == '\0')
    {
        return false;
    }
    if (lang[2] == '\0')
    {
        return (lang[0] == 't' && lang[1] == 'r') ||    // Turkish
               (lang[0] == 'a' && lang[1] == 'z') ||    // Azerbaizhani
               (lang[0] == 't' && lang[1] == 't') ||    // Tatar
               (lang[0] == 'k' && lang[1] == 'k');      // Kazakh
    }
    if (lang[3] == '\0')
    {
        return (lang[0] == 't' && lang[1] == 'u' && lang[2] == 'r') ||    // Turkish
               (lang[0] == 't' && lang[1] == 'a' && lang[2] == 't') ||    // Tatar
               (lang[0] == 'k' && lang[1] == 'a' && lang[2] == 'z') ||    // Kazakh
               (lang[0] == 'a' && lang[1] == 'z' && (
                 lang[2] == 'e' || lang[2] == 'b' || lang[2] == 'j'));    // Azerbaizhani
    }
    return false;
}

}

///////////////////////////////////////////////////////////////////////////
// Public Functions

//////////////////// To Lower

// From char/UTF-8 ///////////////////////////////////////

utf8String toLower
(
    const utf8String&       src,        // I - the source
    const char*             lang        // I - the optional language code, def nullptr
)
{
    ucs4String src4(toUcs4(src));
    if (src4.empty())
    {
        return utf8String();
    }
    return toUtf8(toLower(src4, lang));
}

// From UCS-2/UTF-16 /////////////////////////////////////

utf16String toLower
(
    const utf16String&      src,        // I - the source
    const char*             lang        // I - the optional language code, def nullptr
)
{
    ucs4String src4(toUcs4(src));
    if (src4.empty())
    {
        return utf16String();
    }
    return toUtf16(toLower(src4, lang));
}

// From UCS-4 ////////////////////////////////////////////

ucs4String toLower
(
    const ucs4String&       src,        // I - the source
    const char*             lang        // I - the optional language code, def nullptr
)
{
    auto toLowerFunc = isTurkicLang(lang) ? internal::turkicToLower :
                                            internal::toLower;
    ucs4String result;
    CharacterAdder<char32_t> adder(result);

    for (auto c : src)
    {
        adder(toLowerFunc(c));
    }

    return result;
}

}
