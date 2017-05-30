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
// string_toUtf8.cxx -- a growing collection of routines to convert from
//                      CPxxxx and ISO-8859-x encodings toUtf8
//
///////////////////////////////////////////////////////////////////////////

#include <string.hxx>
#include <string_internal.hxx>

using namespace std;
using namespace ansak::internal;

namespace ansak {

utf8String toUtf8(const char* src, SourceEncoding srcType)
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
