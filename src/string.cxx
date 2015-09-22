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
// string.cxx -- manage any incoming string claiming to be of any unicode
//               compatible encoding and re-encode it to the known state of
//               your choice.
//               strings (either null-terminated or basic_string<C>) that
//               pass the isXXX tests are guaranteed to re-encode in the
//               target encoding.
//               partial escapes in a string are failures, partial escapes
//               at the end of a string are ignored.
//
///////////////////////////////////////////////////////////////////////////

#include "string.hxx"
#include "bits/string_internal.hxx"

using namespace std;
using namespace ansak::internal;

namespace ansak {

///////////////////////////////////////////////////////////////////////////
// Local Data

EncodingTypeFlags encodingToFlag[kUnicode + 1] =
    { kAsciiFlag, kUtf8Flag, kUcs2Flag, kUtf16Flag, kUcs4Flag, kUnicodeFlag };

///////////////////////////////////////////////////////////////////////////
// Local Functions

template<typename C>
bool isFirstHalfUtf16
(
    C                   c       // I - character to test
)
{
    return c >= 0xd800 && c <= 0xdbff;
}

template<typename C>
bool isSecondHalfUtf16
(
    C                   c       // I - character to test
)
{
    return c >= 0xdc00 && c <= 0xdfff;
}

inline bool isUtf16EscapedRange
(
    char32_t            c
)
{
    return isFirstHalfUtf16(c) || isSecondHalfUtf16(c);
}

namespace {

//=========================================================================
// Utility function to decode a single UCS-4 character from "the next
// character" in a 0-terminated string, assumed to be UTF-8.
//
// Returns 0 if no UCS-4 character could be decoded, returns that character
//     otherwise.
// If the null-terminated string ran out of characters, returns 0 and leaves
//     p pointing at the last read character, the null.
// If the UTF-8 character is invalid, i.e. broken in any way, returns 0 and
//     modifies p to equal 0.
// If the UTF-8 character (or characters) are valid, returns that character and
//     p is advanced to point to the last character consumed in decoding.
//
// UTF-8 characters can be broken by:
// 1. not a sequence character -- 0xfe, 0xff
// 2. inappropriate sequences -- starting with 0xa0..0xbf (subsequent byte)
// 3. inappropriate sequences -- not enough subsequent bytes after an initial byte
//                               (0x80..0xdf : 1 byte; 0xe0..0xef : 2 bytes ...)
// 4. UTF-8 sequence leading to broken UTF-16 sequence -- starting with 0xdc00..0xdfff
// 5. UTF-8 sequence leading to broken UTF-16 sequence -- starting with 0xd800..0xdbff
//                               but not continued with 0xdc00..0xdfff

char32_t decodeUtf8
(
    const char*&        p       // I/O - points to current non-null character
)
{
    auto uc = static_cast<unsigned char>(*p);
    if (uc <= 0x7f)
    {
        return uc;
    }
    else if (uc <= 0xbf)
    {
        p = 0;
        return 0;
    }
    else if (uc <= 0xdf)
    {
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = 0; return 0; }
        auto r = rawDecodeUtf8(uc, uc1);
        if (r < 0x80)
        {
            p = 0; return 0;
        }
        return r;
    }
    else if (uc <= 0xef)
    {
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = 0; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = 0; return 0; }
        char32_t w = rawDecodeUtf8(uc, uc1, uc2);
        if (isFirstHalfUtf16(w))
        {
            auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
            if (uc3 < 0xe0 || uc3 > 0xef) { p = 0; return 0; }
            auto uc4 = static_cast<unsigned char>(*++p); if (uc4 == 0) { return 0; }
            if (uc4 < 0x80 || uc4 > 0xbf) { p = 0; return 0; }
            auto uc5 = static_cast<unsigned char>(*++p); if (uc5 == 0) { return 0; }
            if (uc5 < 0x80 || uc5 > 0xbf) { p = 0; return 0; }
            char32_t w2 = rawDecodeUtf8(uc3, uc4, uc5);
            if (isSecondHalfUtf16(w2))
            {
                return 0x10000 + ((w & 0x3ff) << 10) + (w2 & 0x3ff);
            }
            else
            {
                p = 0; return 0;
            }
        }
        else if ((w < 0x800) || (w >= 0xdc00 && w <= 0xdfff))
        {
            p = 0; return 0;
        }
        else
        {
            return w;
        }
    }
    else if (uc <= 0xf7)
    {
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = 0; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = 0; return 0; }
        auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
        if (uc3 < 0x80 || uc3 > 0xbf) { p = 0; return 0; }
        auto r = rawDecodeUtf8(uc, uc1, uc2, uc3);
        if (r < 0x10000)
        {
            p = 0; return 0;
        }
        return r;
    }
    else if (uc <= 0xfb)
    {
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = 0; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = 0; return 0; }
        auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
        if (uc3 < 0x80 || uc3 > 0xbf) { p = 0; return 0; }
        auto uc4 = static_cast<unsigned char>(*++p); if (uc4 == 0) { return 0; }
        if (uc4 < 0x80 || uc4 > 0xbf) { p = 0; return 0; }
        auto r = rawDecodeUtf8(uc, uc1, uc2, uc3, uc4);
        if (r < 0x200000)
        {
            p = 0; return 0;
        }
        return r;
    }
    else if (uc <= 0xfd)
    {
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = 0; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = 0; return 0; }
        auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
        if (uc3 < 0x80 || uc3 > 0xbf) { p = 0; return 0; }
        auto uc4 = static_cast<unsigned char>(*++p); if (uc4 == 0) { return 0; }
        if (uc4 < 0x80 || uc4 > 0xbf) { p = 0; return 0; }
        auto uc5 = static_cast<unsigned char>(*++p); if (uc5 == 0) { return 0; }
        if (uc5 < 0x80 || uc5 > 0xbf) { p = 0; return 0; }
        auto r = rawDecodeUtf8(uc, uc1, uc2, uc3, uc4, uc5);
        if (r < 0x4000000)
        {
            p = 0; return 0;
        }
        return r;
    }
    else
    {
        p = 0;
        return 0;
    }
}

