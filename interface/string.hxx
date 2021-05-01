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
// 2014.10.08 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string.hxx -- manage any incoming string claiming to be of any unicode
//               compatible encoding and re-encode it to the known state of
//               your choice.
//               strings (either null-terminated or basic_string<C>) that
//               pass the isXXX tests are guaranteed to re-encode in the
//               target encoding.
//               partial escapes in a string are failures, partial escapes
//               at the end of a string are ignored.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <stdint.h>

namespace ansak {

///////////////////////////////////////////////////////////////////////////
// String Types

using utf8String = std::string;
using utf16String = std::u16string;
using ucs2String = std::u16string;
using ucs4String = std::u32string;

///////////////////////////////////////////////////////////////////////////
// Enumerations

///////////////////////////////////////////////////////////////////////////
// enum RangeType
//
// Enumeration values to communicate what kind of a string one wants to
// validate to.
//
///////////////////////////////////////////////////////////////////////////

enum RangeType : int {
    kAscii,                 // 0x00..0x7f
    kUtf8,                  // 8-bit encoding of 0x00000000 .. 0x7fffffff
    kUcs2,                  // 0x0000..0xd7ff; 0xe000..0xffff
    kUtf16,                 // 16-bit encoding of 0x0000..0xd7ff; 0xe000..0x10ffff
    kUcs4,                  // 0x00000000..0xffffffff
    kUnicode,               // 0x000000-0x00d7ff; 0x00e000..0x10ffff

    kFirstInvalidRange
};

///////////////////////////////////////////////////////////////////////////
// enum EncodingType
//
// Enumeration values to communicate what validation, beyond ranges of
// values one wants to validate to.
//
///////////////////////////////////////////////////////////////////////////

enum EncodingType : int {
    kIsNone,
    kIsAssigned,
    kIsPrivate,
    kIsControl,
    kIsWhitespace,

    kFirstInvalidEncoding
};

///////////////////////////////////////////////////////////////////////////
// class EncodingCheckPredicate
//
// A functor to validate individual characters based on EncodingType.
//
///////////////////////////////////////////////////////////////////////////

class EncodingCheckPredicate {

    // private constructor (called by factory method "checkIf" and "checkIfNot"
    EncodingCheckPredicate
    (
        EncodingType    t,                  // I - what range to check for
        bool            checkForIt = true   // I - for presence? or absence
    );

public:

    // default constructor gives a no-check-predicate
    EncodingCheckPredicate() : m_mask(0), m_value(0) {}

    // Creates a functor to check if a character IS of a certain type
    static EncodingCheckPredicate checkIf(EncodingType toCheckFor)
    {
        return EncodingCheckPredicate(toCheckFor);
    }
    // Creates a functor to check if a character IS NOT of a certain type
    static EncodingCheckPredicate checkIfNot(EncodingType toCheckFor)
    {
        return EncodingCheckPredicate(toCheckFor, false);
    }

    // Appendors to narrow the functor to check if a character IS (or IS NOT)
    // ALSO of a certain type
    EncodingCheckPredicate andIf
    (
        EncodingType    toCheckFor      // I - an additional range to check for
    );
    EncodingCheckPredicate andIfNot
    (
        EncodingType    toCheckFor      // I - an additional range to check for
    );

    // EncodingCheckPredicate orIf(EncodingCheckPredicate other);

    // Comparision, mostly to short-circuit checking for a null predicate once
    // outside a tight loop (!= also provided by convention)
    bool operator==(const EncodingCheckPredicate& other) const;
    bool operator!=(const EncodingCheckPredicate& other) const { return !operator==(other); }

    // Invoke the functor (not a template to keep the implementation private)
    bool operator()(char c) const;
    bool operator()(char16_t c) const;
    bool operator()(char32_t c) const;

private:

