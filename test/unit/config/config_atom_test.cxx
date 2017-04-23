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
// 2014.12.21 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// configAtomTest.cxx -- unit tests for the configuration atom.
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <config_atom.hxx>
#include <config_types.hxx>
#include <runtime_exception.hxx>

using namespace std;

using namespace ansak;
using namespace ansak::internal;
using namespace testing;

TEST(ConfigAtomTest, testEmptyString)
{
    ConfigAtom aa(ConfigAtom::fromString(string()));
    EXPECT_FALSE(aa.isChangeable());
    EXPECT_FALSE(aa.isChanged());
    EXPECT_FALSE(aa.isBool());
    EXPECT_FALSE(aa.isInt());
    EXPECT_FALSE(aa.isFloat());
    EXPECT_TRUE(aa.isString());
    EXPECT_FALSE(aa.isPoint());
    EXPECT_FALSE(aa.isRect());
    EXPECT_FALSE(aa.canBeBool());
    EXPECT_FALSE(aa.canBeInt());
    EXPECT_FALSE(aa.canBeFloat());
    EXPECT_TRUE(aa.canBeString());
    EXPECT_FALSE(aa.canBePoint());
    EXPECT_FALSE(aa.canBeRect());
}

TEST(ConfigAtomTest, testSadSack)
{
    ConfigAtom a;
    EXPECT_FALSE(a.isChangeable());
    EXPECT_FALSE(a.isChanged());
    EXPECT_FALSE(a.isBool());
    EXPECT_FALSE(a.isInt());
    EXPECT_FALSE(a.isFloat());
    EXPECT_FALSE(a.isString());
    EXPECT_FALSE(a.isPoint());
    EXPECT_FALSE(a.isRect());
    EXPECT_FALSE(a.canBeBool());
    EXPECT_FALSE(a.canBeInt());
    EXPECT_FALSE(a.canBeFloat());
    EXPECT_FALSE(a.canBeString());
    EXPECT_FALSE(a.canBePoint());
    EXPECT_FALSE(a.canBeRect());

    // not a UTF-8 string
    ConfigAtom aa(ConfigAtom::fromString("mohmen R\xe4th\'"));
    EXPECT_FALSE(aa.isChangeable());
    EXPECT_FALSE(aa.isChanged());
    EXPECT_FALSE(aa.isBool());
    EXPECT_FALSE(aa.isInt());
    EXPECT_FALSE(aa.isFloat());
    EXPECT_FALSE(aa.isString());
    EXPECT_FALSE(aa.isPoint());
    EXPECT_FALSE(aa.isRect());
    EXPECT_FALSE(aa.canBeBool());
    EXPECT_FALSE(aa.canBeInt());
    EXPECT_FALSE(aa.canBeFloat());
    EXPECT_FALSE(aa.canBeString());
    EXPECT_FALSE(aa.canBePoint());
    EXPECT_FALSE(aa.canBeRect());
}

TEST(ConfigAtomTest, testManualBool)
{
    ConfigAtom b(true);
    EXPECT_FALSE(b.isChangeable());
    EXPECT_FALSE(b.isChanged());
    EXPECT_TRUE(b.isBool());
    EXPECT_FALSE(b.isInt());
    EXPECT_FALSE(b.isFloat());
    EXPECT_FALSE(b.isString());
    EXPECT_FALSE(b.isPoint());
    EXPECT_FALSE(b.isRect());
    EXPECT_TRUE(b.canBeBool());
    EXPECT_TRUE(b.canBeInt());
    EXPECT_TRUE(b.canBeFloat());
    EXPECT_TRUE(b.canBeString());
    EXPECT_FALSE(b.canBePoint());
    EXPECT_FALSE(b.canBeRect());
}

TEST(ConfigAtomTest, testManualInt)
{
    ConfigAtom c(35);
    EXPECT_FALSE(c.isChangeable());
    EXPECT_FALSE(c.isChanged());
    EXPECT_FALSE(c.isBool());
    EXPECT_TRUE(c.isInt());
    EXPECT_FALSE(c.isFloat());
    EXPECT_FALSE(c.isString());
    EXPECT_FALSE(c.isPoint());
    EXPECT_FALSE(c.isRect());
    EXPECT_TRUE(c.canBeBool());
    EXPECT_TRUE(c.canBeInt());
    EXPECT_TRUE(c.canBeFloat());
    EXPECT_TRUE(c.canBeString());
    EXPECT_FALSE(c.canBePoint());
    EXPECT_FALSE(c.canBeRect());
}

