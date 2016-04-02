///////////////////////////////////////////////////////////////////////////
//
// 2015.11.30
//
// The author (Arthur N. Klassen) disclaims all copyright to this source code.
// In place of a legal notice, here is a blessing:
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
// (Is it too much to suggest retaining this header on this file?)
///////////////////////////////////////////////////////////////////////////
//
// string_with_predicate_test.cxx - some tests of strings with encoding
//                                  predicates.
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "string.hxx"

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

