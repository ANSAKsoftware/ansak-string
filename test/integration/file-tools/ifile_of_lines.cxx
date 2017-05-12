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
// 2014.12.10 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines_test.cxx -- unit tests for the fileOfLines class
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "file_of_lines.hxx"
#include "file_path.hxx"
#include "file_system_path.hxx"
#include "file_handle.hxx"
#include "temp_file_wrapper.hxx"

#include <string>

using namespace std;
using namespace ansak;
using namespace ansak::internal;
using namespace testing;

TEST(FileOfLinesTest, testBeginEmpty)
{
    TempFileWrapper tempDir;
    tempDir.writeTestFile("testEmpty", nullptr, 0);

    FileOfLines testEmpty(tempDir.child("testEmpty"));
    auto i = testEmpty.begin();
    EXPECT_EQ(i, testEmpty.end());
}

TEST(FileOfLinesTest, testBeginNotEmpty)
{
    TempFileWrapper tempDir;
    tempDir.writeTestFile("testNotEmpty", "This\nis\nonly\na\ntest.", 20);

    FileOfLines testNotEmpty(tempDir.child("testNotEmpty"));
    auto i = testNotEmpty.begin();
    EXPECT_NE(i, testNotEmpty.end());
}

TEST(FileOfLinesTest, testUtf8)
{
    char brillig[] = "Es brillig war. Die schlichten Toven\n"
                     "wirrten und wimmelten im Waben\n"
                     "Und aller-m\xc3\xbcmsigen Burg-goven\n"
                     "die mohmen R\xc3\xa4th\' ausgraben.";
    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf8", brillig, sizeof(brillig) - 1);

    FileOfLines testUtf8(tempDir.child("testUtf8"));
    auto i = testUtf8.begin();
    EXPECT_NE(i, testUtf8.end());
    EXPECT_EQ(string("Es brillig war. Die schlichten Toven"), *(i++));

    EXPECT_EQ(string("wirrten und wimmelten im Waben"), *i);

    EXPECT_EQ(string("Und aller-m\xc3\xbcmsigen Burg-goven"), *++i);
    i += 1;

    EXPECT_EQ(string("die mohmen R\xc3\xa4th\' ausgraben."), *(i++));
    EXPECT_EQ(testUtf8.end(), i);

    // test constant-ness of the "iterator"
    {
        auto i = testUtf8.begin();
        EXPECT_FALSE((*i).empty());
        auto s1 = *i;
        ++i;
        EXPECT_TRUE(i == testUtf8.end() || i != testUtf8.end());
        auto s2 = *i;
        i--;
        auto s3 = *i;
        i++;
        i++;
        i -= 1;
        auto s4 = *i;

        EXPECT_EQ(s1, s3);
        EXPECT_NE(s2, s3);
        EXPECT_NE(s4, s3);
        EXPECT_EQ(s4, s2);
    }
}

TEST(FileOfLinesTest, testUtf8WithBom)
{
    char bewahre[] = "\xef\xbb\xbf"
                     "Bewahre doch vor Jammerwoch --\n"
                     "die Z\xc3\xa4hnen knirrschen, krahlen kratzen!\n"
                     "Bewahr\' vor Jubjubvogel, vor\n"
                     "frumi\xc3\xb6sen Banderschn\xc3\xa4tzchen!";
    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf8WithBOM", bewahre, sizeof(bewahre) - 1);

    FileOfLines testUtf8(tempDir.child("testUtf8WithBOM"));
    auto i = testUtf8.begin();
    FileOfLines::const_iterator j = testUtf8.begin();
    auto k = j;
    EXPECT_EQ(i, j);
    EXPECT_EQ(i, k);
    EXPECT_EQ(j, k);

    EXPECT_EQ(string("Bewahre doch vor Jammerwoch --"), *i);
    ++i, ++j, ++k;
    EXPECT_EQ(i, j);
    EXPECT_EQ(i, k);
    EXPECT_EQ(j, k);
    EXPECT_EQ(string("die Z\xc3\xa4hnen knirrschen, krahlen kratzen!"), *j);
    ++i;
    EXPECT_NE(i, j);
    EXPECT_NE(i, k);
    EXPECT_EQ(j, k);
    ++j, ++k;
    EXPECT_EQ(string("Bewahr\' vor Jubjubvogel, vor"), *k);
    EXPECT_EQ(string("frumi\xc3\xb6sen Banderschn\xc3\xa4tzchen!"), *++j);

    EXPECT_EQ(static_cast<size_t>(1), j - k);
    EXPECT_EQ(static_cast<size_t>(4), testUtf8.size());
}

