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
// 2015.10.02 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_tolower.cxx -- Lower-case any UCS-4 value, one char, char16_t or
//                       char32_t at a time.
//
///////////////////////////////////////////////////////////////////////////

#include <string.hxx>
#include "string_internal.hxx"

using namespace std;

namespace ansak {

namespace internal {

// The Simple algorithm for characters: 0x41..0x5A --> 0x61..0x7A
//                                      0xC0..0xD7 --> 0xE0..0xF7
//                                      0xD9..0xDE --> 0xF9..0xFE
char toLowerC(char c)
{
    uint8_t uc = static_cast<uint8_t>(c);
    if (uc < 0x41 || uc > 0xDE || (uc > 0x5A && uc < 0xC0))
    {
        return c;
    }

    return static_cast<char>(uc + 0x20);
}

namespace {

// Various Script Systems, BMP, 0100..01FF
char16_t toLower0100(char16_t c)
{
    if (c == 0x130) // dotted I, turkic or not...
    {
        return 0x69;
    }
    if ((c < 0x138) || 
        (c >= 0x14A && c <= 0x177) ||
        (c >= 0x1A0 && c <= 0x1A5) ||
        (c >= 0x1DE && c <= 0x1EF) ||
        (c >= 0x1F8))
    {
        return c | 1;
    }
    if ((c >= 0x139 && c <= 0x148) ||
        (c >= 0x179 && c <= 0x17E) ||
        (c >= 0x1B3 && c <= 0x1B6) ||
        (c >= 0x1CF && c <= 0x1DC))
    {
        return (c + 1) & ~1;
    }
    switch (c)
    {
        default:        return c;
        case 0x178: return 0x0FF;
        case 0x181: return 0x253;
        case 0x182: return 0x183;
        case 0x184: return 0x185;
        case 0x186: return 0x254;
        case 0x187: return 0x188;
        case 0x189: return 0x256;
        case 0x18A: return 0x257;
        case 0x18B: return 0x18C;
        case 0x18E: return 0x1DD;
        case 0x18F: return 0x259;
        case 0x190: return 0x25B;
        case 0x191: return 0x192;
        case 0x193: return 0x260;
        case 0x194: return 0x263;
        case 0x196: return 0x269;
        case 0x197: return 0x268;
        case 0x198: return 0x199;
        case 0x19C: return 0x26F;
        case 0x19D: return 0x272;
        case 0x19F: return 0x275;
        case 0x1A6: return 0x280;
        case 0x1A7: return 0x1A8;
        case 0x1A9: return 0x283;
        case 0x1AC: return 0x1AD;
        case 0x1AE: return 0x288;
        case 0x1AF: return 0x1B0;
        case 0x1B1: return 0x28A;
        case 0x1B2: return 0x28B;
        case 0x1B7: return 0x292;
        case 0x1B8: return 0x1B9;
        case 0x1BC: return 0x1BD;
        case 0x1C4:
        case 0x1C5: return 0x1C6;
        case 0x1C7:
        case 0x1C8: return 0x1C9;
        case 0x1CA:
        case 0x1CB: return 0x1CC;
        case 0x1CD: return 0x1CE;
        case 0x1F1:
        case 0x1F2: return 0x1F3;
        case 0x1F4: return 0x1F5;
        case 0x1F6: return 0x195;
        case 0x1F7: return 0x1BF;
    }
}

// Various Script Systems, BMP, 0200..02FF
char16_t toLower0200(char16_t c)
{
    if ((c < 0x220) ||
        (c >= 0x222 && c <= 0x233) ||
        (c >= 0x246 && c <= 0x24F))
    {
        return c | 1;
    }
    switch (c)
    {
        default:    return c;
        case 0x220: return 0x019E;
        case 0x23A: return 0x2C65;
        case 0x23B: return 0x023C;
        case 0x23D: return 0x019A;
        case 0x23E: return 0x2C66;
        case 0x241: return 0x0242;
        case 0x243: return 0x0180;
        case 0x244: return 0x0289;
        case 0x245: return 0x028C;
    }
}

// Various Script Systems, BMP, 0300..03FF; Greek et al.
char16_t toLower0300(char16_t c)
{
    if ((c < 0x370) || ((c & 0xF0) == 0xB0))
    {
        return c;
    }
    if ((c >= 0x370 && c <= 0x373) ||
        (c >= 0x376 && c <= 0x377) ||
        (c >= 0x3D8 && c <= 0x3EF))
    {
        return c | 1;
    }
    if ((c >= 0x391 && c <= 0x3A1) ||
        (c >= 0x3A3 && c <= 0x3AB))
    {
        return c + 0x20;
    }
    switch (c)
    {
        default:    return c;
        case 0x37F: return 0x3F3;
        case 0x386: return 0x3AC;
        case 0x388: return 0x3AD;
        case 0x389: return 0x3AE;
        case 0x38A: return 0x3AF;
        case 0x38C: return 0x3CC;
        case 0x38E: return 0x3CD;
        case 0x38F: return 0x3CE;
        case 0x3CF: return 0x3D7;
        case 0x3F4: return 0x3B8;
        case 0x3F7: return 0x3F8;
        case 0x3F9: return 0x3F2;
        case 0x3FA: return 0x3FB;
        case 0x3FD: return 0x37B;
        case 0x3FE: return 0x37C;
        case 0x3FF: return 0x37D;
    }
}

// Various Script Systems, BMP, 0400..04FF
char16_t toLower0400(char16_t c)
{
    if (c < 0x410)
    {
        return c + 0x50;
    }
    else if (c < 0x430)
    {
        return c + 0x20;
    }
    else if ((c >= 0x460 && c <= 0x481) ||
             (c >= 0x48A && c <= 0x4BF) || c >= 0x4D0)
    {
        return c | 1;
    }
    else if (c == 0x4C0)
    {
        return 0x4CF;
    }
    else if (c >= 0x4C1 && c <= 0x4CE)
    {
        return (c + 1) & ~1;
    }

    return c;
}

// Various Script Systems, BMP, 0500..05FF
char16_t toLower0500(char16_t c)
{
    if (c < 0x530)
    {
        return c | 1;
    }
    else if (c >= 0x531 && c <= 0x556)
    {
        return c + 0x30;
    }

    return c;
}

// Various Script Systems, BMP, 1000..1FFF
char16_t toLower1000(char16_t c)
{
    switch (c >> 8)
    {
    default:        break;
    case 0x10:
        if ((c >= 0x10A0 && c <= 0x10C5) ||
            c == 0x10c7 ||
            c == 0x10cd)
        {
            return c + 0x1C60;
        }
        break;

    case 0x13:
        if (c >= 0x13A0 && c < 0x13F0)
        {
            // Cherokee!
            return c + 0x97d0;
        }
        else if (c >= 0x13F0 && c <= 0x13F5)
        {
            return c | 8;
        }
        break;

    case 0x1E:
        if (c == 0x1E9E)
        {
            return 0xDF;
        }
        else
        {
            return c | 1;
        }
        break;

    case 0x1F:
        if (c < 0x1F70 || (c >= 0x1F80 && c < 0x1FB0))
        {
            switch(c)
            {
            default:
                return c & ~8;

            case 0x1f1e:
            case 0x1f1f:
            case 0x1f4e:
            case 0x1f4f:
            case 0x1f58:
            case 0x1f5a:
            case 0x1f5c:
            case 0x1f5e:
                return c;
            }
        }
        else
        {
            switch (c)
            {
                default:        return c;

                case 0x1FB8:    return 0x1FB0;
                case 0x1FB9:    return 0x1FB1;
                case 0x1FBA:    return 0x1F70;
                case 0x1FBB:    return 0x1F71;
                case 0x1FBC:    return 0x1FB3;
                case 0x1FC8:    return 0x1F72;
                case 0x1FC9:    return 0x1F73;
                case 0x1FCA:    return 0x1F74;
                case 0x1FCB:    return 0x1F75;
                case 0x1FCC:    return 0x1FC3;
                case 0x1FD8:    return 0x1FD0;
                case 0x1FD9:    return 0x1FD1;
                case 0x1FDA:    return 0x1F76;
                case 0x1FDB:    return 0x1F77;
                case 0x1FE8:    return 0x1FE0;
                case 0x1FE9:    return 0x1FE1;
                case 0x1FEA:    return 0x1F7A;
                case 0x1FEB:    return 0x1F7B;
                case 0x1FEC:    return 0x1FE5;
                case 0x1FF8:    return 0x1F78;
                case 0x1FF9:    return 0x1F79;
                case 0x1FFA:    return 0x1F7C;
                case 0x1FFB:    return 0x1F7D;
                case 0x1FFC:    return 0x1FF3;
            }
        }
        break;
    }

    return c;
}

// Various Script Systems, BMP, 2000..2FFF
char16_t toLower2000(char16_t c)
{
    switch (c)
    {
        default:
            if ((c & 0xFF00) == 0x2C00)
            {
                if (c < 0x2C2F)
                {
                    return c + 0x30;
                }
                else if ((c >= 0x2C80 && c <= 0x2CE3) ||
                         c == 0x2cF2)
                {
                    return c | 1;
                }
                else if ((c >= 0x2C67 && c <= 0x2C6C) ||
                         (c >= 0x2CEB && c <= 0x2CEE))
                {
                    return (c + 1) & ~1;
                }
                switch (c)
                {
                default:        break;
                case 0x2C60:
                case 0x2C72:    return c | 1;
                case 0x2C75:    return c + 1;
                case 0x2C62:    return 0x026B;
                case 0x2C63:    return 0x1D7D;
                case 0x2C64:    return 0x027D;
                case 0x2C6D:    return 0x0251;
                case 0x2C6E:    return 0x0271;
                case 0x2C6F:    return 0x0250;
                case 0x2C70:    return 0x0252;
                case 0x2C7E:    return 0x023F;
                case 0x2C7F:    return 0x0240;
                }
            }
            else if ((c & 0xFFF0) == 0x2160)
            {
                return c | 0x10;
            }
            else if (c >= 0x24B6 && c < 0x24d0)
            {
                return c + 26;
            }
            break;

        case 0x2126:        return 0x3C9;       // Ohm to small omega
        case 0x212A:        return 0x6B;        // Kelvin to small k
        case 0x212B:        return 0xE5;        // Angstrom to small a-dot
        case 0x2132:        return 0x214E;
        case 0x2183:        return 0x2184;
    }

    return c;
}

// Various Script Systems, BMP, A000..AFFF
char16_t toLowerA000(char16_t c)
{
    switch ((c & 0x0F00) >> 8)
    {
    default:        break;
    case 6:
        if ((c >= 0xA640 && c <= 0xA66D) ||
            (c >= 0xA680 && c <= 0xA69B))
        {
            return c | 1;
        }
        break;

    case 7:
        if ((c >= 0xA722 && c <= 0xA72F) ||
            (c >= 0xA732 && c <= 0xA76F) ||
            (c >= 0xA77E && c <= 0xA787) ||
            (c >= 0xA790 && c <= 0xA793) ||
            (c >= 0xA796 && c <= 0xA7A9) ||
            (c >= 0xA7B4 && c <= 0xA7B7))
        {
            return c | 1;
        }
        else if (c >= 0xA779 && c <= 0xA77C)
        {
            return (c + 1) & ~1;
        }
        else
        {
            switch (c)
            {
                default:        break;
                case 0xA77D:    return 0x1D79;
                case 0xA78B:    return 0xA78C;
                case 0xA78D:    return 0x0265;
                case 0xA7AA:    return 0x0266;
                case 0xA7AB:    return 0x025C;
                case 0xA7AC:    return 0x0261;
                case 0xA7AD:    return 0x026C;
                case 0xA7AE:    return 0x026A;
                case 0xA7B0:    return 0x029E;
                case 0xA7B1:    return 0x0287;
                case 0xA7B2:    return 0x029D;
                case 0xA7B3:    return 0xAB53;
            }
        }
        break;
    }

    return c;
}

// Half-width/Full-width Latin letters
char16_t toLowerF000(char16_t c)
{
    switch ((c & 0x0F00) >> 8)
    {
        default:    break;
        case 15:
            if (c > 0xFF20 && c <= 0xFF3A)
            {
                return c + 0x20;
            }
            break;
    }

    return c;
}

// The Dispatch algorithm for UCS-2: ignore all but 00xx,
// 01xx, 02xx, 03xx, 04xx, 05xx, 1xxx, 2xxx and Axxx
char16_t toLower2(char16_t c)
{
    switch (c >> 8)
    {
    case 0:     return static_cast<char16_t>(
                           static_cast<unsigned char>(
                               toLowerC(static_cast<char>(c))));
    default:    break;

    // 0x0600-0x0FFF have no lowercasing
    case 15:
    case 14:
    case 13:
    case 12:
    case 11:
    case 10:
    case 9:
    case 8:
    case 7:
    case 6:     return c;

    case 5:     return toLower0500(c);
    case 4:     return toLower0400(c);
    case 3:     return toLower0300(c);
    case 2:     return toLower0200(c);
    case 1:     return toLower0100(c);
    }

    switch (c >> 12)
    {
    default:    return c;

    case 1:     return toLower1000(c);
    case 2:     return toLower2000(c);
    case 10:    return toLowerA000(c);
    case 15:    return toLowerF000(c);
    }
}

}

// The Dispatch algorithm for UCS-4: delegate BMP, ignore all but
// some portions of #1 MP
char32_t toLower(char32_t c)
{
    if (c < 0x10000)
    {
        return static_cast<char32_t>(toLower2(static_cast<char16_t>(c)));
    }
    else if (c >= 0x10400 && c <= 0x118BF)
    {
        if (c < 0x10428)
        {
            return c + 0x28;
        }
        else if (c >= 0x104B0 && c <= 0x104D3)
        {
            return c + 0x28;
        }
        else if (c >= 0X10C80 && c <= 0x10CB2)
        {
            return c | 0x40;
        }
        else if (c >= 0x118A0 && c < 0x118C0)
        {
            return c + 0x20;
        }
    }
    else if (c >= 0x1E900 && c <= 0x1E921)
    {
        return c + 0x22;
    }

    return c;
}

// Turkic lowercasing for UCS-4
char32_t turkicToLower(char32_t c)
{
    switch (c)
    {
        default:        return toLower(c);
        case 0x130:     return 0x69;
        case 0x49:      return 0x131;
    }
}

}

}


