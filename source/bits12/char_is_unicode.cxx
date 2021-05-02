///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019, Arthur N. Klassen
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
// 2019.10.12 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// char_is_unicode.cxx -- Code to check if a code-point value is inside
//                        Unicode, locally Unicode 12.0
//
///////////////////////////////////////////////////////////////////////////

#include <string.hxx>
#include "string_internal.hxx"

using namespace std;

namespace ansak {

namespace internal {

const utf8String supportedUnicodeVersion = "12.0.0";

namespace
{

// "isInTheGap" routines for sub-ranges of Unicode domain values. Numbers after isInTheGap
// gives most significant digits of the 21-bit Unicode values. Values less than U+0300 are
// all in the Unicode 12.0 standard

bool isInTheGap0003(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 7:     return row == 8 || row == 9;
    case 8:     return (row < 4) || row == 11 || row == 13;
    case 10:    return row == 2;
    }
}

bool isInTheGap0005(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;

    case 3:
    case 9:     return row == 0;

    case 5:     return row == 7 || row == 8;
    case 8:     return row == 11 || row == 12;
    case 12:    return row >= 8;
    case 14:    return row >= 11 && row <= 14;
    case 15:    return row >= 5;
    }
}

bool isInTheGap0006(char16_t c)
{
    return c == 0x61D;
}

bool isInTheGap0007(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 14;
    case 4:
    case 15:    return row == 11 || row == 12;
    case 11:    return row >= 2;
    }
}

bool isInTheGap0008(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row >= 14;
    case 3:     return row == 15;
    case 5:     return row >= 12 && row != 14;
    case 6:     return row >= 11;

    case 7:
    case 8:
    case 9:
    case 12:    return true;

    case 13:    return row <= 2;
    case 11:    return row == 5 || row >= 14;
    }
}

bool isInTheGap0009(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 8:     return row == 4 || row == 13 || row == 14;
    case 9:     return row == 1 || row == 2;
    case 10:    return row == 9;
    case 11:
        switch (row)
        {
        default:    return false;
        case 1:
        case 3:
        case 4:
        case 5:
        case 10:
        case 11:    return true;
        }
    case 12:
        switch (row)
        {
        default:    return false;
        case 5:
        case 6:
        case 9:
        case 10:
        case 15:    return true;
        }
    case 13:
        switch (row)
        {
        default:    return true;
        case 7:
        case 12:
        case 13:
        case 15:    return false;
        }
    case 14:        return row == 4 || row == 5;
    case 15:        return row == 15;
    }
}

bool isInTheGap000A(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;        // should never be here
    case 0:     return row == 0 || row == 4 || (row >= 11 && row < 15);
    case 1:     return row == 1 || row == 2;
    case 2:     return row == 9;
    case 3:
        switch (row)
        {
        default:    return false;
        case 1:
        case 4:
        case 7:
        case 10:
        case 11:
        case 13:    return true;
        }
    case 4:
        switch (row)
        {
        default:    return false;
        case 3:
        case 4:
        case 5:
        case 6:
        case 9:
        case 10:
        case 14:
        case 15:    return true;
        }
    case 5:
        switch (row)
        {
        default:    return false;
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 13:
        case 15:    return true;
        }
    case 6:     return row <= 5;
    case 7:     return row >= 7;
    case 8:
        switch (row)
        {
        default:    return false;
        case 0:
        case 4:
        case 14:    return true;
        }
    case 9:     return row == 2;
    case 10:    return row == 9;
    case 11:
        switch (row)
        {
        default:    return false;
        case 1:
        case 4:
        case 10:
        case 11:    return true;
        }
    case 12:    return row == 6 || row == 10 || row >= 14;
    case 13:    return row >= 1;
    case 14:    return row == 4 || row == 5;
    case 15:    return row >= 2 && row <= 8;
    }
}

bool isInTheGap000B(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;        // should never be here
    case 0:     return row == 0 || row == 4 || row == 13 || row == 14;
    case 1:     return row == 1 || row == 2;
    case 2:     return row == 9;
    case 3:     return row == 1 || row == 4 || row == 10 || row == 11;
    case 4:
        switch (row)
        {
        default:    return false;
        case 5:
        case 6:
        case 9:
        case 10:
        case 14:
        case 15:    return true;
        }
    case 5:     return row <= 5 || (row >= 8 && row <= 11) || row == 14;
    case 6:     return row == 4 || row == 5;
    case 7:     return row >= 8;
    case 8:
        switch (row)
        {
        default:    return false;
        case 0:
        case 1:
        case 4:
        case 11:
        case 12:
        case 13:    return true;
        }
    case 9:
        switch (row)
        {
        default:    return false;
        case 1:
        case 6:
        case 7:
        case 8:
        case 11:
        case 13:    return true;
        }
    case 10:
        switch (row)
        {
        default:    return true;
        case 3:
        case 4:
        case 8:
        case 9:
        case 10:
        case 14:
        case 15:    return false;
        }
    case 11:    return row >= 10 && row <= 13;
    case 12:
        switch (row)
        {
        default:    return false;
        case 3:
        case 4:
        case 5:
        case 9:
        case 14:
        case 15:    return true;
        }
    case 13:
        switch (row)
        {
        default:    return true;
        case 0:
        case 7:     return false;
        }
    case 14:    return row <= 5;
    case 15:    return row >= 11;
    }
}

