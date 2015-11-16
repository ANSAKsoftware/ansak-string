///////////////////////////////////////////////////////////////////////////
//
// 2014.10.08
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
// string_test.cxx -- CPPUNIT-compatible tests to exercise string.hxx/.cxx
//                    If you find new conditions that I missed, please let
//                    me make my version of this better, too.
//
///////////////////////////////////////////////////////////////////////////

#include <cppunit/extensions/HelperMacros.h>

#include "string.hxx"
#include "bits/string_internal.hxx"
#include <string.h>

using namespace ansak;
using namespace ansak::internal;
using namespace std;

class StringTestFixture : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( StringTestFixture );
    CPPUNIT_TEST( testGetUnicodeEncodableFlag );
    CPPUNIT_TEST( testGetCharEncodableFlags );
    CPPUNIT_TEST( testGetRangeFlag );
    CPPUNIT_TEST( testDecodeUtf8 );
    CPPUNIT_TEST( testEncodeUtf8 );
    CPPUNIT_TEST( testRawDecodeUtf16 );
    CPPUNIT_TEST( testRawEncodeUtf16 );
    CPPUNIT_TEST( testIsUtf8AsAscii );
    CPPUNIT_TEST( testIsUtf8AsUtf8 );
    CPPUNIT_TEST( testIsUtf8AsUcs2 );
    CPPUNIT_TEST( testIsUtf8AsUtf16 );
    CPPUNIT_TEST( testIsUtf8AsUcs4 );
    CPPUNIT_TEST( testIsUtf8AsUnicode );
    CPPUNIT_TEST( testIsUtf8LengthTerminated );
    CPPUNIT_TEST( testIsUcs2AsAscii );
    CPPUNIT_TEST( testIsUcs2AsUtf8 );
    CPPUNIT_TEST( testIsUcs2AsUcs2 );
    CPPUNIT_TEST( testIsUcs2AsUtf16 );
    CPPUNIT_TEST( testIsUcs2AsUcs4 );
    CPPUNIT_TEST( testIsUcs2AsUnicode );
    CPPUNIT_TEST( testIsUtf16AsAscii );
    CPPUNIT_TEST( testIsUtf16AsUtf8 );
    CPPUNIT_TEST( testIsUtf16AsUcs2 );
    CPPUNIT_TEST( testIsUtf16AsUtf16 );
    CPPUNIT_TEST( testIsUtf16AsUcs4 );
    CPPUNIT_TEST( testIsUtf16AsUnicode );
    CPPUNIT_TEST( testIsUcs4AsAscii );
    CPPUNIT_TEST( testIsUcs4AsUtf8 );
    CPPUNIT_TEST( testIsUcs4AsUcs2 );
    CPPUNIT_TEST( testIsUcs4AsUtf16 );
    CPPUNIT_TEST( testIsUcs4AsUcs4 );
    CPPUNIT_TEST( testIsUcs4AsUnicode );
    CPPUNIT_TEST( testToUtf8From16Bit );
    CPPUNIT_TEST( testToUtf8From32Bit );
    CPPUNIT_TEST( testToUcs2From8Bit );
    CPPUNIT_TEST( testToUcs2From32Bit );
    CPPUNIT_TEST( testToUtf16From8Bit );
    CPPUNIT_TEST( testToUtf16From32Bit );
    CPPUNIT_TEST( testToUcs4From8Bit );
    CPPUNIT_TEST( testToUcs4From16Bit );
    CPPUNIT_TEST( testGoodUtf8ToBadValues );
    CPPUNIT_TEST( testUnicodeLengthUtf8 );
    CPPUNIT_TEST( testUnicodeLengthUtf16 );
    CPPUNIT_TEST( testUnicodeLengthUcs4 );
    CPPUNIT_TEST( testConvertFromCP1252 );
    CPPUNIT_TEST( testConvertFromCP1250 );
CPPUNIT_TEST_SUITE_END();

public:
    void testGetUnicodeEncodableFlag();
    void testGetCharEncodableFlags();
    void testGetRangeFlag();
    void testDecodeUtf8();
    void testEncodeUtf8();
    void testRawDecodeUtf16();
    void testRawEncodeUtf16();

    void testIsUtf8AsAscii();
    void testIsUtf8AsUtf8();
    void testIsUtf8AsUcs2();
    void testIsUtf8AsUtf16();
    void testIsUtf8AsUcs4();
    void testIsUtf8AsUnicode();

    void testIsUtf8LengthTerminated();

    void testIsUcs2AsAscii();
    void testIsUcs2AsUtf8();
    void testIsUcs2AsUcs2();
    void testIsUcs2AsUtf16();
    void testIsUcs2AsUcs4();
    void testIsUcs2AsUnicode();

    void testIsUtf16AsAscii();
    void testIsUtf16AsUtf8();
    void testIsUtf16AsUcs2();
    void testIsUtf16AsUtf16();
    void testIsUtf16AsUcs4();
    void testIsUtf16AsUnicode();

    void testIsUcs4AsAscii();
    void testIsUcs4AsUtf8();
    void testIsUcs4AsUcs2();
    void testIsUcs4AsUtf16();
    void testIsUcs4AsUcs4();
    void testIsUcs4AsUnicode();

    void testToUtf8From16Bit();
    void testToUtf8From32Bit();
    void testToUcs2From8Bit();
    void testToUcs2From32Bit();
    void testToUtf16From8Bit();
    void testToUtf16From32Bit();
    void testToUcs4From8Bit();
    void testToUcs4From16Bit();

    void testGoodUtf8ToBadValues();

    void testUnicodeLengthUtf8();
    void testUnicodeLengthUtf16();
    void testUnicodeLengthUcs4();

    void testConvertFromCP1252();
    void testConvertFromCP1250();
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringTestFixture);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(StringTestFixture, "StringClassTests");

void StringTestFixture::testGetUnicodeEncodableFlag()
{
    char32_t c;
    for (c = 1; c < 0x110000; ++c)
    {
        if (c >= 0xd800 && c < 0xe000)
        {
            CPPUNIT_ASSERT_EQUAL(kNoFlag, getUnicodeEncodableRangeFlag(c));
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<int>(kUnicodeFlag),
                                 static_cast<int>(kUnicodeFlag & getUnicodeEncodableRangeFlag(c)));
        }
    }
    CPPUNIT_ASSERT_EQUAL(kNoFlag, getUnicodeEncodableRangeFlag(c));
}

void StringTestFixture::testGetCharEncodableFlags()
{
    char c;
    for (c = 0; c != '\x80'; ++c)
    {
        CPPUNIT_ASSERT_EQUAL(k7BitFlags, getCharEncodableRangeFlags(c));
    }
    for ( ; c != 0; ++c)
    {
        CPPUNIT_ASSERT_EQUAL(k16BitFlags, getCharEncodableRangeFlags(c));
    }

    char16_t d;
    for (d = 0; d <= 0x7f; ++d)
    {
        CPPUNIT_ASSERT_EQUAL(k7BitFlags, getCharEncodableRangeFlags(d));
    }
    for ( ; d < 0xd800; ++d)
    {
        CPPUNIT_ASSERT_EQUAL(k16BitFlags, getCharEncodableRangeFlags(d));
    }
    for ( ; d < 0xe000; ++d)
    {
        CPPUNIT_ASSERT_EQUAL(kInvalidRangeFlag, getCharEncodableRangeFlags(d));
    }
    for ( ; d != 0; ++d)
    {
        CPPUNIT_ASSERT_EQUAL(k16BitFlags, getCharEncodableRangeFlags(d));
    }

    char32_t e = 1;
    CPPUNIT_ASSERT_EQUAL(k7BitFlags, getCharEncodableRangeFlags(e));
    e = 0x7f;
    CPPUNIT_ASSERT_EQUAL(k7BitFlags, getCharEncodableRangeFlags(e));
    ++e;
    CPPUNIT_ASSERT_EQUAL(k16BitFlags, getCharEncodableRangeFlags(e));
    e = 0xd7ff;
    CPPUNIT_ASSERT_EQUAL(k16BitFlags, getCharEncodableRangeFlags(e));
    ++e;
    CPPUNIT_ASSERT_EQUAL(kInvalidRangeFlag, getCharEncodableRangeFlags(e));
    e = 0xdbff;
    CPPUNIT_ASSERT_EQUAL(kInvalidRangeFlag, getCharEncodableRangeFlags(e));
    ++e;
    CPPUNIT_ASSERT_EQUAL(kInvalidRangeFlag, getCharEncodableRangeFlags(e));
    e = 0xdfff;
    CPPUNIT_ASSERT_EQUAL(kInvalidRangeFlag, getCharEncodableRangeFlags(e));
    ++e;
    CPPUNIT_ASSERT_EQUAL(k16BitFlags, getCharEncodableRangeFlags(e));
    e = 0xffff;
    CPPUNIT_ASSERT_EQUAL(k16BitFlags, getCharEncodableRangeFlags(e));
    ++e;
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(k21BitUnicodeFlags),
                         static_cast<int>(k21BitUnicodeFlags & getCharEncodableRangeFlags(e)));
    e = 0x10ffff;
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(k21BitUnicodeFlags),
                         static_cast<int>(k21BitUnicodeFlags & getCharEncodableRangeFlags(e)));
    ++e;
    CPPUNIT_ASSERT_EQUAL(k21BitFlags, getCharEncodableRangeFlags(e));
    e = 0x1fffff;
    CPPUNIT_ASSERT_EQUAL(k21BitFlags, getCharEncodableRangeFlags(e));
    ++e;
    CPPUNIT_ASSERT_EQUAL(k31BitFlags, getCharEncodableRangeFlags(e));
    e = 0x7fffffff;
    CPPUNIT_ASSERT_EQUAL(k31BitFlags, getCharEncodableRangeFlags(e));
    ++e;
    CPPUNIT_ASSERT_EQUAL(k32BitFlags, getCharEncodableRangeFlags(e));
}

void StringTestFixture::testGetRangeFlag()
{
    char c;
    for (c = 0; c != '\x80'; ++c)
    {
        CPPUNIT_ASSERT_EQUAL(k7BitFlags, getRangeFlag(c));
    }
    for ( ; c != '\xc0'; ++c)
    {
        CPPUNIT_ASSERT_EQUAL(kInvalidRangeFlag, getRangeFlag(c));
    }
    for ( ; c != '\xf0'; ++c)
    {
        CPPUNIT_ASSERT_EQUAL(k16BitFlags, getRangeFlag(c));
    }
    for ( ; c != '\xf5'; ++c)
    {
        CPPUNIT_ASSERT_EQUAL(k21BitUnicodeFlags, getRangeFlag(c));
    }
    for ( ; c != '\xfe'; ++c)
    {
        CPPUNIT_ASSERT_EQUAL(k31BitFlags, getRangeFlag(c));
    }
    CPPUNIT_ASSERT_EQUAL(kInvalidRangeFlag, getRangeFlag(c));
}

#if defined(WIN32)
#pragma warning( disable : 4309 )
#endif

void StringTestFixture::testDecodeUtf8()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x80), rawDecodeUtf8(0xc2, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x100), rawDecodeUtf8(0xc4, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x200), rawDecodeUtf8(0xc8, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x400), rawDecodeUtf8(0xd0, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x600), rawDecodeUtf8(0xd8, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x700), rawDecodeUtf8(0xdc, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x780), rawDecodeUtf8(0xde, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x7c0), rawDecodeUtf8(0xdf, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x7e0), rawDecodeUtf8(0xdf, 0xa0));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x7f0), rawDecodeUtf8(0xdf, 0xb0));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x7f8), rawDecodeUtf8(0xdf, 0xb8));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x7fc), rawDecodeUtf8(0xdf, 0xbc));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x7fe), rawDecodeUtf8(0xdf, 0xbe));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x7ff), rawDecodeUtf8(0xdf, 0xbf));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x1000), rawDecodeUtf8(0xe1, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x2000), rawDecodeUtf8(0xe2, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x4000), rawDecodeUtf8(0xe4, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0x8000), rawDecodeUtf8(0xe8, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xc000), rawDecodeUtf8(0xec, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xe000), rawDecodeUtf8(0xee, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xf000), rawDecodeUtf8(0xef, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xf800), rawDecodeUtf8(0xef, 0xa0, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xfc00), rawDecodeUtf8(0xef, 0xb0, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xfe00), rawDecodeUtf8(0xef, 0xb8, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xff00), rawDecodeUtf8(0xef, 0xbc, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xff80), rawDecodeUtf8(0xef, 0xbe, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xffc0), rawDecodeUtf8(0xef, 0xbf, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xffe0), rawDecodeUtf8(0xef, 0xbf, 0xa0));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xfff0), rawDecodeUtf8(0xef, 0xbf, 0xb0));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xfff8), rawDecodeUtf8(0xef, 0xbf, 0xb8));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xfffc), rawDecodeUtf8(0xef, 0xbf, 0xbc));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xfffe), rawDecodeUtf8(0xef, 0xbf, 0xbe));
    CPPUNIT_ASSERT_EQUAL(static_cast<char16_t>(0xffff), rawDecodeUtf8(0xef, 0xbf, 0xbf));

    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x10000), rawDecodeUtf8(0xf0, 0x90, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x20000), rawDecodeUtf8(0xf0, 0xa0, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x40000), rawDecodeUtf8(0xf1, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x80000), rawDecodeUtf8(0xf2, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x100000), rawDecodeUtf8(0xf4, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x180000), rawDecodeUtf8(0xf6, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1c0000), rawDecodeUtf8(0xf7, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1e0000), rawDecodeUtf8(0xf7, 0xa0, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1f0000), rawDecodeUtf8(0xf7, 0xb0, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1f8000), rawDecodeUtf8(0xf7, 0xb8, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1fc000), rawDecodeUtf8(0xf7, 0xbc, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1fe000), rawDecodeUtf8(0xf7, 0xbe, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ff000), rawDecodeUtf8(0xf7, 0xbf, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ff800), rawDecodeUtf8(0xf7, 0xbf, 0xa0, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ffc00), rawDecodeUtf8(0xf7, 0xbf, 0xb0, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ffe00), rawDecodeUtf8(0xf7, 0xbf, 0xb8, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1fff00), rawDecodeUtf8(0xf7, 0xbf, 0xbc, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1fff80), rawDecodeUtf8(0xf7, 0xbf, 0xbe, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1fffc0), rawDecodeUtf8(0xf7, 0xbf, 0xbf, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1fffe0), rawDecodeUtf8(0xf7, 0xbf, 0xbf, 0xa0));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ffff0), rawDecodeUtf8(0xf7, 0xbf, 0xbf, 0xb0));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ffff8), rawDecodeUtf8(0xf7, 0xbf, 0xbf, 0xb8));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ffffc), rawDecodeUtf8(0xf7, 0xbf, 0xbf, 0xbc));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1ffffe), rawDecodeUtf8(0xf7, 0xbf, 0xbf, 0xbe));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1fffff), rawDecodeUtf8(0xf7, 0xbf, 0xbf, 0xbf));

    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x200000), rawDecodeUtf8(0xf8, 0x88, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x400000), rawDecodeUtf8(0xf8, 0x90, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x800000), rawDecodeUtf8(0xf8, 0xa0, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x1000000), rawDecodeUtf8(0xf9, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x2000000), rawDecodeUtf8(0xfa, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3000000), rawDecodeUtf8(0xfb, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3800000), rawDecodeUtf8(0xfb, 0xa0, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3c00000), rawDecodeUtf8(0xfb, 0xb0, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3e00000), rawDecodeUtf8(0xfb, 0xb8, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3f00000), rawDecodeUtf8(0xfb, 0xbc, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3f80000), rawDecodeUtf8(0xfb, 0xbe, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3fc0000), rawDecodeUtf8(0xfb, 0xbf, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3fff000), rawDecodeUtf8(0xfb, 0xbf, 0xbf, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x3ffffff), rawDecodeUtf8(0xfb, 0xbf, 0xbf, 0xbf, 0xbf));

    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x4000000), rawDecodeUtf8(0xfc, 0x84, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x8000000), rawDecodeUtf8(0xfc, 0x88, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x10000000), rawDecodeUtf8(0xfc, 0x90, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x20000000), rawDecodeUtf8(0xfc, 0xa0, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x40000000), rawDecodeUtf8(0xfd, 0x80, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x60000000), rawDecodeUtf8(0xfd, 0xa0, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x70000000), rawDecodeUtf8(0xfd, 0xb0, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x7f000000), rawDecodeUtf8(0xfd, 0xbf, 0x80, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x7ffc0000), rawDecodeUtf8(0xfd, 0xbf, 0xbf, 0x80, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x7ffff000), rawDecodeUtf8(0xfd, 0xbf, 0xbf, 0xbf, 0x80, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x7fffffc0), rawDecodeUtf8(0xfd, 0xbf, 0xbf, 0xbf, 0xbf, 0x80));
    CPPUNIT_ASSERT_EQUAL(static_cast<char32_t>(0x7fffffff), rawDecodeUtf8(0xfd, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf));
}

