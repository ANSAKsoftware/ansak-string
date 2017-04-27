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
// 2015.01.02 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// configTest.cxx -- unit tests for the configuration collection
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <config.hxx>
#include <config_atom.hxx>
#include <config_types.hxx>
#include <runtime_exception.hxx>
#include <string.hxx>
#include <string>
#include <vector>

using namespace std;

using namespace ansak;
using namespace ansak::internal;
using namespace testing;

//===========================================================================

TEST(ConfigTest, testSimpleInit)
{
    Config simple;
    EXPECT_TRUE(simple.getValueNames().empty());

    simple.initValue("foo", true);
    EXPECT_FALSE(simple.getValueNames().empty());
}

//===========================================================================

TEST(ConfigTest, testUnchangeableInitValues)
{
    Config un;
    un.initValue("b", true, Config::immutable);
    un.initValue("i", 23, Config::immutable);
    un.initValue("f", 3.1415926535f, Config::immutable);
    un.initValue("d", 4.222333344444555555, Config::immutable);
    un.initValue("s", string("Now is the time"), Config::immutable);
    un.initValue("cp", "for all good men to come", Config::immutable);
    un.initValue("pt", toPoint(3, 4), Config::immutable);
    un.initValue("r", toRect(100, 60, 50, 30), Config::immutable);

    bool b, changed;
    int i;
    float f;
    double d;
    string s, cp;
    Point pt;
    Rect r; changed = true;
    EXPECT_TRUE(un.get("b", b, &changed)); EXPECT_FALSE(changed);
    EXPECT_FALSE(un.get("bb", b, &changed)); changed = true;
    EXPECT_TRUE(un.get("i", i, &changed)); EXPECT_FALSE(changed);
    EXPECT_FALSE(un.get("ii", i, &changed)); changed = true;
    EXPECT_TRUE(un.get("f", f, &changed)); EXPECT_FALSE(changed);
    EXPECT_FALSE(un.get("ff", f, &changed)); changed = false;
    // retrieving a double value -- officially, we only store floats in settings,
    // so retrieving a double is always a change from the original
    EXPECT_TRUE(un.get("d", d, &changed)); EXPECT_TRUE(changed);
    EXPECT_FALSE(un.get("dd", d, &changed)); changed = true;
    EXPECT_TRUE(un.get("s", s, &changed)); EXPECT_FALSE(changed);
    EXPECT_FALSE(un.get("ss", s, &changed)); changed = true;
    EXPECT_TRUE(un.get("cp", cp, &changed)); EXPECT_FALSE(changed);
    EXPECT_FALSE(un.get("ccp", cp, &changed)); changed = true;
    EXPECT_TRUE(un.get("pt", pt, &changed)); EXPECT_FALSE(changed);
    EXPECT_FALSE(un.get("ppt", pt, &changed)); changed = true;
    EXPECT_TRUE(un.get("r", r, &changed)); EXPECT_FALSE(changed);
    EXPECT_FALSE(un.get("rr", r, &changed)); changed = true;

    EXPECT_TRUE(b);
    EXPECT_EQ(23, i);
    EXPECT_TRUE(floatEqual(3.1415926535f, f));
    EXPECT_TRUE(floatEqual(4.222333344444555555, d));
    EXPECT_EQ(string("Now is the time"), s);
    EXPECT_EQ(string("for all good men to come"), cp);
    EXPECT_EQ(3, x(pt));
    EXPECT_EQ(4, y(pt));
    EXPECT_EQ(100, left(r));
    EXPECT_EQ(60, top(r));
    EXPECT_EQ(50, width(r));
    EXPECT_EQ(30, height(r));

    bool threw = false;
    try
    {
        un.put("b", false);
    }
    catch (RuntimeException& e)
    {
        threw = true;
    }
    EXPECT_TRUE(threw);
}

//===========================================================================

