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

#include <gtest/gtest.h>

#include "string_internal.hxx"
#include <iterator>
#include <iostream>
#include "char_is_unicode_test_data.hxx"

using namespace ansak;
using namespace ansak::internal;
using namespace std;
using namespace testing;

TEST(IsUnicodeTest, testIsControl)
{
    // all the control characters are U+0000 .. U+001F and U+0080 to U+009F
    // In the selected UnicodeData.txt file their second field (and theirs only)
    // is <control>
    for (char32_t i = 0; i < 32; ++i)
    {
        EXPECT_TRUE(isControlChar(i));
        EXPECT_TRUE(isControlChar(static_cast<char16_t>(i)));
        EXPECT_TRUE(isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 32; i < 127; ++i)
    {
        EXPECT_FALSE(isControlChar(i));
        EXPECT_FALSE(isControlChar(static_cast<char16_t>(i)));
        EXPECT_FALSE(isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 127; i < 160; ++i)
    {
        EXPECT_TRUE(isControlChar(i));
        EXPECT_TRUE(isControlChar(static_cast<char16_t>(i)));
        EXPECT_TRUE(isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 160; i <= 255; ++i)
    {
        EXPECT_FALSE(isControlChar(i));
        EXPECT_FALSE(isControlChar(static_cast<char16_t>(i)));
        EXPECT_FALSE(isControlChar(static_cast<char>(i)));
    }
    for (char32_t i = 256; i <= 0xFFFF; ++i)
    {
        EXPECT_FALSE(isControlChar(i));
        EXPECT_FALSE(isControlChar(static_cast<char16_t>(i)));
    }
    for (char32_t i = 65536; i < 0x110000; ++i)
    {
        EXPECT_FALSE(isControlChar(i));
    }
}

TEST(IsUnicodeTest, testIsPrivate)
{
    // all the private characters are U+E000 .. U+F8FF, U+F0000..U+FFFFD and
    // U+100000..U+10FFFD
    // In the UnicodeData.txt file these ranges are identified that way
    for (char32_t i = 0; i <= 255; ++i)
    {
        EXPECT_FALSE(isUnicodePrivate(i));
        EXPECT_FALSE(isUnicodePrivate(static_cast<char16_t>(i)));
        EXPECT_FALSE(isUnicodePrivate(static_cast<char>(i)));
    }
    for (char32_t i = 256; i < 0xE000; ++i)
    {
        EXPECT_FALSE(isUnicodePrivate(i));
        EXPECT_FALSE(isUnicodePrivate(static_cast<char16_t>(i)));
    }
    for (char32_t i = 0xE000; i < 0xF8FF; ++i)
    {
        EXPECT_TRUE(isUnicodePrivate(i));
        EXPECT_TRUE(isUnicodePrivate(static_cast<char16_t>(i)));
    }
    for (char32_t i = 0xF900; i <= 0xFFFF; ++i)
    {
        EXPECT_FALSE(isUnicodePrivate(i));
        EXPECT_FALSE(isUnicodePrivate(static_cast<char16_t>(i)));
    }
    for (char32_t i = 65536; i < 0xF0000; ++i)
    {
        EXPECT_FALSE(isUnicodePrivate(i));
    }
    for (char32_t i = 0xF0000; i < 0xFFFFE; ++i)
    {
        EXPECT_TRUE(isUnicodePrivate(i));
    }
    EXPECT_FALSE(isUnicodePrivate(static_cast<char32_t>(0xFFFFE)));
    EXPECT_FALSE(isUnicodePrivate(static_cast<char32_t>(0xFFFFF)));
    for (char32_t i = 0x100000; i < 0x10FFFE; ++i)
    {
        EXPECT_TRUE(isUnicodePrivate(i));
    }
    EXPECT_FALSE(isUnicodePrivate(static_cast<char32_t>(0x10FFFE)));
    EXPECT_FALSE(isUnicodePrivate(static_cast<char32_t>(0x10FFFF)));
}

TEST(IsUnicodeTest, testIsWhitespace)
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
        EXPECT_TRUE(isSpace == isWhitespaceChar(i));
        EXPECT_TRUE(isSpace == isWhitespaceChar(static_cast<char16_t>(i)));
        EXPECT_TRUE(isSpace == isWhitespaceChar(static_cast<char>(i)));
    }
    for ( ; i < 0x1680; ++i)
    {
        EXPECT_FALSE(isWhitespaceChar(i));
        EXPECT_FALSE(isWhitespaceChar(static_cast<char16_t>(i)));
    }
    EXPECT_TRUE(isWhitespaceChar(i));
    EXPECT_TRUE(isWhitespaceChar(static_cast<char16_t>(i)));
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
        EXPECT_TRUE(isSpace == isWhitespaceChar(i));
        EXPECT_TRUE(isSpace == isWhitespaceChar(static_cast<char16_t>(i)));
    }
    EXPECT_TRUE(isWhitespaceChar(i));
    EXPECT_TRUE(isWhitespaceChar(static_cast<char16_t>(i)));
    for (++i; i <= 0xFFFF; ++i)
    {
        EXPECT_FALSE(isWhitespaceChar(i));
        EXPECT_FALSE(isWhitespaceChar(static_cast<char16_t>(i)));
    }
    for (char32_t i = 65536; i < 0x110000; ++i)
    {
        EXPECT_FALSE(isWhitespaceChar(i));
    }
}