//=========================================================================
// Utility function to decide how many bytes a UTF-8 sequence will use

int sequenceSizeCharStarts
(
    const char*         p       // I/O - points to current non-null character
)
{
    if (p == nullptr)
    {
        return -1;
    }
    auto uc = static_cast<unsigned char>(*p);
    if (uc == 0)
    {
        return 0;
    }
    if (uc <= 0x7f)
    {
        return 1;
    }
    else if (uc <= 0xbf)
    {
        return -1;
    }
    else if (uc <= 0xdf)
    {
        return 2;
    }
    else if (uc <= 0xef)
    {
        return 3;
    }
    else if (uc <= 0xf7)
    {
        return 4;
    }
    else if (uc <= 0xfd)
    {
        return 5;
    }
    else
    {
        return -1;
    }
}

//=========================================================================
// Utility function to decide if a character is the first half of a UTF-16
// character in two UTF-8 values.
//
// Call only if at least three bytes are available in the character stream

bool isFirstHalfUtf16AsUtf8
(
    const char*         p       // I - points to current non-null character
)
{
    if (p == nullptr)
    {
        return false;
    }
    auto uc0 = static_cast<unsigned char>(p[0]);
    if (uc0 != 0xed)
    {
        return false;
    }
    auto uc1 = static_cast<unsigned char>(p[1]);
    auto uc2 = static_cast<unsigned char>(p[2]);
    char16_t oneWord = rawDecodeUtf8(uc0, uc1, uc2);
    return isFirstHalfUtf16(oneWord);
}

//=========================================================================
// Utility function to decide if a to-be-decoded character must be teh second
// half of a UTF-16 character in two UTF-8 values.
//
// Call only if at least two bytes are available in the character stream