#if defined(WIN32)
#pragma warning( default : 4309 )
#endif

void StringTestFixture::testEncodeUtf8()
{
    string result;
    CharacterAdder<char> adder(result);

    encodeUtf8(0x7f, adder);
    CPPUNIT_ASSERT(strcmp(result.c_str(), "\x7f") == 0);

    encodeUtf8(0x80, adder);
    encodeUtf8(0x100, adder);
    encodeUtf8(0x200, adder);
    encodeUtf8(0x400, adder);
    encodeUtf8(0x600, adder);
    encodeUtf8(0x700, adder);
    encodeUtf8(0x780, adder);
    encodeUtf8(0x7c0, adder);
    encodeUtf8(0x7e0, adder);
    encodeUtf8(0x7f0, adder);
    encodeUtf8(0x7f8, adder);
    encodeUtf8(0x7fc, adder);
    encodeUtf8(0x7fe, adder);
    encodeUtf8(0x7ff, adder);
    CPPUNIT_ASSERT(strcmp(result.c_str(), "\x7f\xc2\x80\xc4\x80\xc8\x80\xd0\x80\xd8\x80\xdc\x80\xde\x80\xdf\x80\xdf\xa0\xdf\xb0\xdf\xb8\xdf\xbc\xdf\xbe\xdf\xbf") == 0);

    encodeUtf8(0x1000, adder);
    encodeUtf8(0x2000, adder);
    encodeUtf8(0x4000, adder);
    encodeUtf8(0x8000, adder);
    encodeUtf8(0xc000, adder);
    encodeUtf8(0xe000, adder);
    encodeUtf8(0xf000, adder);
    encodeUtf8(0xf800, adder);
    encodeUtf8(0xfc00, adder);
    encodeUtf8(0xfe00, adder);
    encodeUtf8(0xff00, adder);
    encodeUtf8(0xff80, adder);
    encodeUtf8(0xffc0, adder);
    encodeUtf8(0xffe0, adder);
    encodeUtf8(0xfff0, adder);
    encodeUtf8(0xfff8, adder);
    encodeUtf8(0xfffc, adder);
    encodeUtf8(0xfffe, adder);
    encodeUtf8(0xffff, adder);

    CPPUNIT_ASSERT(strcmp(result.c_str(), "\x7f\xc2\x80\xc4\x80\xc8\x80\xd0\x80\xd8\x80\xdc\x80\xde"
                                          "\x80\xdf\x80\xdf\xa0\xdf\xb0\xdf\xb8\xdf\xbc\xdf\xbe\xdf\xbf"
                                          "\xe1\x80\x80\xe2\x80\x80\xe4\x80\x80\xe8\x80\x80\xec\x80\x80"
                                          "\xee\x80\x80\xef\x80\x80\xef\xa0\x80\xef\xb0\x80\xef\xb8\x80"
                                          "\xef\xbc\x80\xef\xbe\x80\xef\xbf\x80\xef\xbf\xa0\xef\xbf\xb0"
                                          "\xef\xbf\xb8\xef\xbf\xbc\xef\xbf\xbe\xef\xbf\xbf") == 0);

    encodeUtf8(0x10000, adder);
    encodeUtf8(0x20000, adder);
    encodeUtf8(0x40000, adder);
    encodeUtf8(0x80000, adder);
    encodeUtf8(0x100000, adder);
    encodeUtf8(0x180000, adder);
    encodeUtf8(0x1c0000, adder);
    encodeUtf8(0x1e0000, adder);
    encodeUtf8(0x1f0000, adder);
    encodeUtf8(0x1f8000, adder);
    encodeUtf8(0x1fc000, adder);
    encodeUtf8(0x1fe000, adder);
    encodeUtf8(0x1ff000, adder);
    encodeUtf8(0x1ff800, adder);
    encodeUtf8(0x1ffc00, adder);
    encodeUtf8(0x1ffe00, adder);
    encodeUtf8(0x1fff00, adder);
    encodeUtf8(0x1fff80, adder);
    encodeUtf8(0x1fffc0, adder);
    encodeUtf8(0x1fffe0, adder);
    encodeUtf8(0x1ffff0, adder);
    encodeUtf8(0x1ffff8, adder);
    encodeUtf8(0x1ffffc, adder);
    encodeUtf8(0x1ffffe, adder);
    encodeUtf8(0x1fffff, adder);

    CPPUNIT_ASSERT(strcmp(result.c_str(), "\x7f\xc2\x80\xc4\x80\xc8\x80\xd0\x80\xd8\x80\xdc\x80\xde"
                                          "\x80\xdf\x80\xdf\xa0\xdf\xb0\xdf\xb8\xdf\xbc\xdf\xbe\xdf\xbf"
                                          "\xe1\x80\x80\xe2\x80\x80\xe4\x80\x80\xe8\x80\x80\xec\x80\x80"
                                          "\xee\x80\x80\xef\x80\x80\xef\xa0\x80\xef\xb0\x80\xef\xb8\x80"
                                          "\xef\xbc\x80\xef\xbe\x80\xef\xbf\x80\xef\xbf\xa0\xef\xbf\xb0"
                                          "\xef\xbf\xb8\xef\xbf\xbc\xef\xbf\xbe\xef\xbf\xbf"
                                          "\xf0\x90\x80\x80\xf0\xa0\x80\x80\xf1\x80\x80\x80\xf2\x80\x80\x80"
                                          "\xf4\x80\x80\x80\xf6\x80\x80\x80\xf7\x80\x80\x80\xf7\xa0\x80\x80"
                                          "\xf7\xb0\x80\x80\xf7\xb8\x80\x80\xf7\xbc\x80\x80\xf7\xbe\x80\x80"
                                          "\xf7\xbf\x80\x80\xf7\xbf\xa0\x80\xf7\xbf\xb0\x80\xf7\xbf\xb8\x80"
                                          "\xf7\xbf\xbc\x80\xf7\xbf\xbe\x80\xf7\xbf\xbf\x80\xf7\xbf\xbf\xa0"
                                          "\xf7\xbf\xbf\xb0\xf7\xbf\xbf\xb8\xf7\xbf\xbf\xbc\xf7\xbf\xbf\xbe"
                                          "\xf7\xbf\xbf\xbf"
                                          ) == 0);

    encodeUtf8(0x200000, adder);
    encodeUtf8(0x400000, adder);
    encodeUtf8(0x800000, adder);
    encodeUtf8(0x1000000, adder);
    encodeUtf8(0x2000000, adder);
    encodeUtf8(0x3000000, adder);
    encodeUtf8(0x3800000, adder);
    encodeUtf8(0x3c00000, adder);
    encodeUtf8(0x3e00000, adder);
    encodeUtf8(0x3f00000, adder);
    encodeUtf8(0x3f80000, adder);
    encodeUtf8(0x3fc0000, adder);
    encodeUtf8(0x3fff000, adder);
    encodeUtf8(0x3ffffff, adder);

    CPPUNIT_ASSERT(strcmp(result.c_str(), "\x7f\xc2\x80\xc4\x80\xc8\x80\xd0\x80\xd8\x80\xdc\x80\xde"
                                          "\x80\xdf\x80\xdf\xa0\xdf\xb0\xdf\xb8\xdf\xbc\xdf\xbe\xdf\xbf"
                                          "\xe1\x80\x80\xe2\x80\x80\xe4\x80\x80\xe8\x80\x80\xec\x80\x80"
                                          "\xee\x80\x80\xef\x80\x80\xef\xa0\x80\xef\xb0\x80\xef\xb8\x80"
                                          "\xef\xbc\x80\xef\xbe\x80\xef\xbf\x80\xef\xbf\xa0\xef\xbf\xb0"
                                          "\xef\xbf\xb8\xef\xbf\xbc\xef\xbf\xbe\xef\xbf\xbf"
                                          "\xf0\x90\x80\x80\xf0\xa0\x80\x80\xf1\x80\x80\x80\xf2\x80\x80\x80"
                                          "\xf4\x80\x80\x80\xf6\x80\x80\x80\xf7\x80\x80\x80\xf7\xa0\x80\x80"
                                          "\xf7\xb0\x80\x80\xf7\xb8\x80\x80\xf7\xbc\x80\x80\xf7\xbe\x80\x80"
                                          "\xf7\xbf\x80\x80\xf7\xbf\xa0\x80\xf7\xbf\xb0\x80\xf7\xbf\xb8\x80"
                                          "\xf7\xbf\xbc\x80\xf7\xbf\xbe\x80\xf7\xbf\xbf\x80\xf7\xbf\xbf\xa0"
                                          "\xf7\xbf\xbf\xb0\xf7\xbf\xbf\xb8\xf7\xbf\xbf\xbc\xf7\xbf\xbf\xbe"
                                          "\xf7\xbf\xbf\xbf"
                                          "\xf8\x88\x80\x80\x80\xf8\x90\x80\x80\x80\xf8\xa0\x80\x80\x80"
                                          "\xf9\x80\x80\x80\x80\xfa\x80\x80\x80\x80\xfb\x80\x80\x80\x80"
                                          "\xfb\xa0\x80\x80\x80\xfb\xb0\x80\x80\x80\xfb\xb8\x80\x80\x80"
                                          "\xfb\xbc\x80\x80\x80\xfb\xbe\x80\x80\x80\xfb\xbf\x80\x80\x80"
                                          "\xfb\xbf\xbf\x80\x80\xfb\xbf\xbf\xbf\xbf"
                                          ) == 0);

    encodeUtf8(0x4000000, adder);
    encodeUtf8(0x8000000, adder);
    encodeUtf8(0x10000000, adder);
    encodeUtf8(0x20000000, adder);
    encodeUtf8(0x40000000, adder);
    encodeUtf8(0x60000000, adder);
    encodeUtf8(0x70000000, adder);
    encodeUtf8(0x7f000000, adder);
    encodeUtf8(0x7ffc0000, adder);
    encodeUtf8(0x7ffff000, adder);
    encodeUtf8(0x7fffffc0, adder);
    encodeUtf8(0x7fffffff, adder);

    CPPUNIT_ASSERT(strcmp(result.c_str(), "\x7f\xc2\x80\xc4\x80\xc8\x80\xd0\x80\xd8\x80\xdc\x80\xde"
                                          "\x80\xdf\x80\xdf\xa0\xdf\xb0\xdf\xb8\xdf\xbc\xdf\xbe\xdf\xbf"
                                          "\xe1\x80\x80\xe2\x80\x80\xe4\x80\x80\xe8\x80\x80\xec\x80\x80"
                                          "\xee\x80\x80\xef\x80\x80\xef\xa0\x80\xef\xb0\x80\xef\xb8\x80"
                                          "\xef\xbc\x80\xef\xbe\x80\xef\xbf\x80\xef\xbf\xa0\xef\xbf\xb0"
                                          "\xef\xbf\xb8\xef\xbf\xbc\xef\xbf\xbe\xef\xbf\xbf"
                                          "\xf0\x90\x80\x80\xf0\xa0\x80\x80\xf1\x80\x80\x80\xf2\x80\x80\x80"
                                          "\xf4\x80\x80\x80\xf6\x80\x80\x80\xf7\x80\x80\x80\xf7\xa0\x80\x80"
                                          "\xf7\xb0\x80\x80\xf7\xb8\x80\x80\xf7\xbc\x80\x80\xf7\xbe\x80\x80"
                                          "\xf7\xbf\x80\x80\xf7\xbf\xa0\x80\xf7\xbf\xb0\x80\xf7\xbf\xb8\x80"
                                          "\xf7\xbf\xbc\x80\xf7\xbf\xbe\x80\xf7\xbf\xbf\x80\xf7\xbf\xbf\xa0"
                                          "\xf7\xbf\xbf\xb0\xf7\xbf\xbf\xb8\xf7\xbf\xbf\xbc\xf7\xbf\xbf\xbe"
                                          "\xf7\xbf\xbf\xbf"
                                          "\xf8\x88\x80\x80\x80\xf8\x90\x80\x80\x80\xf8\xa0\x80\x80\x80"
                                          "\xf9\x80\x80\x80\x80\xfa\x80\x80\x80\x80\xfb\x80\x80\x80\x80"
                                          "\xfb\xa0\x80\x80\x80\xfb\xb0\x80\x80\x80\xfb\xb8\x80\x80\x80"
                                          "\xfb\xbc\x80\x80\x80\xfb\xbe\x80\x80\x80\xfb\xbf\x80\x80\x80"
                                          "\xfb\xbf\xbf\x80\x80\xfb\xbf\xbf\xbf\xbf"
                                          "\xfc\x84\x80\x80\x80\x80\xfc\x88\x80\x80\x80\x80\xfc\x90\x80\x80\x80\x80"
                                          "\xfc\xa0\x80\x80\x80\x80\xfd\x80\x80\x80\x80\x80\xfd\xa0\x80\x80\x80\x80"
                                          "\xfd\xb0\x80\x80\x80\x80\xfd\xbf\x80\x80\x80\x80\xfd\xbf\xbf\x80\x80\x80"
                                          "\xfd\xbf\xbf\xbf\x80\x80\xfd\xbf\xbf\xbf\xbf\x80\xfd\xbf\xbf\xbf\xbf\xbf"
                                          ) == 0);
}

