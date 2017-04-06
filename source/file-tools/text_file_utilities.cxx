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
// text_file_utilities.cxx -- APIs around files
//
///////////////////////////////////////////////////////////////////////////

#include "text_file_utilities.hxx"

#include "file_handle.hxx"
#include "file_handle_exception.hxx"
#include "endianness.hxx"
#include "file_of_lines_exception.hxx"
#include "nullptr_exception.hxx"

#include <memory>
#include <memory.h>

#include "string.hxx"

using std::unique_ptr;
using ansak::FileSystemPath;

namespace ansak
{
    namespace file
    {

namespace
{

//=======================================================================
// pointsToControlChar -- some characters less than 32 (SP) are okay
//
// Returns true for values less than SP except for TAB, LF, FF or CR;
// false otherwise.

template <typename C> bool pointsToControlChar(const C* p)
{
    return *p != TAB && *p != LF && *p != FF && *p != CR &&
            (*p > 0 && *p < SP);
}

//=======================================================================
// foundControlChar -- some characters less than 32 (SP) are okay
//
// Returns true if the length-terminated string type contains any control
// characters; false otehrwise

template <typename C> bool foundControlChar(const C* p, size_t l)
{
    do
    {
        if (pointsToControlChar(p++))
        {
            return true;
        }
    } while (--l > 0);
    return false;
}

inline const unsigned char* asUCharPtr(const char* p)
{
    return reinterpret_cast<const unsigned char*>(p);
}

FileOfLinesException PathInvalid(const FilePath& filePath)
{
    return FileOfLinesException("path is invalid", filePath);
}

FileOfLinesException PathIsNotFile(const FilePath& filePath)
{
    return FileOfLinesException("path is not a file", filePath);
}

FileOfLinesException ReadFailed(const FilePath& filePath)
{
    return FileOfLinesException("path is not a file", filePath);
}

FileOfLinesException ReadFailed(const FilePath& filePath, const FileHandleException& problem)
{
    return FileOfLinesException("path is not a file", filePath, problem);
}

}

//===========================================================================
// looksLikeText - Reads the first part of the file, up to 4096 characters and
//                 uses bufferIsText to evaluate it
//
// Returns: true if file exists, can be read and bufferIsText returns true for
//          preloaded sample (see Description);
//          throws if no file provided, if it does not exist, if it is not a
//          file, if it cannot be read, if memory for preloading cannot be
//          allocated;
//          false otherwise

bool looksLikeText
(
    const FileSystemPath&   file,      // I - a file to check
    TextType*               encoding   // O - its encoding type if desired, def nullptr
)
{
    try
    {
        if (!file.isValid() || !file.exists())
        {
            throw PathInvalid(file);
        }
        if (!file.isFile())
        {
            throw PathIsNotFile(file);
        }
        auto fileSize = file.size();
        int allocateSize = fileSize > 4096 ? 4096 : static_cast<int>(fileSize);
        if (allocateSize == 0)
        {
            return false;
        }
        unique_ptr<char> work = unique_ptr<char>(new char[allocateSize]);

        // make sure the file can be read, though, even if it's not there
        int rd = 0;
        {
            FileHandle fileH = FileHandle::open(file);
            rd = fileH.read(work.get(), allocateSize);
        }

        if (rd == 0)
        {
            // seems inconceivable, but what if there's a race to the file's contents?
            throw ReadFailed(file);
        }

        // we have data, analyse it
        return bufferIsText(work.get(), rd, encoding);
    }
    catch (FileHandleException& f)
    {
        throw ReadFailed(file, f);
    }
}

//===========================================================================
// bufferIsText
//
// Checks length-terminated content to see if it looks like text -- UTF-8,
// UTF-16, UCS-4 or local encoding. Reports encoding found, if content looks
// like text.
//
// Returns false if content includes non-line-break, non-field-break control
//             characters,
//         false if content doesn't match BOM at start of content,
//         false if non-BOM'd content contains any control characters;
//         throws if length is non-zero and content is nullptr,
//         throws for 16-bit and 32-bit encoding when no processing memory
//             can be allocated;
//         true otherwise.

bool bufferIsText
(
    const char*         content,    // I - a pointer to some data to check
    unsigned int        length,     // I - length of data to check
    TextType*           encoding    // O - its encoding type if desired, def nullptr
)
{
    if (length == 0)
    {
        return true;
    }

    bool bomAsUtf8 = false;         // first three bytes are BF EF EF
    bool bomAsUtf16LE = false;      // first two bytes are FF FE
    bool bomAsUtf16BE = false;      // first two bytes are FE FF
    bool bomAsUcs4LE = false;       // first four bytes are FF FE 00 00
    bool bomAsUcs4BE = false;       // first four bytes are 00 00 FE FF

    if (content == nullptr)
    {
        throw NullPtrException("bufferIsText: No content to test for textiness");
    }

    // classify BOM if any
    switch (*asUCharPtr(content))
    {
        case 0xBF: bomAsUtf8 = length >= 3 &&
                   asUCharPtr(content)[1] == 0xEF &&
                   asUCharPtr(content)[2] == 0xEF;
                   break;

        case 0xFF:
                {
                    bool bomMightBeUtf16LE = length >= 2 &&
                                             asUCharPtr(content)[1] == 0xFE;
                    if (length >= 4)
                    {
                        bomAsUcs4LE = bomMightBeUtf16LE &&
                                      asUCharPtr(content)[2] == 0 &&
                                      asUCharPtr(content)[3] == 0;
                    }
                    if (!bomAsUcs4LE)
                    {
                        bomAsUtf16LE = bomMightBeUtf16LE;
                    }
                }
                // starting with non-BOM "FF" is a bad idea
                if (!bomAsUcs4LE && !bomAsUtf16LE)
                {
                    return false;
                }
                break;

        case 0xFE:
                {
                    if (length >= 2)
                    {
                        bomAsUtf16BE = asUCharPtr(content)[1] == 0xFF;
                    }
                }
                break;

        case 0:
                {
                    if (length >= 4)
                    {
                        bomAsUcs4BE = asUCharPtr(content)[1] == 0 &&
                                      asUCharPtr(content)[2] == 0xFE &&
                                      asUCharPtr(content)[3] == 0xFF;
                    }
                    else
                    {
                        // if it starts with a null, it looks like empty text
                        return true;
                    }
                }
                break;

        default:
                if (pointsToControlChar(content))
                {
                    return false;
                }
                break;
    }

    const char* pContent = content;
    int nLeft = length;

    if (bomAsUtf16LE || bomAsUtf16BE)
    {
        pContent += 2;
        nLeft -= 2;
    }
    if (bomAsUtf8)
    {
        pContent += 3;
        nLeft -= 3;
    }
    if (bomAsUcs4LE || bomAsUcs4BE)
    {
        pContent += 4;
        nLeft -= 4;
    }

    // check encoding of buffer -- first for raw control chars, then for encoding
    // might be bomAsUtf8, certainly isn't:
    if (!bomAsUtf16LE && !bomAsUtf16BE && !bomAsUcs4LE && !bomAsUcs4BE)
    {
        if (foundControlChar(pContent, nLeft))
        {
            return false;
        }
        if (ansak::isUtf8(pContent, nLeft))
        {
            if (encoding != 0)
            {
                *encoding = kUtf8;
            }
            return true;
        }
        else if (bomAsUtf8)
        {
            return false;
        }
        else
        {
            if (encoding != 0)
            {
                *encoding = kLocalText;
            }
            return true;
        }
    }

    // grab a copy so as to be able to massage the content for endian-ness (and
    // null-terminate to boot!)
    unique_ptr<char> work(new char[nLeft + sizeof(char32_t)]);
    auto workP = work.get();
    memcpy(workP, pContent, nLeft);
    *(reinterpret_cast<char32_t*>(workP + nLeft)) = static_cast<char32_t>(0);

    bool sameEndian = isLittleEndian() && (bomAsUcs4LE || bomAsUtf16LE);

    if (sameEndian)
    {
        // bomAsUtf8 is gone... check for control chars (in same-as-machine order)
        if ((bomAsUtf16LE || bomAsUtf16BE) &&
            foundControlChar(reinterpret_cast<char16_t*>(workP), nLeft / 2))
        {
            return false;
        }
        else if ((bomAsUcs4LE || bomAsUcs4BE) &&
                 foundControlChar(reinterpret_cast<char32_t*>(workP), nLeft / 4))
        {
            return false;
        }
        // is it UTF-16?
        if (bomAsUtf16LE || bomAsUtf16BE)
        {
            if (ansak::isUtf16(reinterpret_cast<char16_t*>(workP)))
            {
                if (encoding != nullptr)
                {
                    *encoding = bomAsUtf16LE ? kUtf16LE : kUtf16BE;
                }
                return true;
            }
        }
        // is it UTF-16?
        else if (bomAsUcs4LE || bomAsUcs4BE)
        {
            if (ansak::isUcs4(reinterpret_cast<char32_t*>(workP)))
            {
                if (encoding != nullptr)
                {
                    *encoding = bomAsUcs4LE ? kUcs4LE : kUcs4BE;
                }
                return true;
            }
        }
    }
    else
    {
        if (bomAsUtf16LE || bomAsUtf16BE)
        {
            // reverse the words (do this with a lambda?)
            for (char16_t* p = reinterpret_cast<char16_t*>(workP); *p; ++p)
            {
                auto c = *p;
                c = ((c & 0xFF) << 8) | ((c >> 8) & 0xFF);
                *p = c;
            }
            // now look for control characters
            if (foundControlChar(reinterpret_cast<char16_t*>(workP), nLeft / 2))
            {
                return false;
            }
            // is it UTF-16?
            if (ansak::isUtf16(reinterpret_cast<char16_t*>(workP)))
            {
                if (encoding != nullptr)
                {
                    *encoding = bomAsUtf16LE ? kUtf16LE : kUtf16BE;
                }
                return true;
            }
        }
        else if (bomAsUcs4LE || bomAsUcs4BE)
        {
            // reverse the dwords (do this with a lambda?)
            for (char32_t* p = reinterpret_cast<char32_t*>(workP); *p; ++p)
            {
                auto c = *p;
                c = ((c & 0xFF) << 24) | ((c & 0xFF00) << 8) |
                    ((c >> 8) & 0xFF00) | ((c >> 24) & 0xFF);
                *p = c;
            }
            // now look for control characters
            if (foundControlChar(reinterpret_cast<char32_t*>(workP), nLeft / 4))
            {
                return false;
            }
            // is it UCS-4? Actually, be more picky
            if (ansak::isUcs4(reinterpret_cast<char32_t*>(workP), kUnicode))
            {
                if (encoding != nullptr)
                {
                    *encoding = bomAsUcs4LE ? kUcs4LE : kUcs4BE;
                }
                return true;
            }
        }
    }

    // if we got this far, this isn't text
    return false;
}

    }

}