TEST(ConfigTest, testChangeableInitValues)
{
    Config ch;
    ch.initValue("b", true);
    ch.initValue("i", 23);
    ch.initValue("f", 3.1415926535f);
    ch.initValue("d", 4.222333344444555555);
    ch.initValue("s", string("Now is the time"));
    ch.initValue("cp", "for all good men to come");
    ch.initValue("pt", toPoint(3, 4));
    ch.initValue("r", toRect(100, 60, 50, 30));

    bool b, changed;
    int i;
    float f, d;
    string s, cp;
    Point pt;
    Rect r; changed = true;
    EXPECT_TRUE(ch.get("b", b, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(ch.get("i", i, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(ch.get("f", f, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(ch.get("d", d, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(ch.get("s", s, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(ch.get("cp", cp, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(ch.get("pt", pt, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(ch.get("r", r, &changed)); EXPECT_FALSE(changed); changed = true;

    EXPECT_TRUE(b);
    EXPECT_EQ(23, i);
    EXPECT_TRUE(floatEqual(3.1415926535f, f));
    EXPECT_TRUE(floatEqual(4.222333344444555555, d));
    EXPECT_EQ(string("Now is the time"), s);
    EXPECT_EQ(string("for all good men to come"), cp);
    EXPECT_EQ(3, x(pt));
    EXPECT_EQ(4, y(pt));
    EXPECT_EQ(100, left(r));
    EXPECT_EQ(60, top(r));
    EXPECT_EQ(50, width(r));
    EXPECT_EQ(30, height(r));

    bool threw = false;
    try
    {
        ch.put("b", false);
        threw = false;
    }
    catch (RuntimeException& e)
    {
        threw = true;
    }
    EXPECT_FALSE(threw);

    ch.put("i", 32);
    ch.put("f", 2.7182818301f);
    ch.put("d", 5.6677788889999900000111111);
    ch.put("s", "Et Earello Endorenna utulien.");
    ch.put("cp", "Sinome maruvan ar Hildinyar tenn' ambar metta.");
    ch.put("pt", toPoint(5, 6));
    ch.put("r", toRect(500, 300, 20, 20));

    changed = false;
    EXPECT_TRUE(ch.get("b", b, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_TRUE(ch.get("i", i, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_TRUE(ch.get("f", f, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_TRUE(ch.get("d", d, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_TRUE(ch.get("s", s, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_TRUE(ch.get("cp", cp, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_TRUE(ch.get("pt", pt, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_TRUE(ch.get("r", r, &changed)); EXPECT_TRUE(changed); changed = false;

    EXPECT_FALSE(b);
    EXPECT_EQ(32, i);
    EXPECT_TRUE(floatEqual(2.7182818301f, f));
    EXPECT_TRUE(floatEqual(5.6677788889999900000111111, d));
    EXPECT_EQ(string("Et Earello Endorenna utulien."), s);
    EXPECT_EQ(string("Sinome maruvan ar Hildinyar tenn' ambar metta."), cp);
    EXPECT_EQ(5, x(pt));
    EXPECT_EQ(6, y(pt));
    EXPECT_EQ(500, left(r));
    EXPECT_EQ(300, top(r));
    EXPECT_EQ(20, width(r));
    EXPECT_EQ(20, height(r));
}

//===========================================================================

TEST(ConfigTest, testGetValueNames)
{
    Config ch;
    ch.initValue("b", true);
    ch.initValue("i", 23);
    ch.initValue("f", 3.1415926535f);
    ch.initValue("d", 4.222333344444555555);
    ch.initValue("s", string("Now is the time"));
    ch.initValue("cp", "for all good men to come");
    ch.initValue("pt", toPoint(3, 4));
    ch.initValue("r", toRect(100, 60, 50, 30));

    bool gotB = false;
    bool gotI = false;
    bool gotF = false;
    bool gotD = false;
    bool gotS = false;
    bool gotCP = false;
    bool gotPT = false;
    bool gotR = false;

    auto names = ch.getValueNames();
    EXPECT_EQ(static_cast<size_t>(8), names.size());

    for (auto n : names)
    {
        if (n == "b")           gotB = true;
        else if (n == "i")      gotI = true;
        else if (n == "f")      gotF = true;
        else if (n == "d")      gotD = true;
        else if (n == "s")      gotS = true;
        else if (n == "cp")     gotCP = true;
        else if (n == "pt")     gotPT = true;
        else if (n == "r")      gotR = true;
    }

    EXPECT_TRUE(gotB);
    EXPECT_TRUE(gotI);
    EXPECT_TRUE(gotF);
    EXPECT_TRUE(gotD);
    EXPECT_TRUE(gotS);
    EXPECT_TRUE(gotCP);
    EXPECT_TRUE(gotPT);
    EXPECT_TRUE(gotR);
}

//===========================================================================

TEST(ConfigTest, testTemplatedConstructor)
{
    const vector<string> utf8v{
        "b=true",
        "i=23",
        "f=3.14159265",
        "d=5.66777888899999",
        "s=ash nazg durbatuluk",
        "pt=3,4",
        "",
        "r=(100,40),(60,20)",
        "q=v=w"
    };

    Config c0(utf8v);

    bool b, changed;
    int i;
    float f, d;
    string s,q;
    Point pt;
    Rect r; changed = true;
    EXPECT_TRUE(c0.get("b", b, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(c0.get("i", i, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(c0.get("f", f, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(c0.get("d", d, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(c0.get("s", s, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(c0.get("pt", pt, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(c0.get("r", r, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(c0.get("q", q, &changed)); EXPECT_FALSE(changed); changed = true;

    EXPECT_TRUE(b);
    EXPECT_EQ(23, i);
    EXPECT_TRUE(floatEqual(3.14159265f, f));
    EXPECT_TRUE(floatEqual(5.66777888899999, d));
    EXPECT_EQ(string("ash nazg durbatuluk"), s);
    EXPECT_EQ(3, x(pt));
    EXPECT_EQ(4, y(pt));
    EXPECT_EQ(100, left(r));
    EXPECT_EQ(40, top(r));
    EXPECT_EQ(60, width(r));
    EXPECT_EQ(20, height(r));
    EXPECT_EQ(string("v=w"), q);
}

//===========================================================================

TEST(ConfigTest, testGetValues)
{
    Config ch;
    ch.initValue("b", true);
    ch.initValue("i", 23);
    ch.initValue("f", 3.1415926535f);
    ch.initValue("d", 4.222333344444555555);
    ch.initValue("s", string("Now is the time"));
    ch.initValue("cp", "for all good men to come");
    ch.initValue("pt", toPoint(3, 4));
    ch.initValue("r", toRect(100, 60, 50, 30));

    bool b;
    int i;
    float f;
    double d;
    string s;
    Point pt;
    Rect r;
    EXPECT_TRUE(ch.get("b", b));    EXPECT_TRUE(b);
    EXPECT_TRUE(ch.get("b", i));    EXPECT_EQ(1, i);
    EXPECT_TRUE(ch.get("b", f));    EXPECT_TRUE(floatEqual(1.0f, f));
    EXPECT_TRUE(ch.get("b", d));    EXPECT_TRUE(floatEqual(1.0, d));
    EXPECT_TRUE(ch.get("b", s));    EXPECT_EQ(string("true"), s);
    EXPECT_FALSE(ch.get("b", pt));
    EXPECT_FALSE(ch.get("b", r));
    EXPECT_TRUE(ch.get("i", b));    EXPECT_TRUE(b);
    EXPECT_TRUE(ch.get("i", i));    EXPECT_EQ(23, i);
    EXPECT_TRUE(ch.get("i", f));    EXPECT_TRUE(floatEqual(23.0f, f));
    EXPECT_TRUE(ch.get("i", d));    EXPECT_TRUE(floatEqual(23.0, d));
    EXPECT_TRUE(ch.get("i", s));    EXPECT_EQ(string("23"), s);
    EXPECT_FALSE(ch.get("i", pt));
    EXPECT_FALSE(ch.get("i", r));
    EXPECT_TRUE(ch.get("f", b));    EXPECT_TRUE(b);
    EXPECT_FALSE(ch.get("f", i));
    EXPECT_TRUE(ch.get("f", f));    EXPECT_TRUE(floatEqual(3.1415926535f, f));
    EXPECT_TRUE(ch.get("f", d));    EXPECT_TRUE(floatEqual(3.1415926535, d));
    EXPECT_TRUE(ch.get("f", s));    EXPECT_EQ(string("3.14159"), s);
    EXPECT_FALSE(ch.get("f", pt));
    EXPECT_FALSE(ch.get("f", r));
    EXPECT_TRUE(ch.get("d", b));    EXPECT_TRUE(b);
    EXPECT_FALSE(ch.get("d", i));
    EXPECT_TRUE(ch.get("d", f));    EXPECT_TRUE(floatEqual(4.2223333444f, f));
    EXPECT_TRUE(ch.get("d", d));    EXPECT_TRUE(floatEqual(4.2223333444, d));
    EXPECT_TRUE(ch.get("d", s));    EXPECT_EQ(string("4.22233"), s);
    EXPECT_FALSE(ch.get("d", pt));
    EXPECT_FALSE(ch.get("d", r));
    EXPECT_FALSE(ch.get("s", b));
    EXPECT_FALSE(ch.get("s", i));
    EXPECT_FALSE(ch.get("s", f));
    EXPECT_FALSE(ch.get("s", d));
    EXPECT_TRUE(ch.get("s", s));    EXPECT_EQ(string("Now is the time"), s);
    EXPECT_FALSE(ch.get("s", pt));
    EXPECT_FALSE(ch.get("s", r));
    EXPECT_FALSE(ch.get("cp", b));
    EXPECT_FALSE(ch.get("cp", i));
    EXPECT_FALSE(ch.get("cp", f));
    EXPECT_FALSE(ch.get("cp", d));
    EXPECT_TRUE(ch.get("cp", s));    EXPECT_EQ(string("for all good men to come"), s);
    EXPECT_FALSE(ch.get("cp", pt));
    EXPECT_FALSE(ch.get("cp", r));
    EXPECT_FALSE(ch.get("pt", b));
    EXPECT_FALSE(ch.get("pt", i));
    EXPECT_FALSE(ch.get("pt", f));
    EXPECT_FALSE(ch.get("pt", d));
    EXPECT_TRUE(ch.get("pt", s));    EXPECT_EQ(string("3,4"), s);
    EXPECT_TRUE(ch.get("pt", pt));
    EXPECT_EQ(3, x(pt));
    EXPECT_EQ(4, y(pt));
    EXPECT_FALSE(ch.get("pt", r));
    EXPECT_FALSE(ch.get("r", b));
    EXPECT_FALSE(ch.get("r", i));
    EXPECT_FALSE(ch.get("r", f));
    EXPECT_FALSE(ch.get("r", d));
    EXPECT_TRUE(ch.get("r", s));    EXPECT_EQ(string("(100,60),(50,30)"), s);
    EXPECT_FALSE(ch.get("r", pt));
    EXPECT_TRUE(ch.get("r", r));
    EXPECT_EQ(100, left(r));
    EXPECT_EQ(60, top(r));
    EXPECT_EQ(50, width(r));
    EXPECT_EQ(30, height(r));
}

// these two "case" ideas are being tested in testChangeableInitValues
// TEST(ConfigTest, testPutValues);
// TEST(ConfigTest, testGetChangedValues);

//===========================================================================

TEST(ConfigTest, testPutForceTypeValues)
{
    Config ch;
    ch.initValue("b", true);
    ch.initValue("i", 23);
    ch.initValue("pi", 3.1415926535f, Config::immutable);
    ch.initValue("d", 4.222333344444555555);
    ch.initValue("s", string("Now is the time"));
    ch.initValue("cp", "for all good men to come");
    ch.initValue("pt", toPoint(3, 4));
    ch.initValue("r", toRect(100, 60, 50, 30));

    bool threw = false;

    ch.put("b", 34, Config::forceTypeChange);
    ch.put("i", 2.71828301, Config::forceTypeChange);
    try
    {
        ch.put("pi", 3.14, Config::forceTypeChange);
        threw = false;
    }
    catch (RuntimeException& e)
    {
        threw = true;
    }
    EXPECT_TRUE(threw);
    ch.put("d", "Sorry, Murphy, no real constants are variable.", Config::forceTypeChange);
    ch.put("s", toPoint(16,22), Config::forceTypeChange);
    ch.put("pt", toRect(1,4,9,16), Config::forceTypeChange);
    ch.put("r", false, Config::forceTypeChange);

    bool b, changed;
    int i;
    float pi, d;
    string s;
    Point pt;
    Rect r; changed = false;
    EXPECT_TRUE(ch.get("b", i, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_EQ(34, i);
    EXPECT_TRUE(ch.get("i", d, &changed)); EXPECT_TRUE(changed); changed = true;
    EXPECT_TRUE(floatEqual(2.71828301, d));
    EXPECT_TRUE(ch.get("pi", pi, &changed)); EXPECT_FALSE(changed); changed = false;
    EXPECT_TRUE(floatEqual(3.1415926, pi));
    EXPECT_TRUE(ch.get("d", s, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_EQ(string("Sorry, Murphy, no real constants are variable."), s);
    EXPECT_TRUE(ch.get("s", pt, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_EQ(16, x(pt));
    EXPECT_EQ(22, y(pt));
    EXPECT_TRUE(ch.get("pt", r, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_EQ(1, left(r));
    EXPECT_EQ(4, top(r));
    EXPECT_EQ(9, width(r));
    EXPECT_EQ(16, height(r));
    EXPECT_TRUE(ch.get("r", b, &changed)); EXPECT_TRUE(changed); changed = false;
    EXPECT_FALSE(b);

    ch.put("b", 1.618f, Config::forceTypeChange);
    EXPECT_TRUE(ch.get("b", d, &changed)); EXPECT_TRUE(changed); changed = true;
    EXPECT_TRUE(floatEqual(1.618, d));
}

//===========================================================================

TEST(ConfigTest, testDefaultWith)
{
    Config s;
    s.initValue("a", 3);
    s.initValue("b", 4);
    Config q;
    q.initValue("c", 5);
    s.defaultWith(q);
    int a, b, c;
    bool changed = true;
    EXPECT_TRUE(s.get("a", a, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(s.get("b", b, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(s.get("c", c, &changed)); EXPECT_FALSE(changed);
    EXPECT_EQ(3, a);
    EXPECT_EQ(4, b);
    EXPECT_EQ(5, c);
}

//===========================================================================

TEST(ConfigTest, testOverrideWith)
{
    Config s;
    s.initValue("a", "33");
    s.initValue("b", 4);
    s.initValue("pi", 3.141592653, Config::immutable);
    Config q;
    q.initValue("c", 5);
    q.initValue("a", 3);
    q.initValue("pi", "3.14");
    s.overrideWith(q);
    int a, b, c;
    float f;
    bool changed = true;
    EXPECT_TRUE(s.get("a", a, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(s.get("b", b, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(s.get("c", c, &changed)); EXPECT_FALSE(changed); changed = true;
    EXPECT_TRUE(s.get("pi", f, &changed)); EXPECT_FALSE(changed);
    EXPECT_EQ(3, a);
    EXPECT_EQ(4, b);
    EXPECT_EQ(5, c);
    EXPECT_TRUE(floatEqual(3.1415926, f));
}

//===========================================================================

TEST(ConfigTest, testKeepOverridesOf)
{
    Config s;
    s.initValue("a", 3);
    s.initValue("b", 4);
    s.initValue("c", 55);
    Config q;

    q.initValue("c", 5);
    Config savedQ(q);
    EXPECT_EQ(q, savedQ);
    q.defaultWith(s);

    EXPECT_NE(q, savedQ);

    q.keepOverridesOf(s);
    EXPECT_EQ(q, savedQ);
}

//===========================================================================

TEST(ConfigTest, testAtomTransaction)
{
    const vector<string> utf8v{
        "b= true ",
        "i=23",
        "f=3.14159265",
        "d=5.66777888899999",
        "s=ash nazg durbatuluk",
        "pt=3,4",
        "",
        "r=(100,40),(60,20)",
        "q=v=w"
    };
    Config c0(utf8v);

    vector<ConfigAtomPair> atomPairs;
    atomPairs.push_back(ConfigAtomPair(string("b"),  c0.getAtom("b")));
    atomPairs.push_back(ConfigAtomPair(string("i"),  c0.getAtom("i")));
    atomPairs.push_back(ConfigAtomPair(string("f"),  c0.getAtom("f")));
    atomPairs.push_back(ConfigAtomPair(string("d"),  c0.getAtom("d")));
    atomPairs.push_back(ConfigAtomPair(string("s"),  c0.getAtom("s")));
    atomPairs.push_back(ConfigAtomPair(string("pt"), c0.getAtom("pt")));
    atomPairs.push_back(ConfigAtomPair(string("r"),  c0.getAtom("r")));
    atomPairs.push_back(ConfigAtomPair(string("q"),  c0.getAtom("q")));
    EXPECT_EQ(ConfigAtom(), c0.getAtom("notThere"));

    Config c1(atomPairs);
    EXPECT_EQ(c0, c1);

    Config c2 = c1;
    c2.put("b", false);
    EXPECT_NE(c2, c1);
    c1.put("b", false);
    EXPECT_EQ(c2, c1);

    Config c3;
    c3 = c2;
    EXPECT_EQ(c3, c1);
}

//===========================================================================

TEST(ConfigTest, manualAdd)
{
    const vector<string> utf8v{
        "b=true",
        "i=23",
        "f=3.14159265",
        "d=5.66777888899999",
        "s=ash nazg durbatuluk",
        "pt=3,4",
        "r=(100,40),(60,20)"
    };
    Config c0(utf8v);

    Config c1;
    c1.put("r", toRect(100,40,60,20));
    c1.put("pt", toPoint(3,4));
    c1.put("s", "ash nazg durbatuluk");
    c1.put("d", 5.66777888899999);
    c1.put("f", 3.14159265f);
    c1.put("i", 23);
    c1.put("b", true);

    EXPECT_EQ(c0, c1);
}