void StringTestFixture::testRawDecodeUtf16()
{
    char32_t r = 0x10000;
    for (char16_t d0 = 0xd800; d0 < 0xdc00; ++d0)
    {
        for (char16_t d1 = 0xdc00; d1 < 0xe000; ++d1, ++r)
        {
            CPPUNIT_ASSERT_EQUAL(r, rawDecodeUtf16(d0, d1));
        }
    }
}

void StringTestFixture::testRawEncodeUtf16()
{
    utf16String r;
    CharacterAdder<char16_t> a(r);

    rawEncodeUtf16(0xFFFF, a);          // ffff
    rawEncodeUtf16(0x10000, a);         // d800 dc00  0000000000 0000000000
    rawEncodeUtf16(0x20000, a);         // d840 dc00  0001000000 0000000000
    rawEncodeUtf16(0x30000, a);         // d880 dc00
    rawEncodeUtf16(0x40000, a);         // d8c0 dc00
    rawEncodeUtf16(0x50000, a);         // d900 dc00
    rawEncodeUtf16(0x60000, a);         // d940 dc00
    rawEncodeUtf16(0x70000, a);         // d980 dc00
    rawEncodeUtf16(0x80000, a);         // d9c0 dc00
    rawEncodeUtf16(0x87654, a);         // d9dd de54
    rawEncodeUtf16(0x90000, a);         // da00 dc00
    rawEncodeUtf16(0xa0000, a);         // da40 dc00
    rawEncodeUtf16(0xb0000, a);         // da80 dc00
    rawEncodeUtf16(0xc0000, a);         // dac0 dc00
    rawEncodeUtf16(0xd0000, a);         // db00 dc00
    rawEncodeUtf16(0xe0000, a);         // db40 dc00
    rawEncodeUtf16(0xf0000, a);         // db80 dc00
    rawEncodeUtf16(0x100000, a);        // dbc0 dc00
    rawEncodeUtf16(0x10ffff, a);        // dbff dfff

    char16_t v[] = {
        0xffff, 0xd800, 0xdc00, 0xd840, 0xdc00, 0xd880, 0xdc00, 0xd8c0, 0xdc00,
        0xd900, 0xdc00, 0xd940, 0xdc00, 0xd980, 0xdc00, 0xd9c0, 0xdc00, 0xd9dd, 0xde54,
        0xda00, 0xdc00, 0xda40, 0xdc00, 0xda80, 0xdc00, 0xdac0, 0xdc00, 0xdb00, 0xdc00,
        0xdb40, 0xdc00, 0xdb80, 0xdc00, 0xdbc0, 0xdc00, 0xdbff, 0xdfff, 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(r.c_str(), v, sizeof(v)));
}

void StringTestFixture::testIsUtf8AsAscii()
{
    // empty string, bad target encoding
    CPPUNIT_ASSERT(isUtf8(0, kAscii));
    CPPUNIT_ASSERT(isUtf8("", kAscii));
    CPPUNIT_ASSERT(isUtf8(std::string(), kAscii));
    CPPUNIT_ASSERT(!isUtf8(0, kFirstInvalidRange));

    char good[] = "This is a string that will pass.";
    CPPUNIT_ASSERT(isUtf8(good, kAscii));

    char bad[] = "This is \xc3\xa4 string that will not pass.";
    CPPUNIT_ASSERT(!isUtf8(bad, kAscii));
}

void StringTestFixture::testIsUtf8AsUtf8()
{
    char asciiBad[] = "This is \xc3\xa4 string that will not pass.";
    CPPUNIT_ASSERT(isUtf8(asciiBad, kUtf8));

    // good ones in each range c0-df+1*80-bf; e0-ef+2*80-bf; f0-f7+3*80-bf; f8-fb+4*80-bf; fc-fd+5*80-bf
    char utf8Good[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass "
                      "\xfd\xa1\xa2\xa3\xa4\xa5";
    CPPUNIT_ASSERT(isUtf8(utf8Good, kUtf8));
    char utf8GoodEndDropFC[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass "
                      "\xfd\xa1\xa2\xa3\xa4";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropFC, kUtf8));
    char utf8GoodEndDropF8[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropF8, kUtf8));
    char utf8GoodEndDropF0[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropF0, kUtf8));
    char utf8GoodEndDropE0[] = "This \xc3\xa4 string \xe4\xab";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropE0, kUtf8));
    char utf8GoodEndDropC0[] = "This \xc3";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropC0, kUtf8));

    // bad: in each range, missing one or more 80-bf
    char utf8BadC0_DF[] = "Bad C0-DF \xc0 will fail.";
    CPPUNIT_ASSERT(!isUtf8(utf8BadC0_DF, kUtf8));
    char utf8BadE0_EF[] = "Bad E0-EF \xe4\xab will fail.";
    CPPUNIT_ASSERT(!isUtf8(utf8BadE0_EF, kUtf8));
    char utf8BadF0_F7[] = "Bad F0-F7 \xf2\x81\x82 will fail.";
    CPPUNIT_ASSERT(!isUtf8(utf8BadF0_F7, kUtf8));
    char utf8BadF8_FB[] = "Bad F8-FB \xf9\x84 will fail.";
    CPPUNIT_ASSERT(!isUtf8(utf8BadF0_F7, kUtf8));
    char utf8BadFC_FD[] = "Bad FC-FD \xfd\xa1\xa2 will fail";
    CPPUNIT_ASSERT(!isUtf8(utf8BadFC_FD, kUtf8));

    // bad: fe_6*80-bf
    char utf8BadFE[] = "This is out of range but it could be valid: \xfe\x83\xbe\xb7\x8b\xaa\x98, just not yet.";
    CPPUNIT_ASSERT(!isUtf8(utf8BadFE, kUtf8));

    // bad: only one of d800-dbff (ed a0 80-ed af bf) and dc800-dfff (ed b0 80-ed bf bf)
    char utf8BadSoloUTF16_0[] = "Can't do \xed\xa8\xb2 alone.";
    CPPUNIT_ASSERT(!isUtf8(utf8BadSoloUTF16_0, kUtf8));
    char utf8BadSoloUTF16_1[] = "Can't do \xed\xb8\xb2 alone.";
    CPPUNIT_ASSERT(!isUtf8(utf8BadSoloUTF16_1, kUtf8));

    // good: include full d800-dbff/dc800-dfff pair
    char utf8PairedUTF16[] = "Can do \xed\xa8\xb2\xed\xb8\xb2 together.";
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16, kUtf8));
    
    // good: include 10000..10FFFF characters as UTF-8 straight up
    char utf8UnpairedUTF16[] = "Can do \xf2\x90\x84\xb2 as one.";
    CPPUNIT_ASSERT(isUtf8(utf8UnpairedUTF16, kUtf8));
}

void StringTestFixture::testIsUtf8AsUcs2()
{
    // good ones in each range c0-df+1*80-bf; e0-ef+2*80-bf; f0-f7+3*80-bf; f8-fb+4*80-bf; fc-fd+5*80-bf
    char utf8Good[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass "
                      "\xfd\xa1\xa2\xa3\xa4\xa5";
    CPPUNIT_ASSERT(!isUtf8(utf8Good, kUcs2));
    char utf8GoodX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass.";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodX, kUcs2));
    char utf8GoodXX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly.";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodXX, kUcs2));
    char utf8GoodXXX[] = "This \xc3\xa4 string \xe4\xab\x88 will.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodXXX, kUcs2));

    char utf8GoodEndDropFC[] = "This \xfd\xa1\xa2\xa3\xa4";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodEndDropFC, kUcs2));
    char utf8GoodEndDropF8[] = "This \xf9\x84";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodEndDropF8, kUcs2));
    char utf8GoodEndDropF0[] = "This \xf2\x81\x82";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodEndDropF0, kUcs2));
    char utf8GoodEndDropE0[] = "This \xc3\xa4 string \xe4\xab";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropE0, kUcs2));
    char utf8GoodEndDropC0[] = "This \xc3";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropC0, kUcs2));

    // bad: include full d800-dbff/dc800-dfff pair -- out of range
    char utf8PairedUTF16[] = "Can do \xed\xa8\xb2\xed\xb8\xb2 together.";
    CPPUNIT_ASSERT(!isUtf8(utf8PairedUTF16, kUcs2));
}

void StringTestFixture::testIsUtf8AsUtf16()
{
    // good ones in each range c0-df+1*80-bf; e0-ef+2*80-bf; f0-f7+3*80-bf; f8-fb+4*80-bf; fc-fd+5*80-bf
    char utf8Good[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass "
                      "\xfd\xa1\xa2\xa3\xa4\xa5";
    CPPUNIT_ASSERT(!isUtf8(utf8Good, kUtf16));
    char utf8GoodX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass.";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodX, kUtf16));
    char utf8GoodXX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodXX, kUtf16));
    char utf8GoodXXX[] = "This \xc3\xa4 string \xe4\xab\x88 will.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodXX, kUtf16));

    char utf8GoodEndDropFC[] = "This \xfd\xa1\xa2\xa3\xa4";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodEndDropFC, kUtf16));
    char utf8GoodEndDropF8[] = "This \xf9\x84";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodEndDropF8, kUtf16));
    char utf8GoodEndDropF0[] = "This \xf2\x81\x82";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropF0, kUtf16));
    char utf8GoodEndDropE0[] = "This \xc3\xa4 string \xe4\xab";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropE0, kUtf16));
    char utf8GoodEndDropC0[] = "This \xc3";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropC0, kUtf16));

    // good: include full d800-dbff/dc800-dfff pair -- but in range here...
    char utf8PairedUTF16[] = "Can do \xed\xa8\xb2\xed\xb8\xb2 together.";
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16, kUtf16));
}

void StringTestFixture::testIsUtf8AsUcs4()
{
    // good ones in each range c0-df+1*80-bf; e0-ef+2*80-bf; f0-f7+3*80-bf; f8-fb+4*80-bf; fc-fd+5*80-bf
    char utf8Good[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass "
                      "\xfd\xa1\xa2\xa3\xa4\xa5";
    CPPUNIT_ASSERT(isUtf8(utf8Good, kUcs4));
    char utf8GoodX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodX, kUcs4));
    char utf8GoodXX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodXX, kUcs4));
    char utf8GoodXXX[] = "This \xc3\xa4 string \xe4\xab\x88 will.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodXXX, kUcs4));

    char utf8GoodEndDropFC[] = "This \xfd\xa1\xa2\xa3\xa4";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropFC, kUcs4));
    char utf8GoodEndDropF8[] = "This \xf9\x84";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropF8, kUcs4));
    char utf8GoodEndDropF0[] = "This \xf2\x81\x82";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropF0, kUcs4));
    char utf8GoodEndDropE0[] = "This \xc3\xa4 string \xe4\xab";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropE0, kUcs4));
    char utf8GoodEndDropC0[] = "This \xc3";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropC0, kUcs4));

    // bad: include full d800-dbff/dc800-dfff pair -- out of range
    char utf8PairedUTF16[] = "Can do \xed\xa8\xb2\xed\xb8\xb2 together.";
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16, kUcs4));
    char utf8SameAsPairedUTF16_UTF8[] = "Can do \xf2\xad\xa8\xb2 together.";
    CPPUNIT_ASSERT(isUtf8(utf8SameAsPairedUTF16_UTF8, kUcs4));
}

void StringTestFixture::testIsUtf8AsUnicode()
{
    // good ones in each range c0-df+1*80-bf; e0-ef+2*80-bf; f0-f7+3*80-bf; f8-fb+4*80-bf; fc-fd+5*80-bf
    char utf8Good[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass "
                      "\xfd\xa1\xa2\xa3\xa4\xa5";
    CPPUNIT_ASSERT(!isUtf8(utf8Good, kUnicode));
    char utf8GoodX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass.";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodX, kUnicode));
    char utf8GoodXX[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodXX, kUnicode));
    char utf8GoodXXX[] = "This \xc3\xa4 string \xe4\xab\x88 will.";
    CPPUNIT_ASSERT(isUtf8(utf8GoodXXX, kUnicode));

    char utf8GoodEndDropFC[] = "This \xfd\xa1\xa2\xa3\xa4";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodEndDropFC, kUnicode));
    char utf8GoodEndDropF8[] = "This \xf9\x84";
    CPPUNIT_ASSERT(!isUtf8(utf8GoodEndDropF8, kUnicode));
    char utf8GoodEndDropF0[] = "This \xf2\x81\x82";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropF0, kUnicode));
    char utf8GoodEndDropE0[] = "This \xc3\xa4 string \xe4\xab";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropE0, kUnicode));
    char utf8GoodEndDropC0[] = "This \xc3";
    CPPUNIT_ASSERT(isUtf8(utf8GoodEndDropC0, kUnicode));

    // bad: include full d800-dbff/dc800-dfff pair -- out of range
    char utf8PairedUTF16[] = "Can do \xed\xa8\xb2\xed\xb8\xb2 together.";
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16, kUnicode));
    char utf8SameAsPairedUTF16_UTF8[] = "Can do \xf2\xad\xa8\xb2 together.";
    CPPUNIT_ASSERT(isUtf8(utf8SameAsPairedUTF16_UTF8, kUnicode));

    char utf8UnicodeGood[] = "Can do \xf4\x8f\xbf\xbf";
    CPPUNIT_ASSERT(isUtf8(utf8UnicodeGood, kUnicode));
    char utf8UnicodeBad[] = "Cannot do \xf4\x90\x80\x80";
    CPPUNIT_ASSERT(!isUtf8(utf8UnicodeBad, kUnicode));
}

