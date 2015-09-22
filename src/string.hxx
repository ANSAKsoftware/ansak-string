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

namespace ansak {

///////////////////////////////////////////////////////////////////////////
// String Types

using utf16String = std::basic_string<char16_t>;
using ucs2String = std::basic_string<char16_t>;
using ucs4String = std::basic_string<char32_t>;

///////////////////////////////////////////////////////////////////////////
// Enumerations

///////////////////////////////////////////////////////////////////////////
// enum EncodingType
//
// Enumeration values to communicate what kind of a string one wants to
// validation
//
///////////////////////////////////////////////////////////////////////////

enum EncodingType {
    kAscii,                 // 0x00..0x7f
    kUtf8,                  // 8-bit encoding of 0x00000000 .. 0x7fffffff
    kUcs2,                  // 0x0000..0xd7ff; 0xe000..0xffff
    kUtf16,                 // 16-bit encoding of 0x0000..0xd7ff; 0xe000..0x10ffff
    kUcs4,                  // 0x00000000..0xffffffff
    kUnicode,               // 0x000000-0x00d7ff; 0x00e000..0x10ffff
};

///////////////////////////////////////////////////////////////////////////
// enum SourceEncoding
//
// Enumeration describing what single-byte encoding a string comes from.
//
///////////////////////////////////////////////////////////////////////////

enum SourceEncoding {
    kSrcUnicode,
    kSrcCP1252,
    kSrcCP1250
};

///////////////////////////////////////////////////////////////////////////
// Public Methods
typedef std::basic_string<char16_t> utf16String;
typedef std::basic_string<char16_t> ucs2String;
typedef std::basic_string<char32_t> ucs4String;

///////////////////////////////////////////////////////////////////////////
// is<EncodingType> functions
//
// Each of these scans the incoming data-type, basic_string<C> and const C*
// type, and determines if the data is valid for the EncodingType, further
// checking to see if the result is compatible with a target encodoing type.
//
// Using the defaulted parameter, the result is a bare check of the correct-
// ness of the encoding.
//
// They return true for empty strings and for strings that are encoded
// entirely correctly when the resulting target values are entirely
// compatible with the target encoding type parameter. Incomplete encoding
// sequences at the end-of-string are ignored if they could have been
// completed for the intended targetEncoding. They return false otherwise.

bool isUtf8
(
    const std::string&  test,                   // I - the source
    EncodingType        targetEncoding = kUtf8  // I - the intended target
);

bool isUtf8
(
    const char*     test,                   // I - the 0-terminated source
    EncodingType    targetEncoding = kUtf8  // I - the intended target
);

bool isUtf8
(
    const char*     test,                   // I - the length terminated source
    unsigned int    testLength //,          // I - the length
//  EncodingType    encoding = kUnicode     // I - always targets kUnicode
);

// passim

bool isUtf16(const utf16String& test, EncodingType targetEncoding = kUtf16);
bool isUtf16(const char16_t* test, EncodingType targetEncoding = kUtf16);

bool isUcs2(const ucs2String& test, EncodingType targetEncoding = kUcs2);
bool isUcs2(const char16_t* test, EncodingType targetEncoding = kUcs2);

bool isUcs4(const ucs4String& test, EncodingType targetEncoding = kUcs4);
bool isUcs4(const char32_t* test, EncodingType targetEncoding = kUcs4);


///////////////////////////////////////////////////////////////////////////
// to<EncodingType> functions
//
// Each of these scans the incoming data-type, basic_string<C> and const C*
// type, re-encoding it as it goes along into EncodingType.
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

std::string toUtf8
(
    const utf16String&      src         // I - A source basic_string
);

std::string toUtf8
(
    const char16_t*         src         // I - A null-term'd source
);

// Passim

std::string toUtf8(const ucs4String& src);
std::string toUtf8(const char32_t* src);

ucs2String toUcs2(const std::string& src);
ucs2String toUcs2(const char* src);
ucs2String toUcs2(const ucs4String& src);
ucs2String toUcs2(const char32_t* src);

utf16String toUtf16(const std::string& src);
utf16String toUtf16(const char* src);
utf16String toUtf16(const ucs4String& src);
utf16String toUtf16(const char32_t* src);

ucs4String toUcs4(const std::string& src);
ucs4String toUcs4(const char* src);
ucs4String toUcs4(const utf16String& src);
ucs4String toUcs4(const char16_t* src);

///////////////////////////////////////////////////////////////////////////
// unicodeLength
//
// Finds the number of Unicode code points in a UTF-8 or UTF-16 string.
// Returns 0 when it encounters invalid encoding sequences

unsigned int unicodeLength
(
    const std::string&      src         // I - A source basic_string
);
unsigned int unicodeLength
(
    const char*         src,            // I - A null- or length-term'd source
    unsigned int        textLength = 0  // I - length of source, 0 if null-term'd
);

// passim

unsigned int unicodeLength(const utf16String& src);
unsigned int unicodeLength(const char16_t* src, unsigned int testLength = 0);

unsigned int unicodeLength(const ucs4String& src);
unsigned int unicodeLength(const char32_t* src, unsigned int testLength = 0);

///////////////////////////////////////////////////////////////////////////
// toUtf8 function
//
// Converts a single-byte-encoded string to UTF-8, returns as STL string.

std::string toUtf8
(
    const char*         src,            // I - A source single-byte, null-terminated string
    SourceEncoding      srcType         // I - The source encoding type
);

}

