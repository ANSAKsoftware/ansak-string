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
// 2015.11.28 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// encode_predicate_test.cxx -- unit tests for checking the encoding predicate
//                              addendum to isXXXX()
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "string.hxx"
#include "string_internal.hxx"

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace ansak;
using namespace ansak::internal;
using namespace std;
using namespace testing;

namespace {

char spaces[] = { 9, 10, 11, 12, 13, 32, '\x85', '\xa0', 0};
char16_t spaces16[] = { 9, 10, 11, 12, 13, 32, 133, 160, 8192, 5760,
                        8192, 8193, 8194, 8195, 8196, 8197, 8198, 8199,
                        8200, 8201, 8202, 8232, 8233, 8239, 8287, 12288,
                        0 };
char32_t spaces32[] = { 9, 10, 11, 12, 13, 32, 133, 160, 8192, 5760,
                        8192, 8193, 8194, 8195, 8196, 8197, 8198, 8199,
                        8200, 8201, 8202, 8232, 8233, 8239, 8287, 12288,
                        0};

char controls[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                    '\x7f', '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
                    '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
                    '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
                    '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F', 0 };
char16_t controls16[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                        127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
                        140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152,
                        153, 154, 155, 156, 157, 158, 159, 0 };
char32_t controls32[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                        127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
                        140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152,
                        153, 154, 155, 156, 157, 158, 159, 0 };

char16_t unassigneds16[] = { 0x378, 0x379, 0x10c8, 0x10c9, 0x2072, 0x2073, 0x321f, 0x32ff,
                             0x4db7, 0x4db8, 0x9fd7, 0x9fd8, 0xa62c, 0xa62d, 0xd7a4, 0xd7a5,
                             0xd800, 0xdc00, 0xfa6e, 0xfa6f, 0 };
char32_t unassigneds32[] = { 0x378, 0x379, 0x10c8, 0x10c9, 0x2072, 0x2073, 0x321f, 0x32ff,
                             0x4db7, 0x4db8, 0x9fd7, 0x9fd8, 0xa62c, 0xa62d, 0xd7a4, 0xd7a5,
                             0xd800, 0xdc00, 0xfa6e, 0xfa6f,
                             0x1004e, 0x1004f, 0x12545, 0x12546, 0x16FF0, 0x187ED, 0x1f0c0, 0x1f0d0,
                             0x2a6d7, 0x2a6d8, 0x30001, 0x40001, 0x50001, 0x60001, 0x70001, 0x80001,
                             0x90001, 0xA0001, 0xB0001, 0xC0001, 0xD0001, 0xE0000, 0x110000, 0 };

char16_t privates16[] = { 0xe000, 0xe100, 0xe200, 0xe300, 0xe400, 0xe500, 0xe600, 0xe700,
                          0xe800, 0xe900, 0xea00, 0xeb00, 0xec00, 0xed00, 0xee00, 0xef00,
                          0xf000, 0xf100, 0xf200, 0xf300, 0xf400, 0xf500, 0xf600, 0xf700,
                          0xf800, 0xf8ff, 0 };
char32_t privates32[] = { 0xe000, 0xe100, 0xe200, 0xe300, 0xe400, 0xe500, 0xe600, 0xe700,
                          0xe800, 0xe900, 0xea00, 0xeb00, 0xec00, 0xed00, 0xee00, 0xef00,
                          0xf000, 0xf100, 0xf200, 0xf300, 0xf400, 0xf500, 0xf600, 0xf700,
                          0xf800, 0xf8ff,
                          0xf0000, 0xf1000, 0xf2000, 0xf3000, 0xf4000, 0xf5000, 0xf6000, 0xf7000,
                          0xf8000, 0xf9000, 0xfa000, 0xfb000, 0xfc000, 0xfd000, 0xfe000, 0xff000, 0xffffd,
                          0x100000, 0x101000, 0x102000, 0x103000, 0x104000, 0x105000, 0x106000, 0x107000,
                          0x108000, 0x109000, 0x10a000, 0x10b000, 0x10c000, 0x10d000, 0x10e000, 0x10f000,
                          0x10fffd, 0 };
char quickBrown[] = "The quick brown fox jumps over the lazy dog.";
char16_t quickBrownTamil16[] = {
                0xbb5, 0x0bbf, 0x0bb0, 0x0bc8, 0x0bb5, 0x0bc1, 32, 0x0baa, 0x0bb4, 0x0bc1, 0x0baa, 0x0bcd, 0x0baa, 0x0bc1, 32, 0x0ba8,
                0xbb0, 0x0bbf, 32, 0x0b9a, 0x0bc7, 0x0bbe, 0x0bae, 0x0bcd, 0x0baa, 0x0bc7, 0x0bb1, 0x0bbf, 32, 0x0ba8, 0x0bbe, 0x0baf,
                0xbcd, 32, 0x0bae, 0x0bc0, 0x0ba4, 0x0bc1, 32, 0x0ba4, 0x0bbe, 0x0ba3, 0x0bcd, 0x0b9f, 0x0bc1, 0x0b95, 0x0bbf, 0x0bb1, 0x0ba4, 0x0bc1,
                46, 0 };
char32_t quickBrownTamil32[] = {
                0xbb5, 0x0bbf, 0x0bb0, 0x0bc8, 0x0bb5, 0x0bc1, 32, 0x0baa, 0x0bb4, 0x0bc1, 0x0baa, 0x0bcd, 0x0baa, 0x0bc1, 32, 0x0ba8,
                0xbb0, 0x0bbf, 32, 0x0b9a, 0x0bc7, 0x0bbe, 0x0bae, 0x0bcd, 0x0baa, 0x0bc7, 0x0bb1, 0x0bbf, 32, 0x0ba8, 0x0bbe, 0x0baf,
                0xbcd, 32, 0x0bae, 0x0bc0, 0x0ba4, 0x0bc1, 32, 0x0ba4, 0x0bbe, 0x0ba3, 0x0bcd, 0x0b9f, 0x0bc1, 0x0b95, 0x0bbf, 0x0bb1, 0x0ba4, 0x0bc1,
                46, 0 };
}