void StringTestFixture::testIsUtf8LengthTerminated()
{
    // good ones in each range c0-df+1*80-bf; e0-ef+2*80-bf; f0-f7+3*80-bf; f8-fb+4*80-bf; fc-fd+5*80-bf
    char utf8Good[] = "This \xc3\xa4 string \xe4\xab\x88 will \xf2\x81\x82\x83 certainly \xf9\x84\x85\x86\x87 pass "
                      "\xfd\xa1\xa2\xa3\xa4\xa5";
    unsigned int utf8GoodLength = static_cast<unsigned int>(sizeof(utf8Good) - 1);

    CPPUNIT_ASSERT(isUtf8(utf8Good, utf8GoodLength));
    utf8Good[1] = '\0';
    CPPUNIT_ASSERT(!isUtf8(utf8Good, utf8GoodLength));
    utf8Good[1] = 'h';
    CPPUNIT_ASSERT(isUtf8(utf8Good + 1, utf8GoodLength - 1));
    CPPUNIT_ASSERT(isUtf8(utf8Good + 2, utf8GoodLength - 2));
    CPPUNIT_ASSERT(isUtf8(utf8Good + 3, utf8GoodLength - 3));
    CPPUNIT_ASSERT(isUtf8(utf8Good + 4, utf8GoodLength - 4));
    CPPUNIT_ASSERT(isUtf8(utf8Good + 5, utf8GoodLength - 5));
    CPPUNIT_ASSERT(!isUtf8(utf8Good + 6, utf8GoodLength - 6));  // broken sequence
    CPPUNIT_ASSERT(isUtf8(utf8Good, utf8GoodLength - 1));
    CPPUNIT_ASSERT(isUtf8(utf8Good, utf8GoodLength - 2));
    CPPUNIT_ASSERT(isUtf8(utf8Good, utf8GoodLength - 3));
    CPPUNIT_ASSERT(isUtf8(utf8Good, utf8GoodLength - 4));
    CPPUNIT_ASSERT(isUtf8(utf8Good, utf8GoodLength - 5));
    CPPUNIT_ASSERT(isUtf8(utf8Good, utf8GoodLength - 6));
    CPPUNIT_ASSERT(isUtf8(utf8Good + 5, 1));
    CPPUNIT_ASSERT(!isUtf8(utf8Good + 6, 1));

    char utf8PairedUTF16[] = "Can do \xed\xa8\xb2\xed\xb8\xb2 together.";
    //                        0    5 7   8   9   10  11  12   15   20
    utf8GoodLength = sizeof(utf8PairedUTF16) - 1;
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16, utf8GoodLength));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 1, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 2, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 3, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 4, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 5, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 6, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 7, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 7, 2));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 7, 3));
    CPPUNIT_ASSERT(!isUtf8(utf8PairedUTF16 + 8, 1));
    CPPUNIT_ASSERT(!isUtf8(utf8PairedUTF16 + 9, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 10, 1));
    CPPUNIT_ASSERT(!isUtf8(utf8PairedUTF16 + 10, 2));
    CPPUNIT_ASSERT(!isUtf8(utf8PairedUTF16 + 10, 3));
    CPPUNIT_ASSERT(!isUtf8(utf8PairedUTF16 + 11, 1));
    CPPUNIT_ASSERT(!isUtf8(utf8PairedUTF16 + 12, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 13, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 14, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 15, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 16, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 17, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 18, 1));
    CPPUNIT_ASSERT(isUtf8(utf8PairedUTF16 + 19, 1));
}

void StringTestFixture::testIsUcs2AsAscii()
{
    char16_t nullString[] = { 0 };
    // empty string, bad target encoding
    CPPUNIT_ASSERT(isUcs2(0, kAscii));
    CPPUNIT_ASSERT(isUcs2(nullString, kAscii));
    CPPUNIT_ASSERT(isUcs2(ucs2String(), kAscii));
    CPPUNIT_ASSERT(!isUcs2(0, kFirstInvalidRange));

    char16_t ucs2AsAsciiGood[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                   'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                   'g', 'o', 'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                   'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                   'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                   'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(isUcs2(ucs2AsAsciiGood, kAscii));
    char16_t ucs2AsAsciiBad[] = { 'N', 246, 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                  'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                  'g', 'o', 'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                  'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                  'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                  'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsAsciiBad, kAscii));
}

void StringTestFixture::testIsUcs2AsUtf8()
{
    char16_t ucs2AsUtf8Good[] = { 'u', 'n', 'd', ' ', 'a', 'l', 'l', 'e', 'r', '-', 'm', 252,
                                  'm', 's', 'i', 0x11d, 'e', ' ', 'B', 'u', 'r', 'g', '-', 'g',
                                  'o', 'v', 'e', 'n', ' ', 'd', 'i', 'e', ' ', 'm', 'o', 'h',
                                  'm', 'e', 'n', ' ', 'R', 228, 't', 'h', '\'', ' ', 'a', 'u',
                                  's', 'g', 'r', 'a', 'b', 'e', 'n', '.', 0};
    CPPUNIT_ASSERT(isUcs2(ucs2AsUtf8Good, kUtf8));
}

void StringTestFixture::testIsUcs2AsUcs2()
{
    char16_t ucs2AsUcs2Good[] = { 0x101, 0x106, 0x163, 0xa0, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs2(ucs2AsUcs2Good, kUcs2));
    char16_t ucs2AsUcs2Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad1, kUcs2));
    char16_t ucs2AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad2, kUcs2));
    char16_t ucs2AsUcs2Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad3, kUcs2));
}

void StringTestFixture::testIsUcs2AsUtf16()
{
    char16_t ucs2AsUcs2Good[] = { 0x101, 0x106, 0x163, 0xa0, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs2(ucs2AsUcs2Good, kUtf16));
    char16_t ucs2AsUcs2Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad1, kUtf16));
    char16_t ucs2AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad2, kUtf16));
    char16_t ucs2AsUcs2Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad3, kUtf16));
}

void StringTestFixture::testIsUcs2AsUcs4()
{
    char16_t ucs2AsUcs2Good[] = { 0x101, 0x106, 0x163, 0xa0, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs2(ucs2AsUcs2Good, kUcs4));
    char16_t ucs2AsUcs2Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad1, kUcs4));
    char16_t ucs2AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad2, kUcs4));
    char16_t ucs2AsUcs2Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad3, kUcs4));
}

void StringTestFixture::testIsUcs2AsUnicode()
{
    char16_t ucs2AsUcs2Good[] = { 0x101, 0x106, 0x163, 0xa0, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs2(ucs2AsUcs2Good, kUnicode));
    char16_t ucs2AsUcs2Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad1, kUnicode));
    char16_t ucs2AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad2, kUnicode));
    char16_t ucs2AsUcs2Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs2(ucs2AsUcs2Bad3, kUnicode));
}

void StringTestFixture::testIsUtf16AsAscii()
{
    char16_t nullString[] = { 0 };
    // empty string, bad target encoding
    CPPUNIT_ASSERT(isUtf16(0, kAscii));
    CPPUNIT_ASSERT(isUtf16(nullString, kAscii));
    CPPUNIT_ASSERT(isUcs2(utf16String(), kAscii));
    CPPUNIT_ASSERT(!isUtf16(0, kFirstInvalidRange));

    char16_t utf16AsAsciiGood[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                   'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                   'g', 'o', 'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                   'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                   'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                   'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsAsciiGood, kAscii));
    char16_t utf16AsAsciiBad[] = { 'N', 246, 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                  'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                  'g', 'o', 'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                  'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                  'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                  'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsAsciiBad, kAscii));
    char16_t utf16AsAsciiGoodBut[] = { 'N', 246, 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                  'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                  'g', 'o', 'o', 0xda32, 0xdd94, ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                  'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                  'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                  'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsAsciiGoodBut, kAscii));
}

void StringTestFixture::testIsUtf16AsUtf8()
{
    char16_t utf16AsUtf8Good[] = { 'u', 'n', 'd', ' ', 'a', 'l', 'l', 'e', 'r', '-', 'm', 252,
                                  'm', 's', 'i', 0x11d, 'e', ' ', 'B', 'u', 'r', 'g', '-', 'g',
                                  'o', 'v', 'e', 'n', ' ', 'd', 'i', 'e', ' ', 'm', 'o', 'h',
                                  'm', 'e', 'n', 0xda32, 0xdd94, ' ', 'R', 228, 't', 'h', '\'', ' ', 'a', 'u',
                                  's', 'g', 'r', 'a', 'b', 'e', 'n', '.', 0};
    CPPUNIT_ASSERT(isUtf16(utf16AsUtf8Good, kUtf8));
}

void StringTestFixture::testIsUtf16AsUcs2()
{
    char16_t utf16AsUcs2Good[] = { 0x101, 0x106, 0x163, 0xa0, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsUcs2Good, kUcs2));
    char16_t utf16AsUcs2Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUcs2Bad1, kUcs2));
    char16_t utf16AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUcs2Bad2, kUcs2));
    char16_t utf16AsUcs2Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUcs2Bad3, kUcs2));
}

void StringTestFixture::testIsUtf16AsUtf16()
{
    char16_t utf16AsUtf16Good[] = { 0x101, 0x106, 0x163, 0xa0, 0xdabc, 0xdead, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsUtf16Good, kUtf16));
    char16_t utf16AsUtf16Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUtf16Bad1, kUtf16));
    char16_t utf16AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsUcs2Bad2, kUtf16));
    char16_t utf16AsUtf16Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUtf16Bad3, kUtf16));
}

void StringTestFixture::testIsUtf16AsUcs4()
{
    char16_t utf16AsUcs4Good[] = { 0x101, 0x106, 0x163, 0xa0, 0xdabc, 0xdead, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsUcs4Good, kUcs4));
    char16_t utf16AsUcs4Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUcs4Bad1, kUcs4));
    char16_t utf16AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsUcs2Bad2, kUcs4));
    char16_t utf16AsUcs4Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUcs4Bad3, kUcs4));
}

void StringTestFixture::testIsUtf16AsUnicode()
{
    char16_t utf16AsUnicodeGood[] = { 0x101, 0x106, 0x163, 0xa0, 0xdabc, 0xdead, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsUnicodeGood, kUnicode));
    char16_t utf16AsUnicodeBad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUnicodeBad1, kUnicode));
    char16_t utf16AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUtf16(utf16AsUcs2Bad2, kUnicode));
    char16_t utf16AsUnicodeBad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUtf16(utf16AsUnicodeBad3, kUnicode));
}

void StringTestFixture::testIsUcs4AsAscii()
{
    char32_t nullString[] = { 0 };
    // empty string, bad target encoding
    CPPUNIT_ASSERT(isUcs4(0, kAscii));
    CPPUNIT_ASSERT(isUcs4(nullString, kAscii));
    CPPUNIT_ASSERT(isUcs4(ucs4String(), kAscii));
    CPPUNIT_ASSERT(!isUcs4(0, kFirstInvalidRange));

    char32_t ucs4AsAsciiGood[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                   'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                   'g', 'o', 'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                   'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                   'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                   'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(isUcs4(ucs4AsAsciiGood, kAscii));
    char32_t ucs4AsAsciiBad[] = { 'N', 246, 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                  'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                  'g', 'o', 'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                  'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                  'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                  'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsAsciiBad, kAscii));
    char32_t ucs4AsAsciiGoodBut[] = { 'N', 246, 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                  'i', 'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ',
                                  'g', 'o', 'o', 0xada32, ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ',
                                  'c', 'o', 'm', 'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ',
                                  'a', 'i', 'd', ' ', 'o', 'f', ' ', 't', 'h', 'e', ' ', 'p',
                                  'a', 'r', 't', 'y', '.', 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsAsciiGoodBut, kAscii));
}

void StringTestFixture::testIsUcs4AsUtf8()
{
    char32_t ucs4AsUtf8Good[] = { 'u', 'n', 'd', ' ', 'a', 'l', 'l', 'e', 'r', '-', 'm', 252,
                                  'm', 's', 'i', 0x11d, 'e', ' ', 'B', 'u', 'r', 'g', '-', 'g',
                                  'o', 'v', 'e', 'n', ' ', 'd', 'i', 'e', ' ', 'm', 'o', 'h',
                                  'm', 'e', 'n', 0xada32, ' ', 'R', 228, 't', 'h', '\'', ' ', 'a', 'u',
                                  's', 'g', 'r', 'a', 'b', 'e', 'n', '.', 0};
    CPPUNIT_ASSERT(isUcs4(ucs4AsUtf8Good, kUtf8));
}

void StringTestFixture::testIsUcs4AsUcs2()
{
    char32_t ucs4AsUcs2Good[] = { 0x101, 0x106, 0x163, 0xa0, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs4(ucs4AsUcs2Good, kUcs2));
    char32_t ucs4AsUcs2Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad1, kUcs2));
    char32_t ucs4AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0x1cc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad2, kUcs2));
    char32_t ucs4AsUcs2Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad3, kUcs2));
    char32_t ucs4AsUcs2Bad4[] = { 0x101, 0x106, 0x163, 0xa0, 0x1290382, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad4, kUcs2));
}

void StringTestFixture::testIsUcs4AsUtf16()
{
    char32_t ucs4AsUtf16Good[] = { 0x101, 0x106, 0x163, 0xa0, 0xbf29d, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs4(ucs4AsUtf16Good, kUtf16));
    char32_t ucs4AsUtf16Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUtf16Bad1, kUtf16));
    char32_t ucs4AsUtf16Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUtf16Bad2, kUtf16));
    char32_t ucs4AsUtf16Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUtf16Bad3, kUtf16));
    char32_t ucs4AsUcs2Bad4[] = { 0x101, 0x106, 0x163, 0xa0, 0x1290382, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad4, kUtf16));
}

