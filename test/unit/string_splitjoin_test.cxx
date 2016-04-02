///////////////////////////////////////////////////////////////////////////
//
// 2014.10.31
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
// sringSplitJoinTest.cxx -- unit tests for the string splitting and
//                           joining functions
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "string_splitjoin.hxx"
#include "string.hxx"
#include <string.h>

using namespace std;
using namespace testing;

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