    uint32_t                m_mask;         // one predicate's mask value
    uint32_t                m_value;        // masked result to check for
};

///////////////////////////////////////////////////////////////////////////
// EncodingCheckPredicate out-of-class factories (syntactic sugar)
//
// Delegates to the class factories (so they don't have to be friends) so that
// users don't have to type the class name which would cloud their meaning at
// point of use.
//
// When constructing predicates for isUtf8, isUcs2, isUtf16 and isUcs4, use
// validIf or validIfNot and append additional simultaneous conditions as '.' +
// andIf() or andIfNot as needed.
//
// So, to check if an incoming UCS-4 string is valid and could be re-encoded
// as UCS-2 (Basic Multilingual Plane only), you could write:
//
// if (isUcs4(inputString, kUcs2, validIf(kIsAssigned)
//                                  .andIfNot(kIsControl)
//                                  .andIfNot(kIsWhitespace))
// {
//      return toUcs4(inputString);
// }
//
// see the Unit Test file, string_with_predicate_test.cxx and the
// testCombinedFlagsPredicate method in encode_predicate_test.cxx for more
// examples.
//
///////////////////////////////////////////////////////////////////////////

inline EncodingCheckPredicate validIf(EncodingType toCheckFor)
{
    return EncodingCheckPredicate::checkIf(toCheckFor);
}

inline EncodingCheckPredicate validIfNot(EncodingType toCheckFor)
{
    return EncodingCheckPredicate::checkIfNot(toCheckFor);
}

///////////////////////////////////////////////////////////////////////////
// enum SourceEncoding
//
// Enumeration describing what single-byte encoding a string comes from.
//
///////////////////////////////////////////////////////////////////////////

enum SourceEncoding : int {
    kSrcUnicode,
    kSrcCP1252,
    kSrcCP1250
};

///////////////////////////////////////////////////////////////////////////
// Public Methods

///////////////////////////////////////////////////////////////////////////
// getUnicodeVersion function
//
// Report what Unicode version is supported.
///////////////////////////////////////////////////////////////////////////

utf8String getUnicodeVersionSupported();

///////////////////////////////////////////////////////////////////////////
// is<RangeType> functions
//
// Each of these scans the incoming data-type, basic_string<C> and const C*
// type, and determines if the data is valid for the RangeType, further
// checking to see if the result is compatible with a target encodoing type.
//
// Using the defaulted parameter, the result is a bare check of the correct-
// ness of the encoding.
//
// They return true for empty strings and for strings that are encoded
// entirely correctly when the resulting target values are entirely
// compatible with the target encoding type parameter. Incomplete encoding
// sequences at the end-of-string are ignored if they could have been
// completed for the intended targetRange. They return false otherwise.
//
// Added syntactic-sugar so that string + encoding check predicate is all
// that is required, if the target stays the same.

bool isUtf8
(
    const utf8String&   test,                   // I - the source
    RangeType           targetRange = kUtf8,    // I - optional target range
    const EncodingCheckPredicate&               // I - optional validity check
                        pred = EncodingCheckPredicate()
);

inline bool isUtf8
(
    const utf8String&               test,       // I - the source
    const EncodingCheckPredicate&   pred        // I - validity check
) { return isUtf8(test, kUtf8, pred); }

bool isUtf8
(
    const char*     test,                   // I - the 0-terminated source
    RangeType       targetRange = kUtf8,    // I - optional target range
    const EncodingCheckPredicate&           // I - optional validity check
                    pred = EncodingCheckPredicate()
);

inline bool isUtf8
(
    const char*                     test,       // I - the source
    const EncodingCheckPredicate&   pred        // I - validity check
) { return isUtf8(test, kUtf8, pred); }

bool isUtf8
(
    const char*     test,                   // I - the length terminated source
    unsigned int    testLength,             // I - the length
//  RangeType       encoding = kUnicode,    // I - always targets kUnicode
    const EncodingCheckPredicate&           // I - optional validity check
                    pred = EncodingCheckPredicate()
);

////////////////////////////////////////////////////////////////////////////////
// isUtf16

bool isUtf16(const utf16String& test,
             RangeType targetRange = kUtf16,
             const EncodingCheckPredicate& pred = EncodingCheckPredicate());
bool isUtf16(const char16_t* test,
             RangeType targetRange = kUtf16,
             const EncodingCheckPredicate& pred = EncodingCheckPredicate());

inline bool isUtf16(const utf16String& test,
            const EncodingCheckPredicate&  pred) { return isUtf16(test, kUtf16, pred); }
inline bool isUtf16(const char16_t* test,
                    const EncodingCheckPredicate& pred) { return isUtf16(test, kUtf16, pred); }

////////////////////////////////////////////////////////////////////////////////
// isUcs2

bool isUcs2(const ucs2String& test,
            RangeType targetRange = kUcs2,
            const EncodingCheckPredicate& pred = EncodingCheckPredicate());
bool isUcs2(const char16_t* test,
            RangeType targetRange = kUcs2,
            const EncodingCheckPredicate& pred = EncodingCheckPredicate());

inline bool isUcs2(const utf16String& test,
                   const EncodingCheckPredicate& pred) { return isUcs2(test, kUcs2, pred); }
inline bool isUcs2(const char16_t* test,
                   const EncodingCheckPredicate& pred) { return isUcs2(test, kUcs2, pred); }

////////////////////////////////////////////////////////////////////////////////
// isUcs4

bool isUcs4(const ucs4String& test,
            RangeType targetRange = kUcs4,
            const EncodingCheckPredicate& pred = EncodingCheckPredicate());
bool isUcs4(const char32_t* test,
            RangeType targetRange = kUcs4,
            const EncodingCheckPredicate& pred = EncodingCheckPredicate());

inline bool isUcs4(const ucs4String& test,
                   const EncodingCheckPredicate& pred) { return isUcs4(test, kUcs4, pred); }
inline bool isUcs4(const char32_t* test,
                   const EncodingCheckPredicate& pred) { return isUcs4(test, kUcs4, pred); }

///////////////////////////////////////////////////////////////////////////
// to<RangeType> functions
//
// Each of these scans the incoming data-type, basic_string<C> and const C*
// type, re-encoding it as it goes along into RangeType.
//
// The return type is always a value of the appropriate basic_string<D> type.
//
// 8-bit source strings will be  handled correctly as ASCII and/or UTF-8
// 16-bit source strings will be  handled correctly as UCS-2 and/or UTF-16
//
// They return an empty string if any incorrectness in encoding or any
// incompatibility between the resulting bytes and the target encoding type
// emerges. Otherwise, they return the re-encoded string in the appropriate
// basic_string<C> type. Incomplete but potentially valid encoding sequences
// at end-of-string are ignored. 

////////////////////////////////////////////////////////////////////////////////
// toUtf8

utf8String toUtf8
(
    const utf16String&      src         // I - A source basic_string
);

utf8String toUtf8
(
    const char16_t*         src         // I - A null-term'd source
);

utf8String toUtf8(const ucs4String& src);
utf8String toUtf8(const char32_t* src);

////////////////////////////////////////////////////////////////////////////////
// toUcs2

ucs2String toUcs2(const utf8String& src);
ucs2String toUcs2(const char* src);
ucs2String toUcs2(const ucs4String& src);
ucs2String toUcs2(const char32_t* src);

////////////////////////////////////////////////////////////////////////////////
// toUtf16

utf16String toUtf16(const utf8String& src);
utf16String toUtf16(const char* src);
utf16String toUtf16(const ucs4String& src);
utf16String toUtf16(const char32_t* src);

////////////////////////////////////////////////////////////////////////////////
// toUcs4

ucs4String toUcs4(const utf8String& src);
ucs4String toUcs4(const char* src);
ucs4String toUcs4(const utf16String& src);
ucs4String toUcs4(const char16_t* src);

///////////////////////////////////////////////////////////////////////////
// unicodeLength
//
// Finds the number of Unicode code points in a UTF-8 or UTF-16 string.
// Returns 0 when it encounters invalid encoding sequences

////////////////////////////////////////////////////////////////////////////////
// for utf8

unsigned int unicodeLength
(
    const utf8String&       src         // I - A source basic_string
);
unsigned int unicodeLength
(
    const char*         src,            // I - A null- or length-term'd source
    unsigned int        textLength = 0  // I - length of source, 0 if null-term'd
);

////////////////////////////////////////////////////////////////////////////////
// for utf16

unsigned int unicodeLength(const utf16String& src);
unsigned int unicodeLength(const char16_t* src, unsigned int testLength = 0);

////////////////////////////////////////////////////////////////////////////////
// for ucs4

unsigned int unicodeLength(const ucs4String& src);
unsigned int unicodeLength(const char32_t* src, unsigned int testLength = 0);

///////////////////////////////////////////////////////////////////////////
// toLower function
//
// Lower-cases a string, with sensitivity to an incoming language ID. Only
// Azerbaizhani, Turkish, Tatar and Kazakh are tracked in order to support
// dotted/dotless-I behaviour; all other languages seem to peacefully
// co-exist. This is "officially correct" for Azeri and Turkish and Crimean
// Tatar. Tatar-in-Russia officially uses Cyrillic but adopted Turkish
// behaviour in Latin alphabets in the past. Kazakh officially uses
// Cyrillic but official documents that use a Latin alphabet also follow
// Turkish behaviour.
//
// Turkish behaviour is selected by passing in a pointer to "az", "aze",
// "azj", "azb", "kk", "kaz", "tt", "tat", "tr" or "tur". Any other value
// selects non-Turkish behaviour.
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// for utf8

utf8String toLower
(
    const utf8String&       src,            // I - the source
    const char*             lang = nullptr  // I - the optional language code
);

////////////////////////////////////////////////////////////////////////////////
// for utf16

utf16String toLower
(
    const utf16String&      src,            // I - the source
    const char*             lang = nullptr  // I - the optional language code
);

////////////////////////////////////////////////////////////////////////////////
// for ucs4

ucs4String toLower
(
    const ucs4String&       src,            // I - the source
    const char*             lang = nullptr  // I - the optional language code
);

////////////////////////////////////////////////////////////////////////////////
// isXxxxx and toXxxxx for wchar_t -- Doing as well as we can with a bad deal
//
// wchar_t is a bad data type because it used to be 16-bits everywhere and now
// it's 32-bits everywhere. except Windows.
// In practice, on macOS, NSstrings are really UTF-16 strings, too, yet clang
// and gcc there use 32-bit wchar_t's, but I digress.
//
// What is presented here is an adaptation of the rest of the API, suited to
// dealing with wchar_t (and std::wstring) in a compiler independent way.
//
// The isXxxxx APIs cover all four data-types, but under the hood they call
// isUcs4 or isUtf16 with an appropriate RangeType, so as to help the user
// decide when a string can be convertible to one of the four canonical types.
//
// WCHAR_MAX and the data-type wchar_t appear to be defined by the compiler
// regardless, and using WCHAR_MAX means I can differentiate the variation in
// ways the pre-processor can understand.
////////////////////////////////////////////////////////////////////////////////

#if WCHAR_MAX < 0x10000

////////////////////////////////////////////////////////////////////////////////
// isXxxxx for 16-bit wchar_t

inline bool isUtf8(const wchar_t* test)
{
   return isUtf16(reinterpret_cast<const char16_t*>(test), kUtf8);
}
inline bool isUtf8(const std::wstring& test) { return isUtf8(test.c_str()); }

inline bool isUtf16(const wchar_t* test)
{
   return isUtf16(reinterpret_cast<const char16_t*>(test), kUtf8);
}
inline bool isUtf16(const std::wstring& test) { return isUtf16(test.c_str()); }

inline bool isUcs2(const wchar_t* test)
{
   return isUtf16(reinterpret_cast<const char16_t*>(test), kUcs2);
}
inline bool isUcs2(const std::wstring& test) { return isUcs2(test.c_str()); }

inline bool isUcs4(const wchar_t* test)
{
   return isUtf16(reinterpret_cast<const char16_t*>(test), kUcs4);
}
inline bool isUcs4(const std::wstring& test) { return isUcs4(test.c_str()); }

inline bool isWstring(const utf8String& test)
{
    return isUtf8(test, kUtf16);
}

inline bool isWstring(const char* test)
{
    return isUtf8(test, kUtf16);
}

inline bool isWstring(const utf16String& test)
{
    return isUtf16(test);
}

/* ucs2String and utf16String are the same type under the hood
inline bool isWstring(const ucs2String& test)
{
    return isUcs2(test, kUtf16);
}
*/

inline bool isWstring(const char16_t* test)
{
    return isUtf16(test);
}

inline bool isWstring(const ucs4String& test)
{
    return isUcs4(test, kUtf16);
}

inline bool isWstring(const char32_t* test)
{
    return isUcs4(test, kUtf16);
}

#if defined(WIN32)

// and because TCHAR can be wchar_t sometimes, these have to be provided for
// Windows convenience, so that anyone can use this API with TCHAR's just as
// they would with wchar_t, so that TCHAR code can be portable either way.
// It's only decent, even if the meanings are warped slightly by a local
// dialect.

// isUtf8 is already implemented wtih const char* and const std::string& only
// (and two defaulted parameters), see above, about 300 lines.

inline bool isUtf16(const char* test)
{
  return isUtf8(test, kUtf16);
}
inline bool isUtf16(const std::string& test) { return isUtf16(test.c_str()); }

inline bool isUcs2(const char* test)
{
  return isUtf8(test, kUcs2);
}
inline bool isUcs2(const std::string& test) { return isUcs2(test.c_str()); }

inline bool isUcs4(const char* test)
{
  return isUtf8(test, kUcs4);
}
inline bool isUcs4(const std::string& test) { return isUcs4(test.c_str()); }

#if defined(_MBCS)

inline bool isTstring(const utf8String& test)
{
    return isUtf8(test, kUtf16);
}

inline bool isTstring(const char* test)
{
    return isUtf8(test, kUtf16);
}

inline bool isTstring(const utf16String& test)
{
    return isUtf16(test);
}

/* ucs2String and utf16String are the same type under the hood
inline bool isTstring(const ucs2String& test)
{
    return isUcs2(test, kUtf16);
}
*/

inline bool isTstring(const char16_t* test)
{
    return isUtf16(test);
}

inline bool isTstring(const ucs4String& test)
{
    return isUcs4(test, kUtf16);
}

inline bool isTstring(const char32_t* test)
{
    return isUcs4(test, kUtf16);
}

#elif defined(_UNICODE)

inline bool isTstring(const utf8String& test)
{
    return isUtf8(test, kUtf16);
}

inline bool isTstring(const char* test)
{
    return isUtf8(test, kUtf16);
}

inline bool isTstring(const utf16String& test)
{
    return isUtf16(test);
}

/* ucs2String and utf16String are the same type under the hood
inline bool isTstring(const ucs2String& test)
{
    return isUcs2(test, kUtf16);
}
*/

inline bool isTstring(const char16_t* test)
{
    return isUtf16(test);
}

inline bool isTstring(const ucs4String& test)
{
    return isUcs4(test, kUtf16);
}

inline bool isTstring(const char32_t* test)
{
    return isUcs4(test, kUtf16);
}

#endif

#endif

////////////////////////////////////////////////////////////////////////////////
// toXxxxx for 16-bit wchar_t

inline utf8String toUtf8(const wchar_t* src)
{ return toUtf8(reinterpret_cast<const char16_t*>(src)); }

inline utf8String toUtf8(const std::wstring& src)
{ return toUtf8(src.c_str()); }

inline utf16String toUtf16(const wchar_t* src)
{
    const char16_t* redirect = reinterpret_cast<const char16_t*>(src);
    if (isUtf16(redirect))
    {
        return utf16String(redirect);
    }
    else
    {
        return utf16String();
    }
}

inline utf16String toUtf16(const std::wstring& src)
{ return toUtf16(src.c_str()); }

inline ucs2String toUcs2(const wchar_t* src)
{
    const char16_t* redirect = reinterpret_cast<const char16_t*>(src);
    if (isUcs2(redirect))
    {
        return ucs2String(redirect);
    }
    else
    {
        return ucs2String();
    }
}

inline ucs2String toUcs2(const std::wstring& src)
{ return toUcs2(src.c_str()); }

inline ucs4String toUcs4(const wchar_t* src)
{ return toUcs4(reinterpret_cast<const char16_t*>(src)); }

inline ucs4String toUcs4(const std::wstring& src)
{ return toUcs4(src.c_str()); }

inline std::wstring toWstring(const char* src)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUtf16(src).c_str()));
}
inline std::wstring toWstring(const utf8String& src)
{
    return toWstring(src.c_str());
}