void StringTestFixture::testIsUcs4AsUcs4()
{
    char32_t ucs4AsUcs4Good[] = { 0x101, 0x106, 0x163, 0xa0, 0xbf29d, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs4(ucs4AsUcs4Good, kUcs4));
    char32_t ucs4AsUcs4Bad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs4Bad1, kUcs4));
    char32_t ucs4AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad2, kUcs4));
    char32_t ucs4AsUcs4Bad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs4Bad3, kUcs4));
    char32_t ucs4AsUcs2Good2[] = { 0x101, 0x106, 0x163, 0xa0, 0x1290382, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs4(ucs4AsUcs2Good2, kUcs4));
}

void StringTestFixture::testIsUcs4AsUnicode()
{
    char32_t ucs4AsUnicodeGood[] = { 0x101, 0x106, 0x163, 0xa0, 0xbf29d, 0x144, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(isUcs4(ucs4AsUnicodeGood, kUnicode));
    char32_t ucs4AsUnicodeBad1[] = { 0x101, 0x106, 0x163, 0xa0, 0xd800, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUnicodeBad1, kUnicode));
    char32_t ucs4AsUcs2Bad2[] = { 0x101, 0x106, 0x163, 0xa0, 0xd833, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad2, kUnicode));
    char32_t ucs4AsUnicodeBad3[] = { 0x101, 0x106, 0x163, 0xa0, 0xdc29, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUnicodeBad3, kUnicode));
    char32_t ucs4AsUcs2Bad4[] = { 0x101, 0x106, 0x163, 0xa0, 0x1290382, 0x151, 0x175, 0xa0, 0x2026, 0 };
    CPPUNIT_ASSERT(!isUcs4(ucs4AsUcs2Bad4, kUnicode));
}

void StringTestFixture::testToUtf8From16Bit()
{
    // test null Ucs2
    string ucs2ToUtf8FromNull(toUtf8(static_cast<char16_t*>(nullptr)));
    CPPUNIT_ASSERT(ucs2ToUtf8FromNull.empty());
    char16_t ucs2ToUtf8Empty[] = { 0 };
    ucs2ToUtf8FromNull = toUtf8(ucs2ToUtf8Empty);
    CPPUNIT_ASSERT(ucs2ToUtf8FromNull.empty());

    // test good Ucs2 -- or UTF-16 with no escaped stuff
    char16_t ucs2Good[] = { 0x1234, 0x2345, 0x3456, 0x4567, 0x5678, 0x6789, 0};
    string ucs2GoodToUtf8(toUtf8(ucs2Good));
    char encoded0[] = "\xe1\x88\xb4\xe2\x8d\x85\xe3\x91\x96\xe4\x95\xa7\xe5\x99\xb8\xe6\x9e\x89";
    CPPUNIT_ASSERT_EQUAL(0, strcmp(encoded0, ucs2GoodToUtf8.c_str()));

    // test 16-bit that includes escaped (good)
    char16_t utf16Good[] = { 'H', 'e', 'l', 'l', 'o', '!', ' ', 0xd800, 0xdc00, 0 };
    string utf16GoodUtf8(toUtf8(utf16Good));
    char encoded1[] = "Hello! \xf0\x90\x80\x80";
    CPPUNIT_ASSERT_EQUAL(0, strcmp(encoded1, utf16GoodUtf8.c_str()));
    // test 16-bit that includes half-escaped (bad)
    char16_t utf16Bad[] = { 'H', 'e', 'l', 'l', 'o', '!', ' ', 0xd800, ' ', 0 };
    string utf16BadUtf8(toUtf8(utf16Bad));
    CPPUNIT_ASSERT(utf16BadUtf8.empty());
}

void StringTestFixture::testToUtf8From32Bit()
{
    // test null Ucs4
    string ucs4ToUtf8FromNull(toUtf8(static_cast<char32_t*>(nullptr)));
    CPPUNIT_ASSERT(ucs4ToUtf8FromNull.empty());
    char32_t ucs4ToUtf8Empty[] = { 0 };
    ucs4ToUtf8FromNull = toUtf8(ucs4ToUtf8Empty);
    CPPUNIT_ASSERT(ucs4ToUtf8FromNull.empty());

    // test good Ucs4
    char32_t ucs4Good[] = { 0x12342345, 0x34564567, 0x56786789, 'f', 'o', 'o', ' ', 'f', 'o', 'o', ' ', 'f', 'o', 'o', 0};
    string ucs4GoodToUtf8(toUtf8(ucs4Good));
    char encoded0[] = "\xfc\x92\x8d\x82\x8d\x85\xfc\xb4\x95\xa4\x95\xa7\xfd\x96\x9e\x86\x9e\x89" "foo foo foo";
    CPPUNIT_ASSERT_EQUAL(0, strcmp(encoded0, ucs4GoodToUtf8.c_str()));

    // test 32-bit that includes escaped (bad)
    char32_t ucs4BadEscaped0[] = { 'P', 'l', 'u', 0xd800, 0xdc00, 'g', 'h', '!', 0 };
    CPPUNIT_ASSERT(toUtf8(ucs4BadEscaped0).empty());
    // test 32-bit that includes half-escaped (bad)
    char32_t ucs4BadEscaped1[] = { 'P', 'l', 'u', 0xd800, 'g', 'h', '!', 0 };
    CPPUNIT_ASSERT(toUtf8(ucs4BadEscaped1).empty());
    char32_t ucs4BadEscaped2[] = { 'P', 'l', 'u', 'g', 'h', '!', 0xd800, 0 };
    CPPUNIT_ASSERT(toUtf8(ucs4BadEscaped2).empty());
    // test 32-bit that includes values in all ranges (good)
    char32_t ucs4ToUtf8Sampler[] = { 'A', 0x320, 0x1020, 0x10200, 0x1020304, 0x40506070, 0 };
    string ucs4SamplerAsUtf8 = toUtf8(ucs4ToUtf8Sampler);
    char samplerCheck[] = "A\xcc\xa0\xe1\x80\xa0\xf0\x90\x88\x80\xf8\x84\x82\x80\xb0\xfd\x80\x94\x86\x81\xb0";
    CPPUNIT_ASSERT_EQUAL(strlen(samplerCheck), ucs4SamplerAsUtf8.size());
}

void StringTestFixture::testToUcs2From8Bit()
{
    // test good 7-bit
    char src7bit[] = "Now is the time for all good men to come to the aid of the party.";
    char16_t v7bit[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't', 'i',
                         'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ', 'g', 'o',
                         'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ', 'c', 'o', 'm',
                         'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ', 'a', 'i', 'd', ' ',
                         'o', 'f', ' ', 't', 'h', 'e', ' ', 'p', 'a', 'r', 't', 'y', '.',
                         0 };
    ucs2String dest7bit(toUcs2(src7bit));
    CPPUNIT_ASSERT_EQUAL(strlen(src7bit), dest7bit.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v7bit, dest7bit.c_str(), sizeof(v7bit)));

    // test good 2-byte escapes
    char src2ByteEscapes[] = "Bevor die Welt geschaffen w\xc3\xbcrde war das Wort schon da.";
    char16_t v2ByteEscapes[] = { 'B', 'e', 'v', 'o', 'r', ' ', 'd', 'i', 'e', ' ', 'W', 'e',
                                 'l', 't', ' ', 'g', 'e', 's', 'c', 'h', 'a', 'f', 'f', 'e',
                                 'n', ' ', 'w', 0xfc,'r', 'd', 'e', ' ', 'w', 'a', 'r', ' ',
                                 'd', 'a', 's', ' ', 'W', 'o', 'r', 't', ' ', 's', 'c', 'h',
                                 'o', 'n', ' ', 'd', 'a', '.', 0 };
    ucs2String dest2ByteEscapes(toUcs2(src2ByteEscapes));
    CPPUNIT_ASSERT_EQUAL(strlen(src2ByteEscapes) - 1, dest2ByteEscapes.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v2ByteEscapes, dest2ByteEscapes.c_str(), sizeof(v2ByteEscapes)));

    // test good 3-byte escapes
    char src3ByteEscapes[] = "Oh never \xe4\x85\x96 mind.";
    char16_t v3ByteEscapes[] = { 'O', 'h', ' ', 'n', 'e', 'v', 'e', 'r', ' ', 0x4156,
                                 ' ', 'm', 'i', 'n', 'd', '.', 0 };
    ucs2String dest3ByteEscapes(toUcs2(src3ByteEscapes));
    CPPUNIT_ASSERT_EQUAL(strlen(src3ByteEscapes) - 2, dest3ByteEscapes.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v3ByteEscapes, dest3ByteEscapes.c_str(), sizeof(v3ByteEscapes)));

    // test good (for Utf16) 4-byte escapes (bad)
    char src4ByteEscapes0[] = "Straight UTF-8 to things UTF16 would escape: \xf1\x82\x81\x80";
    CPPUNIT_ASSERT(toUcs2(src4ByteEscapes0).empty());
    char src4ByteEscapes1[] = "UTF-8 of UTF-16 to things UTF16 would escape: \xf0\x90\x80\x80\xf2\x87\xb0\xb0";
    CPPUNIT_ASSERT(toUcs2(src4ByteEscapes1).empty());

    // test good 5- and 6-byte scapes (bad)
    char src5ByteEscapes[] = "5-byte escape: \xf9\xb7\xa6\x95\x84";
    CPPUNIT_ASSERT(toUcs2(src5ByteEscapes).empty());
    char src6ByteEscapes[] = "6-byte escape: \xfd\xb7\xa6\x95\x84\x83";
    CPPUNIT_ASSERT(toUcs2(src6ByteEscapes).empty());

    // test broken escapes (bad)
    char srcBroken2Byte[] = "2-byte \xc0 bad.";
    CPPUNIT_ASSERT(toUcs2(srcBroken2Byte).empty());
    char srcBroken3Byte[] = "3-byte \xe3\x89 bad.";
    CPPUNIT_ASSERT(toUcs2(srcBroken3Byte).empty());
    char srcBroken4Byte[] = "4-byte \xf0\x89\x98 bad.";
    CPPUNIT_ASSERT(toUcs2(srcBroken4Byte).empty());
}

void StringTestFixture::testToUcs2From32Bit()
{
    // test good Ucs4
    char32_t srcGoodUcs4[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0x122, 0x1345, 'i', 'm', 'e', 0 };
    ucs2String goodUcs4(toUcs2(srcGoodUcs4));
    char16_t srcGoodUcs4Check[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0x122, 0x1345, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(srcGoodUcs4Check, goodUcs4.c_str(), goodUcs4.size() + 1));

    // test 32-bit that includes escaped (bad)
    char32_t srcBadUcs40[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0x101345, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT(toUcs2(srcBadUcs40).empty());
    char32_t srcBadUcs41[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0xdbc4, 0xdf45, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT(toUcs2(srcBadUcs41).empty());
    // test 32-bit that includes half-escaped (bad)
    char32_t srcHalfBadUcs4[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                   0xdbc4, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT(toUcs2(srcHalfBadUcs4).empty());

    // test 32-bit that includes values in all ranges (bad)
    char32_t srcSampler[] = { 22, 130, 2100, 32000, 655360, 0x181234, 0x1234567, 0x34445555, 0 };
    CPPUNIT_ASSERT(toUcs2(srcSampler).empty());
}

void StringTestFixture::testToUtf16From8Bit()
{
    // test good 7-bit
    char src7bit[] = "Now is the time for all good men to come to the aid of the party.";
    char16_t v7bit[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't', 'i',
                         'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ', 'g', 'o',
                         'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ', 'c', 'o', 'm',
                         'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ', 'a', 'i', 'd', ' ',
                         'o', 'f', ' ', 't', 'h', 'e', ' ', 'p', 'a', 'r', 't', 'y', '.',
                         0 };
    utf16String dest7bit(toUtf16(src7bit));
    CPPUNIT_ASSERT_EQUAL(strlen(src7bit), dest7bit.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v7bit, dest7bit.c_str(), sizeof(v7bit)));

    // test good 2-byte escapes
    char src2ByteEscapes[] = "Bevor die Welt geschaffen w\xc3\xbcrde war das Wort schon da.";
    char16_t v2ByteEscapes[] = { 'B', 'e', 'v', 'o', 'r', ' ', 'd', 'i', 'e', ' ', 'W', 'e',
                                 'l', 't', ' ', 'g', 'e', 's', 'c', 'h', 'a', 'f', 'f', 'e',
                                 'n', ' ', 'w', 0xfc,'r', 'd', 'e', ' ', 'w', 'a', 'r', ' ',
                                 'd', 'a', 's', ' ', 'W', 'o', 'r', 't', ' ', 's', 'c', 'h',
                                 'o', 'n', ' ', 'd', 'a', '.', 0 };
    utf16String dest2ByteEscapes(toUtf16(src2ByteEscapes));
    CPPUNIT_ASSERT_EQUAL(strlen(src2ByteEscapes) - 1, dest2ByteEscapes.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v2ByteEscapes, dest2ByteEscapes.c_str(), sizeof(v2ByteEscapes)));

    // test good 3-byte escapes
    char src3ByteEscapes[] = "Oh never \xe4\x85\x96 mind.";
    char16_t v3ByteEscapes[] = { 'O', 'h', ' ', 'n', 'e', 'v', 'e', 'r', ' ', 0x4156,
                                 ' ', 'm', 'i', 'n', 'd', '.', 0 };
    utf16String dest3ByteEscapes(toUtf16(src3ByteEscapes));
    CPPUNIT_ASSERT_EQUAL(strlen(src3ByteEscapes) - 2, dest3ByteEscapes.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v3ByteEscapes, dest3ByteEscapes.c_str(), sizeof(v3ByteEscapes)));

    // test good (for Utf16) 4-byte escapes (good)
    char src4ByteEscapes0[] = "Straight UTF-8 to things UTF16 would escape: \xf1\x82\x81\x80";
    utf16String fourByteScapes0(toUtf16(src4ByteEscapes0));
    char16_t v4ByteEscapes0[] = { 'S', 't', 'r', 'a', 'i', 'g', 'h', 't', ' ', 'U',
                                  'T', 'F', '-', '8', ' ', 't', 'o', ' ', 't', 'h',
                                  'i', 'n', 'g', 's', ' ', 'U', 'T', 'F', '1', '6',
                                  ' ', 'w', 'o', 'u', 'l', 'd', ' ', 'e', 's', 'c',
                                  'a', 'p', 'e', ':', ' ', 0xd8c8, 0xdc40, 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v4ByteEscapes0, fourByteScapes0.c_str(), sizeof(v4ByteEscapes0)));
    char src4ByteEscapes1[] = "UTF-8 of UTF-16 to things UTF16 would escape: \xf0\x90\x80\x80\xf2\x87\xb0\xb0";
    utf16String fourByteScapes1(toUtf16(src4ByteEscapes1));
    char16_t v4ByteEscapes1[] = { 'U', 'T', 'F', '-', '8', ' ', 'o', 'f', ' ', 'U',
                                  'T', 'F', '-', '1', '6', ' ', 't', 'o', ' ', 't',
                                  'h', 'i', 'n', 'g', 's', ' ', 'U', 'T', 'F', '1',
                                  '6', ' ', 'w', 'o', 'u', 'l', 'd', ' ', 'e', 's',
                                  'c', 'a', 'p', 'e', ':', ' ', 0xd800, 0xdc00, 0xd9df, 0xdc30, 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v4ByteEscapes1, fourByteScapes1.c_str(), sizeof(v4ByteEscapes1)));

    // test bad (for Utf16) 4-byte escapes (bad)
    char src4ByteBadEscape[] = "You can't switch this: \xf4\x90\x80\x80";
    CPPUNIT_ASSERT(toUtf16(src4ByteBadEscape).empty());
    // test good 3-byte escapes that lead to bad Utf16 escapes
    char srcBad3Byte[] = "Convert \xed\xa0\x80.";
    CPPUNIT_ASSERT(toUtf16(srcBad3Byte).empty());
    // test good 5- and 6-byte scapes (bad)
    char src5ByteEscapes[] = "5-byte escape: \xf9\xb7\xa6\x95\x84";
    CPPUNIT_ASSERT(toUtf16(src5ByteEscapes).empty());
    char src6ByteEscapes[] = "6-byte escape: \xfd\xb7\xa6\x95\x84\x83";
    CPPUNIT_ASSERT(toUtf16(src6ByteEscapes).empty());
    // test broken escapes (bad)
    char srcBroken2Byte[] = "2-byte \xc0 bad.";
    CPPUNIT_ASSERT(toUtf16(srcBroken2Byte).empty());
    char srcBroken3Byte[] = "3-byte \xe3\x89 bad.";
    CPPUNIT_ASSERT(toUtf16(srcBroken3Byte).empty());
    char srcBroken4Byte[] = "4-byte \xf0\x89\x98 bad.";
    CPPUNIT_ASSERT(toUtf16(srcBroken4Byte).empty());
}