bool isInTheGap000C(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;        // should never be here
    case 0:     return row == 13;
    case 1:     return row == 1;
    case 2:     return row == 9;
    case 3:     return row >= 10 && row <= 12;
    case 4:     return row == 5 || row == 9 || row >= 14;
    case 5:     return row <= 4 || row == 7 || row >= 11;
    case 6:     return row == 4 || row == 5;
    case 7:     return row <= 6;
    case 8:     return row == 13;
    case 9:     return row == 1;
    case 10:    return row == 9;
    case 11:    return row == 4 || row == 10 || row == 11;
    case 12:    return row == 5 || row == 9 || row >= 14;
    case 13:    return row <= 4 || (row >= 7 && row != 14);
    case 14:    return row == 4 || row == 5;
    case 15:    return row == 0 || row >= 3;
    }
}

bool isInTheGap000D(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 4 || row == 13;
    case 1:     return row == 1;
    case 4:     return row == 5 || row == 9;
    case 5:     return row <= 3;
    case 6:     return row == 4 || row == 5;
    case 8:     return row < 2 || row == 4;
    case 9:     return row >= 7 && row <= 9;
    case 11:    return row == 2 || (row >= 12 && row != 13);
    case 12:    return row >= 7 && row != 10 && row <= 14;
    case 13:    return row == 5 || row == 7;
    case 14:    return row <= 5;
    case 15:    return row < 2 || row >= 5;
    }
}

bool isInTheGap000E(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 0;
    case 3:     return row >= 11 && row <= 14;
    case 5:     return row >= 12;
    case 6:
    case 7:
    case 14:
    case 15:    return true;
    case 8:
        switch (row)
        {
        case 0:
        case 3:
        case 5:
        case 11:    return true;
        default:    return false;
        }
    case 10:
        switch (row)
        {
        default:    return false;
        case 4:
        case 6:     return true;
        }
    case 11:    return row >= 14;
    case 12:    return row == 5 || row == 7 || row >= 14;
    case 13:    return row == 10 || row == 11;
    }
}

bool isInTheGap000F(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row == 8;
    case 6:     return row >= 13;
    case 7:     return row == 0;
    case 9:     return row == 8;
    case 11:
    case 12:    return row == 13;
    case 13:    return row >= 11;
    case 14:
    case 15:    return true;
    }
}

bool isInTheGap0010(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 12:    return row == 6 || (row >= 8 && row != 13);
    }
}

bool isInTheGap0012(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row == 9 || row >= 14;
    case 5:     return row == 7 || row == 9 || row >= 14;
    case 8:     return row == 9 || row >= 14;
    case 11:    return row == 1 || row == 6 || row == 7|| row == 15;
    case 12:    return row == 1 || row == 6 || row == 7;
    case 13:    return row == 7;
    }
}

bool isInTheGap0013(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:     return row == 1 || row == 6 || row == 7;
    case 5:     return row == 11 || row == 12;
    case 7:     return row >= 13;
    case 9:     return row >= 10;
    case 15:    return (row & 7) >= 6;
    }
}

bool isInTheGap0016(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 9:     return row >= 13;
    case 15:    return row >= 9;
    }
}

bool isInTheGap0017(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 13;
    case 1:     return row >= 5;
    case 3:     return row >= 7;
    case 5:     return row >= 4;
    case 6:     return row == 13;
    case 7:     return row == 1 || row >= 4;
    case 13:    return row >= 14;
    case 14:
    case 15:    return row >= 10;
    }
}

bool isInTheGap0018(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 15;
    case 1:     return row >= 10;
    case 7:     return row >= 9;
    case 10:    return row >= 11;
    case 15:    return row >= 6;
    }
}

bool isInTheGap0019(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:     return row == 15;
    case 2:
    case 3:     return row >= 12;
    case 4:     return row >= 1 && row <= 3;
    case 6:     return row >= 14;
    case 7:     return row >= 5;
    case 10:    return row >= 12;
    case 12:    return row >= 10;
    case 13:    return row >= 11 && row <= 13;
    }
}

bool isInTheGap001A(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:     return row == 12 || row == 13;
    case 5:     return row == 15;
    case 7:     return row == 13 || row == 14;
    case 8:
    case 9:     return row >= 10;
    case 10:    return row >= 14;
    case 11:    return row == 15;
    case 12:
    case 13:
    case 14:
    case 15:    return true;
    }
}

bool isInTheGap001B(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row >= 12;
    case 7:     return row >= 13;
    case 15:    return row >= 4 && row <= 11;
    }
}

bool isInTheGap001C(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 3:     return row >= 8 && row <= 10;
    case 4:     return row >= 10 && row <= 12;
    case 8:     return row >= 9;
    case 11:    return row == 11 || row == 12;
    case 12:    return row >= 8;
    case 15:    return row >= 11;
    }
}

bool isInTheGap001F(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:
    case 4:     return row == 6 || row == 7 || row == 14 || row == 15;
    case 5:     return row == 8 || row == 10 || row == 12 || row == 14;
    case 7:     return row >= 14;
    case 11:
    case 12:    return row == 5;
    case 13:    return row == 4 || row == 5 || row == 12;
    case 15:    return row < 2 || row == 5 || row == 15;
    }
}

bool isInTheGap0020(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 6:     return row == 5;
    case 7:     return row == 2 || row == 3;
    case 8:     return row == 15;
    case 9:     return row >= 13;
    case 12:    return true;
    case 15:    return row != 0;
    }
}

bool isInTheGap0021(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column == 8)
    {
        return (c & 0xF) >= 12;
    }
    return false;
}

bool isInTheGap0024(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row >= 7;
    case 3:
    case 5:     return true;
    case 4:     return row >= 11;
    }
}

bool isInTheGap002B(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 7:     return row == 4 || row == 5;
    case 9:     return row == 6 || row == 7;
    }
}

bool isInTheGap002C(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:        return false;
    case 2:
    case 5:     return row == 15;
    case 15:    return row >= 4 && row <= 8;
    }
}