TEST(FileOfLinesTest, testUtf16BE)
{
    char erGriff[] = "\xfe\xff"
                     "\0E\0r\0 \0g\0r\0i\0f\0f\0 \0s\0e\0i\0n\0 \0V\0o\0r\0p\0a\0l\0s\0c\0h\0w\0e\0r\0t\0c\0h\0e\0n\0 \0z\0u\0 \0-\0-\0\n"
                     "\0L\0a\0n\0g\0 \0s\0u\0c\0h\0t\0e\0t\0 \0e\0r\0 \0d\0a\0s\0 \0m\0a\0n\0c\0h\0s\0a\0m\0'\0 \0D\0i\0n\0g\0!\0\n"
                     "\0S\0o\0.\0 \0S\0t\0e\0h\0e\0n\0d\0 \0u\0n\0t\0e\0r\0m\0 \0T\0u\0m\0t\0u\0m\0b\0a\0u\0m\0\n"
                     "\0e\0r\0 \0a\0n\0z\0u\0d\0e\0n\0k\0e\0n\0f\0i\0n\0g\0.";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(erGriff) & 1);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf16BE", erGriff, sizeof(erGriff) - 1);

    FileOfLines testUtf16(tempDir.child("testUtf16BE"));
    auto i = testUtf16.begin();

    EXPECT_EQ(string("Er griff sein Vorpalschwertchen zu --"), *i);
    EXPECT_EQ(string("Lang suchtet er das manchsam' Ding!"), *++i);
    EXPECT_EQ(string("So. Stehend unterm Tumtumbaum"), *++i);
    EXPECT_EQ(string("Lang suchtet er das manchsam' Ding!"), *--i); ++i;
    EXPECT_EQ(string("er anzudenkenfing."), *++i);
}

TEST(FileOfLinesTest, testUtf16LE)
{
    char undAls[] = "\xff\xfe"
                    "U\0n\0d\0 \0a\0l\0s\0 \0e\0r\0 \0t\0i\0e\0f\0 \0i\0n\0 \0A\0n\0d\0a\0c\0h\0t\0 \0s\0t\0a\0n\0d\0\n\0"
                    "d\0e\0s\0 \0J\0a\0m\0m\0e\0r\0w\0o\0c\0h\0e\0n\0s\0 \0A\0u\0g\0e\0n\0f\0e\0u\0e\0r\0\n\0"
                    "d\0u\0r\0c\0h\0 \0T\0u\0r\0g\0e\0n\0w\0a\0l\0d\0 \0i\0n\0 \0W\0i\0f\0f\0e\0k\0 \0k\0a\0m\0,\0\n\0"
                    "e\0i\0n\0 \0b\0u\0r\0b\0l\0e\0n\0d\0 \0U\0n\0g\0e\0h\0e\0u\0e\0r\0.\0";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(undAls) & 1);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf16LE", undAls, sizeof(undAls) - 1);

    FileOfLines testUtf16(tempDir.child("testUtf16LE"));
    auto i = testUtf16.begin() + 3;
    EXPECT_EQ(string("ein burblend Ungeheuer."), *i);
    EXPECT_EQ(string("durch Turgenwald in Wiffek kam,"), *--i);
    EXPECT_EQ(string("des Jammerwochens Augenfeuer"), *--i);
    EXPECT_EQ(string("Und als er tief in Andacht stand"), *--i);
}