inline std::wstring toWstring(const char16_t* src)
{
    if (isUtf16(src))
    {
        return std::wstring(reinterpret_cast<const wchar_t*>(src));
    }
    else
    {
        return std::wstring();
    }
}
inline std::wstring toWstring(const utf16String& src)
{
    return toWstring(src.c_str());
}
/* ucs2String and utf16String are the same type under the hood
inline std::wstring toWstring(const ucs2String& src)
{
    return toWstring(src.c_str());
}
*/

inline std::wstring toWstring(const char32_t* src)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUtf16(src).c_str()));
}
inline std::wstring toWstring(const ucs4String& src)
{
    return toWstring(src.c_str());
}

#if defined(WIN32)

// TCHAR requires toUtf8 specialization

inline utf8String toUtf8(const char* src)
{
  if (isUtf8(src))
  {
    return utf8String(src);
  }
  else
  {
    return utf8String();
  }
}

inline utf8String toUtf8(const std::string& src) { return toUtf8(src.c_str()); }

#if defined(_MBCS)

inline std::string toTstring(const utf8String& test)
{
    return toTstring(test.c_str());
}

inline std::string toTstring(const char* test)
{
    if (isUtf8(test, kAscii))
    {
        return std::string(test);
    }
    else
    {
        return std::string();
    }
}