TEST(ConfigAtomTest, testManualFloat)
{
    ConfigAtom d(3.5f);
    EXPECT_FALSE(d.isChangeable());
    EXPECT_FALSE(d.isChanged());
    EXPECT_FALSE(d.isBool());
    EXPECT_FALSE(d.isInt());
    EXPECT_TRUE(d.isFloat());
    EXPECT_FALSE(d.isString());
    EXPECT_FALSE(d.isPoint());
    EXPECT_FALSE(d.isRect());
    EXPECT_TRUE(d.canBeBool());
    EXPECT_FALSE(d.canBeInt());
    EXPECT_TRUE(d.canBeFloat());
    EXPECT_TRUE(d.canBeString());
    EXPECT_FALSE(d.canBePoint());
    EXPECT_FALSE(d.canBeRect());

    ConfigAtom p(3.5);
    EXPECT_FALSE(p.isChangeable());
    EXPECT_FALSE(p.isChanged());
    EXPECT_FALSE(p.isBool());
    EXPECT_FALSE(p.isInt());
    EXPECT_TRUE(p.isFloat());
    EXPECT_FALSE(p.isString());
    EXPECT_FALSE(p.isPoint());
    EXPECT_FALSE(p.isRect());
    EXPECT_TRUE(p.canBeBool());
    EXPECT_FALSE(p.canBeInt());
    EXPECT_TRUE(p.canBeFloat());
    EXPECT_TRUE(p.canBeString());
    EXPECT_FALSE(p.canBePoint());
    EXPECT_FALSE(p.canBeRect());
}

TEST(ConfigAtomTest, testIntLikeFloat)
{
    ConfigAtom d(3.0f);
    EXPECT_FALSE(d.isChangeable());
    EXPECT_FALSE(d.isChanged());
    EXPECT_FALSE(d.isBool());
    EXPECT_FALSE(d.isInt());
    EXPECT_TRUE(d.isFloat());
    EXPECT_FALSE(d.isString());
    EXPECT_FALSE(d.isPoint());
    EXPECT_FALSE(d.isRect());
    EXPECT_TRUE(d.canBeBool());
    EXPECT_TRUE(d.canBeInt());
    EXPECT_TRUE(d.canBeFloat());
    EXPECT_TRUE(d.canBeString());
    EXPECT_FALSE(d.canBePoint());
    EXPECT_FALSE(d.canBeRect());

    ConfigAtom p(3.0);
    EXPECT_FALSE(p.isChangeable());
    EXPECT_FALSE(p.isChanged());
    EXPECT_FALSE(p.isBool());
    EXPECT_FALSE(p.isInt());
    EXPECT_TRUE(p.isFloat());
    EXPECT_FALSE(p.isString());
    EXPECT_FALSE(p.isPoint());
    EXPECT_FALSE(p.isRect());
    EXPECT_TRUE(p.canBeBool());
    EXPECT_TRUE(p.canBeInt());
    EXPECT_TRUE(p.canBeFloat());
    EXPECT_TRUE(p.canBeString());
    EXPECT_FALSE(p.canBePoint());
    EXPECT_FALSE(p.canBeRect());
}

TEST(ConfigAtomTest, testManualString)
{
    ConfigAtom e(string("This is a string."));
    EXPECT_FALSE(e.isChangeable());
    EXPECT_FALSE(e.isChanged());
    EXPECT_FALSE(e.isBool());
    EXPECT_FALSE(e.isInt());
    EXPECT_FALSE(e.isFloat());
    EXPECT_TRUE(e.isString());
    EXPECT_FALSE(e.isPoint());
    EXPECT_FALSE(e.isRect());
    EXPECT_FALSE(e.canBeBool());
    EXPECT_FALSE(e.canBeInt());
    EXPECT_FALSE(e.canBeFloat());
    EXPECT_TRUE(e.canBeString());
    EXPECT_FALSE(e.canBePoint());
    EXPECT_FALSE(e.canBeRect());

    ConfigAtom f("This is a string.");
    EXPECT_FALSE(f.isChangeable());
    EXPECT_FALSE(f.isChanged());
    EXPECT_FALSE(f.isBool());
    EXPECT_FALSE(f.isInt());
    EXPECT_FALSE(f.isFloat());
    EXPECT_TRUE(f.isString());
    EXPECT_FALSE(f.isPoint());
    EXPECT_FALSE(f.isRect());
    EXPECT_FALSE(f.canBeBool());
    EXPECT_FALSE(f.canBeInt());
    EXPECT_FALSE(f.canBeFloat());
    EXPECT_TRUE(f.canBeString());
    EXPECT_FALSE(f.canBePoint());
    EXPECT_FALSE(f.canBeRect());
}

