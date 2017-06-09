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
// 2014.11.04 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// parseArgsTest.cxx -- unit tests for the argc/argv parsing
//
///////////////////////////////////////////////////////////////////////////

#include <parse_args.hxx>

#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>
#include <exception>

using namespace ansak;
using std::string;
using namespace testing;

TEST(ParseArgsTest, testHelloWorld)
{
    const char* args[] = { "helloWorld" };
    ParseArgs hello(ParseArgs::preferringShortFlags(1, args));
    EXPECT_EQ(string("helloWorld"), hello.getProcName());
    EXPECT_TRUE(hello.allArgsParsed());
}

TEST(ParseArgsTest, testSingleFlag)
{
    const char* args0[] = { "helloWorld", "-f" };
    ParseArgs hello0(ParseArgs::preferringShortFlags(2, args0));
    ParseArgs hello00(ParseArgs::preferringShortFlagsNoThrow(2, args0));
    EXPECT_EQ(string("helloWorld"), hello0.getProcName());
    EXPECT_EQ(string("helloWorld"), hello00.getProcName());
    EXPECT_TRUE(hello0.hasFlag('f'));
    EXPECT_TRUE(hello00.hasFlag('f'));
    EXPECT_EQ(1, hello0.countFlags());
    EXPECT_EQ(1, hello00.countFlags());
    EXPECT_EQ(0, hello0.countSettings());
    EXPECT_EQ(0, hello00.countSettings());
    EXPECT_EQ(0, hello0.countParsedInputs());
    EXPECT_EQ(0, hello00.countParsedInputs());
    EXPECT_TRUE(hello0.allArgsParsed());
    EXPECT_TRUE(hello00.allArgsParsed());
    EXPECT_TRUE(hello0.allArgsParsed());
    EXPECT_TRUE(hello00.allArgsParsed());

    const char* args1[] = { "helloWorld", "--some-flag" };
    ParseArgs hello1(ParseArgs::preferringShortFlags(2, args1));
    EXPECT_EQ(string("helloWorld"), hello1.getProcName());
    EXPECT_TRUE(hello1.hasFlag(string("some-flag")));
    EXPECT_EQ(1, hello1.countFlags());
    EXPECT_EQ(0, hello1.countSettings());
    EXPECT_EQ(0, hello1.countParsedInputs());
    EXPECT_TRUE(hello1.allArgsParsed());
    EXPECT_TRUE(hello1.allArgsParsed());
}

TEST(ParseArgsTest, testManyFlags)
{
    const char* args0[] = { "helloWorld", "-a", "--bravo", "-c" };
    ParseArgs hello0(ParseArgs::preferringShortFlags(4, args0));
    EXPECT_EQ(string("helloWorld"), hello0.getProcName());
    EXPECT_TRUE(hello0.hasFlag('a'));
    EXPECT_TRUE(hello0.hasFlag("bravo"));
    EXPECT_TRUE(hello0.hasFlag('c'));
    EXPECT_EQ(3, hello0.countFlags());
    EXPECT_EQ(0, hello0.countSettings());
    EXPECT_EQ(0, hello0.countParsedInputs());
    EXPECT_TRUE(hello0.allArgsParsed());

    const char* args1[] = { "helloWorld", "--alpha", "--zulu", "--bravo", "--yankee" };
    ParseArgs hello1(ParseArgs::preferringShortFlags(5, args1));
    EXPECT_EQ(string("helloWorld"), hello0.getProcName());
    EXPECT_TRUE(hello1.hasFlag("alpha"));
    EXPECT_TRUE(hello1.hasFlag("bravo"));
    EXPECT_TRUE(hello1.hasFlag("zulu"));
    EXPECT_TRUE(hello1.hasFlag("yankee"));
    EXPECT_EQ(4, hello1.countFlags());
    EXPECT_EQ(0, hello1.countSettings());
    EXPECT_EQ(0, hello1.countParsedInputs());
    EXPECT_TRUE(hello1.allArgsParsed());
}