void StringTestFixture::testToUtf16From32Bit()
{
    // test good Ucs4
    char32_t srcGoodUtf16[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0x122, 0x1345, 'i', 'm', 'e', 0 };
    utf16String goodUtf16(toUtf16(srcGoodUtf16));
    char16_t srcGoodUtf16Check[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0x122, 0x1345, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(srcGoodUtf16Check, goodUtf16.c_str(), goodUtf16.size() + 1));

    // test 32-bit that includes escaped (good)
    char32_t srcUtf16X0[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0x101345, 'i', 'm', 'e', 0 };
    utf16String goodUtf16X0(toUtf16(srcUtf16X0));
    char16_t vUtf16X0[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0xdbc4, 0xdf45, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(vUtf16X0, goodUtf16X0.c_str(), sizeof(vUtf16X0)));
    char32_t srcUtf16X1[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0xdbc4, 0xdf45, 'i', 'm', 'e', 0 };
    utf16String goodUtf16X1(toUtf16(srcUtf16X0));
    char16_t vUtf16X1[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                               0xdbc4, 0xdf45, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(vUtf16X1, goodUtf16X1.c_str(), sizeof(vUtf16X1)));

    // test 32-bit that includes half-escaped (bad)
    char32_t srcHalfBadUtf16[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't',
                                   0xdbc4, 'i', 'm', 'e', 0 };
    CPPUNIT_ASSERT(toUcs2(srcHalfBadUtf16).empty());

    // test 32-bit that includes values in all ranges (bad)
    char32_t srcSampler[] = { 22, 130, 2100, 32000, 655360, 0x181234, 0x1234567, 0x34445555, 0 };
    CPPUNIT_ASSERT(toUcs2(srcSampler).empty());
}

void StringTestFixture::testToUcs4From8Bit()
{
    // test good 7-bit
    char src7bit[] = "Now is the time for all good men to come to the aid of the party.";
    char32_t v7bit[] = { 'N', 'o', 'w', ' ', 'i', 's', ' ', 't', 'h', 'e', ' ', 't', 'i',
                         'm', 'e', ' ', 'f', 'o', 'r', ' ', 'a', 'l', 'l', ' ', 'g', 'o',
                         'o', 'd', ' ', 'm', 'e', 'n', ' ', 't', 'o', ' ', 'c', 'o', 'm',
                         'e', ' ', 't', 'o', ' ', 't', 'h', 'e', ' ', 'a', 'i', 'd', ' ',
                         'o', 'f', ' ', 't', 'h', 'e', ' ', 'p', 'a', 'r', 't', 'y', '.',
                         0 };
    ucs4String dest7bit(toUcs4(src7bit));
    CPPUNIT_ASSERT_EQUAL(strlen(src7bit), dest7bit.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v7bit, dest7bit.c_str(), sizeof(v7bit)));

    // test good 2-byte escapes
    char src2ByteEscapes[] = "Bevor die Welt geschaffen w\xc3\xbcrde war das Wort schon da.";
    char32_t v2ByteEscapes[] = { 'B', 'e', 'v', 'o', 'r', ' ', 'd', 'i', 'e', ' ', 'W', 'e',
                                 'l', 't', ' ', 'g', 'e', 's', 'c', 'h', 'a', 'f', 'f', 'e',
                                 'n', ' ', 'w', 0xfc,'r', 'd', 'e', ' ', 'w', 'a', 'r', ' ',
                                 'd', 'a', 's', ' ', 'W', 'o', 'r', 't', ' ', 's', 'c', 'h',
                                 'o', 'n', ' ', 'd', 'a', '.', 0 };
    ucs4String dest2ByteEscapes(toUcs4(src2ByteEscapes));
    CPPUNIT_ASSERT_EQUAL(strlen(src2ByteEscapes) - 1, dest2ByteEscapes.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v2ByteEscapes, dest2ByteEscapes.c_str(), sizeof(v2ByteEscapes)));

    // test good 3-byte escapes
    char src3ByteEscapes[] = "Oh never \xe4\x85\x96 mind.";
    char32_t v3ByteEscapes[] = { 'O', 'h', ' ', 'n', 'e', 'v', 'e', 'r', ' ', 0x4156,
                                 ' ', 'm', 'i', 'n', 'd', '.', 0 };
    ucs4String dest3ByteEscapes(toUcs4(src3ByteEscapes));
    CPPUNIT_ASSERT_EQUAL(strlen(src3ByteEscapes) - 2, dest3ByteEscapes.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v3ByteEscapes, dest3ByteEscapes.c_str(), sizeof(v3ByteEscapes)));

    // test good 3-byte escapes that lead to good Utf16 escapes
    // test good 3-byte escapes that lead to bad Utf16 escapes

    // test good (for Utf16) 4-byte escapes (good)
    char src4ByteEscapes0[] = "Straight UTF-8 to things UTF16 would escape: \xf1\x82\x81\x80";
    ucs4String fourByteScapes0(toUcs4(src4ByteEscapes0));
    char32_t v4ByteEscapes0[] = { 'S', 't', 'r', 'a', 'i', 'g', 'h', 't', ' ', 'U',
                                  'T', 'F', '-', '8', ' ', 't', 'o', ' ', 't', 'h',
                                  'i', 'n', 'g', 's', ' ', 'U', 'T', 'F', '1', '6',
                                  ' ', 'w', 'o', 'u', 'l', 'd', ' ', 'e', 's', 'c',
                                  'a', 'p', 'e', ':', ' ', 0x42040, 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v4ByteEscapes0, fourByteScapes0.c_str(), sizeof(v4ByteEscapes0)));
    char src4ByteEscapes1[] = "UTF-8 of UTF-16 to things UTF16 would escape: \xf0\x90\x80\x80\xf2\x87\xb0\xb0";
    ucs4String fourByteScapes1(toUcs4(src4ByteEscapes1));
    char32_t v4ByteEscapes1[] = { 'U', 'T', 'F', '-', '8', ' ', 'o', 'f', ' ', 'U',
                                  'T', 'F', '-', '1', '6', ' ', 't', 'o', ' ', 't',
                                  'h', 'i', 'n', 'g', 's', ' ', 'U', 'T', 'F', '1',
                                  '6', ' ', 'w', 'o', 'u', 'l', 'd', ' ', 'e', 's',
                                  'c', 'a', 'p', 'e', ':', ' ', 0x10000, 0x87c30, 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v4ByteEscapes1, fourByteScapes1.c_str(), sizeof(v4ByteEscapes1)));
    char src4ByteEscape[] = "You can't switch this: \xf4\x90\x80\x80";
    char32_t v4ByteEscape[] = { 'Y', 'o', 'u', ' ', 'c', 'a', 'n', '\'', 't', ' ', 's', 'w',
                                'i', 't', 'c', 'h', ' ', 't', 'h', 'i', 's', ':', ' ', 0x110000, 0 };
    ucs4String dest4ByteEscape(toUcs4(src4ByteEscape));
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v4ByteEscape, dest4ByteEscape.c_str(), sizeof(v4ByteEscape)));

    // test good 5- and 6-byte scapes (good)
    char src5ByteEscape[] = "5-byte escape: \xf9\xb7\xa6\x95\x84";
    ucs4String dest5ByteEscape(toUcs4(src5ByteEscape));
    char32_t v5ByteEscape[] = { '5', '-', 'b', 'y', 't', 'e', ' ', 'e', 's', 'c', 'a', 'p',
                                'e', ':', ' ', 0x1de6544, 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v5ByteEscape, dest5ByteEscape.c_str(), sizeof(v5ByteEscape)));

    char src6ByteEscape[] = "6-byte escape: \xfd\xb7\xa6\x95\x84\x83";
    ucs4String dest6ByteEscape(toUcs4(src6ByteEscape));
    char32_t v6ByteEscape[] = { '6', '-', 'b', 'y', 't', 'e', ' ', 'e', 's', 'c', 'a', 'p',
                                'e', ':', ' ', 0x77995103, 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(v6ByteEscape, dest6ByteEscape.c_str(), sizeof(v6ByteEscape)));

    // test good 3-byte escapes that lead to bad Utf16 escapes
    char srcBad3Byte[] = "Convert \xed\xa0\x80.";
    CPPUNIT_ASSERT(toUcs4(srcBad3Byte).empty());

    // test broken escapes (bad)
    char srcBroken2Byte[] = "2-byte \xc0 bad.";
    CPPUNIT_ASSERT(toUcs4(srcBroken2Byte).empty());
    char srcBroken3Byte[] = "3-byte \xe3\x89 bad.";
    CPPUNIT_ASSERT(toUcs4(srcBroken3Byte).empty());
    char srcBroken4Byte[] = "4-byte \xf0\x89\x98 bad.";
    CPPUNIT_ASSERT(toUcs4(srcBroken4Byte).empty());
    char srcBroken5Byte[] = "5-byte \xf8\x89\x98\xa8 bad.";
    CPPUNIT_ASSERT(toUcs4(srcBroken5Byte).empty());
    char srcBroken6Byte[] = "6-byte \xfc\x89\x98\xa8\xb3 bad.";
    CPPUNIT_ASSERT(toUcs4(srcBroken6Byte).empty());
}

void StringTestFixture::testToUcs4From16Bit()
{
    // test good Ucs2
    char16_t goodUcs2[] = { 'N', 0xf6, 'w', ' ', 'i', 's', ' ', 0x3b8, 'e', ' ', 't', 'i', 'm', 'e', '.', 0 };
    ucs4String fromGoodUcs2(toUcs4(goodUcs2));
    char32_t vGoodUcs2[] = { 'N', 0xf6, 'w', ' ', 'i', 's', ' ', 0x3b8, 'e', ' ', 't', 'i', 'm', 'e', '.', 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(vGoodUcs2, fromGoodUcs2.c_str(), sizeof(vGoodUcs2)));

    // test good Utf16:
    // test 16-bit that includes escaped (good)
    char16_t goodUtf16[] = { 'N', 0xf6, 'w', ' ', 'i', 's', ' ', 0x3b8, 'e', ' ', 0xd800, 0xdfff, 'i', 'm', 'e', '.', 0 };
    ucs4String fromGoodUtf16(toUcs4(goodUtf16));
    char32_t vGoodUtf16[] = { 'N', 0xf6, 'w', ' ', 'i', 's', ' ', 0x3b8, 'e', ' ', 0x010377, 'i', 'm', 'e', '.', 0 };
    CPPUNIT_ASSERT_EQUAL(0, memcmp(vGoodUcs2, fromGoodUcs2.c_str(), sizeof(vGoodUcs2)));
    // test 16-bit that includes half-escaped (bad)
    char16_t badUtf16_0[] = { 'N', 0xf6, 'w', ' ', 'i', 's', ' ', 0x3b8, 'e', ' ', 0xdfff, 'i', 'm', 'e', '.', 0 };
    char16_t badUtf16_1[] = { 'N', 0xf6, 'w', ' ', 'i', 's', ' ', 0x3b8, 'e', ' ', 0xd800, 'i', 'm', 'e', '.', 0 };
    CPPUNIT_ASSERT(toUcs4(badUtf16_0).empty());
    CPPUNIT_ASSERT(toUcs4(badUtf16_1).empty());
}