inline std::string toTstring(const utf16String& test)
{
    return toTstring(test.c_str());
}

/* ucs2String and utf16String are the same type under the hood
inline std::string toTstring(const ucs2String& test)
{
    return std::string(toUtf8(test).c_str());
}
*/

inline std::string toTstring(const char16_t* test)
{
    if (isUtf16(test, kAscii))
    {
        return std::string(toUtf8(test).c_str());
    }
    else
    {
        return std::string();
    }
}

inline std::string toTstring(const ucs4String& test)
{
    return toTstring(test.c_str());
}

inline std::string toTstring(const char32_t* test)
{
    if (isUcs4(test, kAscii))
    {
        return std::string(toUtf8(test).c_str());
    }
    else
    {
        return std::string();
    }
}

#elif defined(_UNICODE)

inline std::wstring toTstring(const utf8String& test)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUtf16(test).c_str()));
}

inline std::wstring toTstring(const char* test)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUtf16(test).c_str()));
}

inline std::wstring toTstring(const utf16String& test)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(test.c_str()));
}

/* ucs2String and utf16String are the same type under the hood
inline std::wstring toTstring(const ucs2String& test)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(test.c_str()));
}
*/

inline std::wstring toTstring(const char16_t* test)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(test));
}

inline std::wstring toTstring(const ucs4String& test)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUtf16(test).c_str()));
}