TEST(ParseArgsTest, testManyLumpedFlags)
{
    const char* args0[] = { "helloWorld", "-abcdef" };
    ParseArgs hello0(ParseArgs::preferringShortFlags(2, args0));
    EXPECT_EQ(string("helloWorld"), hello0.getProcName());
    EXPECT_TRUE(hello0.hasFlag('a'));
    EXPECT_TRUE(hello0.hasFlag('b'));
    EXPECT_TRUE(hello0.hasFlag('c'));
    EXPECT_TRUE(hello0.hasFlag('d'));
    EXPECT_TRUE(hello0.hasFlag('e'));
    EXPECT_TRUE(hello0.hasFlag('f'));
    EXPECT_EQ(6, hello0.countFlags());
    EXPECT_EQ(0, hello0.countSettings());
    EXPECT_EQ(0, hello0.countParsedInputs());
    EXPECT_TRUE(hello0.allArgsParsed());

    const char* args1[] = { "fatty", "-ab", "wise-nose" };
    ParseArgs hello1(ParseArgs::preferringShortFlags(3, args1));
    EXPECT_EQ(string("fatty"), hello1.getProcName());
    EXPECT_TRUE(hello1.hasFlag('a'));
    EXPECT_TRUE(hello1.hasFlag('b'));
    EXPECT_EQ(string("wise-nose"), hello1.getParsedInput(0));
    EXPECT_EQ(2, hello1.countFlags());
    EXPECT_EQ(0, hello1.countSettings());
    EXPECT_EQ(1, hello1.countParsedInputs());
    EXPECT_TRUE(hello1.allArgsParsed());

    const char* args2[] = { "fatty", "-a", "wise-nose", "-b" };
    ParseArgs hello2(ParseArgs::preferringShortFlags(4, args2));
    EXPECT_EQ(string("fatty"), hello2.getProcName());
    EXPECT_TRUE(hello2.hasSetting('a'));
    EXPECT_EQ(string("wise-nose"), hello2.getSetting('a'));
    EXPECT_TRUE(hello2.hasFlag('b'));
    EXPECT_EQ(1, hello2.countFlags());
    EXPECT_EQ(1, hello2.countSettings());
    EXPECT_EQ(0, hello2.countParsedInputs());
    EXPECT_TRUE(hello2.allArgsParsed());

    const char* args3[] = { "fatty", "-a", "-b", "wise-nose" };
    ParseArgs hello3(ParseArgs::preferringShortFlags(4, args3));
    EXPECT_EQ(string("fatty"), hello3.getProcName());
    EXPECT_TRUE(hello3.hasFlag('a'));
    EXPECT_TRUE(hello3.hasSetting('b'));
    EXPECT_EQ(string("wise-nose"), hello3.getSetting('b'));
    EXPECT_EQ(1, hello3.countFlags());
    EXPECT_EQ(1, hello3.countSettings());
    EXPECT_EQ(0, hello3.countParsedInputs());
    EXPECT_TRUE(hello3.allArgsParsed());
}

TEST(ParseArgsTest, testDoubleHyphen)
{
    const char* args[] = { "ho", "-tom", "--bombadil", "--", "-tom", "--bombadillo" };
    ParseArgs ho0(ParseArgs::preferringShortFlags(6, args));
    EXPECT_TRUE(ho0.hasFlag("t"));
    EXPECT_TRUE(ho0.hasFlag("o"));
    EXPECT_TRUE(ho0.hasFlag("m"));
    EXPECT_TRUE(ho0.hasFlag("bombadil"));
    EXPECT_EQ(string("-tom"), ho0.getParsedInput(0));
    EXPECT_EQ(string("--bombadillo"), ho0.getParsedInput(1));
    EXPECT_EQ(4, ho0.countFlags());
    EXPECT_EQ(0, ho0.countSettings());
    EXPECT_EQ(2, ho0.countParsedInputs());
    EXPECT_TRUE(ho0.allArgsParsed());

    int reargc;
    const char** reargv;
    ho0.set("iarwain");
    ho0(reargc, reargv);
    EXPECT_TRUE(&args[0] != const_cast<const char**>(reargv));
    EXPECT_EQ(9, reargc); // splits -t, -o -m as flags

}

TEST(ParseArgsTest, testGetNotThere)
{
    const char* args[] = { "ho", "-tom", "--bombadil", "--", "-tom", "--bombadillo" };
    ParseArgs ho0(ParseArgs::preferringShortFlags(6, args));

    EXPECT_TRUE(ho0.getSetting('q').empty());
    EXPECT_TRUE(ho0.getSetting("goldberry").empty());
    EXPECT_TRUE(ho0.getParsedInput(35).empty());
}

