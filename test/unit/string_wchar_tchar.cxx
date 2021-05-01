///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021, Arthur N. Klassen
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
// 2021.03.20 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_wchar_tchar.cxx - testing wchar_t (and for WIN32, TCHAR) adaptation.
//
///////////////////////////////////////////////////////////////////////////

#if defined(WIN32)
#if defined(_UNICODE)
#undef _UNICODE
#endif
#if !defined(_MBCS)
#define _MBCS
#endif
#endif

#include <gtest/gtest.h>

#include "string.hxx"
#if defined(WIN32)
#include <windows.h>
#endif

using namespace ansak;
using namespace std;
using namespace testing;

#if WCHAR_MAX < 0x10000

TEST(StringAdaptation, test16BitWcharIsUtf8)
{
    // string that can be UTF-16 (and hence convertible to utf8)
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUtf8(testGood));

    // string that cannot be UTF-16 -- illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isUtf8(testBadIllegalEscape));
}

TEST(StringAdaptation, test16BitWcharIsUtf16)
{
    // string that can be UTF-16
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUtf16(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isUtf16(testBadIllegalEscape));
}

TEST(StringAdaptation, test16BitWcharIsUcs2)
{
    // string that can be UCS-2
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUcs2(testGood));

    // string that cannot be UCS-2 -- out-of-range, or UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isUcs2(testBadIllegalEscape));
    wchar_t testBadGoodEscape[] = { 0xd801, 0xdc01, 0 };
    EXPECT_FALSE(isUcs2(testBadGoodEscape));
}

TEST(StringAdaptation, test16BitWcharIsUcs4)
{
    // string that can be UTF-16 (and hence convertible to ucs4)
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUcs4(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isUcs4(testBadIllegalEscape));
}

#if defined(WIN32)

TEST(StringAdaptation, testNarrowTcharIsUtf16)
{
    // utf8 string that can be UTF-16 (and hence convertible to utf8)
    char testGood[] = "This is some string, any string.";
    EXPECT_TRUE(isUtf16(testGood));

    // utf8 string that cannot be UTF-16 -- illegal UTF-16 escapes
    char utf8BadSoloUTF16_1[] = "Can't do \xed\xb8\xb2 alone.";
    EXPECT_FALSE(isUtf16(utf8BadSoloUTF16_1));
}

TEST(StringAdaptation, testNarrowTcharIsUcs2)
{
    // utf8 string that can be UCS-2
    char testGood[] = "This is some string, any string.";
    EXPECT_TRUE(isUcs2(testGood));

    // utf8 string that cannot be UCS-2 -- UTF-16 escapes
    char utf8PairedUcs2[] = "Can do \xed\xa8\xb2\xed\xb8\xb2 together.";
    EXPECT_FALSE(isUtf8(utf8PairedUcs2, kUcs2));
}

TEST(StringAdaptation, testNarrowTcharIsUcs4)
{
    // some UTF-8 string, any string
    char testGood[] = "This is some string, any string.";
    EXPECT_TRUE(isUcs4(testGood));

    // string that cannot be UTF-8
    char utf8BadC0_DF[] = "Bad C0-DF \xc0 will fail.";
    EXPECT_FALSE(isUcs4(utf8BadC0_DF));
}

TEST(StringAdaptation, test8BitIsTstring)
{
    // string that can be UTF-8 (and hence convertible to tstring)
    char testGood[] = "This is some string, any string.";
    EXPECT_TRUE(isTstring(testGood));

    // string that cannot be UTF-8 -- illegal UTF-16 escapes
    char bad[] = "Can do \xed\xa8\xb2\xed\xa8\xb2 together.";
    EXPECT_FALSE(isTstring(bad));
}

TEST(StringAdaptation, test16BitIsTstring)
{
    // string that can be UTF-16 (and hence convertible to tstring)
    char16_t testGood[] = u"This is some string, any string.";
    EXPECT_TRUE(isTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char16_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isTstring(testBadIllegalEscape));
}

TEST(StringAdaptation, test32BitIsTstring)
{
    // string that can be UTF-16 (and hence convertible to tstring)
    char32_t testGood[] = U"This is some string, any string.";
    EXPECT_TRUE(isTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char32_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isTstring(testBadIllegalEscape));
}

#endif

