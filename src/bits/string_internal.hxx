///////////////////////////////////////////////////////////////////////////
//
// 2014.10.08
//
// The author (Arthur N. Klassen) disclaims all copyright to this source code.
// In place of a legal notice, here is a blessing:
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
// (Is it too much to suggest retaining this header on this file?)
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

enum EncodingTypeFlags {
    kNoFlag = 0,
    kAsciiFlag = 0x0001,
    kUtf8Flag  = 0x0002,
    kUcs2Flag  = 0x0004,
    kUtf16Flag = 0x0008,
    kUcs4Flag  = 0x0010,
    kUnicodeFlag = 0x0020,
    kInvalidRangeFlag = 0x0040,

    k7BitFlags  = kAsciiFlag | kUtf8Flag | kUcs2Flag | kUtf16Flag | kUcs4Flag | kUnicodeFlag,   // 0x3f
    k16BitFlags =              kUtf8Flag | kUcs2Flag | kUtf16Flag | kUcs4Flag | kUnicodeFlag,   // 0x3e
    k21BitFlags =              kUtf8Flag |             kUtf16Flag | kUcs4Flag,   // 0x1a, many are kUnicodeFlag, too, (0x3a) but not all
    k21BitUnicodeFlags = k21BitFlags | kUnicodeFlag,                             // 0x3a,
    k31BitFlags =              kUtf8Flag |                          kUcs4Flag,   // 0x12
    k32BitFlags =                                                   kUcs4Flag    // 0x10
};

//=========================================================================
// A utility functor for appending encoded characters to a growing string

template< typename C >
class CharacterAdder : public std::unary_function<void, C >
{
public:
    CharacterAdder(std::basic_string< C >& result) : std::unary_function<void, C>(),
                                                     m_destination(result) {}
    void operator()(C c) { m_destination.push_back(c); }
private:
    std::basic_string<C>& m_destination;
};

///////////////////////////////////////////////////////////////////////////
// Local Functions

//=========================================================================
// A utility function to check if a character is allowed within Unicode

template <typename C>
EncodingTypeFlags getUnicodeEncodableFlag(C c)
{
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
EncodingTypeFlags getCharEncodableFlags(C c)
{
    // TODO: add compiler assert to make sure this is used with the right types
    // only valid for unsigned char, char16_t, char32_t; only useful for
    // char16_t and char32_t
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
        return static_cast<EncodingTypeFlags>(k21BitFlags | getUnicodeEncodableFlag(c));
    }
    else
    {
        return ((c & 0x80000000) == 0) ? k31BitFlags : k32BitFlags;
    }
}

//=========================================================================
// A utility function to check what families of values a character once
// decoded will belong to

inline EncodingTypeFlags getRangeFlag(char firstChar)
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

inline EncodingTypeFlags getRangeFlag(char16_t firstChar)
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

//=========================================================================
// Utility template function to encode a single UCS-4 character into UTF-8,
// regardless of its value

template< typename C >
void encodeUtf8
(
    C                       c,          // I - A UCS-4 character to encode
    CharacterAdder<char>&   adder       // I/O - A character collecter to add it to
)
{
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

}

}
