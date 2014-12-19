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

// compile from here with
// g++ -std=c++11 -I .. ../../unittest/UnitTest.cpp stringSplitJoinTest.cxx `cppunit-config --libs` -o stringSplitJoinTest

#include <cppunit/extensions/HelperMacros.h>

#include "string_splitjoin.hxx"
#include "string.hxx"
#include <string.h>

using namespace std;

class StringSplitJoinTestFixture : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( StringSplitJoinTestFixture );
    CPPUNIT_TEST( testSplitter );
    CPPUNIT_TEST( testJoiner );
    CPPUNIT_TEST( testSingle );
CPPUNIT_TEST_SUITE_END();

public:
    void testSplitter();
    void testJoiner();
    void testSingle();
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringSplitJoinTestFixture);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(StringSplitJoinTestFixture, "StringSplitJoinTests");

void StringSplitJoinTestFixture::testSplitter()
{
    string t0("Now is the time for all good men to come to the aid of the party");
    vector<string> r0 = ansak::split(t0, ' ');
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), r0.size());
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[0].c_str(), "Now"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[1].c_str(), "is"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[2].c_str(), "the"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[3].c_str(), "time"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[4].c_str(), "for"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[5].c_str(), "all"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[6].c_str(), "good"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[7].c_str(), "men"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[8].c_str(), "to"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[9].c_str(), "come"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[10].c_str(), "to"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[11].c_str(), "the"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[12].c_str(), "aid"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[13].c_str(), "of"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[14].c_str(), "the"));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(r0[15].c_str(), "party"));

    ansak::ucs4String t1(U"Now  is  the  time.");
    vector<ansak::ucs4String> r1 = ansak::split(t1, U' ');
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), r1.size());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(r1[0].c_str(), U"Now", 16));
    CPPUNIT_ASSERT(r1[1].empty());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(r1[2].c_str(), U"is", 12));
    CPPUNIT_ASSERT(r1[3].empty());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(r1[4].c_str(), U"the", 16));
    CPPUNIT_ASSERT(r1[5].empty());
    CPPUNIT_ASSERT_EQUAL(0, memcmp(r1[6].c_str(), U"time.", 24));
}

void StringSplitJoinTestFixture::testJoiner()
{
    vector<string> a;
    a.push_back("c:");
    a.push_back("windows");
    a.push_back("system32");
    a.push_back("drivers");
    a.push_back("etc");
    a.push_back("hosts");
    string s(ansak::join(a, '\\'));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(s.c_str(), "c:\\windows\\system32\\drivers\\etc\\hosts"));

    string b("/home/akla/bust/me/up");
    vector<string> bSplit(ansak::split(b, '/'));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), bSplit.size());
    CPPUNIT_ASSERT_EQUAL(string(), bSplit[0]);
    CPPUNIT_ASSERT_EQUAL(string("home"), bSplit[1]);
    CPPUNIT_ASSERT_EQUAL(string("akla"), bSplit[2]);
    CPPUNIT_ASSERT_EQUAL(string("bust"), bSplit[3]);
    CPPUNIT_ASSERT_EQUAL(string("me"), bSplit[4]);
    CPPUNIT_ASSERT_EQUAL(string("up"), bSplit[5]);
    string bJoin(ansak::join(bSplit, '/'));
    CPPUNIT_ASSERT_EQUAL(b, bJoin);
}

void StringSplitJoinTestFixture::testSingle()
{
    string b("whole");
    auto c(ansak::split(b, '/'));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), c.size());
    CPPUNIT_ASSERT_EQUAL(b, c[0]);
    auto d(ansak::join(c, ':'));
    CPPUNIT_ASSERT_EQUAL(b, d);
}