TEST(ConfigAtomTest, testManualPoint)
{
    ConfigAtom g(toPoint(3, 5));
    EXPECT_FALSE(g.isChangeable());
    EXPECT_FALSE(g.isChanged());
    EXPECT_FALSE(g.isBool());
    EXPECT_FALSE(g.isInt());
    EXPECT_FALSE(g.isFloat());
    EXPECT_FALSE(g.isString());
    EXPECT_TRUE(g.isPoint());
    EXPECT_FALSE(g.isRect());
    EXPECT_FALSE(g.canBeBool());
    EXPECT_FALSE(g.canBeInt());
    EXPECT_FALSE(g.canBeFloat());
    EXPECT_TRUE(g.canBeString());
    EXPECT_TRUE(g.canBePoint());
    EXPECT_FALSE(g.canBeRect());
}

TEST(ConfigAtomTest, testManualRect)
{
    ConfigAtom h(toRect(3, 5, 35, 44));
    EXPECT_FALSE(h.isChangeable());
    EXPECT_FALSE(h.isChanged());
    EXPECT_FALSE(h.isBool());
    EXPECT_FALSE(h.isInt());
    EXPECT_FALSE(h.isFloat());
    EXPECT_FALSE(h.isString());
    EXPECT_FALSE(h.isPoint());
    EXPECT_TRUE(h.isRect());
    EXPECT_FALSE(h.canBeBool());
    EXPECT_FALSE(h.canBeInt());
    EXPECT_FALSE(h.canBeFloat());
    EXPECT_TRUE(h.canBeString());
    EXPECT_FALSE(h.canBePoint());
    EXPECT_TRUE(h.canBeRect());
}

TEST(ConfigAtomTest, testChangeableBool)
{
    ConfigAtom b(true, ConfigAtom::changeable);
    EXPECT_TRUE(b.isChangeable());
    EXPECT_FALSE(b.isChanged());
    EXPECT_TRUE(b.isBool());
    EXPECT_FALSE(b.isInt());
    EXPECT_FALSE(b.isFloat());
    EXPECT_FALSE(b.isString());
    EXPECT_FALSE(b.isPoint());
    EXPECT_FALSE(b.isRect());
    EXPECT_TRUE(b.canBeBool());
    EXPECT_TRUE(b.canBeInt());
    EXPECT_TRUE(b.canBeFloat());
    EXPECT_TRUE(b.canBeString());
    EXPECT_FALSE(b.canBePoint());
    EXPECT_FALSE(b.canBeRect());

    b.set(true);
    EXPECT_FALSE(b.isChanged());
    b.set(false);
    EXPECT_TRUE(b.isChanged());
}

TEST(ConfigAtomTest, testChangeableInt)
{
    ConfigAtom c(35, ConfigAtom::changeable);
    EXPECT_TRUE(c.isChangeable());
    EXPECT_FALSE(c.isChanged());
    EXPECT_FALSE(c.isBool());
    EXPECT_TRUE(c.isInt());
    EXPECT_FALSE(c.isFloat());
    EXPECT_FALSE(c.isString());
    EXPECT_FALSE(c.isPoint());
    EXPECT_FALSE(c.isRect());
    EXPECT_TRUE(c.canBeBool());
    EXPECT_TRUE(c.canBeInt());
    EXPECT_TRUE(c.canBeFloat());
    EXPECT_TRUE(c.canBeString());
    EXPECT_FALSE(c.canBePoint());
    EXPECT_FALSE(c.canBeRect());

    c.set(35);
    EXPECT_FALSE(c.isChanged());
    c.set(36);
    EXPECT_TRUE(c.isChanged());
}