TEST(ParseArgsTest, testSplitters)
{
    const char* args[] = { "ho", "-tom", "--bombadil", "--tom:bombadillo", "--jacket=bright blue",
                           "--boots", "yellow"};
    ParseArgs ho1(ParseArgs::preferringShortFlags(7, args));
    EXPECT_TRUE(ho1.hasFlag("t"));
    EXPECT_TRUE(ho1.hasFlag("o"));
    EXPECT_TRUE(ho1.hasFlag("m"));
    EXPECT_TRUE(ho1.hasFlag("bombadil"));
    EXPECT_TRUE(ho1.hasSetting("tom"));
    EXPECT_EQ(string("bombadillo"), ho1.getSetting("tom"));
    EXPECT_TRUE(ho1.hasSetting("jacket"));
    EXPECT_EQ(string("bright blue"), ho1.getSetting("jacket"));
    EXPECT_TRUE(ho1.hasSetting("boots"));
    EXPECT_EQ(string("yellow"), ho1.getSetting("boots"));
    EXPECT_EQ(4, ho1.countFlags());
    EXPECT_EQ(3, ho1.countSettings());
    EXPECT_EQ(0, ho1.countParsedInputs());
    EXPECT_TRUE(ho1.allArgsParsed());
}

TEST(ParseArgsTest, testTooLongOnlyInputs)
{
    const char* args[] = { "radio", "alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf",
                           "hotel", "india", "juliet", "kilo", "lima", "mike", "november", "oscar",
                           "papa", "queen", "romeo", "sierra", "tango", "uniform", "victor",
                           "whisky", "x-ray", "yankee", "zulu"  };
    ParseArgs alpha(ParseArgs::preferringShortFlags(27, args));
    EXPECT_FALSE(alpha.allArgsParsed());
    EXPECT_EQ(20, alpha.countParsedArgs());
    EXPECT_EQ(19, alpha.countParsedInputs());
    EXPECT_EQ(0, alpha.countFlags());
    EXPECT_EQ(0, alpha.countSettings());

    const char* argz[] = { "radio", "alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf",
                           "hotel", "india", "juliet", "kilo", "lima", "mike", "november", "oscar",
                           "papa", "queen", "romeo", "sierra", "tango", "uniform", "victor",
                           "whisky", "x-ray", "yankee", "zulu", "-d"  };
    ParseArgs alfa(ParseArgs::preferringShortFlags(28, argz));
    EXPECT_TRUE(alfa.allArgsParsed());
    EXPECT_EQ(28, alfa.countParsedArgs());
    EXPECT_EQ(26, alfa.countParsedInputs());
    EXPECT_EQ(1, alfa.countFlags());
    EXPECT_EQ(0, alfa.countSettings());
}

TEST(ParseArgsTest, testGetSetFlags)
{
    const char* args[] = { "radio", "-ckvu", "--tv", "cbut", "--radio", "ckwx", "-t",
                           "-i", "-m", "--wwv" };
    ParseArgs flagTest(ParseArgs::preferringShortFlags(10, args));
    EXPECT_FALSE(flagTest.hasFlag('a'));
    EXPECT_FALSE(flagTest.hasFlag('b'));
    EXPECT_TRUE(flagTest.hasFlag('c'));
    EXPECT_FALSE(flagTest.hasFlag('d'));
    EXPECT_FALSE(flagTest.hasFlag('e'));
    EXPECT_FALSE(flagTest.hasFlag('f'));
    EXPECT_FALSE(flagTest.hasFlag('g'));
    EXPECT_FALSE(flagTest.hasFlag('h'));
    EXPECT_TRUE(flagTest.hasFlag('i'));
    EXPECT_TRUE(flagTest.hasFlag('k'));
    EXPECT_TRUE(flagTest.hasFlag('v'));
    EXPECT_TRUE(flagTest.hasFlag('u'));
    EXPECT_TRUE(flagTest.hasFlag('t'));
    EXPECT_TRUE(flagTest.hasFlag('m'));
    EXPECT_TRUE(flagTest.hasFlag("wwv"));
    EXPECT_FALSE(flagTest.hasFlag("tv"));
    EXPECT_FALSE(flagTest.hasFlag("radio"));

    auto flags(flagTest.getFlags());
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "c"));
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "k"));
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "v"));
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "u"));
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "t"));
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "i"));
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "m"));
    EXPECT_NE(flags.end(), std::find(flags.begin(), flags.end(), "wwv"));

    flagTest.unset('k');
    flagTest.unset('v');
    flagTest.unset('u');
    flagTest.set('j');
    flagTest.set('o');
    flagTest.set('r');
    EXPECT_FALSE(flagTest.hasFlag('k'));
    EXPECT_FALSE(flagTest.hasFlag('v'));
    EXPECT_FALSE(flagTest.hasFlag('u'));
    EXPECT_TRUE(flagTest.hasFlag('j'));
    EXPECT_TRUE(flagTest.hasFlag('o'));
    EXPECT_TRUE(flagTest.hasFlag('r'));
}

