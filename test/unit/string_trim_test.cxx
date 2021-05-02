///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, Arthur N. Klassen
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
// 2017.03.15 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_trim_test.cxx -- unit tests for string trimming
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <string.hxx>
#include <string_trim.hxx>

using namespace std;
using namespace ansak;
using namespace testing;

namespace {

template<typename T>
basic_string<T> testTrim(const T* src)
{
    basic_string<T> wrk(src);
    trim(wrk);
    return wrk;
}

template<typename T>
basic_string<T> testTrimWhiteSpace(const T* src)
{
    basic_string<T> wrk(src);
    trimWhiteSpace(wrk);
    return wrk;
}

template<typename T>
basic_string<T> testTrimAscii7WhiteSpace(const T* src)
{
    basic_string<T> wrk(src);
    trimAscii7WhiteSpace(wrk);
    return wrk;
}

}

TEST(StringTrimTest, tabs)
{
    EXPECT_TRUE(testTrimAscii7WhiteSpace("\t\t\t\t").empty());
    EXPECT_TRUE(testTrimWhiteSpace("\t\t\t\t").empty());
    EXPECT_TRUE(testTrimWhiteSpace(u"\t\t\t\t").empty());
    EXPECT_TRUE(testTrimWhiteSpace(U"\t\t\t\t").empty());

    string theEnd { "A more humane Mikado never did in Japan exist." };
    EXPECT_EQ(theEnd, testTrimAscii7WhiteSpace("\t\tA more humane Mikado never did in Japan exist.\t\t"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace("\t\tA more humane Mikado never did in Japan exist.\t\t"));
    EXPECT_EQ(theEnd, toUtf8(testTrimWhiteSpace(u"\t\tA more humane Mikado never did in Japan exist.\t\t")));
    EXPECT_EQ(theEnd, toUtf8(testTrimWhiteSpace(U"\t\tA more humane Mikado never did in Japan exist.\t\t")));
}

TEST(StringTrimTest, trim)
{
    string theEnd { "A more humane Mikado never did in Japan exist." };
    EXPECT_EQ(theEnd, testTrim("  A more humane Mikado never did in Japan exist.  " ));
    EXPECT_EQ(theEnd, toUtf8(testTrim(u"  A more humane Mikado never did in Japan exist.  " )));
    EXPECT_EQ(theEnd, toUtf8(testTrim(U"  A more humane Mikado never did in Japan exist.  " )));
}

TEST(StringTrimTest, otherWhites8)
{
    string theEnd { "A\x09more\x0ahumane\x0dMikado never did in Japan exist." };
    EXPECT_NE(theEnd, testTrim("\x09" "A\x09more\x0ahumane\x0dMikado never did in Japan exist.\x09" ));
    EXPECT_NE(theEnd, testTrim("\x0a" "A\x09more\x0ahumane\x0dMikado never did in Japan exist.\x0a" ));
    EXPECT_NE(theEnd, testTrim("\x0d" "A\x09more\x0ahumane\x0dMikado never did in Japan exist.\x0d" ));
    EXPECT_EQ(theEnd, testTrim(" A\x09more\x0ahumane\x0dMikado never did in Japan exist. " ));
    EXPECT_EQ(theEnd, testTrimWhiteSpace("\x09" "A\x09more\x0ahumane\x0dMikado never did in Japan exist.\x09" ));
    EXPECT_EQ(theEnd, testTrimWhiteSpace("\x0a" "A\x09more\x0ahumane\x0dMikado never did in Japan exist.\x0a" ));
    EXPECT_EQ(theEnd, testTrimWhiteSpace("\x0d" "A\x09more\x0ahumane\x0dMikado never did in Japan exist.\x0d" ));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(" A\x09more\x0ahumane\x0dMikado never did in Japan exist. " ));
}

TEST(StringTrimTest, otherWhites16)
{
    utf16String theEnd { u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                         u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f." };
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u0009" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u0009"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u000a" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u000a"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u000d" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u000d"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u" A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f. "));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u00a0" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u00a0"));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2000" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2000"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2001" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2001"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2002" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2002"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2003" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2003"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2004" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2004"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2005" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2005"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2006" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2006"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2007" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2007"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2008" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2008"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u2009" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u2009"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u200a" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u200a"));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u202f" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u202f"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u205f" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u205f"));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(u"\u3000" u"A\u0009more\u000ahumane\u000dMikado\u00a0never\u2000did\u2001in\u2002Japan\u2003exist."
                           u"\u3000To\u2004nobody\u2005second\u2006I'm\u2007certainly\u2008reckoned\u2009a\u200atrue\u202fphilanthropist\u205f."
                           u"\u3000"));
}

TEST(StringTrimTest, otherWhites32)
{
    ucs4String theEnd { U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                        U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f." };
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00000009" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00000009"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U0000000a" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U0000000a"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U0000000d" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U0000000d"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U" A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f. "));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U000000a0" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U000000a0"));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002000" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002000"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002001" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002001"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002002" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002002"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002003" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002003"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002004" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002004"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002005" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002005"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002006" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002006"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002007" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002007"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002008" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002008"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00002009" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00002009"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U0000200a" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U0000200a"));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U0000202f" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U0000202f"));
    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U0000205f" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U0000205f"));

    EXPECT_EQ(theEnd, testTrimWhiteSpace(U"\U00003000" U"A\U00000009more\U0000000ahumane\U0000000dMikado\U000000a0never\U00002000did\U00002001in\U00002002Japan\U00002003exist."
                           U"\U00003000To\U00002004nobody\U00002005second\U00002006I'm\U00002007certainly\U00002008reckoned\U00002009a\U0000200atrue\U0000202fphilanthropist\U0000205f."
                           U"\U00003000"));
}