inline std::wstring toTstring(const char32_t* test)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUtf16(test).c_str()));
}

#endif

#endif

#endif

#if WCHAR_MAX > 0x10000

////////////////////////////////////////////////////////////////////////////////
// isXxxxx for 32-bit wchar_t

inline bool isUtf8(const wchar_t* test)
{
   // return isUcs4(reinterpret_cast<const char32_t*>(test), kUtf8);
   // anything that's ucs-4 will go to utf8
   // but systems that use narrow wchar_t often trade in utf-16 there...
   return isUcs4(reinterpret_cast<const char32_t*>(test), kUtf16);
}
inline bool isUtf8(const std::wstring& test) { return isUtf8(test.c_str()); }

inline bool isUtf16(const wchar_t* test)
{
   return isUcs4(reinterpret_cast<const char32_t*>(test), kUtf16);
}
inline bool isUtf16(const std::wstring& test) { return isUtf16(test.c_str()); }

inline bool isUcs2(const wchar_t* test)
{
   return isUcs4(reinterpret_cast<const char32_t*>(test), kUcs2);
}
inline bool isUcs2(const std::wstring& test) { return isUcs2(test.c_str()); }

inline bool isUcs4(const wchar_t* test)
{
   // return isUcs4(reinterpret_cast<const char32_t*>(test), kUcs4);
   // anything that's ucs-4 will go to ucs-4
   // but systems that use narrow wchar_t often trade in utf-16 there...
   return isUcs4(reinterpret_cast<const char32_t*>(test), kUtf16);
}
inline bool isUcs4(const std::wstring& test) { return isUcs4(test.c_str()); }