TEST(ParseArgsTest, testGetSetSettings)
{
    const char* args[] = { "radio", "-c", "kvu", "--tv", "cbut", "--radio", "ckwx", "-t",
                           "-i", "-m", "--wwv" };
    ParseArgs settingTest(ParseArgs::preferringShortFlags(11, args));
    EXPECT_TRUE(settingTest.hasSetting('c'));
    EXPECT_TRUE(settingTest.hasSetting("tv"));
    EXPECT_FALSE(settingTest.hasSetting("world-wide-web"));
    EXPECT_TRUE(settingTest.hasSetting("radio"));

    EXPECT_EQ(string("kvu"), settingTest.getSetting('c'));
    EXPECT_EQ(string("cbut"), settingTest.getSetting("tv"));
    EXPECT_EQ(string("ckwx"), settingTest.getSetting("radio"));

    auto settings(settingTest.getSettingNames());
    EXPECT_EQ(static_cast<size_t>(3), settings.size());
    EXPECT_NE(settings.end(), std::find(settings.begin(), settings.end(), "c"));
    EXPECT_NE(settings.end(), std::find(settings.begin(), settings.end(), "tv"));
    EXPECT_NE(settings.end(), std::find(settings.begin(), settings.end(), "radio"));
}

TEST(ParseArgsTest, testGetSetParsedInputs)
{
    const char* args[] = { "filibuster", "bills", "amendments", "--", "-missed-opportunities-",
                           "buffalo", "beefalo", "hyphens" };
    ParseArgs inputsTest(ParseArgs::preferringShortFlags(8, args));
    EXPECT_EQ(6, inputsTest.countParsedInputs());
    EXPECT_EQ(string("bills"), inputsTest.getParsedInput(0));
    EXPECT_EQ(string("amendments"), inputsTest.getParsedInput(1));
    EXPECT_EQ(string("-missed-opportunities-"), inputsTest.getParsedInput(2));
    EXPECT_EQ(string("buffalo"), inputsTest.getParsedInput(3));
    EXPECT_EQ(string("beefalo"), inputsTest.getParsedInput(4));
    EXPECT_EQ(string("hyphens"), inputsTest.getParsedInput(5));

    inputsTest.deleteParsedInput(2);
    inputsTest.appendInput("tarsiers");
    EXPECT_EQ(string("buffalo"), inputsTest.getParsedInput(2));
    EXPECT_EQ(string("beefalo"), inputsTest.getParsedInput(3));
    EXPECT_EQ(string("hyphens"), inputsTest.getParsedInput(4));
    EXPECT_EQ(string("tarsiers"), inputsTest.getParsedInput(5));
}

TEST(ParseArgsTest, testColonsEquals)
{
    const char* arg0[] = { "one-setting", "-f", "rabbit" };
    const char* arg1[] = { "one-setting", "--f", "rabbit" };
    const char* arg2[] = { "one-setting", "--f:rabbit" };
    const char* arg3[] = { "one-setting", "--f=rabbit" };

    ParseArgs a0(ParseArgs::preferringShortFlags(3, arg0));
    ParseArgs a1(ParseArgs::preferringShortFlags(3, arg1));
    ParseArgs a2(ParseArgs::preferringShortFlags(2, arg2));
    ParseArgs a3(ParseArgs::preferringShortFlags(2, arg3));

    EXPECT_EQ(a0.getSetting('f'), a1.getSetting('f'));
    EXPECT_EQ(a0.getSetting('f'), a2.getSetting('f'));
    EXPECT_EQ(a0.getSetting('f'), a3.getSetting('f'));
}