bool isInTheGap002D(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:
        switch (row)
        {
        default:    return true;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 7:
        case 13:    return false;
        }
    case 6:     return row >= 8 && row <= 14;
    case 7:     return row != 0 && row != 15;
    case 9:     return row >= 7;
    case 10:
    case 11:
    case 12:
    case 13:    return row == 7 || row == 15;
    }
}

bool isInTheGap002E(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 5:
    case 6:
    case 7:     return true;
    case 9:     return row == 10;
    case 15:    return row >= 4;
    }
}

bool isInTheGap002F(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 13:    return row >= 6;
    case 14:    return true;
    case 15:    return row >= 12;
    }
}

bool isInTheGap0030(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row == 0;
    case 9:     return row == 7 || row == 8;
    }
}

bool isInTheGap0031(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row <= 4;
    case 3:     return row == 0;
    case 8:     return row == 15;
    case 11:    return row >= 11;
    case 14:    return row >= 4;
    }
}

bool isInTheGap0032(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column == 1 || column == 15)
    {
        return (c & 0xF) == 15;
    }
    return false;
}

bool isInTheGap004D(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column == 11)
    {
        return (c & 0xF) >= 6;
    }
    return false;
}

bool isInTheGap009F(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column == 15)
    {
        return true;
    }
    return false;
}

bool isInTheGap00A4(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 8:     return row >= 13;
    case 12:    return row >= 7;
    }
}

bool isInTheGap00A6(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row >= 12;
    case 3:     return true;
    case 15:    return row >= 8;
    }
}

bool isInTheGap00A7(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 13:
    case 14:    return true;
    case 12:    return row <= 1 || row >= 7;
    case 15:    return row <= 6;
    }
}

bool isInTheGap00A8(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row >= 12;
    case 3:     return row >= 10;
    case 7:     return row >= 8;
    case 12:    return row >= 6 && row <= 13;
    case 13:    return row >= 10;
    }
}

bool isInTheGap00A9(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 5:     return row >= 4 && row <= 14;
    case 7:     return row >= 13;
    case 12:    return row == 14;
    case 13:    return row >= 10 && row <= 13;
    case 15:    return row == 15;
    }
}

bool isInTheGap00AA(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 3:     return row >= 7;
    case 4:     return row >= 14;
    case 5:     return row == 10 || row == 11;
    case 12:    return row >= 3;
    case 13:    return row <= 10;
    case 15:    return row >= 7;
    }
}

bool isInTheGap00AB(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 0 || row == 7 || row == 8 || row == 15;
    case 1:     return row == 0 || row >= 7;
    case 2:     return row == 7 || row == 15;
    case 6:     return row >= 8;
    case 14:    return row >= 14;
    case 15:    return row >= 10;
    }
}

bool isInTheGap00D7_DF(char16_t c)
{
    auto page = (c >> 8) & 0xF;
    if (page == 7)
    {
        auto column = (c >> 4) & 0xF;
        auto row = c & 0xF;
        switch (column)
        {
        default:    return false;
        case 10:    return row >= 4;
        case 12:    return row >= 7 && row <= 10;
        case 15:    return row >= 12;
        }
    }
    return true;
}

bool isInTheGap00FA(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 6:     return row >= 14;
    case 13:    return row >= 10;
    case 14:
    case 15:    return true;
    }
}

bool isInTheGap00FB(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row >= 7;
    case 1:     return row <= 2 || (row >= 8 && row <= 12);
    case 3:     return row == 7 || row ==13 || row ==15;
    case 4:     return row == 2 || row == 5;
    case 12:    return row >= 2;
    case 13:    return row <= 2;
    }
}

bool isInTheGap00FD(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return true;
    case 9:     return row <= 1;
    case 12:    return row >= 8;
    case 13:
    case 14:    return true;
    case 15:    return row >= 14;
    }
}

bool isInTheGap00FE(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:     return row >= 10;
    case 5:     return row == 3;
    case 6:     return row == 7 || row >= 12;
    case 7:     return row == 5;
    case 15:    return row == 13 || row == 14;
    }
}

bool isInTheGap00FF(char16_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 0;
    case 11:    return row == 15;
    case 12:    return row == 0 || row == 1 || row == 8 || row == 9;
    case 13:    return row <= 1 || row == 8 || row == 9 || row >= 13;
    case 14:    return row == 7 || row == 15;
    case 15:    return row <= 8 || row >= 14;
    }
}

bool isInTheGap0100(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 12;
    case 2:     return row == 7;
    case 3:     return row == 11 || row == 14;
    case 4:
    case 5:     return row >= 14;
    case 6:
    case 7:     return true;
    case 15:    return row >= 11;
    }
}

bool isInTheGap0101(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row >= 3 && row <= 6;
    case 3:     return row >= 4 && row <= 6;
    case 8:     return row == 15;
    case 9:     return row >= 12;
    case 10:    return row != 0;
    case 11:
    case 12:    return true;
    case 15:    return row >= 14;
    }
}

bool isInTheGap0102(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 8:
    case 10:
    case 11:
    case 12:
    case 14:    return false;
    case 9:     return row >= 13;
    case 13:    return row != 0;
    case 15:    return row >= 12;
    }
}

bool isInTheGap0103(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row >= 4 && row <= 12;
    case 4:
    case 7:     return row >= 11;
    case 9:     return row == 14;
    case 12:    return row >= 4 && row <= 7;
    case 13:    return row >= 6;
    case 14:
    case 15:    return true;
    }
}

bool isInTheGap0104(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 9:     return row >= 14;
    case 10:    return row >= 10;
    case 13:    return row >= 4 && row <= 7;
    case 15:    return row >= 12;
    }
}

bool isInTheGap0105(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 7)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row >= 8;
    case 6:     return row >= 4 && row <= 14;
    }
}