TEST(FileOfLinesTest, testUcs4BE)
{
    char einsZwei[] = "\0\0\xfe\xff"
                      "\0\0\0E\0\0\0i\0\0\0n\0\0\0s\0\0\0!\0\0\0 \0\0\0Z\0\0\0w\0\0\0e\0\0\0i\0\0\0!\0\0\0 \0\0\0E\0\0\0i\0\0\0n\0\0\0s\0\0\0!\0\0\0 \0\0\0Z\0\0\0w\0\0\0e\0\0\0i\0\0\0!\0\0\0 \0\0\0u\0\0\0n\0\0\0d\0\0\0 \0\0\0d\0\0\0u\0\0\0r\0\0\0c\0\0\0h\0\0\0 \0\0\0u\0\0\0n\0\0\0d\0\0\0 \0\0\0d\0\0\0u\0\0\0r\0\0\0c\0\0\0h\0\0\0\n"
                      "\0\0\0d\0\0\0a\0\0\0s\0\0\0 \0\0\0V\0\0\0o\0\0\0r\0\0\0p\0\0\0a\0\0\0l\0\0\0s\0\0\0c\0\0\0h\0\0\0w\0\0\0e\0\0\0r\0\0\0t\0\0\0 \0\0\0z\0\0\0e\0\0\0r\0\0\0s\0\0\0c\0\0\0h\0\0\0n\0\0\0i\0\0\0e\0\0\0f\0\0\0e\0\0\0r\0\0\0s\0\0\0c\0\0\0h\0\0\0n\0\0\0\xfc\0\0\0c\0\0\0k\0\0\0.\0\0\0\n"
                      "\0\0\0S\0\0\0o\0\0\0 \0\0\0l\0\0\0i\0\0\0e\0\0\0g\0\0\0t\0\0\0 \0\0\0e\0\0\0s\0\0\0 \0\0\0t\0\0\0o\0\0\0d\0\0\0.\0\0\0 \0\0\0E\0\0\0r\0\0\0 \0\0\0K\0\0\0o\0\0\0p\0\0\0f\0\0\0 \0\0\0i\0\0\0m\0\0\0 \0\0\0H\0\0\0a\0\0\0n\0\0\0d\0\0\0\n"
                      "\0\0\0g\0\0\0e\0\0\0l\0\0\0\xe4\0\0\0u\0\0\0m\0\0\0f\0\0\0i\0\0\0g\0\0\0 \0\0\0z\0\0\0o\0\0\0g\0\0\0 \0\0\0z\0\0\0u\0\0\0r\0\0\0\xfc\0\0\0c\0\0\0k\0\0\0.";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(einsZwei) & 3);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUcs4BE", einsZwei, sizeof(einsZwei) - 1);

    FileOfLines testUcs4(tempDir.child("testUcs4BE"));
    auto l = testUcs4.begin();
    EXPECT_EQ(string("Eins! Zwei! Eins! Zwei! und durch und durch"), *l);
    EXPECT_EQ(string("das Vorpalschwert zerschnieferschn\xc3\xbc" "ck."), *++l);
    EXPECT_EQ(string("So liegt es tod. Er Kopf im Hand"), *++l);
    EXPECT_EQ(string("gel\xc3\xa4umfig zog zur\xc3\xbc" "ck."), *++l);
}