inline bool isWstring(const utf8String& test)
{
    return isUtf8(test, kUtf16);
}

inline bool isWstring(const char* test)
{
    return isUtf8(test, kUtf16);
}

inline bool isWstring(const utf16String& test)
{
    return isUtf16(test, kUcs4);
}

/* ucs2String and utf16String are the same type under the hood
inline bool isWstring(const ucs2String& test)
{
    return isUcs2(test, kUtf16);
}
*/

inline bool isWstring(const char16_t* test)
{
    return isUtf16(test, kUcs4);
}

inline bool isWstring(const ucs4String& test)
{
    return isUcs4(test, kUtf16);
}

inline bool isWstring(const char32_t* test)
{
    return isUcs4(test, kUtf16);
}

////////////////////////////////////////////////////////////////////////////////
// toXxxxx for 32-bit wchar_t

inline utf8String toUtf8(const wchar_t* src)
{ return toUtf8(reinterpret_cast<const char32_t*>(src)); }

inline utf8String toUtf8(const std::wstring& src)
{ return toUtf8(src.c_str()); }

inline utf16String toUtf16(const wchar_t* src)
{ return toUtf16(reinterpret_cast<const char32_t*>(src)); }

inline utf16String toUtf16(const std::wstring& src)
{ return toUtf16(src.c_str()); }