bool isInTheGap0107(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 0:
    case 1:
    case 2:
    case 4:     return false;
    case 3:     return row >= 7;
    case 5:     return row >= 6;
    case 6:     return row >= 8;
    }
}

bool isInTheGap0108(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 6 || row == 7 || row == 9;
    case 3:     return row == 6 || (row >= 9 && row <= 14 && row != 12);
    case 5:     return row == 6;
    case 9:     return row == 15;
    case 10:    return row <= 6;
    case 11:
    case 12:
    case 13:    return true;
    case 15:    return row == 3 || (row >= 6 && row <= 10);
    }
}

bool isInTheGap0109(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:     return row >= 12 && row <= 14;
    case 3:     return row >= 10 && row <= 14;
    case 4:
    case 5:
    case 6:
    case 7:     return true;
    case 11:    return row >= 8 && row <= 11;
    case 13:    return row <= 1;
    }
}

bool isInTheGap010A(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 4 || (row >= 7 && row <= 11);
    case 1:     return row == 4 || row == 8;
    case 3:     return row == 6 || row == 7 || (row >= 11 && row <= 14);
    case 4:
    case 5:     return row >= 9;
    case 10:
    case 11:    return true;
    case 14:    return row >= 7 && row <= 10;
    case 15:    return row >= 7;
    }
}

bool isInTheGap010B(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 3:     return row >= 6 && row <= 8;
    case 5:     return row == 6 || row == 7;
    case 7:     return row >= 3 && row <= 7;
    case 9:     return (row >= 2 && row <= 8) || row >= 13;
    case 10:    return row <= 8;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:    return true;
    }
}

bool isInTheGap010C(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row >= 9;
    case 5:
    case 6:
    case 7:     return true;
    case 11:    return row >= 3;
    case 15:    return row >= 3 && row <= 9;
    }
}

bool isInTheGap010D(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 0:
    case 1:     return false;
    case 2:     return row >= 8;
    case 3:     return row >= 10;
    }
}

bool isInTheGap010E(char32_t c)
{
    auto offset = c & 0xFF;
    return offset <= 0x5f || offset >= 0x7f;
}

bool isInTheGap010F(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    case 0:
    case 1:
    case 3:
    case 4:
    case 14:    return false;

    case 2:     return row >= 8;
    case 5:     return row >= 10;
    case 15:    return row >= 7;
    default:    return true;
    }
}

bool isInTheGap0110(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row >= 14;
    case 5:     return row <= 1;
    case 7:     return row <= 14;
    case 12:    return row >= 2 && row != 13;
    case 14:    return row >= 9;
    case 15:    return row >= 10;
    }
}

bool isInTheGap0111(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 3:     return row == 5;
    case 4:
    case 7:     return row >= 7;
    case 12:    return row >= 14;
    case 14:    return row == 0;
    case 15:    return row >= 5;
    }
}

bool isInTheGap0112(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:     return row == 2;
    case 3:     return row == 15;
    case 4:
    case 5:
    case 6:
    case 7:     return true;
    case 8:     return row == 7 || row == 9 || row == 14;
    case 9:     return row == 14;
    case 10:    return row >= 10;
    case 14:    return row >= 11;
    case 15:    return row >= 10;
    }
}

bool isInTheGap0113(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 8)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 4 || row == 13 || row == 14;
    case 1:     return row == 1 || row == 2;
    case 2:     return row == 9;
    case 3:     return row == 1 || row == 4 || row == 10;
    case 4:
        switch (row)
        {
            default:    return false;
            case 5:
            case 6:
            case 9:
            case 10:
            case 14:
            case 15:    return true;
        }
    case 5:     return (row >= 1 && row <= 6) || (row >= 8 && row <= 12);
    case 6:     return row == 4 || row == 5 || row >= 13;
    case 7:     return row >= 5;
    }
}

bool isInTheGap0114(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 5:     return row == 10 || row == 12;
    case 12:    return row >= 8;
    case 13:    return row >= 10;
    case 6:
    case 7:
    case 14:
    case 15:    return true;
    }
}

bool isInTheGap0115(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 8:
    case 9:
    case 10:
    case 12:    return false;
    case 11:    return row == 6 || row == 7;
    case 13:    return row >= 14;
    }
}

bool isInTheGap0116(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row >= 5;
    case 5:     return row >= 10;
    case 6:     return row >= 13;
    case 7:
    case 13:
    case 14:
    case 15:    return true;
    case 11:    return row >= 9;
    case 12:    return row >= 10;
    }
}

bool isInTheGap0117(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 0:
    case 3:     return false;
    case 1:     return row == 11 || row == 12;
    case 2:     return row >= 12;
    }
}

bool isInTheGap0118(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 3:     return row >= 12;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:     return true;
    case 15:    return row >= 3 && row <= 14;
    }
}

bool isInTheGap0119(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 10:
    case 13:    return row == 8 || row == 9;
    case 14:    return row >= 5;
    case 11:
    case 12:    return false;
    }
}

bool isInTheGap011A(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row >= 8;
    case 10:    return row >= 3;
    case 11:    return true;
    case 15:    return row >= 9;
    }
}

bool isInTheGap011C(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 9;
    case 3:     return row == 7;
    case 4:     return row >= 6;
    case 6:     return row >= 13;
    case 9:     return row <= 1;
    case 10:    return row == 8;
    case 11:    return row >= 7;
    case 12:
    case 13:
    case 14:
    case 15:    return true;
    }
}

bool isInTheGap011D(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    case 0:     return row == 7 || row == 10;
    case 1:
    case 2:
    case 7:     return false;
    case 3:     return ( row >= 7 && row <= 9 ) || row == 11 || row == 14;
    case 4:     return row >= 8;
    case 5:
    case 10:    return row >= 10;
    case 6:     return row == 6 || row == 9;
    case 8:     return row == 15;
    case 9:     return row == 2 || row >= 9;
    default:    return true;
    }
}

