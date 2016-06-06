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
#include "string_internal.hxx"
#include "internal/string_decode_utf8.hxx"

using namespace std;
using namespace ansak::internal;

namespace ansak {

namespace {

///////////////////////////////////////////////////////////////////////////
// Local Data

RangeTypeFlags rangeTypeToRangeFlag[RangeType::kFirstInvalidRange] =
    { kAsciiFlag, kUtf8Flag, kUcs2Flag, kUtf16Flag, kUcs4Flag, kUnicodeFlag };

///////////////////////////////////////////////////////////////////////////
// Local Functions

//=========================================================================
// Is character (of whatever type) encodable as UTF-16? (in range 0..10FFFF)
//
// Returns true if in range, false otherwise.

template<typename C>
bool isUtf16Encodable
(
    C                   c       // I - character to test
)
{
    static_assert(std::is_integral<C>::value, "isUtf16Encodable needs an integral type.");

    // don't depend on signed-ness -- first half covers unsigned case,
    // second half covers negative numbers without calling them so.
    return c <= 0x10ffff && (c & ~0x1fffff) == 0;
}

//=========================================================================
// Is character (of whatever type) second half of non-BMP UTF-16 pair
// (in range DC00..DFFF)
//
// Returns true if in range, false otherwise.

inline bool isUtf16EscapedRange
(
    char32_t            c
)
{
    return isFirstHalfUtf16(c) || isSecondHalfUtf16(c);
}

//=========================================================================
// Utility function to decide how many bytes a UTF-8 sequence will use
//
// Given a pointer, if it's nullptr, Returns -1. Otherwise, given the character
// it points to,
// if it's 0, Returns 0;
// if it's not a valid first-character-in-UTF-8 sequence, Returns -1;
// otherwise, Returns the length in bytes of the UTF-8 sequence.
//
// Note: This routine does NOT notice d800..dfff values

int sequenceSizeCharStarts
(
    const char*         p       // I/O - points to current non-null character
)
{
    if (p == nullptr)
    {
        return -1;      // should never get here, trimmed out at higher levels
    }
    auto uc = static_cast<unsigned char>(*p);
    if (uc == 0)
    {
        return 0;       // should never get here, trimmed out at higher levels
    }
    if (uc <= 0x7f)
    {
        return 1;
    }
    else if (uc <= 0xbf)
    {
        return -1;      // should never get here, trimmed out at higher levels
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
    else if (uc <= 0xfb)
    {
        return 5;
    }
    else if (uc <= 0xfd)
    {
        return 6;
    }
    else
    {
        return -1;      // should never get here, trimmed out at higher levels
    }
}

//=========================================================================
// Utility function to decide if a character is the first half of a UTF-16
// character in two UTF-8 values.
//
// Note: Call only if p points to 3-character sequence start AND there are
//       at least two more bytes available in the character stream
//
// Returns true if byte starts a 3-byte UTF-8 character and isFirstHalfUtf16

inline bool isFirstHalfUtf16AsUtf8
(
    const char*         p       // I - points to current non-null character
)
{
    return isFirstHalfUtf16(rawDecodeUtf8(
                static_cast<unsigned char>(p[0]),
                static_cast<unsigned char>(p[1]),
                static_cast<unsigned char>(p[2])));
}

//=========================================================================
// Utility function to decide if a to-be-decoded character must be teh second
// half of a UTF-16 character in two UTF-8 values.
//
// Note: Call only if p points to 3-character sequence start AND there are
//       at least two more bytes available in the character stream
//
// Returns true if byte starts a 3-byte UTF-8 character and isSecondHalfUtf16

inline bool isSecondHalfUtf16AsUtf8
(
    const char*         p       // I/O - points to current non-null character
)
{
    return isSecondHalfUtf16(rawDecodeUtf8(
                static_cast<unsigned char>(p[0]),
                static_cast<unsigned char>(p[1]),
                static_cast<unsigned char>(p[2])));
}

//=========================================================================
// Utility function to check the UTF-8 encoding of a length- or null-
// terminated string of bytes.
//
// Called by all public variations of isUtf8-of-single-byte-run
//
// Returns true for valid UTF-8 stream (length terminated when appropriate)
// that can be encoded a certain way; false otherwise (invalid, can't fit in
// destination)

bool isUtf8
(
    const char*     test,               // I - pointer to bytes to be scanned
    unsigned int    testLength,         // I - length (if not 0-terminated) to scan
    RangeType       targetRange,        // I - a target encoding
    const EncodingCheckPredicate&       // I - optional validity check (def. no-check)
                    pred
)
{
    // some behaviour is changed if the scan is length terminated
    bool lengthTerminated = testLength != 0;

    // validate encoding and length parameters
    if (targetRange < kAscii || targetRange > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    RangeTypeFlags restrictToThis = rangeTypeToRangeFlag[targetRange];
    bool isNullPred = pred == EncodingCheckPredicate();

    auto lengthLeft = testLength;
    auto pLast = test - 1;
    unsigned int usedThisTime = 0;
    for (auto p = test; *p; ++p)
    {
        RangeTypeFlags rangeFlag = getRangeFlag(*p);
        if ((rangeFlag & restrictToThis) == 0)
        {
            return false;
        }
        // if we won't have enough length-terminted buffer left to satisfy this
        // sequence, quit, now
        // result will always be positive, p is never nullptr
        unsigned int seqStarts = sequenceSizeCharStarts(p);
        if (lengthTerminated)
        {
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
            // decoding to 0 when *p != 0 is invalid UTF-8
            return *p == 0;
        }

        // decoded -- how much was used?
        if (lengthTerminated)
        {
            usedThisTime = static_cast<unsigned int>(p - pLast);
            if (seqStarts != usedThisTime)
            {
                return false;   // should never get here
            }
            pLast = p;
        }
        RangeTypeFlags charFlag = getCharEncodableRangeFlags(c);

        // target encoding cannot be satisfied
        if ((charFlag & restrictToThis) == 0 || (!isNullPred && !pred(c)))
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

///////////////////////////////////////////////////////////////////////////
// getUnicodeVersion function
//
// Report what Unicode version is supported.
///////////////////////////////////////////////////////////////////////////

utf8String getUnicodeVersionSupported()
{
    return supportedUnicodeVersion;
}

//////////// Is it (valid) UTF-8, compatible with this encoding?

bool isUtf8(const std::string& test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    return isUtf8(test.c_str(), 0, targetRange, pred);
}

bool isUtf8(const char* test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    return isUtf8(test, 0, targetRange, pred);
}

bool isUtf8(const char* test, unsigned int testLength, const EncodingCheckPredicate& pred)
{
    return isUtf8(test, testLength, kUtf8, pred);
}

//////////////////// Is it (valid) UTF-16, compatible with this encoding?

bool isUtf16(const utf16String& test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    return isUtf16(test.c_str(), targetRange, pred);
}

bool isUtf16(const char16_t* test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    if (targetRange < kAscii || targetRange > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    RangeTypeFlags restrictToThis = rangeTypeToRangeFlag[targetRange];
    bool isNullPred = pred == EncodingCheckPredicate();
    
    for (auto p = test; *p; ++p)
    {
        RangeTypeFlags rangeFlag = getRangeFlag(*p);
        if ((rangeFlag & restrictToThis) == 0)
        {
            return false;
        }
        auto c = *p;
        RangeTypeFlags charFlag;
        /* Never hit, 2nd half UTF16 is invalid range
        if (isSecondHalfUtf16(c))
        {
            return false;
        } */
        if (isFirstHalfUtf16(c))
        {
            char16_t c1 = *++p; if (c1 == 0) { break; }
            if (!isSecondHalfUtf16(c1))
            {
                return false;
            }
            // Could make this function call but it is unnecessary
            // charFlag = getCharEncodableRangeFlags(rawDecodeUtf16(c, c1));
            charFlag = k21BitUnicodeFlags;
        }
        else
        {
            charFlag = getCharEncodableRangeFlags(c);
        }
        if ((charFlag & restrictToThis) == 0 || (!isNullPred && !pred(c)))
        {
            return false;
        }
    }

    return true;
}

//////////////////// Is it (valid) UCS-2, compatible with this encoding?

bool isUcs2(const ucs2String& test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    return isUcs2(test.c_str(), targetRange, pred);
}

bool isUcs2(const char16_t* test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    if (targetRange < kAscii || targetRange > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    RangeTypeFlags restrictToThis = rangeTypeToRangeFlag[targetRange];
    bool isNullPred = pred == EncodingCheckPredicate();
    
    for (auto p = test; *p; ++p)
    {
        auto c = *p;
        RangeTypeFlags charFlag = getCharEncodableRangeFlags(c);
        if (isUtf16EscapedRange(c))
        {
            return false;
        }
        if ((charFlag & restrictToThis) == 0 || (!isNullPred && !pred(c)))
        {
            return false;
        }
    }

    return true;
}

//////////////////// Is it (valid) UCS-4, compatible with this encoding?

bool isUcs4(const ucs4String& test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    return isUcs4(test.c_str(), targetRange, pred);
}

bool isUcs4(const char32_t* test, RangeType targetRange, const EncodingCheckPredicate& pred)
{
    if (targetRange < kAscii || targetRange > kUnicode)
    {
        return false;
    }
    if (!test || !*test)
    {
        return true;
    }

    RangeTypeFlags restrictToThis = rangeTypeToRangeFlag[targetRange];
    bool isNullPred = pred == EncodingCheckPredicate();
    
    for (auto p = test; *p; ++p)
    {
        auto c = *p;
        if ((getCharEncodableRangeFlags(c) & restrictToThis) == 0 || (!isNullPred && !pred(c)))
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
            result.push_back(static_cast<char16_t>(*p));
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
            if (p == nullptr || !isUtf16Encodable(c))
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
            if (p == nullptr || !isUtf16Encodable(c) ||
                isFirstHalfUtf16(c) || isSecondHalfUtf16(c))
            {
                return utf16String();
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

// From char/UTF-8 ///////////////////////////////////////

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
    RangeTypeFlags restrictToUnicode = rangeTypeToRangeFlag[kUnicode];

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
        // GUARANTEE: if lengthTerminated, lengthLeft >= to-be-usedThisTime

        // decode a character
        auto c = decodeUtf8(p);

        // how did decoding go?
        if (p == nullptr || (restrictToUnicode & getCharEncodableRangeFlags(c)) == 0 )
        {
            return 0;
        }

        if (c == 0)
        {
            // decoding to 0 when *p != 0 is invalid UTF-8
            return (*p == 0) ? (r - 1) : 0;
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
            // lengthLeft < usedThisTime -- see GUARANTEE above
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

    // if lengthTerminated, falling out due to nulls is bad, otherwise,
    // return collected length.
    return lengthTerminated ? 0 : r;
}

//////////////////// Test Unicode Length

// From UCS-2/UTF-16 /////////////////////////////////////

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
        // fetch the current character -- is it invalid UTF-16
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
            return r - 1;
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

//////////////////// Test Unicode Length

// From UCS-4 ////////////////////////////////////////////

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
        // In UCS-4, UTF-16 escape pairs are not allowed
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

}