TEST(EncodePredicateTest, testCharToEncoding)
{
    // test a non-assigned, non-private, non-control, non-whitespace
    EXPECT_EQ(0u, charToEncodingTypeMask(static_cast<char32_t>(0x30000)));
    // there are no non-assigned: control or whitespace values
    // test a non-assigned, private, non-control, non-whitespace
    EXPECT_EQ(2u, charToEncodingTypeMask(static_cast<char32_t>(0xf0020)));
    // test a assigned, non-private, non-control, non-whitespace
    EXPECT_EQ(1u, charToEncodingTypeMask(static_cast<char32_t>(0x2f920)));
    // test a assigned, non-private, non-control, whitespace
    EXPECT_EQ(9u, charToEncodingTypeMask(static_cast<char16_t>(160)));
    // test a assigned, non-private, control, non-whitespace
    EXPECT_EQ(5u, charToEncodingTypeMask(static_cast<char>(16)));
    // test a assigned, non-private, control, whitespace
    EXPECT_EQ(13u, charToEncodingTypeMask(static_cast<char>(9)));
    // there are no assigned-private characters

    EncodingCheckPredicate null;
    EXPECT_TRUE(null('3'));
    EXPECT_TRUE(null(static_cast<char16_t>('3')));
    EXPECT_TRUE(null(static_cast<char32_t>('3')));
}