bool isInTheGap011E(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 14:    return false;
    case 15:    return row >= 9;
    }
}

bool isInTheGap011F(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 12:
    case 13:
    case 14:    return false;
    case 15:    return row >= 2 && row < 15;
    }
}

bool isInTheGap0123(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 10)
    {
        return true;
    }

    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 9:     return row >= 10;
    }
}

bool isInTheGap0124(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 6:     return row == 15;
    case 7:     return row >= 5;
    }
}

bool isInTheGap0125(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column < 4)
    {
        return false;
    }
    if (column > 4)
    {
        return true;
    }
    return (c & 0xF) >= 4;
}

bool isInTheGap0134(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    case 0:
    case 1:     return false;
    case 2:     return row == 15;
    case 3:     return row >= 9;
    default:    return true;
    }
}

bool isInTheGap0146(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column < 4)
    {
        return false;
    }
    if (column > 4)
    {
        return true;
    }
    return (c & 0xF) >= 7;
}

bool isInTheGap016A(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 7 && column <= 12)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 3:     return row >= 9;
    case 5:     return row == 15;
    case 6:     return row >= 10 && row <= 13;
    case 14:    return row >= 14;
    case 15:    return row >= 6;
    }
}

bool isInTheGap016B(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 9)
    {
        return true;
    }

    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row >= 6;
    case 5:     return row == 10;
    case 6:     return row == 2;
    case 7:     return row >= 8 && row <= 12;
    }
}

bool isInTheGap016E(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column < 4 || column > 9)
    {
        return true;
    }
    if (column != 9)
    {
        return false;
    }

    return (c & 0xF) >= 11;
}

bool isInTheGap016F(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 10 && column != 14)
    {
        return true;
    }

    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 4:     return row >= 11 && row <= 14;
    case 8:     return row >= 8 && row <= 14;
    case 14:    return row >= 4;
    }
}

bool isInTheGap0187(char32_t c)
{
    if (((c >> 4) & 0xF) != 15)
    {
        return false;
    }
    return (c & 0xF) >= 8;
}

bool isInTheGap018A(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column == 15)
    {
        return (c & 0xF) >= 3;
    }
    else
    {
        return false;
    }
}

bool isInTheGap01B1(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 1:     return row == 15;
    case 5:     return row >= 3;
    case 6:     return row <= 3 || row >= 8;
    case 2:
    case 3:
    case 4:     return true;
    }
}

bool isInTheGap01B2(char32_t c)
{
    return (c & 0xFF) >= 0xFC;
}

bool isInTheGap01BC(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 11)
    {
        return true;
    }

    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 6:     return row >= 11;
    case 7:     return row >= 13;
    case 8:     return row >= 9;
    case 9:     return row == 10 || row == 11;
    case 10:    return row >= 4;
    }
}

bool isInTheGap01D0(char32_t c)
{
    return (c & 0xFF) >= 0xF6;
}

bool isInTheGap01D1(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row == 7 || row == 8;
    case 14:    return row >= 9;
    case 15:    return true;
    }
}

bool isInTheGap01D2(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 0:
    case 1:
    case 2:
    case 3:
    case 14:    return false;
    case 4:     return row >= 6;
    case 15:    return row >= 4;
    }
    if (column < 4)
    {
        return false;
    }
    if (column > 4)
    {
        return true;
    }
    return (c & 0xF) >= 6;
}

bool isInTheGap01D3(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:     return false;
    case 5:     return row >= 7;
    case 7:     return row >= 9;
    }
}

bool isInTheGap01D4(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 5:     return row == 5;
    case 9:     return row == 13;
    case 10:
        switch (row)
        {
        default:    return false;
        case 0:
        case 1:
        case 3:
        case 4:
        case 7:
        case 8:
        case 13:    return true;
        }
    case 11:    return row == 10 || row == 12;
    case 12:    return row == 4;
    }
}

bool isInTheGap01D5(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 6 || row == 11 || row == 12;
    case 1:     return row == 5 || row == 13;
    case 3:     return row == 10 || row == 15;
    case 4:     return row >= 5 && row <= 9 && row != 6;
    case 5:     return row == 1;
    }
}

bool isInTheGap01D6(char32_t c)
{
    auto offset = c & 0xFF;
    return offset == 0xA6 || offset == 0xA7;
}

bool isInTheGap01D7(char32_t c)
{
    auto offset = c & 0xFF;
    return offset == 0xCC || offset == 0xCD;
}

bool isInTheGap01DA(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 11)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 8:     return row >= 12;
    case 9:     return row <= 10;
    case 10:    return row == 0;
    }
}

bool isInTheGap01E0(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 3)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    case 0:     return row == 7;
    case 1:     return row == 9 || row == 10;
    case 2:     return row == 2 || row == 5 || row >= 11;
    default:    return true;
    }
}

bool isInTheGap01E1(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    case 0:
    case 1:     return false;
    case 2:     return row >= 13;
    case 3:     return row >= 14;
    case 4:     return row >= 10 && row <= 13;
    default:    return true;
    }
}

bool isInTheGap01E2(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    case 12:
    case 13:
    case 14:    return false;
    case 15:    return row >= 10 && row <= 14;
    default:    return true;
    }
}

bool isInTheGap01E8(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 14)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 12:    return row == 5 || row == 6;
    case 13:    return row >= 7;
    }
}

bool isInTheGap01E9(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 6)
    {
        return true;
    }
    else if (column < 4)
    {
        return false;
    }
    auto row = c & 0xF;
    if (column == 4)
    {
        return row >= 12;
    }
    else    // if (column == 5)
    {
        return row >= 10 && row <= 13;
    }
}