void StringTestFixture::testGoodUtf8ToBadValues()
{
    // test lower boundary of good 2-byte escape values, both is... and to...
    char bad2_0[] = "I'm bad: \xc0\x80."; CPPUNIT_ASSERT(!isUtf8(bad2_0)); CPPUNIT_ASSERT(toUcs4(bad2_0).empty());
    char bad2_1[] = "I'm bad: \xc1\xbf."; CPPUNIT_ASSERT(!isUtf8(bad2_1)); CPPUNIT_ASSERT(toUcs4(bad2_1).empty());
    char good2[] = "I'm good: \xc2\x80."; CPPUNIT_ASSERT(isUtf8(good2)); CPPUNIT_ASSERT(!toUcs4(good2).empty());

    // test lower boundary of good 3-byte escape values, both is... and to...
    char bad3_0[] = "I'm bad: \xe0\x80\x80."; CPPUNIT_ASSERT(!isUtf8(bad3_0)); CPPUNIT_ASSERT(toUcs4(bad3_0).empty());
    char bad3_1[] = "I'm bad: \xe0\x9f\xbf."; CPPUNIT_ASSERT(!isUtf8(bad3_1)); CPPUNIT_ASSERT(toUcs4(bad3_1).empty());
    char good3[] = "I'm good: \xe0\xa0\x80."; CPPUNIT_ASSERT(isUtf8(good3)); CPPUNIT_ASSERT(!toUcs4(good3).empty());

    // test lower boundary of good 4-byte escape values, both is... and to...
    char bad4_0[] = "I'm bad: \xf0\x80\x80\x80."; CPPUNIT_ASSERT(!isUtf8(bad4_0)); CPPUNIT_ASSERT(toUcs4(bad4_0).empty());
    char bad4_1[] = "I'm bad: \xf0\x8f\xbf\xbf."; CPPUNIT_ASSERT(!isUtf8(bad4_1)); CPPUNIT_ASSERT(toUcs4(bad4_1).empty());
    char good4[] = "I'm good: \xf0\x90\x80\x80."; CPPUNIT_ASSERT(isUtf8(good4)); CPPUNIT_ASSERT(!toUcs4(good4).empty());

    // test lower boundary of good 5-byte escape values, both is... and to...
    char bad5_0[] = "I'm bad: \xf8\x80\x80\x80\x80."; CPPUNIT_ASSERT(!isUtf8(bad5_0)); CPPUNIT_ASSERT(toUcs4(bad5_0).empty());
    char bad5_1[] = "I'm bad: \xf8\x87\xbf\xbf\xbf."; CPPUNIT_ASSERT(!isUtf8(bad5_1)); CPPUNIT_ASSERT(toUcs4(bad5_1).empty());
    char good5[] = "I'm good: \xf8\x88\x80\x80\x80."; CPPUNIT_ASSERT(isUtf8(good5)); CPPUNIT_ASSERT(!toUcs4(good5).empty());

    // test lower boundary of good 6-byte escape values, both is... and to...
    char bad6_0[] = "I'm bad: \xfc\x80\x80\x80\x80\x80."; CPPUNIT_ASSERT(!isUtf8(bad6_0)); CPPUNIT_ASSERT(toUcs4(bad6_0).empty());
    char bad6_1[] = "I'm bad: \xfc\x83\xbf\xbf\xbf\xbf."; CPPUNIT_ASSERT(!isUtf8(bad6_1)); CPPUNIT_ASSERT(toUcs4(bad6_1).empty());
    char good6[] = "I'm good: \xfc\x84\x80\x80\x80\x80."; CPPUNIT_ASSERT(isUtf8(good6)); CPPUNIT_ASSERT(!toUcs4(good6).empty());
}

void StringTestFixture::testUnicodeLengthUtf8()
{
    char easy[] = "Now is the time for all good men to come to the aid of the party.";
    CPPUNIT_ASSERT_EQUAL(65u, unicodeLength(easy));
    CPPUNIT_ASSERT_EQUAL(60u, unicodeLength(easy, 60));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(easy, 66));

    // test a good UTF-8 string and then successive truncations of the same.
    char medium[] = "Jetzt gab es wieder Sch\xc3\xbcsse.";
    CPPUNIT_ASSERT_EQUAL(28u, unicodeLength(medium));
    CPPUNIT_ASSERT_EQUAL(28u, unicodeLength(medium, 29));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(medium, 30));
    CPPUNIT_ASSERT_EQUAL(27u, unicodeLength(medium, 28));   // Jetzt gab es wieder Sch\xc3\xbcsse
    CPPUNIT_ASSERT_EQUAL(26u, unicodeLength(medium, 27));   // Jetzt gab es wieder Sch\xc3\xbcss
    CPPUNIT_ASSERT_EQUAL(25u, unicodeLength(medium, 26));   // Jetzt gab es wieder Sch\xc3\xbcs
    CPPUNIT_ASSERT_EQUAL(24u, unicodeLength(medium, 25));   // Jetzt gab es wieder Sch\xc3\xbc
    CPPUNIT_ASSERT_EQUAL(23u, unicodeLength(medium, 24));   // Jetzt gab es wieder Sch\xc3
    CPPUNIT_ASSERT_EQUAL(23u, unicodeLength(medium, 23));   // Jetzt gab es wieder Sch
    CPPUNIT_ASSERT_EQUAL(22u, unicodeLength(medium, 22));   // Jetzt gab es wieder Sc
    CPPUNIT_ASSERT_EQUAL(21u, unicodeLength(medium, 21));   // Jetzt gab es wieder S
    CPPUNIT_ASSERT_EQUAL(20u, unicodeLength(medium, 20));   // Jetzt gab es wieder 

    char harder[] = "\xd7\x91\xd7\xa8\xd7\xa9\xd7\x99\xd7\x94 \xd7\x91\xd7\xa8\xd7"
                    "\x90 \xd7\x90\xd7\x9c\xd7\x95\xd7\x94\xd7\x99\xd7\x9d \xd7"
                    "\x90\xd7\xaa \xd7\x94\xd7\xa9\xd7\x9e\xd7\x99\xd7\x9d \xd7"
                    "\x95\xd7\x90\xd7\xaa \xd7\x94\xd7\x90\xd7\xa8\xd7\xa5";
    CPPUNIT_ASSERT_EQUAL(34u, unicodeLength(harder));
    CPPUNIT_ASSERT_EQUAL(34u, unicodeLength(harder, 62));   // \x95\xd7\x90\xd7\xaa \xd7\x94\xd7\x90\xd7\xa8\xd7\xa5
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(harder, 63));
    CPPUNIT_ASSERT_EQUAL(33u, unicodeLength(harder, 61));   // \x95\xd7\x90\xd7\xaa \xd7\x94\xd7\x90\xd7\xa8\xd7
    CPPUNIT_ASSERT_EQUAL(33u, unicodeLength(harder, 60));   // \x95\xd7\x90\xd7\xaa \xd7\x94\xd7\x90\xd7\xa8
    CPPUNIT_ASSERT_EQUAL(32u, unicodeLength(harder, 59));   // \x95\xd7\x90\xd7\xaa \xd7\x94\xd7\x90\xd7
    CPPUNIT_ASSERT_EQUAL(32u, unicodeLength(harder, 58));   // \x95\xd7\x90\xd7\xaa \xd7\x94\xd7\x90
    CPPUNIT_ASSERT_EQUAL(31u, unicodeLength(harder, 57));   // \x95\xd7\x90\xd7\xaa \xd7\x94\xd7
    CPPUNIT_ASSERT_EQUAL(31u, unicodeLength(harder, 56));   // \x95\xd7\x90\xd7\xaa \xd7\x94
    CPPUNIT_ASSERT_EQUAL(30u, unicodeLength(harder, 55));   // \x95\xd7\x90\xd7\xaa \xd7
    CPPUNIT_ASSERT_EQUAL(30u, unicodeLength(harder, 54));   //"\x95\xd7\x90\xd7\xaa "
    CPPUNIT_ASSERT_EQUAL(29u, unicodeLength(harder, 53));   //"\x95\xd7\x90\xd7\xaa"
    CPPUNIT_ASSERT_EQUAL(28u, unicodeLength(harder, 52));   // \x95\xd7\x90\xd7
    CPPUNIT_ASSERT_EQUAL(28u, unicodeLength(harder, 51));   // \x95\xd7\x90
    CPPUNIT_ASSERT_EQUAL(27u, unicodeLength(harder, 50));   // \x95\xd7

    char morish[] = "\xe5\xa4\xaa\xe5\x88\x9d\xe6\x9c\x89\xe9\x81\x93\xef\xbc\x8c\xe9"
                    "\x81\x93\xe8\x88\x87\xe7\xa5\x9e\xe5\x90\x8c\xe5\x9c\xa8\xef\xbc"
                    "\x8c\xe9\x81\x93\xe5\xb0\xb1\xe6\x98\xaf\xe7\xa5\x9e\xe3\x80\x82"
                    "\x0a\xe9\x80\x99\xe9\x81\x93\xe5\xa4\xaa\xe5\x88\x9d\xe8\x88\x87"
                    "\xe7\xa5\x9e\xe5\x90\x8c\xe5\x9c\xa8\xe3\x80\x82\x0a\xe8\x90\xac"
                    "\xe7\x89\xa9\xe6\x98\xaf\xe8\x97\x89\xe8\x91\x97\xe4\xbb\x96\xe9"
                    "\x80\xa0\xe7\x9a\x84\xef\xbc\x9b\xe5\x87\xa1\xe8\xa2\xab\xe9\x80"
                    "\xa0\xe7\x9a\x84\xef\xbc\x8c\xe6\xb2\x92\xe6\x9c\x89\xe4\xb8\x80"
                    "\xe6\xa8\xa3\xe4\xb8\x8d\xe6\x98\xaf\xe8\x97\x89\xe8\x91\x97\xe4"
                    "\xbb\x96\xe9\x80\xa0\xe7\x9a\x84\xe3\x80\x82\x0a\xe7\x94\x9f\xe5"
                    "\x91\xbd\xe5\x9c\xa8\xe4\xbb\x96\xe8\xa3\xa1\xe9\xa0\xad\xef\xbc"
                    "\x8c\xe9\x80\x99\xe7\x94\x9f\xe5\x91\xbd\xe5\xb0\xb1\xe6\x98\xaf"
                    "\xe4\xba\xba\xe7\x9a\x84\xe5\x85\x89\xe3\x80\x82\x0a\xe5\x85\x89"
                    "\xe7\x85\xa7\xe5\x9c\xa8\xe9\xbb\x91\xe6\x9a\x97\xe8\xa3\xa1\xef"
                    "\xbc\x8c\xe9\xbb\x91\xe6\x9a\x97\xe5\x8d\xbb\xe4\xb8\x8d\xe6\x8e"
                    "\xa5\xe5\x8f\x97\xe5\x85\x89\xe3\x80\x82";
    CPPUNIT_ASSERT_EQUAL(86u, unicodeLength(morish));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(morish, 251));
    CPPUNIT_ASSERT_EQUAL(86u, unicodeLength(morish, 250));

    CPPUNIT_ASSERT_EQUAL(86u, unicodeLength(morish));
    CPPUNIT_ASSERT_EQUAL(85u, unicodeLength(morish, 249));
    CPPUNIT_ASSERT_EQUAL(85u, unicodeLength(morish, 248));
    CPPUNIT_ASSERT_EQUAL(85u, unicodeLength(morish, 247));
    CPPUNIT_ASSERT_EQUAL(84u, unicodeLength(morish, 246));
    CPPUNIT_ASSERT_EQUAL(84u, unicodeLength(morish, 245));
    CPPUNIT_ASSERT_EQUAL(84u, unicodeLength(morish, 244));
    CPPUNIT_ASSERT_EQUAL(83u, unicodeLength(morish, 243));
    CPPUNIT_ASSERT_EQUAL(83u, unicodeLength(morish, 242));
    CPPUNIT_ASSERT_EQUAL(83u, unicodeLength(morish, 241));
    CPPUNIT_ASSERT_EQUAL(82u, unicodeLength(morish, 240));
    CPPUNIT_ASSERT_EQUAL(82u, unicodeLength(morish, 239));
    CPPUNIT_ASSERT_EQUAL(82u, unicodeLength(morish, 238));
    CPPUNIT_ASSERT_EQUAL(81u, unicodeLength(morish, 237));
    CPPUNIT_ASSERT_EQUAL(81u, unicodeLength(morish, 236));
    CPPUNIT_ASSERT_EQUAL(81u, unicodeLength(morish, 235));
    CPPUNIT_ASSERT_EQUAL(80u, unicodeLength(morish, 234));
    CPPUNIT_ASSERT_EQUAL(80u, unicodeLength(morish, 233));
    CPPUNIT_ASSERT_EQUAL(80u, unicodeLength(morish, 232));
    CPPUNIT_ASSERT_EQUAL(79u, unicodeLength(morish, 231));
    CPPUNIT_ASSERT_EQUAL(79u, unicodeLength(morish, 230));
    CPPUNIT_ASSERT_EQUAL(79u, unicodeLength(morish, 229));
    CPPUNIT_ASSERT_EQUAL(78u, unicodeLength(morish, 228));
    CPPUNIT_ASSERT_EQUAL(78u, unicodeLength(morish, 227));
    CPPUNIT_ASSERT_EQUAL(78u, unicodeLength(morish, 226));
    CPPUNIT_ASSERT_EQUAL(77u, unicodeLength(morish, 225));
    CPPUNIT_ASSERT_EQUAL(77u, unicodeLength(morish, 224));
    CPPUNIT_ASSERT_EQUAL(77u, unicodeLength(morish, 223));
    CPPUNIT_ASSERT_EQUAL(76u, unicodeLength(morish, 222));
    CPPUNIT_ASSERT_EQUAL(76u, unicodeLength(morish, 221));
    CPPUNIT_ASSERT_EQUAL(76u, unicodeLength(morish, 220));
    CPPUNIT_ASSERT_EQUAL(75u, unicodeLength(morish, 219));
    CPPUNIT_ASSERT_EQUAL(75u, unicodeLength(morish, 218));
    CPPUNIT_ASSERT_EQUAL(75u, unicodeLength(morish, 217));
    CPPUNIT_ASSERT_EQUAL(74u, unicodeLength(morish, 216));
    CPPUNIT_ASSERT_EQUAL(74u, unicodeLength(morish, 215));
    CPPUNIT_ASSERT_EQUAL(74u, unicodeLength(morish, 214));
    CPPUNIT_ASSERT_EQUAL(73u, unicodeLength(morish, 213));
    CPPUNIT_ASSERT_EQUAL(73u, unicodeLength(morish, 212));
    CPPUNIT_ASSERT_EQUAL(73u, unicodeLength(morish, 211));
    CPPUNIT_ASSERT_EQUAL(72u, unicodeLength(morish, 210));
    CPPUNIT_ASSERT_EQUAL(72u, unicodeLength(morish, 209));
    CPPUNIT_ASSERT_EQUAL(72u, unicodeLength(morish, 208));
    CPPUNIT_ASSERT_EQUAL(71u, unicodeLength(morish, 207));
    CPPUNIT_ASSERT_EQUAL(71u, unicodeLength(morish, 206));
    CPPUNIT_ASSERT_EQUAL(71u, unicodeLength(morish, 205));
    CPPUNIT_ASSERT_EQUAL(70u, unicodeLength(morish, 204));
    CPPUNIT_ASSERT_EQUAL(69u, unicodeLength(morish, 203));
    CPPUNIT_ASSERT_EQUAL(69u, unicodeLength(morish, 202));
    CPPUNIT_ASSERT_EQUAL(69u, unicodeLength(morish, 201));
    CPPUNIT_ASSERT_EQUAL(68u, unicodeLength(morish, 200));

    char asUtf16Good[] = "escape: \xed\xa0\x80\xed\xb0\xb0";
    CPPUNIT_ASSERT_EQUAL(9u, unicodeLength(asUtf16Good));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(asUtf16Good,15));
    CPPUNIT_ASSERT_EQUAL(9u, unicodeLength(asUtf16Good,14));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Good,13));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Good,12));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Good,11));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Good,10));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Good,9));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Good,8));
    CPPUNIT_ASSERT_EQUAL(7u, unicodeLength(asUtf16Good,7));
    CPPUNIT_ASSERT_EQUAL(6u, unicodeLength(asUtf16Good,6));
    CPPUNIT_ASSERT_EQUAL(5u, unicodeLength(asUtf16Good,5));
    CPPUNIT_ASSERT_EQUAL(4u, unicodeLength(asUtf16Good,4));
    CPPUNIT_ASSERT_EQUAL(3u, unicodeLength(asUtf16Good,3));
    CPPUNIT_ASSERT_EQUAL(2u, unicodeLength(asUtf16Good,2));
    CPPUNIT_ASSERT_EQUAL(1u, unicodeLength(asUtf16Good,1));

    char asUtf16Bad[] = "escape: \xed\xb0\xb0";
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(asUtf16Bad));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(asUtf16Bad, 11));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(asUtf16Bad, 10));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Bad, 9));
    CPPUNIT_ASSERT_EQUAL(8u, unicodeLength(asUtf16Bad, 8));
    CPPUNIT_ASSERT_EQUAL(7u, unicodeLength(asUtf16Bad, 7));
    CPPUNIT_ASSERT_EQUAL(6u, unicodeLength(asUtf16Bad, 6));
    CPPUNIT_ASSERT_EQUAL(5u, unicodeLength(asUtf16Bad, 5));
    CPPUNIT_ASSERT_EQUAL(4u, unicodeLength(asUtf16Bad, 4));
    CPPUNIT_ASSERT_EQUAL(3u, unicodeLength(asUtf16Bad, 3));
    CPPUNIT_ASSERT_EQUAL(2u, unicodeLength(asUtf16Bad, 2));
    CPPUNIT_ASSERT_EQUAL(1u, unicodeLength(asUtf16Bad, 1));

    char elevenBit[] = "\xc4\xbd";
    CPPUNIT_ASSERT_EQUAL(1u, unicodeLength(elevenBit));
    char sixteenBit[] = "\xe8\xaf\xb8";
    CPPUNIT_ASSERT_EQUAL(1u, unicodeLength(sixteenBit));
    char eighteenBit[] = "\xf0\x90\x80\x80\xf0\xa0\x80\x80";
    CPPUNIT_ASSERT_EQUAL(2u, unicodeLength(eighteenBit));
    char twentyBit[] = "\xf1\x80\x80\x80\xf2\x80\x80\x80";
    CPPUNIT_ASSERT_EQUAL(2u, unicodeLength(twentyBit));
    char twentyOneBit[] = "\xf4\x80\x80\x80\xf4\x88\x80\x80\xf4\x8c\x80\x80\xf4"
            "\x8e\x80\x80\xf4\x8f\x80\x80\xf4\x8f\xa0\x80\xf4\x8f\xb0\x80\xf4"
            "\x8f\xb8\x80\xf4\x8f\xbc\x80\xf4\x8f\xbe\x80\xf4\x8f\xbf\x80\xf4"
            "\x8f\xbf\xa0\xf4\x8f\xbf\xb0\xf4\x8f\xbf\xb8\xf4\x8f\xbf\xbc\xf4"
            "\x8f\xbf\xbe\xf4\x8f\xbf\xbf\xf4\x90\x80\x80";
    CPPUNIT_ASSERT_EQUAL(17u, unicodeLength(twentyOneBit, 68));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(twentyOneBit, 72));
}