bool isSecondHalfUtf16AsUtf8
(
    const char*         p       // I/O - points to current non-null character
)
{
    if (p == nullptr)
    {
        return false;
    }
    auto uc0 = static_cast<unsigned char>(p[0]);
    auto uc1 = static_cast<unsigned char>(p[1]);
    return (uc0 == 0xed) && ((uc1 & 0xf0) == 0xb0);
}

//=========================================================================
// Utility function to check the UTF-8 encoding of a length- or null-
// terminated string of bytes.
//
// Called by all public variations of isUtf8

bool isUtf8
(
    const char*     test,               // I - pointer to bytes to be scanned
    unsigned int    testLength,         // I - length (if not 0-terminated) to scan
    EncodingType    targetEncoding      // I - a target encoding
)
{
    // some behaviour is changed if the scan is length terminated
    bool lengthTerminated = testLength != 0;

    // validate encoding and length parameters
    if (targetEncoding < kAscii || targetEncoding > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    EncodingTypeFlags restrictToThis = encodingToFlag[targetEncoding];

    auto lengthLeft = testLength;
    auto pLast = test - 1;
    unsigned int usedThisTime = 0;
    for (auto p = test; *p; ++p)
    {
        EncodingTypeFlags rangeFlag = getRangeFlag(*p);
        if ((rangeFlag & restrictToThis) == 0)
        {
            return false;
        }
        // if we won't have enough length-terminted buffer left to satisfy this
        // sequence, quit, now
        if (lengthTerminated)
        {
            // result will always be positive, p is never nullptr
            unsigned int seqStarts = sequenceSizeCharStarts(p);
            // "3" long sequences might be UTF-16 parts
            if (seqStarts == 3)
            {
                // is this out-of-order?
                if (lengthLeft > 1 && isSecondHalfUtf16AsUtf8(p))
                {
                    return false;
                }
                // will we be out of space?
                if (lengthLeft >= 3 && isFirstHalfUtf16AsUtf8(p))
                {
                    seqStarts = 6;
                }
            }
            // if out of space, and not otherwise invalid, leave with success
            if (seqStarts > lengthLeft)
            {
                return true;
            }
        }

        auto c = decodeUtf8(p);

        // how did decoding go?
        if (p == nullptr)
        {
            return false;
        }

        if (c == 0)
        {
            // decoding to 0 when *p != 0 is bad
            return *p == 0;
        }

        // decoded -- how much was used?
        if (lengthTerminated)
        {
            usedThisTime = static_cast<unsigned int>(p - pLast);
            pLast = p;
        }
        EncodingTypeFlags charFlag = getCharEncodableFlags(c);

        // target encoding cannot be satisfied
        if ((charFlag & restrictToThis) == 0)
        {
            return false;
        }

        // exit condition for length-terminated scans
        if (lengthTerminated)
        {
            // this should only be true if lengthLeft == usedThisTime but...
            if (lengthLeft <= usedThisTime)
            {
                return true;
            }
            lengthLeft -= usedThisTime;
        }
    }

    // falling out due to nulls is good -- except for length-terminated
    return !lengthTerminated;
}

}

///////////////////////////////////////////////////////////////////////////
// Public Functions

//////////// Is it (valid) UTF-8, compatible with this encoding?

bool isUtf8(const std::string& test, EncodingType targetEncoding)
{
    return isUtf8(test.c_str(), 0, targetEncoding);
}

bool isUtf8(const char* test, EncodingType targetEncoding)
{
    return isUtf8(test, 0, targetEncoding);
}

bool isUtf8(const char* test, unsigned int testLength)
{
    return isUtf8(test, testLength, kUtf8);
}

//////////////////// Is it (valid) UTF-16, compatible with this encoding?

bool isUtf16(const utf16String& test, EncodingType targetEncoding)
{
    return isUtf16(test.c_str(), targetEncoding);
}

