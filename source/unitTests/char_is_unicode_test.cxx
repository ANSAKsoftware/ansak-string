///////////////////////////////////////////////////////////////////////////
//
// 2015.11.10
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
// char_is_unicode_test.cxx -- unit tests for checking if a character is in
//                             or out
//
///////////////////////////////////////////////////////////////////////////

#include <cppunit/extensions/HelperMacros.h>

#include "string_internal.hxx"
#include <iterator>
#include <iostream>
#include "char_is_unicode_test_data.hxx"

using namespace ansak;
using namespace ansak::internal;
using namespace std;

class IsUnicodeTestFixture : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( IsUnicodeTestFixture );
    CPPUNIT_TEST( testIsControl );
    CPPUNIT_TEST( testIsPrivate );
    CPPUNIT_TEST( testIsWhitespace );
    CPPUNIT_TEST( testIsUnicode8 );
CPPUNIT_TEST_SUITE_END();

public:
    void testIsControl();
    void testIsPrivate();
    void testIsWhitespace();
    void testIsUnicode8();

};

CPPUNIT_TEST_SUITE_REGISTRATION(IsUnicodeTestFixture);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(IsUnicodeTestFixture, "StringClassTests");

void IsUnicodeTestFixture::testIsControl()
{
    // all the control characters are U+0000 .. U+001F and U+0080 to U+009F
    // In the Unicode8Data.txt file their second field (and theirs only) is <control>
    for (char32_t i = 0; i < 32; ++i)
    {
        CPPUNIT_ASSERT(isControlChar(i));
        CPPUNIT_ASSERT(isControlChar(static_cast<char16_t>(i)));
        CPPUNIT_ASSERT(isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 32; i < 127; ++i)
    {
        CPPUNIT_ASSERT(!isControlChar(i));
        CPPUNIT_ASSERT(!isControlChar(static_cast<char16_t>(i)));
        CPPUNIT_ASSERT(!isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 127; i < 160; ++i)
    {
        CPPUNIT_ASSERT(isControlChar(i));
        CPPUNIT_ASSERT(isControlChar(static_cast<char16_t>(i)));
        CPPUNIT_ASSERT(isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 160; i <= 255; ++i)
    {
        CPPUNIT_ASSERT(!isControlChar(i));
        CPPUNIT_ASSERT(!isControlChar(static_cast<char16_t>(i)));
        CPPUNIT_ASSERT(!isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 256; i <= 0xFFFF; ++i)
    {
        CPPUNIT_ASSERT(!isControlChar(i));
        CPPUNIT_ASSERT(!isControlChar(static_cast<char16_t>(i)));
    }
    for (char32_t i = 65536; i < 0x110000; ++i)
    {
        CPPUNIT_ASSERT(!isControlChar(i));
    }
}

void IsUnicodeTestFixture::testIsPrivate()
{
    // all the private characters are U+E000 .. U+F8FF, U+F0000..U+FFFFD and
    // U+100000..U+10FFFD
    // In the Unicode8Data.txt file these ranges are identified that way
    for (char32_t i = 0; i <= 255; ++i)
    {
        CPPUNIT_ASSERT(!isUnicode8Private(i));
        CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char16_t>(i)));
        CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char>(i)));
    }
    for (char32_t i = 256; i < 0xE000; ++i)
    {
        CPPUNIT_ASSERT(!isUnicode8Private(i));
        CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char16_t>(i)));
    }
    for (char32_t i = 0xE000; i < 0xF8FF; ++i)
    {
        CPPUNIT_ASSERT(isUnicode8Private(i));
        CPPUNIT_ASSERT(isUnicode8Private(static_cast<char16_t>(i)));
    }
    for (char32_t i = 0xF900; i <= 0xFFFF; ++i)
    {
        CPPUNIT_ASSERT(!isUnicode8Private(i));
        CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char16_t>(i)));
    }
    for (char32_t i = 65536; i < 0xF0000; ++i)
    {
        CPPUNIT_ASSERT(!isUnicode8Private(i));
    }
    for (char32_t i = 0xF0000; i < 0xFFFFE; ++i)
    {
        CPPUNIT_ASSERT(isUnicode8Private(i));
    }
    CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char32_t>(0xFFFFE)));
    CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char32_t>(0xFFFFF)));
    for (char32_t i = 0x100000; i < 0x10FFFE; ++i)
    {
        CPPUNIT_ASSERT(isUnicode8Private(i));
    }
    CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char32_t>(0x10FFFE)));
    CPPUNIT_ASSERT(!isUnicode8Private(static_cast<char32_t>(0x10FFFF)));
}