TEST(StringAdaptation, test16BitWcharToUtf8)
{
    // string that can be UTF-16 (and hence convertible to utf8)
    wchar_t testGood[] = L"This is some string, any string.";
    char targetGood[] = "This is some string, any string.";
    EXPECT_EQ(utf8String(targetGood), toUtf8(testGood));

    // string that cannot be UTF-16 -- illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toUtf8(testBadIllegalEscape).empty());
}

TEST(StringAdaptation, test16BitWcharToUtf16)
{
    // string that can be UTF-16
    wchar_t testGood[] = L"This is some string, any string.";
    char16_t targetGood[] = u"This is some string, any string.";
    EXPECT_EQ(utf16String(targetGood), toUtf16(testGood));

    // string that cannot be UTF-16 -- illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toUtf16(testBadIllegalEscape).empty());
}

TEST(StringAdaptation, test16BitWcharToUcs2)
{
    // UTF-16 string that can be UCS-2
    wchar_t testGood[] = L"This is some string, any string.";
    char16_t targetGood[] = u"This is some string, any string.";
    EXPECT_EQ(ucs2String(targetGood), toUcs2(testGood));

    // string that cannot be UCS-2 -- UTF-16 escapes
    wchar_t testBadGoodEscape[] = { 0xd801, 0xdc01, 0 };
    EXPECT_TRUE(toUcs2(testBadGoodEscape).empty());
}

TEST(StringAdaptation, test16BitWcharToUcs4)
{
    // some UTF-16 string, any string
    wchar_t testGood[] = L"This is some string, any string.";
    char32_t targetGood[] = U"This is some string, any string.";
    EXPECT_EQ(ucs4String(targetGood), toUcs4(testGood));

    // string that cannot be UTF-16
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toUcs4(testBadIllegalEscape).empty());
}

TEST(StringAdaptation, testUtf8ToWstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char testGood[] = "This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toWstring(testGood));

    // string that cannot be UTF-8 -- illegal UTF-16 escapes
    char bad[] = "Can do \xed\xa8\xb2\xed\xa8\xb2 together.";
    EXPECT_TRUE(toWstring(bad).empty());
}

TEST(StringAdaptation, testUtf16ToWstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char16_t testGood[] = u"This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toWstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char16_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toWstring(bad).empty());
}

TEST(StringAdaptation, testUcs4ToWstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char32_t testGood[] = U"This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toWstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char32_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toWstring(bad).empty());
}

#if defined(WIN32)

TEST(StringAdaptation, testNarrowTcharToUtf8)
{
    // string that can be UTF-8
    char testGood[] = "This is some string, any string.";
    EXPECT_EQ(utf8String(testGood), toUtf8(testGood));

    // string that cannot be UTF-8
    char utf8BadC0_DF[] = "Bad C0-DF \xc0 will fail.";
    EXPECT_TRUE(toUtf8(utf8BadC0_DF).empty());
}

TEST(StringAdaptation, testUtf8toNarrowTstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char testGood[] = "This is some string, any string.";
    EXPECT_EQ(string(testGood), toTstring(testGood));

    // string that cannot be UTF-8 -- illegal UTF-16 escapes
    char bad[] = "Can do \xed\xa8\xb2\xed\xa8\xb2 together.";
    EXPECT_TRUE(toTstring(bad).empty());
}

TEST(StringAdaptation, testUtf16toNarrowTstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char16_t testGood[] = u"This is some string, any string.";
    char targetGood[] = "This is some string, any string.";
    EXPECT_EQ(string(targetGood), toTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char16_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toTstring(bad).empty());
}

TEST(StringAdaptation, testUcs4toNarrowTstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char32_t testGood[] = U"This is some string, any string.";
    char targetGood[] = "This is some string, any string.";
    EXPECT_EQ(string(targetGood), toTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char32_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toTstring(bad).empty());
}

#endif

#endif

#if WCHAR_MAX > 0x10000

TEST(StringAdaptation, test32BitWcharIsUtf8)
{
    // string that can be UTF-16 (and hence convertible to utf8)
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUtf8(testGood));

    // string that cannot be UTF-16 -- illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isUtf8(testBadIllegalEscape));
}

TEST(StringAdaptation, test32BitWcharIsUtf16)
{
    // string that can be UTF-16
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUtf16(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    wchar_t testBadOutOfRange[] = { 0x200000, 0 };
    EXPECT_FALSE(isUtf16(testBadOutOfRange));
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isUtf16(testBadIllegalEscape));
}

