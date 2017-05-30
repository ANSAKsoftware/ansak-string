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
// string_decode_utf8.hxx -- extracted utf-8 single-point decode routine for
//        internal use in other ANSAK modules, primarily.
//        Pulling ANSAK-string out on its own for consumption by file-of-lines
//        necessitated it. Maybe this API will become public in future.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <string.hxx>

namespace ansak {

namespace internal {

///////////////////////////////////////////////////////////////////////////
// Local Functions

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
);

}

}