TEST(ConfigAtomTest, testChangeableFloat)
{
    ConfigAtom d(3.5f, ConfigAtom::changeable);
    EXPECT_TRUE(d.isChangeable());
    EXPECT_FALSE(d.isChanged());
    EXPECT_FALSE(d.isBool());
    EXPECT_FALSE(d.isInt());
    EXPECT_TRUE(d.isFloat());
    EXPECT_FALSE(d.isString());
    EXPECT_FALSE(d.isPoint());
    EXPECT_FALSE(d.isRect());
    EXPECT_TRUE(d.canBeBool());
    EXPECT_FALSE(d.canBeInt());
    EXPECT_TRUE(d.canBeFloat());
    EXPECT_TRUE(d.canBeString());
    EXPECT_FALSE(d.canBePoint());
    EXPECT_FALSE(d.canBeRect());

    d.set(3.5f);
    EXPECT_FALSE(d.isChanged());
    d.set(4.5f);
    EXPECT_TRUE(d.isChanged());
    d.set(4.0f);
    EXPECT_TRUE(d.canBeInt());

    ConfigAtom p(4.5, ConfigAtom::changeable);
    EXPECT_TRUE(p.isChangeable());
    EXPECT_FALSE(p.isChanged());
    EXPECT_FALSE(p.isBool());
    EXPECT_FALSE(p.isInt());
    EXPECT_TRUE(p.isFloat());
    EXPECT_FALSE(p.isString());
    EXPECT_FALSE(p.isPoint());
    EXPECT_FALSE(p.isRect());
    EXPECT_TRUE(p.canBeBool());
    EXPECT_FALSE(p.canBeInt());
    EXPECT_TRUE(p.canBeFloat());
    EXPECT_TRUE(p.canBeString());
    EXPECT_FALSE(p.canBePoint());
    EXPECT_FALSE(p.canBeRect());

    p.set(4.5);
    EXPECT_FALSE(p.isChanged());
    p.set(6.5);
    EXPECT_TRUE(p.isChanged());
}

TEST(ConfigAtomTest, testChangeableString)
{
    ConfigAtom e(string("This is a string."), ConfigAtom::changeable);
    EXPECT_TRUE(e.isChangeable());
    EXPECT_FALSE(e.isChanged());
    EXPECT_FALSE(e.isBool());
    EXPECT_FALSE(e.isInt());
    EXPECT_FALSE(e.isFloat());
    EXPECT_TRUE(e.isString());
    EXPECT_FALSE(e.isPoint());
    EXPECT_FALSE(e.isRect());
    EXPECT_FALSE(e.canBeBool());
    EXPECT_FALSE(e.canBeInt());
    EXPECT_FALSE(e.canBeFloat());
    EXPECT_TRUE(e.canBeString());
    EXPECT_FALSE(e.canBePoint());
    EXPECT_FALSE(e.canBeRect());

    e.set("This is a string.");
    EXPECT_FALSE(e.isChanged());
    e.set("Give me a break.");
    EXPECT_TRUE(e.isChanged());

    ConfigAtom f("This is a string.", ConfigAtom::changeable);
    EXPECT_TRUE(f.isChangeable());
    EXPECT_FALSE(f.isChanged());
    EXPECT_FALSE(f.isBool());
    EXPECT_FALSE(f.isInt());
    EXPECT_FALSE(f.isFloat());
    EXPECT_TRUE(f.isString());
    EXPECT_FALSE(f.isPoint());
    EXPECT_FALSE(f.isRect());
    EXPECT_FALSE(f.canBeBool());
    EXPECT_FALSE(f.canBeInt());
    EXPECT_FALSE(f.canBeFloat());
    EXPECT_TRUE(f.canBeString());
    EXPECT_FALSE(f.canBePoint());
    EXPECT_FALSE(f.canBeRect());

    f.set(string("This is a string."));
    EXPECT_FALSE(f.isChanged());
    f.set(string("Give me a break."));
    EXPECT_TRUE(f.isChanged());
}

TEST(ConfigAtomTest, testChangeablePoint)
{
    ConfigAtom g(toPoint(3, 5), ConfigAtom::changeable);
    EXPECT_TRUE(g.isChangeable());
    EXPECT_FALSE(g.isChanged());
    EXPECT_FALSE(g.isBool());
    EXPECT_FALSE(g.isInt());
    EXPECT_FALSE(g.isFloat());
    EXPECT_FALSE(g.isString());
    EXPECT_TRUE(g.isPoint());
    EXPECT_FALSE(g.isRect());
    EXPECT_FALSE(g.canBeBool());
    EXPECT_FALSE(g.canBeInt());
    EXPECT_FALSE(g.canBeFloat());
    EXPECT_TRUE(g.canBeString());
    EXPECT_TRUE(g.canBePoint());
    EXPECT_FALSE(g.canBeRect());

    g.set(toPoint(3, 5));
    EXPECT_FALSE(g.isChanged());
    g.set(toPoint(5, 3));
    EXPECT_TRUE(g.isChanged());
}