TEST(EncodePredicateTest, testSimpleAssignedsPredicate)
{
    auto pred = validIf(kIsAssigned);
    EXPECT_TRUE(pred == pred);
    EXPECT_FALSE(pred != pred);
    for_each(begin(quickBrown), end(quickBrown), [&](const char c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(quickBrownTamil16), end(quickBrownTamil16), [&](const char16_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
    for_each(begin(quickBrownTamil32), end(quickBrownTamil32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testSimpleSpacesPredicate)
{
    auto pred = validIf(kIsWhitespace);
    for_each(begin(spaces), end(spaces), [&](const char c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(spaces16), end(spaces16), [&](const char16_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
    for_each(begin(spaces32), end(spaces32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testSimpleControlPredicate)
{
    auto pred = validIf(kIsControl);
    for_each(begin(controls), end(controls), [&](const char c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(controls16), end(controls16), [&](const char16_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
    for_each(begin(controls32), end(controls32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testSimplePrivatesPredicate)
{
    auto pred = validIfNot(kIsAssigned);
    for_each(begin(privates16), end(privates16), [&](const char16_t c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(privates32), end(privates32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testSimpleNonAssignedsPredicate)
{
    auto pred = validIfNot(kIsAssigned);
    for_each(begin(unassigneds16), end(unassigneds16), [&](const char16_t c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(unassigneds32), end(unassigneds32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testSimpleNonSpacesPredicate)
{
    auto pred = validIfNot(kIsWhitespace);
    char camelCaseSentence[] = "aQuickBrownFoxJumpsOverTheLazyDog";
    char16_t camelCaseSentence16[] = { 0x61, 0x51, 0x75, 0x69, 0x63, 0x6b, 0x42, 0x72, 0x6f, 0x77, 0x6e,
                                       0x46, 0x6f, 0x78, 0x4a, 0x75, 0x6d, 0x70, 0x72, 0x4f, 0x76, 0x65,
                                       0x72, 0x54, 0x68, 0x65, 0x4c, 0x61, 0x7a, 0x79, 0x44, 0x6f, 0x67, 0 };
    char32_t camelCaseSentence32[] = { 0x61, 0x51, 0x75, 0x69, 0x63, 0x6b, 0x42, 0x72, 0x6f, 0x77, 0x6e,
                                       0x46, 0x6f, 0x78, 0x4a, 0x75, 0x6d, 0x70, 0x72, 0x4f, 0x76, 0x65,
                                       0x72, 0x54, 0x68, 0x65, 0x4c, 0x61, 0x7a, 0x79, 0x44, 0x6f, 0x67, 0 };
    for_each(begin(camelCaseSentence), end(camelCaseSentence), [&](const char c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(camelCaseSentence16), end(camelCaseSentence16), [&](const char16_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
    for_each(begin(camelCaseSentence32), end(camelCaseSentence32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testSimpleNonControlPredicate)
{
    auto pred = validIfNot(kIsControl);
    for_each(begin(quickBrown), end(quickBrown), [&](const char c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(quickBrownTamil16), end(quickBrownTamil16), [&](const char16_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
    for_each(begin(quickBrownTamil32), end(quickBrownTamil32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testSimpleNonPrivatesPredicate)
{
    auto pred = validIfNot(kIsPrivate);
    for_each(begin(quickBrown), end(quickBrown), [&](const char c){
        if (c != 0)
        {
            if (!pred(c))
            {
                cout << endl << "Problem with value, " << static_cast<int>(c) << ": pred(c) fails." << endl;
            }
            EXPECT_TRUE(pred(c));
        }
    });
    for_each(begin(quickBrownTamil16), end(quickBrownTamil16), [&](const char16_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
    for_each(begin(quickBrownTamil32), end(quickBrownTamil32), [&](const char32_t c){ if (c != 0) EXPECT_TRUE(pred(c)); });
}

TEST(EncodePredicateTest, testCombinedFlagsPredicate)
{
    auto pred = validIf(kIsAssigned);
    auto spacePred = pred.andIf(kIsWhitespace).andIfNot(kIsControl)
                            .andIfNot(kIsPrivate);
    auto tabsPred = pred.andIf(kIsWhitespace).andIf(kIsControl)
                            .andIfNot(kIsPrivate);

    EXPECT_TRUE(pred(spaces[0]));EXPECT_FALSE(spacePred(spaces[0]));EXPECT_TRUE(tabsPred(spaces[0]));  // 9
    EXPECT_TRUE(pred(spaces[1]));EXPECT_FALSE(spacePred(spaces[1]));EXPECT_TRUE(tabsPred(spaces[1]));  // 10
    EXPECT_TRUE(pred(spaces[2]));EXPECT_FALSE(spacePred(spaces[2]));EXPECT_TRUE(tabsPred(spaces[2]));  // 11
    EXPECT_TRUE(pred(spaces[3]));EXPECT_FALSE(spacePred(spaces[3]));EXPECT_TRUE(tabsPred(spaces[3]));  // 12
    EXPECT_TRUE(pred(spaces[4]));EXPECT_FALSE(spacePred(spaces[4]));EXPECT_TRUE(tabsPred(spaces[4]));  // 13
    EXPECT_TRUE(pred(spaces[5]));EXPECT_TRUE(spacePred(spaces[5]));EXPECT_FALSE(tabsPred(spaces[5]));  // 32
    EXPECT_TRUE(pred(spaces[6]));EXPECT_FALSE(spacePred(spaces[6]));EXPECT_TRUE(tabsPred(spaces[6]));  // 133
    EXPECT_TRUE(pred(spaces[7]));EXPECT_TRUE(spacePred(spaces[7]));EXPECT_FALSE(tabsPred(spaces[7]));  // 160
}

