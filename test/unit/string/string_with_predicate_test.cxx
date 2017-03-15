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
// 2015.11.30 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_with_predicate_test.cxx - some tests of strings with encoding
//                                  predicates.
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <string.hxx>

using namespace ansak;
using namespace std;
using namespace testing;

TEST(StringWithPredicateTest, testIsUtf8WithPredicates)
{
    utf8String theString("The quick brown fox jumps over the lazy dog.");
    EXPECT_TRUE(isUtf8(theString, validIf(kIsAssigned)));
    EXPECT_TRUE(isUtf8(theString, kAscii, validIf(kIsAssigned)));
    EXPECT_FALSE(isUtf8(theString, validIf(kIsPrivate)));
    EXPECT_FALSE(isUtf8(theString, validIf(kIsControl)));
    EXPECT_FALSE(isUtf8(theString, validIf(kIsWhitespace)));
    EXPECT_FALSE(isUtf8(theString, validIfNot(kIsWhitespace)));
}

TEST(StringWithPredicateTest, testIsUcs2WithPredicates)
{
    ucs2String theString(toUcs2("The quick brown fox jumps over the lazy dog."));
    EXPECT_TRUE(isUcs2(theString, validIf(kIsAssigned)));
    EXPECT_TRUE(isUcs2(theString, kAscii, validIf(kIsAssigned)));
    EXPECT_FALSE(isUcs2(theString, validIf(kIsPrivate)));
    EXPECT_FALSE(isUcs2(theString, validIf(kIsControl)));
    EXPECT_FALSE(isUcs2(theString, validIf(kIsWhitespace)));
    EXPECT_FALSE(isUcs2(theString, validIfNot(kIsWhitespace)));
}

TEST(StringWithPredicateTest, testIsUtf16WithPredicates)
{
    utf16String theString(toUtf16("The quick brown fox jumps over the lazy dog."));
    EXPECT_TRUE(isUtf16(theString, validIf(kIsAssigned)));
    EXPECT_TRUE(isUtf16(theString, kAscii, validIf(kIsAssigned)));
    EXPECT_FALSE(isUtf16(theString, validIf(kIsPrivate)));
    EXPECT_FALSE(isUtf16(theString, validIf(kIsControl)));
    EXPECT_FALSE(isUtf16(theString, validIf(kIsWhitespace)));
    EXPECT_FALSE(isUtf16(theString, validIfNot(kIsWhitespace)));
}

TEST(StringWithPredicateTest, testIsUcs4WithPredicates)
{
    ucs4String theString(toUcs4("The quick brown fox jumps over the lazy dog."));
    EXPECT_TRUE(isUcs4(theString, validIf(kIsAssigned)));
    EXPECT_TRUE(isUcs4(theString, kAscii, validIf(kIsAssigned)));
    EXPECT_FALSE(isUcs4(theString, validIf(kIsPrivate)));
    EXPECT_FALSE(isUcs4(theString, validIf(kIsControl)));
    EXPECT_FALSE(isUcs4(theString, validIf(kIsWhitespace)));
    EXPECT_FALSE(isUcs4(theString, validIfNot(kIsWhitespace)));
}