TEST(ConfigAtomTest, testChangeableRect)
{
    ConfigAtom h(toRect(3, 5, 35, 44), ConfigAtom::changeable);
    EXPECT_TRUE(h.isChangeable());
    EXPECT_FALSE(h.isChanged());
    EXPECT_FALSE(h.isBool());
    EXPECT_FALSE(h.isInt());
    EXPECT_FALSE(h.isFloat());
    EXPECT_FALSE(h.isString());
    EXPECT_FALSE(h.isPoint());
    EXPECT_TRUE(h.isRect());
    EXPECT_FALSE(h.canBeBool());
    EXPECT_FALSE(h.canBeInt());
    EXPECT_FALSE(h.canBeFloat());
    EXPECT_TRUE(h.canBeString());
    EXPECT_FALSE(h.canBePoint());
    EXPECT_TRUE(h.canBeRect());

    h.set(toRect(3, 5, 35, 44));
    EXPECT_FALSE(h.isChanged());
    h.set(toRect(5, 3, 35, 44));
    EXPECT_TRUE(h.isChanged());
}

TEST(ConfigAtomTest, testStringForString)
{
    ConfigAtom justAString(ConfigAtom::fromString(string("I'm just a string")));
    EXPECT_FALSE(justAString.isChangeable());
    EXPECT_FALSE(justAString.isChanged());
    EXPECT_TRUE(justAString.isString());
    EXPECT_TRUE(justAString.canBeString());

    ConfigAtom aChangeableString(ConfigAtom::fromString("I'm just a string",
                                                        ConfigAtom::changeable));
    EXPECT_TRUE(aChangeableString.isChangeable());
    EXPECT_FALSE(aChangeableString.isChanged());
    EXPECT_TRUE(aChangeableString.isString());
    EXPECT_TRUE(aChangeableString.canBeString());

    aChangeableString.set("I'm just a string");
    EXPECT_FALSE(aChangeableString.isChanged());
    aChangeableString.set(string("Of course, I'm more than Just a string!"));
    EXPECT_TRUE(aChangeableString.isChanged());
}

TEST(ConfigAtomTest, testStringForBool)
{
    ConfigAtom b(ConfigAtom::fromString("true"));
    EXPECT_FALSE(b.isChangeable());
    EXPECT_TRUE(b.isBool());
    EXPECT_TRUE(b.canBeBool());
    EXPECT_TRUE(b.canBeInt());
    EXPECT_TRUE(b.canBeFloat());
    EXPECT_TRUE(b.canBeString());

    ConfigAtom bb(ConfigAtom::fromString(string("false"), ConfigAtom::changeable));
    EXPECT_TRUE(bb.isChangeable());
    EXPECT_FALSE(bb.isChanged());
    EXPECT_TRUE(bb.isBool());
    EXPECT_TRUE(bb.canBeBool());
    EXPECT_TRUE(bb.canBeInt());
    EXPECT_TRUE(bb.canBeFloat());
    EXPECT_TRUE(bb.canBeString());

    bb.set(false);
    EXPECT_FALSE(bb.isChanged());
    bb.set(true);
    EXPECT_TRUE(bb.isChanged());
}

TEST(ConfigAtomTest, testStringForInt)
{
    ConfigAtom i0(ConfigAtom::fromString("0"));
    EXPECT_TRUE(i0.isInt());
    ConfigAtom i_1(ConfigAtom::fromString("-1"));
    EXPECT_TRUE(i_1.isInt());
    ConfigAtom i1(ConfigAtom::fromString("1"));
    EXPECT_TRUE(i1.isInt());
    ConfigAtom i1To10(ConfigAtom::fromString("1234567890"));
    EXPECT_TRUE(i1To10.isInt());
    ConfigAtom iBiggest(ConfigAtom::fromString("2147483647"));
    EXPECT_TRUE(iBiggest.isInt());
    ConfigAtom iSmallest(ConfigAtom::fromString("-2147483647"));
    EXPECT_TRUE(iSmallest.isInt());
    ConfigAtom iPi(ConfigAtom::fromString("314159265358979323846264"));
    EXPECT_FALSE(iPi.isInt());
    EXPECT_TRUE(iPi.isFloat());
    ConfigAtom iJunk(ConfigAtom::fromString("3-4+2"));
    EXPECT_FALSE(iJunk.isInt());
    EXPECT_TRUE(iJunk.isString());
}