inline ucs2String toUcs2(const wchar_t* src)
{ return toUcs2(reinterpret_cast<const char32_t*>(src)); }

inline ucs2String toUcs2(const std::wstring& src)
{ return toUcs2(src.c_str()); }

inline ucs4String toUcs4(const wchar_t* src)
{
    if (isUcs4(reinterpret_cast<const char32_t*>(src), kUtf16))
    {
        return ucs4String(reinterpret_cast<const char32_t*>(src));
    }
    else
    {
        return ucs4String();
    }
}

inline ucs4String toUcs4(const std::wstring& src)
{ return toUcs4(src.c_str()); }

inline std::wstring toWstring(const char* src)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUcs4(src).c_str()));
}
inline std::wstring toWstring(const utf8String& src)
{
    return toWstring(src.c_str());
}

inline std::wstring toWstring(const char16_t* src)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(toUcs4(src).c_str()));
}
inline std::wstring toWstring(const utf16String& src)
{
    return toWstring(src.c_str());
}
/* ucs2String and utf16String are the same type under the hood
inline std::wstring toWstring(const ucs2String& src)
{
    return toWstring(src.c_str());
}
*/

inline std::wstring toWstring(const char32_t* src)
{
    if (isUcs4(src, kUtf16))
    {
        return std::wstring(reinterpret_cast<const wchar_t*>(src));
    }
    else
    {
        return std::wstring();
    }
}

inline std::wstring toWstring(const ucs4String& src)
{
    return toWstring(src.c_str());
}

#endif

///////////////////////////////////////////////////////////////////////////
// toUtf8 function
//
// Converts a single-byte-encoded string to UTF-8, returns as STL string.

utf8String toUtf8
(
    const char*         src,            // I - A source single-byte, null-terminated string
    SourceEncoding      srcType         // I - The source encoding type
);

}