TEST(ParseArgsTest, testSetAndInvoke)
{
    const char* args[] = { "radio", "-c", "kvu", "--tv", "cbut", "--radio", "ckwx", "-t",
                           "-i", "-m", "--wwv" };
    ParseArgs settingTest(ParseArgs::preferringShortFlags(11, args));
    int reargc;
    const char** reargv;
    settingTest(reargc, reargv);
    EXPECT_EQ(&args[0], reargv);
    EXPECT_EQ(11, reargc);

    settingTest.set('q', "jester");
    settingTest(reargc, reargv);
    EXPECT_EQ(13, reargc);

    settingTest.set('q', "jostler");
    settingTest(reargc, reargv);
    EXPECT_EQ(13, reargc);

    settingTest.set('q', "");
    settingTest(reargc, reargv);
    EXPECT_EQ(11, reargc);
}

TEST(ParseArgsTest, testThrowOnTaintedData)
{
    {
        const char* argv[] = { "too-much", 0 };
        try
        {
            ParseArgs bounceOne(ParseArgs::preferringShortFlags(10001, argv));
            ASSERT_TRUE(false);
        }
        catch(BadArgsException& e)
        {
            string what(e.what(), 16);
            EXPECT_EQ(string("BadArgsException"), what);
        }
        try
        {
            ParseArgs bounceTwo(ParseArgs::preferringWords(10001, argv));
            ASSERT_TRUE(false);
        }
        catch(BadArgsException&)
        {
        }
    }

    {
        static char reallyLongArg[1010];

        const char* argv[2];
        argv[0] = "too-much";
        argv[1] = reallyLongArg;
        memset(reallyLongArg, '*', 1009); reallyLongArg[1009] = '0';
        try
        {
            ParseArgs bounceOne(ParseArgs::preferringShortFlags(2, argv));
            ASSERT_TRUE(false);
        }
        catch(BadArgsException&)
        {
        }
        try
        {
            ParseArgs bounceTwo(ParseArgs::preferringWords(2, argv));
            ASSERT_TRUE(false);
        }
        catch(BadArgsException&)
        {
        }
    }

    {
        static char longArg[880];

        const char* argv[16];
        argv[0] = "too-much";
        memset(longArg, '*', 880); longArg[879] = '\0';
        for (int i = 1; i < 16; ++i)
        {
            argv[i] = longArg;
        }
        try
        {
            ParseArgs bounceOne(ParseArgs::preferringShortFlags(16, argv));
            ASSERT_TRUE(false);
        }
        catch(BadArgsException&)
        {
        }
        try
        {
            ParseArgs bounceTwo(ParseArgs::preferringShortFlags(16, argv));
            ASSERT_TRUE(false);
        }
        catch(BadArgsException&)
        {
        }
    }
}

TEST(ParseArgsTest, testDontThrowOnTaintedData)
{
    {
        const char* argv[] = { "too-much", 0 };
        ParseArgs bounceOne(ParseArgs::preferringShortFlagsNoThrow(10001, argv));
        ParseArgs bounceTwo(ParseArgs::preferringWordsNoThrow(10001, argv));
    }

    static char reallyLongArg[1010];

    {
        const char* argv[2];
        argv[0] = "too-much";
        argv[1] = reallyLongArg;
        memset(reallyLongArg, '*', 1009); reallyLongArg[1009] = '\0';
        ParseArgs bounceOne(ParseArgs::preferringShortFlagsNoThrow(2, argv));
        ParseArgs bounceTwo(ParseArgs::preferringWordsNoThrow(2, argv));
    }

    static char longArg[880];

    {
        const char* argv[16];
        argv[0] = "too-much";
        memset(longArg, '*', 880); longArg[879] = '\0';
        for (int i = 1; i < 16; ++i)
        {
            argv[i] = longArg;
        }
        ParseArgs bounceOne(ParseArgs::preferringShortFlagsNoThrow(16, argv));
        ParseArgs bounceTwo(ParseArgs::preferringWordsNoThrow(16, argv));
    }
}

TEST(ParseArgsTest, testSingleDashVariety)
{
    const char* argv[] = { "chequebox", "-display", "800x600", 0 };
    ParseArgs doTheDash(ParseArgs::preferringWords(3, argv));
    EXPECT_EQ(string("800x600"), doTheDash.getSetting("display"));
    ParseArgs doTheDashNoThrow(ParseArgs::preferringWordsNoThrow(3, argv));
    EXPECT_EQ(string("800x600"), doTheDashNoThrow.getSetting("display"));
}

