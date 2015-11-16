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

inline utf8String getUnicodeVersionSupported()
{
    return "8.0";
}

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

// passim

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

utf8String toUtf8
(
    const utf16String&      src         // I - A source basic_string
);

utf8String toUtf8
(
    const char16_t*         src         // I - A null-term'd source
);

// Passim

utf8String toUtf8(const ucs4String& src);
utf8String toUtf8(const char32_t* src);

ucs2String toUcs2(const utf8String& src);
ucs2String toUcs2(const char* src);
ucs2String toUcs2(const ucs4String& src);
ucs2String toUcs2(const char32_t* src);

utf16String toUtf16(const utf8String& src);
utf16String toUtf16(const char* src);
utf16String toUtf16(const ucs4String& src);
utf16String toUtf16(const char32_t* src);

ucs4String toUcs4(const utf8String& src);
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
    const utf8String&       src         // I - A source basic_string
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

utf8String toLower
(
    const utf8String&       src,            // I - the source
    const char*             lang = nullptr  // I - the optional language code
);

utf16String toLower
(
    const utf16String&      src,            // I - the source
    const char*             lang = nullptr  // I - the optional language code
);

ucs4String toLower
(
    const ucs4String&       src,            // I - the source
    const char*             lang = nullptr  // I - the optional language code
);


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