void StringTestFixture::testUnicodeLengthUtf16()
{
    char16_t easy[] = u"Now is the time for all good men to come to the aid of the party.";
    CPPUNIT_ASSERT_EQUAL(65u, unicodeLength(easy));
    CPPUNIT_ASSERT_EQUAL(65u, unicodeLength(easy, 65));
    CPPUNIT_ASSERT_EQUAL(60u, unicodeLength(easy, 60));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(easy, 66));

    char16_t hard[] = u"Now is the time for all good men to come to the aid of the\xd800 party.";
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard, 66));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard, 65));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard, 64));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard, 63));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard, 62));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard, 61));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard, 60));
    CPPUNIT_ASSERT_EQUAL(58u, unicodeLength(hard, 59));
    CPPUNIT_ASSERT_EQUAL(58u, unicodeLength(hard, 58));
    CPPUNIT_ASSERT_EQUAL(57u, unicodeLength(hard, 57));

    char16_t boundaryConditions[] = u"Now is the time for all good men t\xd800\xdeab.";
    CPPUNIT_ASSERT_EQUAL(36u, unicodeLength(boundaryConditions));
    CPPUNIT_ASSERT_EQUAL(36u, unicodeLength(boundaryConditions, 37));
    CPPUNIT_ASSERT_EQUAL(35u, unicodeLength(boundaryConditions, 36));
    CPPUNIT_ASSERT_EQUAL(34u, unicodeLength(boundaryConditions, 35));
    CPPUNIT_ASSERT_EQUAL(34u, unicodeLength(boundaryConditions, 34));
    CPPUNIT_ASSERT_EQUAL(33u, unicodeLength(boundaryConditions, 33));
    CPPUNIT_ASSERT_EQUAL(32u, unicodeLength(boundaryConditions, 32));
    CPPUNIT_ASSERT_EQUAL(31u, unicodeLength(boundaryConditions, 31));
    CPPUNIT_ASSERT_EQUAL(30u, unicodeLength(boundaryConditions, 30));
}

void StringTestFixture::testUnicodeLengthUcs4()
{
    char32_t easy[] = U"Now is the time for all good men to come to the aid of the party.";
    CPPUNIT_ASSERT_EQUAL(65u, unicodeLength(easy));
    CPPUNIT_ASSERT_EQUAL(65u, unicodeLength(easy, 65));
    CPPUNIT_ASSERT_EQUAL(60u, unicodeLength(easy, 60));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(easy, 66));

    char32_t hard1[] = U"Now is the time for all good men to come to the aid of the\xd800 party.";
    char32_t hard2[] = U"Now is the time for all good men to come to the aid of the\xdc00 party.";
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1));         CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2));     
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 66));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 66));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 65));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 65));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 64));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 64));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 63));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 63));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 62));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 62));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 61));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 61));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 60));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 60));
    CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard1, 59));     CPPUNIT_ASSERT_EQUAL(0u, unicodeLength(hard2, 59));
    CPPUNIT_ASSERT_EQUAL(58u, unicodeLength(hard1, 58));    CPPUNIT_ASSERT_EQUAL(58u, unicodeLength(hard2, 58));
    CPPUNIT_ASSERT_EQUAL(57u, unicodeLength(hard1, 57));    CPPUNIT_ASSERT_EQUAL(57u, unicodeLength(hard2, 57));
}

void StringTestFixture::testConvertFromCP1252()
{
    char cp1252Good0[] = "\x80\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8e";
    char cp1252Good0Conv[] = u8"\u20ac\u201a\u0192\u201e\u2026\u2020\u2021\u02c6\u2030\u0160\u2039\u0152\u017d";
    string sp1252Good0AsString(toUtf8(cp1252Good0, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good0AsString.c_str(), cp1252Good0Conv));

    char cp1252Good1[] = "\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9e\x9f";
    char cp1252Good1Conv[] = u8"\u2018\u2019\u201c\u201d\u2022\u2013\u2014\u02dc\u2122\u0161\u203a\u0153\u017e\u0178";
    string sp1252Good1AsString(toUtf8(cp1252Good1, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good1AsString.c_str(), cp1252Good1Conv));

    char cp1252Good2[] = "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf";
    char cp1252Good2Conv[] = u8"\u00a0\u00a1\u00a2\u00a3\u00a4\u00a5\u00a6\u00a7\u00a8\u00a9\u00aa\u00ab\u00ac\u00ad\u00ae\u00af";
    string sp1252Good2AsString(toUtf8(cp1252Good2, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good2AsString.c_str(), cp1252Good2Conv));

    char cp1252Good3[] = "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf";
    char cp1252Good3Conv[] = u8"\u00b0\u00b1\u00b2\u00b3\u00b4\u00b5\u00b6\u00b7\u00b8\u00b9\u00ba\u00bb\u00bc\u00bd\u00be\u00bf";
    string sp1252Good3AsString(toUtf8(cp1252Good3, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good3AsString.c_str(), cp1252Good3Conv));

    char cp1252Good4[] = "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf";
    char cp1252Good4Conv[] = u8"\u00c0\u00c1\u00c2\u00c3\u00c4\u00c5\u00c6\u00c7\u00c8\u00c9\u00ca\u00cb\u00cc\u00cd\u00ce\u00cf";
    string sp1252Good4AsString(toUtf8(cp1252Good4, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good4AsString.c_str(), cp1252Good4Conv));

    char cp1252Good5[] = "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf";
    char cp1252Good5Conv[] = u8"\u00d0\u00d1\u00d2\u00d3\u00d4\u00d5\u00d6\u00d7\u00d8\u00d9\u00da\u00db\u00dc\u00dd\u00de\u00df";
    string sp1252Good5AsString(toUtf8(cp1252Good5, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good5AsString.c_str(), cp1252Good5Conv));

    char cp1252Good6[] = "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef";
    char cp1252Good6Conv[] = u8"\u00e0\u00e1\u00e2\u00e3\u00e4\u00e5\u00e6\u00e7\u00e8\u00e9\u00ea\u00eb\u00ec\u00ed\u00ee\u00ef";
    string sp1252Good6AsString(toUtf8(cp1252Good6, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good6AsString.c_str(), cp1252Good6Conv));

    char cp1252Good7[] = "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";
    char cp1252Good7Conv[] = u8"\u00f0\u00f1\u00f2\u00f3\u00f4\u00f5\u00f6\u00f7\u00f8\u00f9\u00fa\u00fb\u00fc\u00fd\u00fe\u00ff";
    string sp1252Good7AsString(toUtf8(cp1252Good7, kSrcCP1252));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1252Good7AsString.c_str(), cp1252Good7Conv));

    char cp1252Bad[] = "\x81\x8d\x8f\x90\x9d";
    for (auto p = &cp1252Bad[0]; *p; ++p)
    {
        CPPUNIT_ASSERT(toUtf8(p, kSrcCP1252).empty());
    }
}

void StringTestFixture::testConvertFromCP1250()
{
    char cp1250Good0[] = "\x80\x82\x84\x85\x86\x87\x89\x8a\x8b\x8c\x8d\x8e\x8f";
    char cp1250Good0Conv[] = u8"\u20ac\u201a\u201e\u2026\u2020\u2021\u2030\u0160\u2039\u015a\u0164\u017d\u0179";
    string sp1250Good0AsString(toUtf8(cp1250Good0, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good0AsString.c_str(), cp1250Good0Conv));

    char cp1250Good1[] = "\x91\x92\x93\x94\x95\x96\x97\x99\x9a\x9b\x9c\x9d\x9e\x9f";
    char cp1250Good1Conv[] = u8"\u2018\u2019\u201c\u201d\u2022\u2013\u2014\u2122\u0161\u203a\u015b\u0165\u017e\u017a";
    string sp1250Good1AsString(toUtf8(cp1250Good1, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good1AsString.c_str(), cp1250Good1Conv));

    char cp1250Good2[] = "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf";
    char cp1250Good2Conv[] = u8"\u00a0\u02c7\u02d8\u0141\u00a4\u0104\u00a6\u00a7\u00a8\u00a9\u015e\u00ab\u00ac\u00ad\u00ae\u017b";
    string sp1250Good2AsString(toUtf8(cp1250Good2, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good2AsString.c_str(), cp1250Good2Conv));

    char cp1250Good3[] = "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf";
    char cp1250Good3Conv[] = u8"\u00b0\u00b1\u02db\u0142\u00b4\u00b5\u00b6\u00b7\u00b8\u0105\u015f\u00bb\u013d\u02dd\u013e\u017c";
    string sp1250Good3AsString(toUtf8(cp1250Good3, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good3AsString.c_str(), cp1250Good3Conv));

    char cp1250Good4[] = "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf";
    char cp1250Good4Conv[] = u8"\u0154\u00c1\u00c2\u0102\u00c4\u0139\u0106\u00c7\u010c\u00c9\u0118\u00cb\u011a\u00cd\u00ce\u010e";
    string sp1250Good4AsString(toUtf8(cp1250Good4, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good4AsString.c_str(), cp1250Good4Conv));

    char cp1250Good5[] = "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf";
    char cp1250Good5Conv[] = u8"\u0110\u0143\u0147\u00d3\u00d4\u0150\u00d6\u00d7\u0158\u016e\u00da\u0170\u00dc\u00dd\u0162\u00df";
    string sp1250Good5AsString(toUtf8(cp1250Good5, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good5AsString.c_str(), cp1250Good5Conv));

    char cp1250Good6[] = "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef";
    char cp1250Good6Conv[] = u8"\u0155\u00e1\u00e2\u0103\u00e4\u013a\u0107\u00e7\u010d\u00e9\u0119\u00eb\u011b\u00ed\u00ee\u010f";
    string sp1250Good6AsString(toUtf8(cp1250Good6, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good6AsString.c_str(), cp1250Good6Conv));

    char cp1250Good7[] = "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";
    char cp1250Good7Conv[] = u8"\u0111\u0144\u0148\u00f3\u00f4\u0151\u00f6\u00f7\u0159\u016f\u00fa\u0171\u00fc\u00fd\u0163\u02d9";
    string sp1250Good7AsString(toUtf8(cp1250Good7, kSrcCP1250));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(sp1250Good7AsString.c_str(), cp1250Good7Conv));

    char cp1250Bad[] = "\x81\x83\x88\x90\x98";
    for (auto p = &cp1250Bad[0]; *p; ++p)
    {
        CPPUNIT_ASSERT(toUtf8(p, kSrcCP1250).empty());
    }
}

