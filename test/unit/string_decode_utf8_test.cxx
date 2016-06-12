///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2014, Arthur N. Klassen
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
// 2014.10.31 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_decode_utf8_test.cxx -- unit tests to cover the slightly more
//                                public decodeUtf8 API
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "internal/string_decode_utf8.hxx"

using namespace std;
using namespace testing;
using namespace ansak::internal;

TEST(DecodeUtf8Test, test7bit)
{
    const char test7Bit[] = "No";
    const char* p = &test7Bit[0];
    auto q = p;
    auto c = decodeUtf8(p);
    EXPECT_EQ(U'N', c);
    EXPECT_EQ(q, p);
}

TEST(DecodeUtf8Test, testContinuationChar)
{
    const char testContinue[] = "\xa0o";
    const char* p = &testContinue[0];
    auto c = decodeUtf8(p);
    EXPECT_EQ(U'\0', c);
    EXPECT_EQ(nullptr, p);
}

TEST(DecodeUtf8Test, testTwoByteSequence)
{
    {
        const char testGoodFirstPlusNull[] = "\xc0";
        const char* p = &testGoodFirstPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 1, p);
    }

    {
        const char testGoodFirstPlusBadSecond[] = "\xc0#";
        const char* p = &testGoodFirstPlusBadSecond[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    // test boundary of too-lowness -- encoding must be for 0x80 and higher
    {
        const char testGoodTwoByteTooLow[] = "\xc0\x80";
        const char* p = &testGoodTwoByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        const char testGoodTwoByteTooLow[] = "\xc1\xbf";
        const char* p = &testGoodTwoByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        const char testGoodTwoByteNotTooLow[] = "\xc2\x80";
        const char* p = &testGoodTwoByteNotTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_NE(U'\0', c);
        EXPECT_NE(nullptr, p);
    }

    {
        const char testGoodTwoByte[] = "\xc2\xa0";
        const char* p = &testGoodTwoByte[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(c, U'\U000000a0');
        EXPECT_EQ(q + 1, p);
    }
}

TEST(DecodeUtf8Test, testThreeByteSequenceNonUtf16)
{
    {
        const char testGoodFirstPlusNull[] = "\xe0";
        const char* p = &testGoodFirstPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 1, p);
    }

    {
        const char testGoodFirstPlusBadSecond[] = "\xe0#";
        const char* p = &testGoodFirstPlusBadSecond[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstTwoPlusNull[] = "\xe0\xbf";
        const char* p = &testGoodFirstTwoPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 2, p);
    }

    {
        const char testGoodFirstTwoPlusBadThird[] = "\xe0\xbf#";
        const char* p = &testGoodFirstTwoPlusBadThird[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodThreeByteTooLow[] = "\xe0\x80\x80";
        const char* p = &testGoodThreeByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        const char testGoodThreeByteTooLow[] = "\xe0\x9f\xbf";
        const char* p = &testGoodThreeByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        const char testGoodThreeByteNotTooLow[] = "\xe0\xa0\x80";
        const char* p = &testGoodThreeByteNotTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_NE(U'\0', c);
        EXPECT_NE(nullptr, p);
    }

    {
        const char testGoodThreeByte[] = "\xe0\xb4\x9b";
        const char* p = &testGoodThreeByte[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(c, U'\U00000d1b');
        EXPECT_EQ(q + 2, p);
    }
}

TEST(DecodeUtf8Test, testThreeByteSequenceUtf16)
{
    {
        const char testGoodThreeByteNonUTF16[] = "\xed\x9f\xbf";
        const char* p = &testGoodThreeByteNonUTF16[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(c, U'\U0000d7ff');
        EXPECT_EQ(q + 2, p);
    }

    {
        const char testOnePlusNullUTF16[] = "\xed\xa0\x80";
        const char* p = &testOnePlusNullUTF16[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 3, p);
    }

    {
        const char testOnePlusGoodFirstPlusBadSecond[] = "\xed\xa0\x80\xe0#";
        const char* p = &testOnePlusGoodFirstPlusBadSecond[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testOnePlusGoodFirstTwoPlusNull[] = "\xed\xa0\x80\xe0\xbf";
        const char* p = &testOnePlusGoodFirstTwoPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 5, p);
    }

    {
        const char testOnePlusGoodFirstTwoPlusBadThird[] = "\xed\xa0\x80\xe0\xbf#";
        const char* p = &testOnePlusGoodFirstTwoPlusBadThird[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        // encodes to d800 dbff -- 2nd value must be dc00 or above
        const char testOneGoodPlusGoodThreeByteTooLow[] = "\xed\xa0\x80\xed\xaf\xbf";
        const char* p = &testOneGoodPlusGoodThreeByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        // encodes to d800 dc00 -- good!
        const char testOneGoodPlusGoodThreeByteNotTooLow[] = "\xed\xa0\x80\xed\xb0\x80";
        const char* p = &testOneGoodPlusGoodThreeByteNotTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_NE(U'\0', c);
        EXPECT_NE(nullptr, p);
    }
    {
        // encodes to d800 dfff -- good!
        const char testOneGoodPlusGoodThreeByteNotTooHigh[] = "\xed\xa0\x80\xed\xbf\xbf";
        const char* p = &testOneGoodPlusGoodThreeByteNotTooHigh[0];
        auto c = decodeUtf8(p);
        EXPECT_NE(U'\0', c);
        EXPECT_NE(nullptr, p);
    }
    {
        // encodes to d800 e000 -- 2nd value must be dfff or below
        const char testOneGoodPlusGoodThreeByteTooHigh[] = "\xed\xa0\x80\xee\x80\x80";
        const char* p = &testOneGoodPlusGoodThreeByteTooHigh[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        // 0xd800 0xdcd2 --> 100d2; Linear B Monogram B247 Dipte
        const char testTwoGoodThreeByte[] = "\xed\xa0\x80\xed\xb3\x92";
        const char* p = &testTwoGoodThreeByte[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(c, U'\U000100d2');
        EXPECT_EQ(q + 5, p);
    }
}

TEST(DecodeUtf8Test, testFourByteSequence)
{
    {
        const char testGoodFirstPlusNull[] = "\xf0";
        const char* p = &testGoodFirstPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 1, p);
    }

    {
        const char testGoodFirstPlusBadSecond[] = "\xf0#";
        const char* p = &testGoodFirstPlusBadSecond[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstTwoPlusNull[] = "\xf0\xbf";
        const char* p = &testGoodFirstTwoPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 2, p);
    }

    {
        const char testGoodFirstTwoPlusBadThird[] = "\xf0\xbf#";
        const char* p = &testGoodFirstTwoPlusBadThird[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstThreePlusNull[] = "\xf0\xbf\xbf";
        const char* p = &testGoodFirstThreePlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 3, p);
    }

    {
        const char testGoodFirstThreePlusBadFourth[] = "\xf0\xbf\xbf#";
        const char* p = &testGoodFirstThreePlusBadFourth[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFourByteTooLow[] = "\xf0\x8f\xbf\xbf";
        const char* p = &testGoodFourByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        const char testGoodFourByteNotTooLow[] = "\xf0\x90\x80\x80";
        const char* p = &testGoodFourByteNotTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_NE(U'\0', c);
        EXPECT_NE(nullptr, p);
    }

    {
        const char testGoodFourByte[] = "\xf0\x90\x83\x92";
        const char* p = &testGoodFourByte[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(c, U'\U000100d2');
        EXPECT_EQ(q + 3, p);
    }
}

TEST(DecodeUtf8Test, testFiveByteSequence)
{
    {
        const char testGoodFirstPlusNull[] = "\xf8";
        const char* p = &testGoodFirstPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 1, p);
    }

    {
        const char testGoodFirstPlusBadSecond[] = "\xf8#";
        const char* p = &testGoodFirstPlusBadSecond[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstTwoPlusNull[] = "\xf8\xbf";
        const char* p = &testGoodFirstTwoPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 2, p);
    }

    {
        const char testGoodFirstTwoPlusBadThird[] = "\xf8\xbf#";
        const char* p = &testGoodFirstTwoPlusBadThird[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstThreePlusNull[] = "\xf8\xbf\xbf";
        const char* p = &testGoodFirstThreePlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 3, p);
    }

    {
        const char testGoodFirstThreePlusBadFourth[] = "\xf8\xbf\xbf#";
        const char* p = &testGoodFirstThreePlusBadFourth[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstFourPlusNull[] = "\xf8\xbf\xbf\xbf";
        const char* p = &testGoodFirstFourPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 4, p);
    }

    {
        const char testGoodFirstFourPlusBadFifth[] = "\xf8\xbf\xbf\xbf#";
        const char* p = &testGoodFirstFourPlusBadFifth[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFiveByteTooLow[] = "\xf8\x87\xbf\xbf\xbf";
        const char* p = &testGoodFiveByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        const char testGoodFiveByteNotTooLow[] = "\xf8\x88\x80\x80\x80";
        const char* p = &testGoodFiveByteNotTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_NE(U'\0', c);
        EXPECT_NE(nullptr, p);
    }

    {
        const char testGoodFiveByte[] = "\xfb\x91\x96\x9e\x89";
        const char* p = &testGoodFiveByte[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(c, U'\U03456789');
        EXPECT_EQ(q + 4, p);
    }
}

TEST(DecodeUtf8Test, testSixByteSequence)
{
    {
        const char testGoodFirstPlusNull[] = "\xfc";
        const char* p = &testGoodFirstPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 1, p);
    }

    {
        const char testGoodFirstPlusBadSecond[] = "\xfc#";
        const char* p = &testGoodFirstPlusBadSecond[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstTwoPlusNull[] = "\xfc\xbf";
        const char* p = &testGoodFirstTwoPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 2, p);
    }

    {
        const char testGoodFirstTwoPlusBadThird[] = "\xfc\xbf#";
        const char* p = &testGoodFirstTwoPlusBadThird[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstThreePlusNull[] = "\xfc\xbf\xbf";
        const char* p = &testGoodFirstThreePlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 3, p);
    }

    {
        const char testGoodFirstThreePlusBadFourth[] = "\xfc\xbf\xbf#";
        const char* p = &testGoodFirstThreePlusBadFourth[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstFourPlusNull[] = "\xfc\xbf\xbf\xbf";
        const char* p = &testGoodFirstFourPlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 4, p);
    }

    {
        const char testGoodFirstFourPlusBadFifth[] = "\xfc\xbf\xbf\xbf#";
        const char* p = &testGoodFirstFourPlusBadFifth[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodFirstFivePlusNull[] = "\xfc\xbf\xbf\xbf\xbf";
        const char* p = &testGoodFirstFivePlusNull[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(q + 5, p);
    }

    {
        const char testGoodFirstFivePlusBadFifth[] = "\xfc\xbf\xbf\xbf\xbf#";
        const char* p = &testGoodFirstFivePlusBadFifth[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testGoodSixByteTooLow[] = "\xfc\x83\xbf\xbf\xbf\xbf";
        const char* p = &testGoodSixByteTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
    {
        const char testGoodSixByteNotTooLow[] = "\xfc\x84\x80\x80\x80\x80";
        const char* p = &testGoodSixByteNotTooLow[0];
        auto c = decodeUtf8(p);
        EXPECT_NE(U'\0', c);
        EXPECT_NE(nullptr, p);
    }

    {
        const char testGoodSixByte[] = "\xfd\xa7\xa2\x9a\xaf\x8d";
        const char* p = &testGoodSixByte[0];
        auto q = p;
        auto c = decodeUtf8(p);
        EXPECT_EQ(c, U'\U6789abcd');
        EXPECT_EQ(q + 5, p);
    }
}

TEST(DecodeUtf8Test, testTooLargeStarter)
{
    {
        const char testStartsWithFE[] = "\xfe\xa7\xa2\x9a\xaf\x8d";
        const char* p = &testStartsWithFE[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }

    {
        const char testStartsWithFF[] = "\xff\xa7\xa2\x9a\xaf\x8d";
        const char* p = &testStartsWithFF[0];
        auto c = decodeUtf8(p);
        EXPECT_EQ(U'\0', c);
        EXPECT_EQ(nullptr, p);
    }
}