TEST(IsUnicodeTest, testIsUnicode)
{
    cout << "testIsUnicode progress: ";
    // test depends on processed form of the selected UnicodeData.txt
    auto iValid = begin(validUnicodeCodePoints);
    char32_t i;
    for (i = 0; i < 0x100; ++i)
    {
        if (iValid != end(validUnicodeCodePoints) && i == *iValid)
        {
            if (!isUnicodeAssigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            EXPECT_TRUE(isUnicodeAssigned(i));
            EXPECT_TRUE(isUnicodeAssigned(static_cast<char16_t>(i)));
            EXPECT_TRUE(isUnicodeAssigned(static_cast<char>(i)));
            ++iValid;
        }
        else
        {
            if (isUnicodeAssigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", shouldn't be assigned, found assigned." << endl;
            }
            EXPECT_FALSE(isUnicodeAssigned(i));
            EXPECT_FALSE(isUnicodeAssigned(static_cast<char16_t>(i)));
            EXPECT_FALSE(isUnicodeAssigned(static_cast<char>(i)));
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
            if (!isUnicodeAssigned(i))
            {
                cout << "Problem with CJK value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            EXPECT_TRUE(isUnicodeAssigned(i));
            EXPECT_TRUE(isUnicodeAssigned(static_cast<char16_t>(i)));
            if (i == endCJKSection)
            {
                ++iValid;
            }
        }
        else if (iValid != end(validUnicodeCodePoints) && i == *iValid)
        {
            if (!isUnicodeAssigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            EXPECT_TRUE(isUnicodeAssigned(i));
            EXPECT_TRUE(isUnicodeAssigned(static_cast<char16_t>(i)));
            ++iValid;
        }
        else
        {
            if (isUnicodeAssigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", shouldn't be assigned, found assigned." << endl;
            }
            EXPECT_FALSE(isUnicodeAssigned(i));
            EXPECT_FALSE(isUnicodeAssigned(static_cast<char16_t>(i)));
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
        case 0x17000:
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
            if (!isUnicodeAssigned(i))
            {
                cout << "Problem with CJK/Tangut value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            EXPECT_TRUE(isUnicodeAssigned(i));
            if (i == endCJKSection)
            {
                ++iValid;
            }
        }
        else if (iValid != end(validUnicodeCodePoints) && i == *iValid)
        {
            if (!isUnicodeAssigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", should be assigned, found unassigned." << endl;
            }
            EXPECT_TRUE(isUnicodeAssigned(i));
            ++iValid;
        }
        else
        {
            if (isUnicodeAssigned(i))
            {
                cout << "Problem with value, " << static_cast<int>(i) <<
                        ", shouldn't be assigned, found assigned." << endl;
            }
            EXPECT_FALSE(isUnicodeAssigned(i));
        }
        if (i % 0x10000 == 0xFFFF)
        {
            cout << "x";
        }
    }
    cout << " ... done!" << endl;
}

