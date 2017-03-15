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
// string_splitjoin_test.cxx -- unit tests for the string splitting and
//                              joining functions
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <string_splitjoin.hxx>
#include <string.hxx>
#include <string.h>

using namespace std;
using namespace testing;

TEST(StringSplitJoinTest, testEmpties)
{
    string emptyString;
    ASSERT_TRUE(ansak::split(emptyString, ',').empty());

    vector<string> emptyVector;
    ASSERT_TRUE(ansak::join(emptyVector, ',').empty());
}

TEST(StringSplitJoinTest, testUnicodeVersion)
{
    string emptyString;
    ASSERT_TRUE(ansak::getUnicodeVersionSupported() >= "7.0");
}

TEST(StringSplitJoinTest, testSplitter)
{
    string t0("Now is the time for all good men to come to the aid of the party");
    vector<string> r0 = ansak::split(t0, ' ');
    EXPECT_EQ(static_cast<size_t>(16), r0.size());
    EXPECT_EQ(0, strcmp(r0[0].c_str(), "Now"));
    EXPECT_EQ(0, strcmp(r0[1].c_str(), "is"));
    EXPECT_EQ(0, strcmp(r0[2].c_str(), "the"));
    EXPECT_EQ(0, strcmp(r0[3].c_str(), "time"));
    EXPECT_EQ(0, strcmp(r0[4].c_str(), "for"));
    EXPECT_EQ(0, strcmp(r0[5].c_str(), "all"));
    EXPECT_EQ(0, strcmp(r0[6].c_str(), "good"));
    EXPECT_EQ(0, strcmp(r0[7].c_str(), "men"));
    EXPECT_EQ(0, strcmp(r0[8].c_str(), "to"));
    EXPECT_EQ(0, strcmp(r0[9].c_str(), "come"));
    EXPECT_EQ(0, strcmp(r0[10].c_str(), "to"));
    EXPECT_EQ(0, strcmp(r0[11].c_str(), "the"));
    EXPECT_EQ(0, strcmp(r0[12].c_str(), "aid"));
    EXPECT_EQ(0, strcmp(r0[13].c_str(), "of"));
    EXPECT_EQ(0, strcmp(r0[14].c_str(), "the"));
    EXPECT_EQ(0, strcmp(r0[15].c_str(), "party"));

    ansak::ucs4String t1(U"Now  is  the  time.");
    vector<ansak::ucs4String> r1 = ansak::split(t1, U' ');
    EXPECT_EQ(static_cast<size_t>(7), r1.size());
    EXPECT_EQ(0, memcmp(r1[0].c_str(), U"Now", 16));
    EXPECT_TRUE(r1[1].empty());
    EXPECT_EQ(0, memcmp(r1[2].c_str(), U"is", 12));
    EXPECT_TRUE(r1[3].empty());
    EXPECT_EQ(0, memcmp(r1[4].c_str(), U"the", 16));
    EXPECT_TRUE(r1[5].empty());
    EXPECT_EQ(0, memcmp(r1[6].c_str(), U"time.", 24));
}

TEST(StringSplitJoinTest, testJoiner)
{
    vector<string> a;
    a.push_back("c:");
    a.push_back("windows");
    a.push_back("system32");
    a.push_back("drivers");
    a.push_back("etc");
    a.push_back("hosts");
    string s(ansak::join(a, '\\'));
    EXPECT_EQ(0, strcmp(s.c_str(), "c:\\windows\\system32\\drivers\\etc\\hosts"));

    string b("/home/akla/bust/me/up");
    vector<string> bSplit(ansak::split(b, '/'));
    EXPECT_EQ(static_cast<size_t>(6), bSplit.size());
    EXPECT_EQ(string(), bSplit[0]);
    EXPECT_EQ(string("home"), bSplit[1]);
    EXPECT_EQ(string("akla"), bSplit[2]);
    EXPECT_EQ(string("bust"), bSplit[3]);
    EXPECT_EQ(string("me"), bSplit[4]);
    EXPECT_EQ(string("up"), bSplit[5]);
    string bJoin(ansak::join(bSplit, '/'));
    EXPECT_EQ(b, bJoin);
}

TEST(StringSplitJoinTest, testSingle)
{
    string b("whole");
    auto c(ansak::split(b, '/'));
    EXPECT_EQ(static_cast<size_t>(1), c.size());
    EXPECT_EQ(b, c[0]);
    auto d(ansak::join(c, ':'));
    EXPECT_EQ(b, d);
}