TEST(FileOfLinesTest, testUcs4LE)
{
    char undSchlagst[] = "\xff\xfe\0\0"
                         "\x1e\x20\0\0U\0\0\0n\0\0\0d\0\0\0 \0\0\0s\0\0\0c\0\0\0h\0\0\0l\0\0\0a\0\0\0g\0\0\0s\0\0\0t\0\0\0 \0\0\0d\0\0\0u\0\0\0,\0\0\0 \0\0\0j\0\0\0a\0\0\0,\0\0\0 \0\0\0d\0\0\0e\0\0\0n\0\0\0 \0\0\0J\0\0\0a\0\0\0m\0\0\0m\0\0\0e\0\0\0r\0\0\0w\0\0\0o\0\0\0c\0\0\0h\0\0\0?\0\0\0\n\0\0\0"
                         "U\0\0\0m\0\0\0a\0\0\0r\0\0\0m\0\0\0e\0\0\0 \0\0\0m\0\0\0i\0\0\0c\0\0\0h\0\0\0 \0\0\0m\0\0\0e\0\0\0i\0\0\0n\0\0\0 \0\0\0b\0\0\0\xf6\0\0\0h\0\0\0m\0\0\0s\0\0\0c\0\0\0h\0\0\0e\0\0\0s\0\0\0 \0\0\0K\0\0\0i\0\0\0n\0\0\0d\0\0\0!\0\0\0\n\0\0\0"
                         "O\0\0\0 \0\0\0f\0\0\0r\0\0\0e\0\0\0u\0\0\0e\0\0\0n\0\0\0 \0\0\0T\0\0\0a\0\0\0g\0\0\0!\0\0\0 \0\0\0O\0\0\0 \0\0\0h\0\0\0a\0\0\0l\0\0\0l\0\0\0o\0\0\0o\0\0\0 \0\0\0s\0\0\0c\0\0\0h\0\0\0l\0\0\0a\0\0\0g\0\0\0!\0\0\0\x1f\x20\0\0\n\0\0\0"
                         "e\0\0\0r\0\0\0 \0\0\0s\0\0\0c\0\0\0h\0\0\0o\0\0\0r\0\0\0t\0\0\0e\0\0\0l\0\0\0t\0\0\0 \0\0\0f\0\0\0r\0\0\0o\0\0\0h\0\0\0 \0\0\0g\0\0\0e\0\0\0s\0\0\0i\0\0\0n\0\0\0n\0\0\0t\0\0\0.\0\0\0";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(undSchlagst) & 3);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUcs4LE", undSchlagst, sizeof(undSchlagst) - 1);

    FileOfLines testUcs4(tempDir.child("testUcs4LE"));
    auto l = testUcs4.begin();
    EXPECT_EQ(string("\xe2\x80\x9eUnd schlagst du, ja, den Jammerwoch?"), *l);
    EXPECT_EQ(string("Umarme mich mein b\xc3\xb6hmsches Kind!"), *++l);
    EXPECT_EQ(string("O freuen Tag! O halloo schlag!\xe2\x80\x9f"), *++l);
    EXPECT_EQ(string("er schortelt froh gesinnt."), *++l);
}

TEST(FileOfLinesTest, testAsIterator)
{
    const char mikado[] = "A more humane Mikado never did in Japan exist.\n"
                          "To nobody second I'm certainly reckoned a truephilanthropist.\n"
                          "It is my very humane endeavour to make to some extent\n"
                          "Each evil liver a running river of harmless merriment.\n"
                          "My object all sublime I shall achieve in time.\n"
                          "To let the punishment fit the crime, the punishment fit the crime\n"
                          "And make each prisoner pent unwillingly represent\n"
                          "A source of innocent merriment, of innocent merriment.\n";

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testAsIterator", mikado, sizeof(mikado) - 1);

    const char* mikados[] = { "A more humane Mikado never did in Japan exist.",
                              "To nobody second I'm certainly reckoned a truephilanthropist.",
                              "It is my very humane endeavour to make to some extent",
                              "Each evil liver a running river of harmless merriment.",
                              "My object all sublime I shall achieve in time.",
                              "To let the punishment fit the crime, the punishment fit the crime",
                              "And make each prisoner pent unwillingly represent",
                              "A source of innocent merriment, of innocent merriment." };
    FileOfLines testIterator(tempDir.child("testAsIterator"));
    const char** m = &mikados[0];
    for (auto l : testIterator)
    {
        EXPECT_EQ(string(*m), l);
        ++m;
    }
}