bool isInTheGap01EC(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 7:     return row == 0;
    case 8:
    case 9:
    case 10:    return false;
    case 11:    return row >= 5;
    }
}

bool isInTheGap01ED(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return true;
    case 0:     return row == 0;
    case 1:
    case 2:     return false;
    case 3:     return row >= 14;
    }
}

bool isInTheGap01EE(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row == 4;
    case 2:
        switch (row)
        {
        default:    return false;
        case 0:
        case 3:
        case 5:
        case 6:
        case 8:     return true;
        }
    case 3:
        switch (row)
        {
        default:    return false;
        case 3:
        case 8:
        case 10:
        case 12:
        case 13:
        case 14:
        case 15:    return true;
        }
     case 4:
        switch (row)
        {
        default:    return false;
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 10:
        case 12:    return true;
        }
    case 5:
        switch (row)
        {
        default:    return false;
        case 0:
        case 3:
        case 5:
        case 6:
        case 8:
        case 10:
        case 12:
        case 14:    return true;
        }
    case 6:     return row == 0 || row == 3 || row == 5 || row == 6 || row == 11;
    case 7:     return row == 3 || row == 8 || row == 13 || row == 15;
    case 8:     return row == 10;
    case 9:     return row >= 12;
    case 10:    return row == 0 || row == 4 || row == 10;
    case 11:    return row >= 12;
    case 12:
    case 13:
    case 14:    return true;
    case 15:    return row >= 2;
    }
}

bool isInTheGap01F0(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 2:     return row >= 12;
    case 9:     return row >= 4;
    case 10:    return row == 15;
    case 11:
    case 12:
    case 13:    return row == 0;
    case 15:    return row >= 6;
    }
}

bool isInTheGap01F1(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row >= 13;
    case 6:     return row >= 13;
    case 10:    return row >= 13;
    case 11:
    case 12:
    case 13:    return true;
    case 14:    return row <= 5;
    }
}

bool isInTheGap01F2(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column >= 7)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row >= 3;
    case 3:     return row >= 12;
    case 4:     return row >= 9;
    case 5:     return row >= 2;
    case 6:     return row >= 6;
    }
}

bool isInTheGap01F6(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column < 13)
    {
        return false;
    }

    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 13:    return row >= 6;
    case 14:    return row >= 13;
    case 15:    return row >= 11;
    }
}

bool isInTheGap01F7(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column < 7)
    {
        return false;
    }

    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 7:     return row >= 4;
    case 13:    return row >= 9;
    case 14:    return row >= 12;
    case 15:    return true;
    }
}

bool isInTheGap01F8(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column > 10)
    {
        return true;
    }
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;
    case 0:     return row >= 12;
    case 4:     return row >= 8;
    case 5:     return row >= 10;
    case 8:     return row >= 8;
    case 10:    return row >= 14;
    }
}

bool isInTheGap01F9(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false; 
    case 0:     return row == 12;
    case 7:     return row == 2 || (row >= 7 && row <= 9);
    case 10:    return row == 3 || row == 4 || (row >= 11 && row <= 13);
    case 12:    return row == 11 || row == 12;
    }
}

bool isInTheGap01FA(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:     return false;
    case 5:     return row >= 4;
    case 6:     return row >= 14;
    case 7:     return (row >= 4 && row <= 7) || row >= 11;
    case 8:     return row >= 3;
    case 9:     return row >= 6;
    default:    return true;
    }
}