void IsUnicodeTestFixture::testIsWhitespace()
{
    // all the whitespace characters are U+0009..U+000D; U+0020;
    // U+0085; U+00A0; U+1680; U+2000..U+200A; U+2028,U+2029,U+202F;
    // U+205F; U+3000
    char32_t i;
    for (i = 0; i <= 255; ++i)
    {
        bool isSpace = (i >= 9 && i <= 13) || i == 32 || i == 133 || i == 160;
        if (isSpace != isWhitespaceChar(i) ||
            isSpace != isWhitespaceChar(static_cast<char16_t>(i)) ||
            isSpace != isWhitespaceChar(static_cast<char>(i)))
        {
            cout << "Problem with whitespace check at value, " << static_cast<int>(i) <<
                    "." << endl;
        }
        CPPUNIT_ASSERT(isSpace == isWhitespaceChar(i));
        CPPUNIT_ASSERT(isSpace == isWhitespaceChar(static_cast<char16_t>(i)));
        CPPUNIT_ASSERT(isSpace == isWhitespaceChar(static_cast<char>(i)));
    }
    for ( ; i < 0x1680; ++i)
    {
        CPPUNIT_ASSERT(!isWhitespaceChar(i));
        CPPUNIT_ASSERT(!isWhitespaceChar(static_cast<char16_t>(i)));
    }
    CPPUNIT_ASSERT(isWhitespaceChar(i));
    CPPUNIT_ASSERT(isWhitespaceChar(static_cast<char16_t>(i)));
    for (++i; i <= 0x2FFF; ++i)
    {
        bool isSpace = (i >= 0x2000 && i <= 0x200a) ||
                        i == 0x2028 || i == 0x2029 || i == 0x202f ||
                        i == 0x205f;
        if (isSpace != isWhitespaceChar(i) ||
            isSpace != isWhitespaceChar(static_cast<char16_t>(i)))
        {
            cout << "Problem with whitespace check at value, " << static_cast<int>(i) <<
                    "." << endl;
        }
        CPPUNIT_ASSERT(isSpace == isWhitespaceChar(i));
        CPPUNIT_ASSERT(isSpace == isWhitespaceChar(static_cast<char16_t>(i)));
    }
    CPPUNIT_ASSERT(isWhitespaceChar(i));
    CPPUNIT_ASSERT(isWhitespaceChar(static_cast<char16_t>(i)));
    for (++i; i <= 0xFFFF; ++i)
    {
        CPPUNIT_ASSERT(!isWhitespaceChar(i));
        CPPUNIT_ASSERT(!isWhitespaceChar(static_cast<char16_t>(i)));
    }
    for (char32_t i = 65536; i < 0x110000; ++i)
    {
        CPPUNIT_ASSERT(!isWhitespaceChar(i));
    }
}

void IsUnicodeTestFixture::testIsUnicode8()
{
    // test depends on processed form of Unicode8Data.txt
    auto iValid = begin(validUnicode8CodePoints);
    char32_t i;
    for (i = 0; i < 0x100; ++i)
    {
        if (iValid != end(validUnicode8CodePoints) && i == *iValid)
        {
            if (!isUnicode8Assigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            CPPUNIT_ASSERT(isUnicode8Assigned(i));
            CPPUNIT_ASSERT(isUnicode8Assigned(static_cast<char16_t>(i)));
            CPPUNIT_ASSERT(isUnicode8Assigned(static_cast<char>(i)));
            ++iValid;
        }
        else
        {
            if (isUnicode8Assigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", shouldn't be assigned, found assigned." << endl;
            }
            CPPUNIT_ASSERT(!isUnicode8Assigned(i));
            CPPUNIT_ASSERT(!isUnicode8Assigned(static_cast<char16_t>(i)));
            CPPUNIT_ASSERT(!isUnicode8Assigned(static_cast<char>(i)));
        }
        if (i % 0x10 == 0xF)
        {
            cout << "*";
        }
    }
    char32_t startCJKSection = 0x100;
    char32_t endCJKSection = 0x0FF;
    for (; i < 0x10000; ++i)
    {
        switch (*iValid)
        {
        default:
            break;
        case 0x3400:
        case 0x4E00:
        case 0xAC00:
            startCJKSection = *iValid;
            endCJKSection = *++iValid;
            break;
        }
        if (i >= startCJKSection && i <= endCJKSection)
        {
            if (!isUnicode8Assigned(i))
            {
                cout << "Problem with CJK value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            CPPUNIT_ASSERT(isUnicode8Assigned(i));
            CPPUNIT_ASSERT(isUnicode8Assigned(static_cast<char16_t>(i)));
            if (i == endCJKSection)
            {
                ++iValid;
            }
        }
        else if (iValid != end(validUnicode8CodePoints) && i == *iValid)
        {
            if (!isUnicode8Assigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            CPPUNIT_ASSERT(isUnicode8Assigned(i));
            CPPUNIT_ASSERT(isUnicode8Assigned(static_cast<char16_t>(i)));
            ++iValid;
        }
        else
        {
            if (isUnicode8Assigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", shouldn't be assigned, found assigned." << endl;
            }
            CPPUNIT_ASSERT(!isUnicode8Assigned(i));
            CPPUNIT_ASSERT(!isUnicode8Assigned(static_cast<char16_t>(i)));
        }
        if (i % 0x1000 == 0xFFF)
        {
            cout << "+";
        }
    }
    for (; i < 0x10FFFF; ++i)
    {
        switch (*iValid)
        {
        default:
            break;
        case 0x20000:
        case 0x2A700:
        case 0x2B740:
        case 0x2B820:
            startCJKSection = *iValid;
            endCJKSection = *++iValid;
            break;
        }
        if (i >= startCJKSection && i <= endCJKSection)
        {
            if (!isUnicode8Assigned(i))
            {
                cout << "Problem with CJK value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            CPPUNIT_ASSERT(isUnicode8Assigned(i));
            if (i == endCJKSection)
            {
                ++iValid;
            }
        }
        else if (iValid != end(validUnicode8CodePoints) && i == *iValid)
        {
            if (!isUnicode8Assigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            CPPUNIT_ASSERT(isUnicode8Assigned(i));
            ++iValid;
        }
        else
        {
            if (isUnicode8Assigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", shouldn't be assigned, found assigned." << endl;
            }
            CPPUNIT_ASSERT(!isUnicode8Assigned(i));
        }
        if (i % 0x10000 == 0xFFFF)
        {
            cout << "x";
        }
    }
}