TEST(StringAdaptation, test32BitWcharIsUcs2)
{
    // string that can be UCS-2
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUcs2(testGood));

    // string that cannot be UCS-2 -- out-of-range, or UTF-16 escapes
    wchar_t testBadOutOfRange[] = { 0x200000, 0 };
    EXPECT_FALSE(isUcs2(testBadOutOfRange));
    wchar_t testBadGoodEscape[] = { 0xd801, 0xdc01, 0 };
    EXPECT_FALSE(isUcs2(testBadGoodEscape));
}

TEST(StringAdaptation, test32BitWcharIsUcs4)
{
    // string that can be UTF-16 (and hence convertible to ucs4)
    wchar_t testGood[] = L"This is some string, any string.";
    EXPECT_TRUE(isUcs4(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isUcs4(testBadIllegalEscape));
}

TEST(StringAdaptation, test32BitWcharToUtf8)
{
    // string that can be UTF-16 (and hence convertible to utf8)
    wchar_t testGood[] = L"This is some string, any string.";
    char targetGood[] = "This is some string, any string.";
    EXPECT_EQ(utf8String(targetGood), toUtf8(testGood));

    // string that cannot be UTF-16 -- illegal UTF-16 escapes
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toUtf8(testBadIllegalEscape).empty());
}

TEST(StringAdaptation, test32BitWcharToUtf16)
{
    // string that can be UTF-16
    wchar_t testGood[] = L"This is some string, any string.";
    char16_t targetGood[] = u"This is some string, any string.";
    EXPECT_EQ(utf16String(targetGood), toUtf16(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    wchar_t testBadOutOfRange[] = { 0x200000, 0 };
    EXPECT_TRUE(toUtf16(testBadOutOfRange).empty());
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toUtf16(testBadIllegalEscape).empty());
}

TEST(StringAdaptation, test32BitWcharToUcs2)
{
    // string that can be UCS-2
    wchar_t testGood[] = L"This is some string, any string.";
    char16_t targetGood[] = u"This is some string, any string.";
    EXPECT_EQ(ucs2String(targetGood), toUcs2(testGood));

    // string that cannot be UCS-2 -- out-of-range, or UTF-16 escapes
    wchar_t testBadOutOfRange[] = { 0x200000, 0 };
    EXPECT_TRUE(toUcs2(testBadOutOfRange).empty());
    wchar_t testBadGoodEscape[] = { 0xd801, 0xdc01, 0 };
    EXPECT_TRUE(toUcs2(testBadGoodEscape).empty());
}

TEST(StringAdaptation, test32BitWcharToUcs4)
{
    // some UTF-16 string, any string
    wchar_t testGood[] = L"This is some string, any string.";
    char32_t targetGood[] = U"This is some string, any string.";
    EXPECT_EQ(ucs4String(targetGood), toUcs4(testGood));

    // string that cannot be UTF-16
    wchar_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toUcs4(testBadIllegalEscape).empty());
}

TEST(StringAdaptation, test8BitTo32bitWString)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char testGood[] = "This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toWstring(testGood));

    // string that cannot be UTF-8 -- illegal UTF-16 escapes
    char bad[] = "Can do \xed\xa8\xb2\xed\xa8\xb2 together.";
    EXPECT_TRUE(toWstring(bad).empty());
}

TEST(StringAdaptation, test16BitTo32bitWString)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char16_t testGood[] = u"This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toWstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char16_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toWstring(bad).empty());
}

TEST(StringAdaptation, test32BitTo32bitWString)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char32_t testGood[] = U"This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toWstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char32_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toWstring(bad).empty());
}

#endif

TEST(StringAdaptation, test8BitIsWstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char testGood[] = "This is some string, any string.";
    EXPECT_TRUE(isWstring(testGood));

    // string that cannot be UTF-8 -- illegal UTF-16 escapes
    char bad[] = "Can do \xed\xa8\xb2\xed\xa8\xb2 together.";
    EXPECT_FALSE(isWstring(bad));
}

TEST(StringAdaptation, test16BitIsWstring)
{
    // string that can be UTF-16 (and hence convertible to wstring)
    char16_t testGood[] = u"This is some string, any string.";
    EXPECT_TRUE(isWstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char16_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isWstring(testBadIllegalEscape));
}

TEST(StringAdaptation, test32BitIsWstring)
{
    // string that can be UTF-16 (and hence convertible to wstring)
    char32_t testGood[] = U"This is some string, any string.";
    EXPECT_TRUE(isWstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char32_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isWstring(testBadIllegalEscape));
}