TEST(ConfigAtomTest, testStringForFloat)
{
    ConfigAtom f0(ConfigAtom::fromString("3.5"));
    EXPECT_TRUE(f0.isFloat());
    ConfigAtom f1(ConfigAtom::fromString("3."));
    EXPECT_TRUE(f1.isFloat());
    ConfigAtom f2(ConfigAtom::fromString("3e20"));
    EXPECT_TRUE(f2.isFloat());
    ConfigAtom f3(ConfigAtom::fromString("3e-20"));
    EXPECT_TRUE(f3.isFloat());
    ConfigAtom f_0(ConfigAtom::fromString("-3.5"));
    EXPECT_TRUE(f_0.isFloat());
    ConfigAtom f_1(ConfigAtom::fromString("-3."));
    EXPECT_TRUE(f_1.isFloat());
    ConfigAtom f_2(ConfigAtom::fromString("-3e20"));
    EXPECT_TRUE(f_2.isFloat());
    ConfigAtom f_3(ConfigAtom::fromString("-3e-20"));
    EXPECT_TRUE(f_3.isFloat());
}

TEST(ConfigAtomTest, testStringForPoint)
{
    ConfigAtom p0(ConfigAtom::fromString("0,0"));
    EXPECT_TRUE(p0.isPoint());
    ConfigAtom p1(ConfigAtom::fromString("2,4"));
    EXPECT_TRUE(p1.isPoint());
    ConfigAtom p2(ConfigAtom::fromString("-2,4"));
    EXPECT_TRUE(p2.isPoint());
    ConfigAtom p3(ConfigAtom::fromString("2,-4"));
    EXPECT_TRUE(p3.isPoint());
    ConfigAtom p4(ConfigAtom::fromString("-2,-4"));
    EXPECT_TRUE(p4.isPoint());
    ConfigAtom notP(ConfigAtom::fromString("1,2,3,4,5,6,7,8"));
    EXPECT_FALSE(notP.isPoint());

    EXPECT_EQ(string("-2,-4"), p4.asString());
}

TEST(ConfigAtomTest, testStringForRect)
{
    ConfigAtom r0(ConfigAtom::fromString("(0,0),(35,80)"));
    EXPECT_TRUE(r0.isRect());
    ConfigAtom r1(ConfigAtom::fromString("(0,0)(35,80)"));
    EXPECT_FALSE(r1.isRect());
    EXPECT_TRUE(r1.isString());
    r1 = ConfigAtom::fromString("(0,0,4),(35,80)");
    EXPECT_FALSE(r1.isRect());
    EXPECT_TRUE(r1.isString());

    EXPECT_EQ(string("(0,0),(35,80)"), r0.asString());
}

TEST(ConfigAtomTest, testAsBool)
{
    ConfigAtom rS(ConfigAtom::fromString("true"));
    auto r = rS.asBool();
    EXPECT_TRUE(r);

    ConfigAtom xx;
    EXPECT_THROW(xx.asBool(), RuntimeException);
    bool canDo = true;
    auto xb = xx.asBool(&canDo);
    EXPECT_FALSE(xb);
    EXPECT_FALSE(canDo);
}

TEST(ConfigAtomTest, testAsInt)
{
    ConfigAtom rS(ConfigAtom::fromString("102"));
    auto r = rS.asInt();
    EXPECT_EQ(102, r);

    ConfigAtom xx;
    EXPECT_THROW(xx.asInt(), RuntimeException);
    bool canDo = true;
    auto xi = xx.asInt(&canDo);
    EXPECT_EQ(0, xi);
    EXPECT_FALSE(canDo);
}

TEST(ConfigAtomTest, testAsFloat)
{
    ConfigAtom rS(ConfigAtom::fromString("10.2"));
    auto r = rS.asFloat();
    EXPECT_TRUE(floatEqual(r, 10.2f));

    ConfigAtom xx;
    EXPECT_THROW(xx.asFloat(), RuntimeException);
    bool canDo = true;
    auto xf = xx.asFloat(&canDo);
    EXPECT_TRUE(floatEqual(xf, 0.0));
    EXPECT_FALSE(canDo);
}