bool isUtf16(const char16_t* test, EncodingType targetEncoding)
{
    if (targetEncoding < kAscii || targetEncoding > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    EncodingTypeFlags restrictToThis = encodingToFlag[targetEncoding];
    
    for (auto p = test; *p; ++p)
    {
        EncodingTypeFlags rangeFlag = getRangeFlag(*p);
        if ((rangeFlag & restrictToThis) == 0)
        {
            return false;
        }
        auto c = *p;
        EncodingTypeFlags charFlag;
        if (c >= 0xdc00 && c <= 0xdfff)
        {
            return false;
        }
        else if (c >= 0xd000 && c <= 0xdbff)
        {
            char16_t c1 = *++p; if (c1 == 0) { break; }
            if (c1 < 0xdc00 || c1 > 0xdfff)
            {
                return false;
            }
            // Could make this function call but it is unnecessary
            // charFlag = getCharEncodableFlags(rawDecodeUtf16(c, c1));
            charFlag = k21BitUnicodeFlags;
        }
        else
        {
            charFlag = getCharEncodableFlags(c);
        }
        if ((charFlag & restrictToThis) == 0)
        {
            return false;
        }
    }

    return true;
}

//////////////////// Is it (valid) UCS-2, compatible with this encoding?

bool isUcs2(const ucs2String& test, EncodingType targetEncoding)
{
    return isUcs2(test.c_str(), targetEncoding);
}

bool isUcs2(const char16_t* test, EncodingType targetEncoding)
{
    if (targetEncoding < kAscii || targetEncoding > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    EncodingTypeFlags restrictToThis = encodingToFlag[targetEncoding];
    
    for (auto p = test; *p; ++p)
    {
        auto c = *p;
        EncodingTypeFlags charFlag = getCharEncodableFlags(c);
        if (c >= 0xd000 && c <= 0xdfff)
        {
            return false;
        }
        if ((charFlag & restrictToThis) == 0)
        {
            return false;
        }
    }

    return true;
}

//////////////////// Is it (valid) UCS-4, compatible with this encoding?

bool isUcs4(const ucs4String& test, EncodingType targetEncoding)
{
    return isUcs4(test.c_str(), targetEncoding);
}

bool isUcs4(const char32_t* test, EncodingType targetEncoding)
{
    if (targetEncoding < kAscii || targetEncoding > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    EncodingTypeFlags restrictToThis = encodingToFlag[targetEncoding];
    
    for (auto p = test; *p; ++p)
    {
        auto c = *p;
        if ((getCharEncodableFlags(c) & restrictToThis) == 0)
        {
            return false;
        }
    }

    return true;
}

//////////////////// Convert to UTF-8

// From UCS-2 or UTF-16 //////////////////////////////////

string toUtf8(const char16_t* src)
{
    string result;
    if (src && *src)
    {
        CharacterAdder<char> adder(result);
        for (auto p = src; *p; ++p)
        {
            auto c = *p;
            if (isFirstHalfUtf16(c))
            {
                char16_t c1 = *++p; if (c1 == 0) { break; }
                if (!isSecondHalfUtf16(c1))
                {
                    return string();
                }
                char32_t c32 = rawDecodeUtf16(c, c1);
                encodeUtf8(c32, adder);
            }
            else
            {
                encodeUtf8(c, adder);
            }
        }
    }

    return result;
}

string toUtf8(const utf16String& src)
{
    return toUtf8(src.c_str());
}

// From UCS-4 ////////////////////////////////////////////

string toUtf8(const char32_t* src)
{
    string result;
    if (src && *src)
    {
        CharacterAdder<char> adder(result);
        for (auto p = src; *p; ++p)
        {
            auto c = *p;
            if ((c & 0x80000000) == 0 && !isFirstHalfUtf16(c) && !isSecondHalfUtf16(c))
            {
                encodeUtf8(c, adder);
            }
            else
            {
                return string();
            }
        }
    }

    return result;
}

string toUtf8(const ucs4String& src)
{
    return toUtf8(src.c_str());
}

//////////////////// Convert to UCS-2

// From char or UTF-8 ////////////////////////////////////

ucs2String toUcs2(const char* src)
{
    ucs2String result;
    if (src && *src)
    {
        for (auto p = src; *p; ++p)
        {
            auto c = decodeUtf8(p);
            if (p == nullptr || c > 0xffff)
            {
                return ucs2String();
            }
            else if (c == 0)
            {
                break;
            }
            result.push_back(static_cast<char16_t>(c));
        }
    }

    return result;
}

ucs2String toUcs2(const string& src)
{
    return toUcs2(src.c_str());
}

// From UCS-4 ////////////////////////////////////////////

ucs2String toUcs2(const char32_t* src)
{
    ucs2String result;
    if (src && *src)
    {
        for (auto p = src; *p; ++p)
        {
            if (isFirstHalfUtf16(*p) || isSecondHalfUtf16(*p) || *p >= 0x10000)
            {
                return ucs2String();
            }
            result.push_back(*p);
        }
    }

    return result;
}

ucs2String toUcs2(const ucs4String& src)
{
    return toUcs2(src.c_str());
}

//////////////////// Convert to UTF-16

// From char or UTF-8 ////////////////////////////////////

utf16String toUtf16(const char* src)
{
    utf16String result;
    if (src && *src)
    {
        CharacterAdder<char16_t> adder(result);
        for (auto p = src; *p; ++p)
        {
            auto c = decodeUtf8(p);
            if (p == nullptr || c > 0x10ffff)
            {
                return utf16String();
            }
            else if (c == 0)
            {
                break;
            }
            rawEncodeUtf16(c, adder);
        }
    }

    return result;
}

utf16String toUtf16(const string& src)
{
    return toUtf16(src.c_str());
}

// From UCS-4 ////////////////////////////////////////////

utf16String toUtf16(const char32_t* src)
{
    utf16String result;
    if (src && *src)
    {
        CharacterAdder<char16_t> adder(result);
        for (auto p = src; *p; ++p)
        {
            auto c = *p;
            if (p == nullptr || c > 0x10ffff)
            {
                return utf16String();
            }
            else if (c == 0)
            {
                break;
            }
            rawEncodeUtf16(c, adder);
        }
    }

    return result;
}

utf16String toUtf16(const ucs4String& src)
{
    return toUtf16(src.c_str());
}

//////////////////// Convert to UCS-4

// From char or UTF-8 ////////////////////////////////////

ucs4String toUcs4(const char* src)
{
    ucs4String result;
    if (src && *src)
    {
        for (auto p = src; *p; ++p)
        {
            auto c = decodeUtf8(p);
            if (p == nullptr)
            {
                return ucs4String();
            }
            else if (c == 0)
            {
                break;
            }
            result.push_back(c);
        }
    }

    return result;
}

ucs4String toUcs4(const string& src)
{
    return toUcs4(src.c_str());
}

// From UCS-2 or UTF-16 //////////////////////////////////

ucs4String toUcs4(const char16_t* src)
{
    ucs4String result;
    if (src && *src)
    {
        for (auto p = src; *p; ++p)
        {
            auto c = *p;
            if (isFirstHalfUtf16(c))
            {
                char16_t c1 = *++p; if (c1 == 0) { break; }
                if (!isSecondHalfUtf16(c1))
                {
                    return ucs4String();
                }
                char32_t c32 = rawDecodeUtf16(c, c1);
                result.push_back(c32);
            }
            else if (isSecondHalfUtf16(c))
            {
                return ucs4String();
            }
            else
            {
                result.push_back(c);
            }
        }
    }

    return result;
}

ucs4String toUcs4(const utf16String& src)
{
    return toUcs4(src.c_str());
}

//////////////////// Test Unicode Length

// From char or UTF-8 ////////////////////////////////////

unsigned int unicodeLength(const std::string& src)
{
    return unicodeLength(src.c_str());
}

unsigned int unicodeLength(const char* src, unsigned int testLength)
{
    if (!src || !*src)
    {
        return 0;
    }

    // some behaviour is changed if the scan is length terminated
    bool lengthTerminated = testLength != 0;

    auto lengthLeft = testLength;
    auto pLast = src - 1;
    unsigned int usedThisTime = 0;
    unsigned int r = 0;
    EncodingTypeFlags restrictToUnicode = encodingToFlag[kUnicode];

    for (auto p = src; *p; ++p, ++r)
    {
        // if we won't have enough length-terminted buffer left to satisfy this
        // sequence, quit, now
        if (lengthTerminated)
        {
            // result will always be positive, p is never nullptr
            unsigned int seqStarts = sequenceSizeCharStarts(p);
            // "3" long sequences might be UTF-16 parts
            if (seqStarts == 3)
            {
                // is this out-of-order?
                if (lengthLeft > 1 && isSecondHalfUtf16AsUtf8(p))
                {
                    return 0;
                }
                // will we be out of space?
                if (lengthLeft >= 3 && isFirstHalfUtf16AsUtf8(p))
                {
                    seqStarts = 6;
                }
            }
            // if out of space, and not otherwise invalid, leave with success
            if (seqStarts > lengthLeft)
            {
                return r;
            }
        }

        // decode a character
        auto c = decodeUtf8(p);

        // how did decoding go?
        if (p == nullptr || (restrictToUnicode & getCharEncodableFlags(c)) == 0 )
        {
            return 0;
        }

        if (c == 0)
        {
            // decoding to 0 when *p != 0 is bad
            return (*p == 0) ? r : 0;
        }

        // decoded -- how much was used?
        if (lengthTerminated)
        {
            usedThisTime = static_cast<unsigned int>(p - pLast);
            pLast = p;
        }

        // exit condition for length-terminated scans
        if (lengthTerminated)
        {
            // lengthLeft < usedThisTime is checked for in the first block, above
            /* if (lengthLeft < usedThisTime)
            {
                return r;
            }
            else */ if (lengthLeft == usedThisTime)
            {
                return r + 1;
            }
            lengthLeft -= usedThisTime;
        }
    }

    // falling out due to nulls is bad -- this is always length terminated
    return lengthTerminated ? 0 : r;
}

//////////////////// Test Unicode Length

// From UTF-16 ///////////////////////////////////////////


unsigned int unicodeLength(const utf16String& src)
{
    return unicodeLength(src.c_str());
}

unsigned int unicodeLength(const char16_t* src, unsigned int testLength)
{
    // some behaviour is changed if the scan is length terminated
    bool lengthTerminated = testLength != 0;

    auto lengthLeft = testLength;
    auto pLast = src - 1;
    unsigned int usedThisTime = 0;
    unsigned int r = 0;

    for (auto p = src; *p; ++p, ++r)
    {
        // fetch the current character -- is it bad?
        auto c = *p;
        if (isSecondHalfUtf16(c))
        {
            return 0;
        }

        // is this the first of two parts?
        bool isFirstHalf = isFirstHalfUtf16(c);
        unsigned int wordsNeeded = isFirstHalf ? 2 : 1;

        // have we got another half to deal with it?
        if (lengthTerminated && wordsNeeded > lengthLeft)
        {
            return r;
        }

        // or if null terminated, is there another piece?
        auto c1 = p[1];
        if (isFirstHalf && c1 == 0)
        {
            return r;
        }

        // and if in two pieces, is it valid?
        bool isTwoHalves = isFirstHalf && isSecondHalfUtf16(c1);

        if (isTwoHalves)
        {
            // ah, okay -- advance the pointer an extra bit
            ++p;
        }
        else if (isFirstHalf)
        {
            // not so good! encoding error...
            return 0;
        }

        // decoded -- how much was used?
        if (lengthTerminated)
        {
            usedThisTime = static_cast<unsigned int>(p - pLast);
            pLast = p;
        }

        if (lengthTerminated)
        {
            if (lengthLeft == usedThisTime)
            {
                return r + 1;
            }
            lengthLeft -= usedThisTime;
        }
    }

    return lengthTerminated ? 0 : r;
}

unsigned int unicodeLength(const ucs4String& src)
{
    return unicodeLength(src.c_str());
}

unsigned int unicodeLength(const char32_t* src, unsigned int testLength)
{
    // some behaviour is changed if the scan is length terminated
    bool lengthTerminated = testLength != 0;

    unsigned int r = 0;
    for (auto p = src; *p; ++p, ++r)
    {
        if (isFirstHalfUtf16(*p) || isSecondHalfUtf16(*p))
        {
            return 0;
        }
        if (lengthTerminated && p + 1 == src + testLength)
        {
            return r + 1;
        }
    }

    return lengthTerminated ? 0 : r;
}

string toUtf8(const char* src, SourceEncoding srcType)
{
    if (src == nullptr || *src == '\0')
    {
        return string();
    }
    else if (srcType == kSrcUnicode)
    {
        return string(src);
    }
    else if (srcType == kSrcCP1252)
    {
        string result;
        CharacterAdder<char> adder(result);

        for (auto p = src; *p; ++p)
        {
            unsigned char uc = static_cast<unsigned char>(*p);
            if (uc <= 0x7f)
            {
                adder(uc);
            }
            else if (uc <= 0x9f)
            {
                static const char16_t cp125280_9F[] = {
                    0x20ac, 0, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
                    0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0, 0x017d, 0,
                    0, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
                    0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0, 0x017e, 0x0178 };
                auto uniP = cp125280_9F[uc - 0x80];
                if (uniP == 0)
                {
                    return string();
                }
                else
                {
                    encodeUtf8(uniP, adder);
                }
            }
            else
            {
                encodeUtf8(uc, adder);
            }
        }
        return result;
    }
    else if (srcType == kSrcCP1250)
    {
        string result;
        CharacterAdder<char> adder(result);

        for (auto p = src; *p; ++p)
        {
            unsigned char uc = static_cast<unsigned char>(*p);
            if (uc <= 0x7f)
            {
                adder(uc);
            }
            else
            {
                static const char16_t cp125080_FF[] = {
                    0x20ac, 0, 0x201a, 0, 0x201e, 0x2026, 0x2020, 0x2021,       // 0x80
                    0, 0x2030, 0x0160, 0x2039, 0x015a, 0x0164, 0x017d, 0x0179,
                    0, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,  // 0x90
                    0, 0x2122, 0x0161, 0x203a, 0x015b, 0x0165, 0x017e, 0x017a,
                    0xa0, 0x02c7, 0x02d8, 0x0141, 0xa4, 0x0104, 0xa6, 0xa7,     // 0xa0
                    0xa8, 0xa9, 0x015e, 0xab, 0xac, 0xad, 0xae, 0x017b,
                    0xb0, 0xb1, 0x02db, 0x0142, 0xb4, 0xb5, 0xb6, 0xb7,         // 0xb0
                    0xb8, 0x0105, 0x015f, 0xbb, 0x013d, 0x02dd, 0x013e, 0x017c,
                    0x0154, 0xc1, 0xc2, 0x0102, 0xc4, 0x0139, 0x0106, 0xc7,     // 0xc0
                    0x010c, 0xc9, 0x0118, 0xcb, 0x011a, 0xcd, 0xce, 0x010e,
                    0x0110, 0x0143, 0x0147, 0xd3, 0xd4, 0x0150, 0xd6, 0xd7,     // 0xd0
                    0x0158, 0x016e, 0xda, 0x0170, 0xdc, 0xdd, 0x0162, 0xdf,
                    0x0155, 0xe1, 0xe2, 0x0103, 0xe4, 0x013a, 0x0107, 0xe7,     // 0xe0
                    0x010d, 0xe9, 0x0119, 0xeb, 0x011b, 0xed, 0xee, 0x010f,
                    0x0111, 0x0144, 0x0148, 0xf3, 0xf4, 0x0151, 0xf6, 0xf7,     // 0xf0
                    0x0159, 0x016f, 0xfa, 0x0171, 0xfc, 0xfd, 0x0163, 0x02d9
                    };
                auto uniP = cp125080_FF[uc - 0x80];
                if (uniP == 0)
                {
                    return string();
                }
                else
                {
                    encodeUtf8(uniP, adder);
                }
            }
        }
        return result;
    }
    else
    {
        return string();
    }
}

}

