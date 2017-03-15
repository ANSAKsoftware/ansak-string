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
// 2016.06.03 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_decode_utf8.cxx -- extracted utf-8 single-point decode routine for
//        internal use in other ANSAK modules, primarily.
//        Pulling ANSAK-string out on its own for consumption by file-of-lines
//        necessitated it. Maybe this API will become public in future.
//
///////////////////////////////////////////////////////////////////////////

#include <string_decode_utf8.hxx>

#include <string.hxx>
#include <string_internal.hxx>

using namespace std;

namespace ansak {

namespace internal {

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
        // 7-bit
        return uc;
    }
    else if (uc <= 0xbf)
    {
        // 80..BF are non-first bytes
        p = nullptr; return 0;
    }
    else if (uc <= 0xdf)
    {
        // 2-byte, next 1 must be A0..BF range
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = nullptr; return 0; }
        auto r = rawDecodeUtf8(uc, uc1);
        if (r < 0x80)
        {
            p = nullptr; return 0;
        }
        return r;
    }
    else if (uc <= 0xef)
    {
        // 3-byte, next 2 must be A0..BF range
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = nullptr; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = nullptr; return 0; }
        char32_t w = rawDecodeUtf8(uc, uc1, uc2);
        if (isFirstHalfUtf16(w))
        {
            // must have 2nd-half of escape from UTF-16 -- must be 3-byte UTF-8 range
            auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
            if (uc3 < 0xe0 || uc3 > 0xef) { p = nullptr; return 0; }
            auto uc4 = static_cast<unsigned char>(*++p); if (uc4 == 0) { return 0; }
            if (uc4 < 0x80 || uc4 > 0xbf) { p = nullptr; return 0; }
            auto uc5 = static_cast<unsigned char>(*++p); if (uc5 == 0) { return 0; }
            if (uc5 < 0x80 || uc5 > 0xbf) { p = nullptr; return 0; }
            // turn to UCS-4
            char32_t w2 = rawDecodeUtf8(uc3, uc4, uc5);
            if (isSecondHalfUtf16(w2))
            {
                return 0x10000 + ((w & 0x3ff) << 10) + (w2 & 0x3ff);
            }
            else
            {
                p = nullptr; return 0;
            }
        }
        else if ((w < 0x800) || isSecondHalfUtf16(w))
        {
            p = nullptr; return 0;
        }
        else
        {
            return w;
        }
    }
    else if (uc <= 0xf7)
    {
        // 4-byte, next 3 must be A0..BF range
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = nullptr; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = nullptr; return 0; }
        auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
        if (uc3 < 0x80 || uc3 > 0xbf) { p = nullptr; return 0; }
        auto r = rawDecodeUtf8(uc, uc1, uc2, uc3);
        if (r < 0x10000)
        {
            p = nullptr; return 0;
        }
        return r;
    }
    else if (uc <= 0xfb)
    {
        // 5-byte, next 4 must be A0..BF range (speculative! 10FFFF is covered in 4 bytes)
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = nullptr; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = nullptr; return 0; }
        auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
        if (uc3 < 0x80 || uc3 > 0xbf) { p = nullptr; return 0; }
        auto uc4 = static_cast<unsigned char>(*++p); if (uc4 == 0) { return 0; }
        if (uc4 < 0x80 || uc4 > 0xbf) { p = nullptr; return 0; }
        auto r = rawDecodeUtf8(uc, uc1, uc2, uc3, uc4);
        if (r < 0x200000)
        {
            p = nullptr; return 0;
        }
        return r;
    }
    else if (uc <= 0xfd)
    {
        // 6-byte, next 5 must be A0..BF range (speculative! 10FFFF is covered in 4 bytes)
        auto uc1 = static_cast<unsigned char>(*++p); if (uc1 == 0) { return 0; }
        if (uc1 < 0x80 || uc1 > 0xbf) { p = nullptr; return 0; }
        auto uc2 = static_cast<unsigned char>(*++p); if (uc2 == 0) { return 0; }
        if (uc2 < 0x80 || uc2 > 0xbf) { p = nullptr; return 0; }
        auto uc3 = static_cast<unsigned char>(*++p); if (uc3 == 0) { return 0; }
        if (uc3 < 0x80 || uc3 > 0xbf) { p = nullptr; return 0; }
        auto uc4 = static_cast<unsigned char>(*++p); if (uc4 == 0) { return 0; }
        if (uc4 < 0x80 || uc4 > 0xbf) { p = nullptr; return 0; }
        auto uc5 = static_cast<unsigned char>(*++p); if (uc5 == 0) { return 0; }
        if (uc5 < 0x80 || uc5 > 0xbf) { p = nullptr; return 0; }
        auto r = rawDecodeUtf8(uc, uc1, uc2, uc3, uc4, uc5);
        if (r < 0x4000000)
        {
            p = nullptr; return 0;
        }
        return r;
    }
    else
    {
        p = nullptr; return 0;
    }
}

}

}
