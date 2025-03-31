///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015, 2021, Arthur N. Klassen
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
// 2014.10.08 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_internal.hxx -- some enums, templates and inlines to make string.cxx
//                        implementation cleaner
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "string.hxx"
#include <functional>

namespace ansak {

namespace internal {

///////////////////////////////////////////////////////////////////////////
// Local Types

//=========================================================================
// flags that show what classifications a particular character can fit into
// (by range)

enum RangeTypeFlags : int {
    kNoFlag =              0x0000,
    kAsciiFlag =           0x0001,
    kUtf8Flag  =           0x0002,
    kUcs2Flag  =           0x0004,
    kUtf16Flag =           0x0008,
    kUcs4Flag  =           0x0010,
    kUnicodeFlag =         0x0020,
    kInvalidRangeFlag =    0x8000,

    k7BitFlags  = kAsciiFlag | kUtf8Flag | kUcs2Flag | kUtf16Flag | kUcs4Flag | kUnicodeFlag,   // 0x3f
    k16BitFlags =              kUtf8Flag | kUcs2Flag | kUtf16Flag | kUcs4Flag | kUnicodeFlag,   // 0x3e
    k21BitFlags =              kUtf8Flag |             kUtf16Flag | kUcs4Flag,                  // 0x1a

    k21BitUnicodeFlags = k21BitFlags | kUnicodeFlag,                             // 0x3a,
    k31BitFlags =              kUtf8Flag |                          kUcs4Flag,   // 0x12
    k32BitFlags =                                                   kUcs4Flag    // 0x10
};

//=========================================================================
// flags that show what classifications a particular character can fit into
// (by value type)

enum EncodingTypeFlags : int {
    kNoneFlag =             0x0000,
    kIsAssignedFlag =       0x0001,
    kIsPrivateFlag =        0x0002,
    kIsControlFlag =        0x0004,
    kIsWhiteSpaceFlag =     0x0008
};

//=========================================================================
// A utility functor for appending encoded characters to a growing string

template <typename C>
class CharacterAdder
{
    static_assert(std::is_integral<C>::value, "CharacterAdder needs an integral type.");

public:
    CharacterAdder(std::basic_string<C>& result) : m_destination(result) {}
    void operator()(C c) { m_destination.push_back(c); }
private:
    std::basic_string<C>& m_destination;
};

///////////////////////////////////////////////////////////////////////////
// Local Functions

//=========================================================================
// Get current implementation's supported Unicode version string.

extern const utf8String supportedUnicodeVersion;

//=========================================================================
// Returns true if the character is in the selected Unicode 8 standard, false
// otherwise. Note, all 8-bit values are in the standard. Gaps first begin
// after U+0377

inline bool isUnicodeAssigned(char ) { return true; }
bool isUnicodeAssigned(char16_t c);
bool isUnicodeAssigned(char32_t c);

inline bool isUnicodePrivate(char ) { return false; }
bool isUnicodePrivate(char16_t c);
bool isUnicodePrivate(char32_t c);

bool isControlChar(char c);
bool isControlChar(char16_t c);
bool isControlChar(char32_t c);

bool isWhitespaceChar(char c);
bool isWhitespaceChar(char16_t c);
bool isWhitespaceChar(char32_t c);

//=========================================================================
// A utility function to check if a character is allowed within Unicode

template <typename C>
RangeTypeFlags getUnicodeEncodableRangeFlag(C c)
{
    static_assert(std::is_integral<C>::value, "getUnicodeEncodableRangeFlag needs an integral type.");

    if (sizeof(C) == 1 || (c != 0 && (c < 0xd800 || c > 0xdfff) && c < 0x110000))
    {
        return kUnicodeFlag;
    }
    return kNoFlag;
}

//=========================================================================
// A utility function to check what families of values a character belongs
// to

template <typename C>
RangeTypeFlags getCharEncodableRangeFlags(C c)
{
    static_assert(std::is_integral<C>::value, "getCharEncodableRangeFlags needs an integral type.");

    if (sizeof(C) == 1)
    {
        return ((c & 0x80) == 0) ? k7BitFlags : k16BitFlags;
    }
    else if (c <= 0x7f)
    {
        return k7BitFlags;
    }
    else if (c >= 0xd800 && c <= 0xdfff)
    {
        return kInvalidRangeFlag;
    }
    else if (c < 0x10000)
    {
        return k16BitFlags;
    }
    else if (c < 0x200000)
    {
        return static_cast<RangeTypeFlags>(k21BitFlags | getUnicodeEncodableRangeFlag(c));
    }
    else
    {
        return ((c & 0x80000000) == 0) ? k31BitFlags : k32BitFlags;
    }
}

//=========================================================================
// A utility function to check what families of values a character once
// decoded will belong to

inline RangeTypeFlags getRangeFlag(char firstChar)
{
    auto f = static_cast<unsigned char>(firstChar);
    // start of 2-byte escapes -- anything but ASCII
    if (f <= 0x7f)
    {
        return k7BitFlags;
    }
    else if (f <= 0xbf)
    {
        return kInvalidRangeFlag;
    }
    // 2, 3-byte escapes -- it's a 16-bit value
    else if (f <= 0xef)
    {
        return k16BitFlags;
    }
    // 4-byte escapes, up to 21 bits, could still be unicode
    else if (f >= 0xf0 && f <= 0xf4)
    {
        return k21BitUnicodeFlags;
    }
    // start of 5,6-byte escapes -- anything but ASCII, 2-byte, Unicode
    else if (f >= 0xf5 && f <= 0xfd)
    {
        return k31BitFlags;
    }
    // let the other bits catch the rest
    return kInvalidRangeFlag;
}

//=========================================================================
// A utility function to check what families of values a character once
// decoded will belong to

inline RangeTypeFlags getRangeFlag(char16_t firstChar)
{
    // start of 21-bit escapes: only work for 21-bit and Unicode
    if (firstChar >= 0xd800 && firstChar <= 0xdbff)
    {
        return k21BitUnicodeFlags;
    }
    else if (firstChar >= 0xdc00 && firstChar <= 0xdfff)
    {
        return kInvalidRangeFlag;
    }
    else if (firstChar <= 0x7f)
    {
        return k7BitFlags;
    }
    return k16BitFlags;
}

//=========================================================================
// A template to generate an encoding mask (isAssigned? isPrivate? isControl?
// isWhitespace?) for any character

template<typename C>
uint32_t charToEncodingTypeMask(C c)
{
    static_assert(std::is_integral<C>::value, "charToEncodingTypeMask needs an integral type.");

    return (isUnicodeAssigned(c) ? kIsAssignedFlag : 0) |
           (isUnicodePrivate(c) ? kIsPrivateFlag : 0) |
           (isControlChar(c) ? kIsControlFlag : 0) |
           (isWhitespaceChar(c) ? kIsWhiteSpaceFlag : 0);
}

//=========================================================================
// Is character (of whatever type) first half of non-BMP UTF-16 pair
// (in range D800..DBFF)
//
// Returns true if in range, false otherwise.

template<typename C>
bool isFirstHalfUtf16
(
    C                   c       // I - character to test
)
{
    static_assert(std::is_integral<C>::value, "isFirstHalfUtf16 needs an integral type.");

    return c >= 0xd800 && c <= 0xdbff;
}

//=========================================================================
// Is character (of whatever type) second half of non-BMP UTF-16 pair
// (in range DC00..DFFF)
//
// Returns true if in range, false otherwise.

template<typename C>
bool isSecondHalfUtf16
(
    C                   c       // I - character to test
)
{
    static_assert(std::is_integral<C>::value, "isSecondHalfUtf16 needs an integral type.");

    return c >= 0xdc00 && c <= 0xdfff;
}

//=========================================================================
// Utility overloaded functions to convert 2-byte UTF-8 sequences to a single value

inline char16_t rawDecodeUtf8(char c0, char c1)
{
    unsigned char uc0 = c0, uc1 = c1;
    return ((uc0 & 0x1f) << 6) + (uc1 & 0x3f);
}

//=========================================================================
// Utility overloaded functions to convert 3-byte UTF-8 sequences to a single value.
// (Doesn't understand/handle d800-dbff/dc00-dfff pairs in UTF-16)

inline char16_t rawDecodeUtf8(char c0, char c1, char c2)
{
    unsigned char uc0 = c0, uc1 = c1, uc2 = c2;
    return ((uc0 & 0x0f) << 12) + ((uc1 & 0x3f) << 6) + (uc2 & 0x3f);
}

//=========================================================================
// Utility overloaded functions to convert 4-byte UTF-8 sequences to a single value.
// (Doesn't understand 11000 and up are not Unicode)

inline char32_t rawDecodeUtf8(char c0, char c1, char c2, char c3)
{
    unsigned char uc0 = c0, uc1 = c1, uc2 = c2, uc3 = c3;
    return ((uc0 & 0x07) << 18) + ((uc1 & 0x3f) << 12) + ((uc2 & 0x3f) << 6) +
            (uc3 & 0x3f);
}

//=========================================================================
// Utility overloaded functions to convert 5-byte UTF-8 sequences to a single value.
// (Doesn't understand that these chars are not Unicode)

inline char32_t rawDecodeUtf8(char c0, char c1, char c2, char c3, char c4)
{
    unsigned char uc0 = c0, uc1 = c1, uc2 = c2, uc3 = c3, uc4 = c4;
    char32_t wc0 = uc0, wc1 = uc1, wc2 = uc2, wc3 = uc3, wc4 = uc4;
    return ((wc0 & 0x03) << 24) + ((wc1 & 0x3f) << 18) + ((wc2 & 0x3f) << 12) +
           ((wc3 & 0x3f) << 6) + (wc4 & 0x3f);
}

//=========================================================================
// Utility overloaded functions to convert 6-byte UTF-8 sequences to a single value.
// (Doesn't understand that these chars are not Unicode)

inline char32_t rawDecodeUtf8(char c0, char c1, char c2, char c3, char c4, char c5)
{
    unsigned char uc0 = c0, uc1 = c1, uc2 = c2, uc3 = c3, uc4 = c4, uc5 = c5;
    char32_t wc0 = uc0, wc1 = uc1, wc2 = uc2, wc3 = uc3, wc4 = uc4, wc5 = uc5;
    return ((wc0 & 0x01) << 30) + ((wc1 & 0x3f) << 24) + ((wc2 & 0x3f) << 18) +
           ((wc3 & 0x3f) << 12) + ((wc4 & 0x3f) << 6) + (wc5 & 0x3f);
}

#if defined(_MSC_VER)
// MSVC can't figure out that some code never gets run when the associated
// shift would be too wide and warns needlessly.
#pragma warning( disable : 4333 )
#endif

//=========================================================================
// Utility template function to encode a single UCS-4 character into UTF-8,
// regardless of its value

template <typename C>
void encodeUtf8
(
    C                       c,          // I - A UCS-4 character to encode
    CharacterAdder<char>&   adder       // I/O - A character collecter to add it to
)
{
    static_assert(std::is_integral<C>::value, "encodeUtf8 needs an integral type.");

    if ((c & 0xffffff80) == 0)
    {
        adder(static_cast<char>(c));
    }
    else if (c <= 0x7ff)
    {
        adder(static_cast<char>(((c >> 6) & 0x1f) | 0xc0));
        adder(static_cast<char>((c & 0x3f) | 0x80));
    }
    else if (c <= 0xffff)
    {
        adder(static_cast<char>(((c >> 12) & 0x0f) | 0xe0));
        adder(static_cast<char>(((c >>  6) & 0x3f) | 0x80));
        adder(static_cast<char>((c & 0x3f) | 0x80));
    }
    else if (c <= 0x1fffff)
    {
        adder(static_cast<char>(((c >> 18) & 0x07) | 0xf0));
        adder(static_cast<char>(((c >> 12) & 0x3f) | 0x80));
        adder(static_cast<char>(((c >>  6) & 0x3f) | 0x80));
        adder(static_cast<char>((c & 0x3f) | 0x80));
    }
    else if (c <= 0x3ffffff)
    {
        adder(static_cast<char>(((c >> 24) & 0x03) | 0xf8));
        adder(static_cast<char>(((c >> 18) & 0x3f) | 0x80));
        adder(static_cast<char>(((c >> 12) & 0x3f) | 0x80));
        adder(static_cast<char>(((c >>  6) & 0x3f) | 0x80));
        adder(static_cast<char>((c & 0x3f) | 0x80));
    }
    else if (c <= 0x7fffffff)
    {
        adder(static_cast<char>(((c >> 30) & 0x01) | 0xfc));
        adder(static_cast<char>(((c >> 24) & 0x3f) | 0x80));
        adder(static_cast<char>(((c >> 18) & 0x3f) | 0x80));
        adder(static_cast<char>(((c >> 12) & 0x3f) | 0x80));
        adder(static_cast<char>(((c >>  6) & 0x3f) | 0x80));
        adder(static_cast<char>((c & 0x3f) | 0x80));
    }
}

#if defined(_MSC_VER)
#pragma warning( default : 4333 )
#endif

//=========================================================================
// Utility function to decode a single UCS-4 character from known UTF-16
// elements.

inline char32_t rawDecodeUtf16
(
    char16_t        c0,         // I - known 0xd800..0xdbff value
    char16_t        c1          // I - known 0xdc00..0xdfff value
)
{
    char32_t wc0 = c0, wc1 = c1;
    return 0x10000 + ((wc0 & 0x3FF) << 10) + (wc1 & 0x3ff);
}

//=========================================================================
// Template Utility function to encode a single UCS-4 character, known to be
// valid for UTF-16 into one or two UTF-16 elements.

template <typename C>
void rawEncodeUtf16
(
    C                           c,          // I - A UCS-4 character to encode
    CharacterAdder<char16_t>    adder       // I/O - A character collecter to add it to
)
{
    static_assert(std::is_integral<C>::value, "rawEncodeUtf16 needs an integral type.");

    if (c < 0x10000)
    {
        adder(static_cast<char16_t>(c));
    }
    else
    {
        c -= 0x10000;
        adder(static_cast<char16_t>(0xd800 + ((c >> 10) & 0x3ff)));
        adder(static_cast<char16_t>(0xdc00 + (c & 0x3ff)));
    }
}

//=========================================================================
// toLower of one character, from a Turkic and non-Turkic point of view.
//
// Returns the value of the lower case form, if any, of the input parameter;
// returns the same character otherwise.

char32_t turkicToLower(char32_t c);
char32_t toLower(char32_t c);

}

}