bool isInTheGap01(char32_t c)
{
    auto page = (c >> 8) & 0xFF;
    auto sheaf = page >> 4;
    page &= 0xF;

    switch (sheaf)
    {
    default:    return true;
    case 0:     // U+10xxx
        switch (page)
        {
        default:    return false;

        case 0:     return isInTheGap0100(c);
        case 1:     return isInTheGap0101(c);
        case 2:     return isInTheGap0102(c);
        case 3:     return isInTheGap0103(c);
        case 4:     return isInTheGap0104(c);
        case 5:     return isInTheGap0105(c);
        case 7:     return isInTheGap0107(c);
        case 8:     return isInTheGap0108(c);
        case 9:     return isInTheGap0109(c);
        case 10:    return isInTheGap010A(c);
        case 11:    return isInTheGap010B(c);
        case 12:    return isInTheGap010C(c);
        case 13:    return isInTheGap010D(c);
        case 14:    return isInTheGap010E(c);
        case 15:    return isInTheGap010F(c);
        }

    case 1:     // U+11xxx
        switch (page)
        {
        default:    return true;

        case 0:     return isInTheGap0110(c);
        case 1:     return isInTheGap0111(c);
        case 2:     return isInTheGap0112(c);
        case 3:     return isInTheGap0113(c);
        case 4:     return isInTheGap0114(c);
        case 5:     return isInTheGap0115(c);
        case 6:     return isInTheGap0116(c);
        case 7:     return isInTheGap0117(c);
        case 8:     return isInTheGap0118(c);
        case 9:     return isInTheGap0119(c);
        case 10:    return isInTheGap011A(c);
        case 12:    return isInTheGap011C(c);
        case 13:    return isInTheGap011D(c);
        case 14:    return isInTheGap011E(c);
        case 15:    return isInTheGap011F(c);
        }

    case 2:     // U+12xxx
        switch (page)
        {
        default:    return true;
        case 0:
        case 1:
        case 2:     return false;
        case 3:     return isInTheGap0123(c);
        case 4:     return isInTheGap0124(c);
        case 5:     return isInTheGap0125(c);
        }

    case 3:     // U+13xxx
        switch (page)
        {
        default:    return true;
        case 0:
        case 1:
        case 2:
        case 3:     return false;
        case 4:     return isInTheGap0134(c);
        }

    case 4:     // U+14xxx
        switch (page)
        {
        default:    return true;
        case 4:
        case 5:     return false;
        case 6:     return isInTheGap0146(c);
        }

    case 6:     // U+16xxx
        switch (page)
        {
        default:    return true;
        case 8:
        case 9:     return false;
        case 10:    return isInTheGap016A(c);
        case 11:    return isInTheGap016B(c);
        case 14:    return isInTheGap016E(c);
        case 15:    return isInTheGap016F(c);
        }

    case 7:         return false;
    case 8:
        switch (page)
        {
        default:    return false;
        case 7:     return isInTheGap0187(c);
        case 10:    return isInTheGap018A(c);
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:    return true;
        }

    case 11:    // U+1Bxxx
        switch (page)
        {
        default:    return true;
        case 0:     return false;
        case 1:     return isInTheGap01B1(c);
        case 2:     return isInTheGap01B2(c);
        case 12:    return isInTheGap01BC(c);
        }

    case 13:    // U+1Dxxx
        switch(page)
        {
        default:    return true;

        case 8:
        case 9:     return false;

        case 0:     return isInTheGap01D0(c);
        case 1:     return isInTheGap01D1(c);
        case 2:     return isInTheGap01D2(c);
        case 3:     return isInTheGap01D3(c);
        case 4:     return isInTheGap01D4(c);
        case 5:     return isInTheGap01D5(c);
        case 6:     return isInTheGap01D6(c);
        case 7:     return isInTheGap01D7(c);
        case 10:    return isInTheGap01DA(c);
        }

    case 14:    // U+1Exxx
        switch(page)
        {
        default:    return true;
        case 0:     return isInTheGap01E0(c);
        case 1:     return isInTheGap01E1(c);
        case 2:     return isInTheGap01E2(c);
        case 8:     return isInTheGap01E8(c);
        case 9:     return isInTheGap01E9(c);
        case 12:    return isInTheGap01EC(c);
        case 13:    return isInTheGap01ED(c);
        case 14:    return isInTheGap01EE(c);
        }

    case 15:    // U+1Fxxx
        switch(page)
        {
        default:    return true;
        case 3:
        case 4:
        case 5:     return false;

        case 0:     return isInTheGap01F0(c);
        case 1:     return isInTheGap01F1(c);
        case 2:     return isInTheGap01F2(c);
        case 6:     return isInTheGap01F6(c);
        case 7:     return isInTheGap01F7(c);
        case 8:     return isInTheGap01F8(c);
        case 9:     return isInTheGap01F9(c);
        case 10:    return isInTheGap01FA(c);
        }
    }
}

bool isInTheGap02A6(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column < 13)
    {
        return false;
    }
    if (column > 13)
    {
        return true;
    }
    return (c & 0xF) >= 7;
}

bool isInTheGap02B7(char32_t c)
{
    if (((c >> 4) & 0xF) == 3)
    {
        return (c & 0xF) >= 5;
    }
    else
    {
        return false;
    }
}

bool isInTheGap02B8(char32_t c)
{
    if (((c >> 4) & 0xF) == 1)
    {
        return (c & 0xF) >= 14;
    }
    else
    {
        return false;
    }
}

bool isInTheGap02CE(char32_t c)
{
    // 2ce00..2ce9f -- all in
    // 2cea0, 2cea1 -- in; 2cea2..2ceaf -- out
    // 2ceb0..2ceff -- in
    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    switch (column)
    {
    default:    return false;   // 0..9; 11..15
    case 10:    return row >= 2;
    }
}

bool isInTheGap02EB(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    switch (column)
    {
    default:    return false;
    case 14:    return (c & 0xF) > 0;
    case 15:    return true;
    }
}

bool isInTheGap02FA(char32_t c)
{
    auto column = (c >> 4) & 0xF;
    if (column < 1)
    {
        return false;
    }
    if (column > 1)
    {
        return true;
    }
    return (c & 0xF) >= 14;
}

bool isInTheGap02(char32_t c)
{
    auto page = (c >> 8) & 0xFF;
    auto sheaf = page >> 4;
    page &= 0xF;

    switch (sheaf)
    {
    default:    return false;
    case 10:
        if (page == 6)
        {
            return isInTheGap02A6(c);
        }
        else
        {
            return false;
        }
    case 11:
        switch (page)
        {
        default:    return false;
        case 7:     return isInTheGap02B7(c);
        case 8:     return isInTheGap02B8(c);
        }
    case 12:
        switch (page)
        {
        default:    return false;
        case 14:    return isInTheGap02CE(c);
        }
    case 14: // 0x2e000...0x2eaff -- all in
        switch (page)
        {
        default:    return false;
        case 11:    return isInTheGap02EB(c);
        case 12:
        case 13:
        case 14:
        case 15:    return true;
        }
    case 15:
        if (page < 8 || page > 10)
        {
            return true;
        }
        if (page < 10)
        {
            return false;
        }
        return isInTheGap02FA(c);
    }
}

bool isInTheGap0E(char32_t c)
{
    auto page = (c >> 8) & 0xFF;
    auto sheaf = page >> 4;
    page &= 0xF;

    if (sheaf > 0 || page > 1)
    {
        return true;
    }

    auto column = (c >> 4) & 0xF;
    auto row = c & 0xF;
    if (page == 0)
    {
        if (column >= 8 || column == 1)
        {
            return true;
        }
        else if (column == 0)
        {
            return row != 1;
        }
        return false;
    }
    else
    {
        return column > 14;
    }
}

}

