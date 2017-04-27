///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2014, Arthur N. Klassen
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
// 2014.11.18 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// text_file_utilities.hxx -- APIs around files
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <file_system_path.hxx>
#include <string>

namespace ansak
{

class FilePath;

namespace file
{

enum TextType {
    kLocalText,
    kUtf8,
    kUtf16LE,
    kUtf16BE,
    kUcs4LE,
    kUcs4BE
};

const char TAB =  9;
const char LF  = 10;
const char SP  = 32;
const char FF  = 11;
const char CR  = 13;

//===========================================================================
// isEndOfLine - Three values to check, three datatypes to do it on, one
//               type-safe-macro-like to test it
//
// Returns: true if character value is 10, 12 or 13; false otherwise

template<typename C>
bool isEndOfLine
(
    C                   c           // I - a codepoint to check
)
{
    return c == LF || c == FF || c == CR;
}

//===========================================================================
// looksLikeText - Reads the first part of the file, up to 4096 characters and
//                 uses bufferIsText to evaluate it
//
// Description: checks to see if the incoming value is a file that exists and
//              can be read. If so, it reads up to the first 4K and scans it
//              using length-terminated pointer form.
//
// Returns: true if file exists, can be read and bufferIsText returns true for
//          preloaded sample (see Description);
//          throws if no file provided, if it does not exist, if it is not a
//          file, if it cannot be read, if memory for preloading cannot be
//          allocated;
//          false otherwise

bool looksLikeText
(
    const FileSystemPath&   file,               // I - a file to check
    TextType*               encoding = nullptr  // O - its encoding type if desired
);

//===========================================================================
// bufferIsText
//
// Checks length-terminated content to see if it looks like text -- UTF-8,
// UTF-16 or UCS-4. Reports encoding found, if content looks like text
//
// Looks for BOM (FE FF, FF FE, EF BB BF, 00 00 FE FF or FF FE 00 00) at the
// the beginning. Scans the remainder accordingly.
//
// Treats BOM-less content as UTF-8 (7-bit ASCII will appear as UTF-8) or
// LocalText.
//
// If a pointer to an encoding type is provided, it is populated iff the file
// looks like text.
//
// Returns false if content includes non-line-break, non-field-break control
//             characters,
//         false if content doesn't match BOM at start of content,
//         false if non-BOM'd content contains any control characters;
//         throws if length is non-zero and content is nullptr,
//         throws for 16-bit and 32-bit encoding when no processing memory
//             can be allocated;
//         true otherwise

bool bufferIsText
(
    const char*         content,            // I - a pointer to some data to check
    unsigned int        length,             // I - length of data to check
    TextType*           encoding = nullptr  // O - its encoding type if desired
);

}

}