TEST(ConfigAtomTest, testAsString)
{
    ConfigAtom rS(ConfigAtom::fromString("0,10"));
    auto r = rS.asString();
    EXPECT_EQ(r, "0,10");

    ConfigAtom xx;
    EXPECT_THROW(xx.asString(), RuntimeException);
    bool canDo = true;
    auto xs = xx.asString(&canDo);
    EXPECT_TRUE(xs.empty());
    EXPECT_FALSE(canDo);
}

TEST(ConfigAtomTest, testAsPoint)
{
    ConfigAtom rA(ConfigAtom::fromString("0,10"));
    auto r = rA.asPoint();
    EXPECT_EQ(x(r), 0);
    EXPECT_EQ(y(r), 10);

    ConfigAtom xx;
    EXPECT_THROW(xx.asPoint(), RuntimeException);
    bool canDo = true;
    auto xp = xx.asPoint(&canDo);
    EXPECT_EQ(x(xp), 0);
    EXPECT_EQ(y(xp), 0);
    EXPECT_FALSE(canDo);
}

TEST(ConfigAtomTest, testAsRect)
{
    ConfigAtom rA(ConfigAtom::fromString("(0,10),(35,80)"));
    auto r = rA.asRect();
    EXPECT_EQ(left(r), 0);
    EXPECT_EQ(top(r), 10);
    EXPECT_EQ(width(r), 35);
    EXPECT_EQ(height(r), 80);

    ConfigAtom x;
    EXPECT_THROW(x.asRect(), RuntimeException);
    bool canDo = true;
    auto xr = x.asRect(&canDo);
    EXPECT_EQ(left(xr), 0);
    EXPECT_EQ(top(xr), 0);
    EXPECT_EQ(width(xr), 0);
    EXPECT_EQ(height(xr), 0);
    EXPECT_FALSE(canDo);
}

TEST(ConfigAtomTest, testEqualityOperator)
{
    {
        ConfigAtom nothing;
        EXPECT_EQ(ConfigAtom(), nothing);
    }
    ConfigAtom canoBool(true);
    ConfigAtom canoInt(35);
    ConfigAtom canoFloat(2.71828);
    ConfigAtom canoString("Mikado");
    ConfigAtom canoPoint(toPoint(3,4));
    ConfigAtom canoRect(toRect(100, 80, 200, 130));

    EXPECT_NE(canoBool, canoInt);
    EXPECT_NE(canoBool, canoFloat);
    EXPECT_NE(canoBool, canoString);
    EXPECT_NE(canoBool, canoPoint);
    EXPECT_NE(canoBool, canoRect);
    EXPECT_NE(canoInt, canoFloat);
    EXPECT_NE(canoInt, canoString);
    EXPECT_NE(canoInt, canoPoint);
    EXPECT_NE(canoInt, canoRect);
    EXPECT_NE(canoFloat, canoString);
    EXPECT_NE(canoFloat, canoPoint);
    EXPECT_NE(canoFloat, canoRect);
    EXPECT_NE(canoString, canoPoint);
    EXPECT_NE(canoString, canoRect);
    EXPECT_NE(canoPoint, canoRect);

    ConfigAtom changeOBool(true, true);
    ConfigAtom changeOInt(35, true);
    ConfigAtom changeOFloat(2.71828, true);
    ConfigAtom changeOString("Mikado", true);
    ConfigAtom changeOPoint(toPoint(3,4), true);
    ConfigAtom changeORect(toRect(100, 80, 200, 130), true);

    EXPECT_EQ(canoBool, changeOBool);
    EXPECT_EQ(canoInt, changeOInt);
    EXPECT_EQ(canoFloat, changeOFloat);
    EXPECT_EQ(canoString, changeOString);
    EXPECT_EQ(canoPoint, changeOPoint);
    EXPECT_EQ(canoRect, changeORect);

    changeOBool.set(false);
    changeOInt.set(34);
    changeOFloat.set(3.1415926);
    changeOString.set("Poo Bah");
    changeOPoint.set(toPoint(4, 5));
    changeORect.set(toRect(101, 80, 200, 130));

    EXPECT_NE(canoBool, changeOBool);
    EXPECT_NE(canoInt, changeOInt);
    EXPECT_NE(canoFloat, changeOFloat);
    EXPECT_NE(canoString, changeOString);
    EXPECT_NE(canoPoint, changeOPoint);
    EXPECT_NE(canoRect, changeORect);
}