bool isUnicodeAssigned(char16_t c)
{
    auto page = (c >> 8) & 0xFF;
    auto sheaf = page >> 4;
    page &= 0xF;

    switch (sheaf)
    {
    default:    return true;
    case 0:
        switch (page)
        {
        default:    return true;
        case 3:     return !isInTheGap0003(c);
        case 5:     return !isInTheGap0005(c);
        case 6:     return !isInTheGap0006(c);
        case 7:     return !isInTheGap0007(c);
        case 8:     return !isInTheGap0008(c);
        case 9:     return !isInTheGap0009(c);
        case 10:    return !isInTheGap000A(c);
        case 11:    return !isInTheGap000B(c);
        case 12:    return !isInTheGap000C(c);
        case 13:    return !isInTheGap000D(c);
        case 14:    return !isInTheGap000E(c);
        case 15:    return !isInTheGap000F(c);
        }

    case 1:
        switch (page)
        {
        default:    return true;
        case 0:     return !isInTheGap0010(c);
        case 2:     return !isInTheGap0012(c);
        case 3:     return !isInTheGap0013(c);
        case 6:     return !isInTheGap0016(c);
        case 7:     return !isInTheGap0017(c);
        case 8:     return !isInTheGap0018(c);
        case 9:     return !isInTheGap0019(c);
        case 10:    return !isInTheGap001A(c);
        case 11:    return !isInTheGap001B(c);
        case 12:    return !isInTheGap001C(c);
        case 13:    return c != 0x1DFA;
        case 15:    return !isInTheGap001F(c);
        }

    case 2:
        switch (page)
        {
        default:    return true;
        case 0:     return !isInTheGap0020(c);
        case 1:     return !isInTheGap0021(c);
        case 4:     return !isInTheGap0024(c);
        case 11:    return !isInTheGap002B(c);
        case 12:    return !isInTheGap002C(c);
        case 13:    return !isInTheGap002D(c);
        case 14:    return !isInTheGap002E(c);
        case 15:    return !isInTheGap002F(c);
        }

    case 3:
        switch (page)
        {
        default:    return true;
        case 0:     return !isInTheGap0030(c);
        case 1:     return !isInTheGap0031(c);
        case 2:     return !isInTheGap0032(c);
        }

    case 4:     return (page == 13) ? !isInTheGap004D(c) : true;

    case 9:     return (page == 15) ? !isInTheGap009F(c) : true;

    case 10:
        switch (page)
        {
        default:    return true;
        case 4:     return !isInTheGap00A4(c);
        case 6:     return !isInTheGap00A6(c);
        case 7:     return !isInTheGap00A7(c);
        case 8:     return !isInTheGap00A8(c);
        case 9:     return !isInTheGap00A9(c);
        case 10:    return !isInTheGap00AA(c);
        case 11:    return !isInTheGap00AB(c);
        }

    case 13:    return (page >= 7) ? !isInTheGap00D7_DF(c) : true;
    case 14:    return false;
    case 15:
        switch (page)
        {
        default:    return true;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:     return false;
        case 10:    return !isInTheGap00FA(c);
        case 11:    return !isInTheGap00FB(c);
        case 13:    return !isInTheGap00FD(c);
        case 14:    return !isInTheGap00FE(c);
        case 15:    return !isInTheGap00FF(c);
        }
    }

    return true;
}

bool isUnicodeAssigned(char32_t c)
{
    switch(c >> 16)
    {
    default:    return false;
    case 0:     return isUnicodeAssigned(static_cast<char16_t>(c & 0xFFFF));
    case 1:     return !isInTheGap01(c);
    case 2:     return !isInTheGap02(c);
    case 14:    return !isInTheGap0E(c);
    }
}

bool isUnicodePrivate(char16_t c)
{
    auto page = (c >> 8) & 0xFF;
    auto sheaf = page >> 4;
    page &= 0xF;

    switch (sheaf)
    {
    default:    return false;
    case 14:    return true;
    case 15:    return page < 9;
    }
}

bool isUnicodePrivate(char32_t c)
{
    switch(c >> 16)
    {
    default:    return false;
    case 0:     return isUnicodePrivate(static_cast<char16_t>(c & 0xFFFF));
    case 15:
    case 16:    return (c & 0xFFFF) <= 0xFFFD;
    }
}

bool isControlChar(char c)
{
    unsigned u = static_cast<unsigned char>(c);
    return u < 0x20 || (u >= 0x7f && u < 0xA0);
}

bool isControlChar(char16_t c)
{
    if ((c & ~0xFF) != 0)
    {
        return false;
    }
    return isControlChar(static_cast<char>(c & 0xFF));
}

bool isControlChar(char32_t c)
{
    if ((c & ~0xFF) != 0)
    {
        return false;
    }
    return isControlChar(static_cast<char>(c & 0xFF));
}

bool isWhitespaceChar(char c)
{
    switch (static_cast<unsigned char>(c))
    {
    default:    return false;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 32:
    case 133:
    case 160:   return true;
    }
}

bool isWhitespaceChar(char16_t c)
{
    if ((c & 0xFF) == c)
    {
        return isWhitespaceChar(static_cast<char>(c));
    }

    auto page = (c >> 8) & 0xFF;
    auto sheaf = page >> 4;
    page &= 0xF;
    auto byte = static_cast<unsigned char>(c);
    switch (sheaf)
    {
    default:    return false;
    case 1:     return page == 6 && byte == 0x80;
    case 2:
        if (page != 0)
        {
            return false;
        }
        else
        {
            return byte <= 10 ||
                   byte == 0x28 || byte == 0x29 || byte == 0x2f || byte == 0x5f;
        }
    case 3:     return page == 0 && byte == 0;
    }
}

bool isWhitespaceChar(char32_t c)
{
    if (c > 0x3000)
    {
        return false;
    }
    else
    {
        return isWhitespaceChar(static_cast<char16_t>(c));
    }
}

}

}
