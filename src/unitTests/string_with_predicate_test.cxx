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

#include <cppunit/extensions/HelperMacros.h>

#include "string.hxx"

using namespace ansak;
using namespace std;

class StringWithPredicateTestFixture : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( StringWithPredicateTestFixture );
    CPPUNIT_TEST( testIsUtf8WithPredicates );
    CPPUNIT_TEST( testIsUcs2WithPredicates );
    CPPUNIT_TEST( testIsUtf16WithPredicates );
    CPPUNIT_TEST( testIsUcs4WithPredicates );
CPPUNIT_TEST_SUITE_END();

public:
    void testIsUtf8WithPredicates();
    void testIsUcs2WithPredicates();
    void testIsUtf16WithPredicates();
    void testIsUcs4WithPredicates();
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringWithPredicateTestFixture);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(StringWithPredicateTestFixture, "StringClassTests");

void StringWithPredicateTestFixture::testIsUtf8WithPredicates()
{
    utf8String theString("The quick brown fox jumps over the lazy dog.");
    CPPUNIT_ASSERT(isUtf8(theString, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(isUtf8(theString, kAscii, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(!isUtf8(theString, validIf(kIsPrivate)));
    CPPUNIT_ASSERT(!isUtf8(theString, validIf(kIsControl)));
    CPPUNIT_ASSERT(!isUtf8(theString, validIf(kIsWhitespace)));
    CPPUNIT_ASSERT(!isUtf8(theString, validIfNot(kIsWhitespace)));
}

void StringWithPredicateTestFixture::testIsUcs2WithPredicates()
{
    ucs2String theString(toUcs2("The quick brown fox jumps over the lazy dog."));
    CPPUNIT_ASSERT(isUcs2(theString, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(isUcs2(theString, kAscii, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(!isUcs2(theString, validIf(kIsPrivate)));
    CPPUNIT_ASSERT(!isUcs2(theString, validIf(kIsControl)));
    CPPUNIT_ASSERT(!isUcs2(theString, validIf(kIsWhitespace)));
    CPPUNIT_ASSERT(!isUcs2(theString, validIfNot(kIsWhitespace)));
}

void StringWithPredicateTestFixture::testIsUtf16WithPredicates()
{
    utf16String theString(toUtf16("The quick brown fox jumps over the lazy dog."));
    CPPUNIT_ASSERT(isUtf16(theString, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(isUtf16(theString, kAscii, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(!isUtf16(theString, validIf(kIsPrivate)));
    CPPUNIT_ASSERT(!isUtf16(theString, validIf(kIsControl)));
    CPPUNIT_ASSERT(!isUtf16(theString, validIf(kIsWhitespace)));
    CPPUNIT_ASSERT(!isUtf16(theString, validIfNot(kIsWhitespace)));
}

void StringWithPredicateTestFixture::testIsUcs4WithPredicates()
{
    ucs4String theString(toUcs4("The quick brown fox jumps over the lazy dog."));
    CPPUNIT_ASSERT(isUcs4(theString, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(isUcs4(theString, kAscii, validIf(kIsAssigned)));
    CPPUNIT_ASSERT(!isUcs4(theString, validIf(kIsPrivate)));
    CPPUNIT_ASSERT(!isUcs4(theString, validIf(kIsControl)));
    CPPUNIT_ASSERT(!isUcs4(theString, validIf(kIsWhitespace)));
    CPPUNIT_ASSERT(!isUcs4(theString, validIfNot(kIsWhitespace)));
}

