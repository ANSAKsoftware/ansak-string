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
// 2021.04.30 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_wide_tchar.cxx - testing wchar_t (and for WIN32, TCHAR) adaptation.
//
///////////////////////////////////////////////////////////////////////////

#if defined(WIN32)

#if defined(_MBCS)
#undef _MBCS
#endif
#if !defined(_UNICODE)
#define _UNICODE
#endif

#include <gtest/gtest.h>

#include "string.hxx"
#include <windows.h>
#include <tchar.h>

using namespace ansak;
using namespace std;
using namespace testing;

TEST(WideTStringAdaptation, test8bitIsTstring)
{
    // string that can be UTF-8 (and hence convertible to tstring)
    char testGood[] = "This is some string, any string.";
    EXPECT_TRUE(isTstring(testGood));

    // string that cannot be UTF-8 -- illegal UTF-16 escapes
    char bad[] = "Can do \xed\xa8\xb2\xed\xa8\xb2 together.";
    EXPECT_FALSE(isTstring(bad));
}

TEST(WideTStringAdaptation, test16bitIsTstring)
{
    // string that can be UTF-16 (and hence convertible to tstring)
    char16_t testGood[] = u"This is some string, any string.";
    EXPECT_TRUE(isTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char16_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isTstring(testBadIllegalEscape));
}

TEST(WideTStringAdaptation, test32bitIsTstring)
{
    // string that can be UTF-16 (and hence convertible to tstring)
    char32_t testGood[] = U"This is some string, any string.";
    EXPECT_TRUE(isTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char32_t testBadIllegalEscape[] = { 0xd801, 0xd801, 0 };
    EXPECT_FALSE(isTstring(testBadIllegalEscape));
}

TEST(WideTStringAdaptation, testUtf8toTstring)
{
    // string that can be UTF-8 (and hence convertible to tstring)
    char testGood[] = "This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toTstring(testGood));

    // string that cannot be UTF-8 -- illegal UTF-16 escapes
    char bad[] = "Can do \xed\xa8\xb2\xed\xa8\xb2 together.";
    EXPECT_TRUE(toTstring(bad).empty());
}

TEST(WideTStringAdaptation, testUtf16toTstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char16_t testGood[] = u"This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char16_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toWstring(bad).empty());
}

TEST(WideTStringAdaptation, testUcs4toTstring)
{
    // string that can be UTF-8 (and hence convertible to wstring)
    char32_t testGood[] = U"This is some string, any string.";
    wchar_t targetGood[] = L"This is some string, any string.";
    EXPECT_EQ(wstring(targetGood), toTstring(testGood));

    // string that cannot be UTF-16 -- out-of-range, or illegal UTF-16 escapes
    char32_t bad[] = { 0xd801, 0xd801, 0 };
    EXPECT_TRUE(toTstring(bad).empty());
}

#endif
