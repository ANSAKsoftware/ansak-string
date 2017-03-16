///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, Arthur N. Klassen
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
// 2016.02.07 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// time_stamp_test.cxx -- Tests for FilePath in file_path.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <time_stamp.hxx>

#include <gtest/gtest.h>

using namespace ansak;
using namespace testing;

TEST(TimeStampTest, testTimeStamp)
{
    TimeStamp t0 { 1867, 7, 1, 18, 0, 0 };
    TimeStamp t1 { 1861, 4, 12, 11, 30, 0 };

    EXPECT_TRUE(t1 != t0);
    EXPECT_TRUE(t1 <= t0);
    EXPECT_TRUE(t1 < t0);
    EXPECT_TRUE(t0 >= t1);
    EXPECT_TRUE(t0 > t1);
    EXPECT_TRUE(t0 == t0);
    EXPECT_TRUE(t0 <= t0);
    EXPECT_TRUE(t0 >= t0);

    t1.year = 1867;
    EXPECT_TRUE(t1 != t0);
    EXPECT_TRUE(t1 <= t0);
    EXPECT_TRUE(t1 < t0);
    EXPECT_TRUE(t0 >= t1);
    EXPECT_TRUE(t0 > t1);

    t1.month = 7;
    EXPECT_TRUE(t1 != t0);
    EXPECT_TRUE(t0 <= t1);
    EXPECT_TRUE(t0 < t1);
    EXPECT_TRUE(t1 >= t0);
    EXPECT_TRUE(t1 > t0);

    t0.day = 12;
    EXPECT_TRUE(t1 != t0);
    EXPECT_TRUE(t1 <= t0);
    EXPECT_TRUE(t1 < t0);
    EXPECT_TRUE(t0 >= t1);
    EXPECT_TRUE(t0 > t1);

    t1.hour = 18;
    EXPECT_TRUE(t1 != t0);
    EXPECT_TRUE(t0 <= t1);
    EXPECT_TRUE(t0 < t1);
    EXPECT_TRUE(t1 >= t0);
    EXPECT_TRUE(t1 > t0);

    t0.minute = 30;
    EXPECT_TRUE(t1 == t0);
    EXPECT_TRUE(t0 <= t1);
    EXPECT_TRUE(t0 >= t1);
    EXPECT_TRUE(t1 >= t0);
    EXPECT_TRUE(t1 <= t0);

    t0.second = 50;
    EXPECT_TRUE(t1 != t0);
    EXPECT_TRUE(t1 <= t0);
    EXPECT_TRUE(t1 < t0);
    EXPECT_TRUE(t0 >= t1);
    EXPECT_TRUE(t0 > t1);
}


