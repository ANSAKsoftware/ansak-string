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
// 2014.12.04
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
// file_of_lines_core_test.cxx -- unit tests for the FileOfLinesCore class
//
///////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>

#include "file_of_lines_core.hxx"
#include "file_of_lines_exception.hxx"
#include "runtime_exception.hxx"
#include "temp_file_wrapper.hxx"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

using namespace std;
using namespace ansak;
using namespace ansak::internal;
using namespace testing;

// test outline... (public interface)
// open()
// get()

//=========================================================================
// All required tests
// variations: UTF-8 (with and without BOM)
//             UTF-16 (LE and BE)
//             UCS-4 (LE and BE)
//             "Local Text" -- that should throw
//             "Local Text" -- after first 1024 chars
//             binary
//             binary after first 1024 chars
//
// sized less than bufferSize, more than bufferSize, more than 2 x bufferSize
//      (one test with a file longer than 4Gb? I don't think so.)
//
// files that end CR, LF, CR-LF, LF-CR
//
//
// get(), sequentially on next line, next line, next line
// get() on known last line, then randomly all over file
// get() -- check endOfFile return
//
// 10 x 3 (+1) x 4  -- 1200 possible combinations; if we test all rows and all
// columns at least once, that'd be good -- but even that is too big to qualify
// as strictly "UnitTests".
//
// This unit test will test open() and get() for small files in all 10 encoding
// cases and leave sampling of the larger set to a separate tool (like RDMTest)

TEST(FileOfLinesCoreTest, testOpen)
{
    TempFileWrapper tempDir;
    tempDir.writeTestFile("testOpen", nullptr, 0);

    FileOfLinesCore testOpen(tempDir.child("testOpen"));
    EXPECT_FALSE(testOpen.isFileOpen());

    try
    {
        testOpen.open();
        ASSERT_TRUE(true);
        EXPECT_FALSE(testOpen.isFileOpen()); // file isn't opened because it's 0-length
    }
    catch (FileOfLinesException& e)
    {
        ASSERT_TRUE(false);
    }

    bool isEndOfFile = false;
    string empty = testOpen.get(0, &isEndOfFile);
    EXPECT_TRUE(empty.empty());
    EXPECT_TRUE(isEndOfFile);
    EXPECT_TRUE(testOpen.isEndOfFileKnown());
    EXPECT_EQ(static_cast<size_t>(0), testOpen.lineCount());
}

TEST(FileOfLinesCoreTest, testGetUnopened)
{
    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUnopened", "This\nis\nonly\na\ntest.", 20);

    FileOfLinesCore testUnopened(tempDir.child("testUnopened"));
    EXPECT_FALSE(testUnopened.isFileOpen());

    try
    {
        testUnopened.get(0);
        ASSERT_FALSE(false);
    }
    catch (RuntimeException& e)
    {
        // EXPECT_EQ(static_cast<int>(ansak::error::Runtime), e.cause());
    }
    EXPECT_FALSE(testUnopened.isEndOfFileKnown());
    try
    {
        EXPECT_EQ(static_cast<size_t>(0), testUnopened.lineCount());
    }
    catch (RuntimeException& e)
    {
        // EXPECT_EQ(static_cast<int>(ansak::error::Runtime), e.cause());
    }
}

TEST(FileOfLinesCoreTest, testUtf8)
{
    char brillig[] = "Es brillig war. Die schlichten Toven\n"
                     "wirrten und wimmelten im Waben\n"
                     "Und aller-m\xc3\xbcmsigen Burg-goven\n"
                     "die mohmen R\xc3\xa4th\' ausgraben.";

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf8", brillig, sizeof(brillig) - 1);

    FileOfLinesCore testUtf8(tempDir.child("testUtf8"));
    EXPECT_FALSE(testUtf8.isFileOpen());
    testUtf8.open();
    EXPECT_TRUE(testUtf8.isFileOpen());
    EXPECT_FALSE(testUtf8.isEndOfFileKnown());

    bool eofRef = false;
    string line = testUtf8.get(0, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_FALSE(testUtf8.isEndOfFileKnown());
    EXPECT_EQ(string("Es brillig war. Die schlichten Toven"), line);

    line = testUtf8.get(1, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_FALSE(testUtf8.isEndOfFileKnown());
    EXPECT_EQ(string("wirrten und wimmelten im Waben"), line);

    line = testUtf8.get(2, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_FALSE(testUtf8.isEndOfFileKnown());
    EXPECT_EQ(string("Und aller-m\xc3\xbcmsigen Burg-goven"), line);

    line = testUtf8.get(3, &eofRef);
    EXPECT_TRUE(eofRef);
    EXPECT_TRUE(testUtf8.isEndOfFileKnown());
    EXPECT_EQ(string("die mohmen R\xc3\xa4th\' ausgraben."), line);
    EXPECT_EQ(static_cast<size_t>(4), testUtf8.lineCount());

    line = testUtf8.get(4);
}

TEST(FileOfLinesCoreTest, testUtf8WithBom)
{
    char bewahre[] = "\xef\xbb\xbf"
                     "Bewahre doch vor Jammerwoch --\n"
                     "die Z\xc3\xa4hnen knirrschen, krahlen kratzen!\n"
                     "Bewahr\' vor Jubjubvogel, vor\n"
                     "frumi\xc3\xb6sen Banderschn\xc3\xa4tzchen!";
    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf8WithBOM", bewahre, sizeof(bewahre) - 1);

    FileOfLinesCore testUtf8(tempDir.child("testUtf8WithBOM"));
    testUtf8.open();

    bool eofRef = false;
    string line = testUtf8.get(0, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("Bewahre doch vor Jammerwoch --"), line);

    line = testUtf8.get(1, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("die Z\xc3\xa4hnen knirrschen, krahlen kratzen!"), line);

    line = testUtf8.get(2, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("Bewahr\' vor Jubjubvogel, vor"), line);

    line = testUtf8.get(3, &eofRef);
    EXPECT_TRUE(eofRef);
    EXPECT_EQ(string("frumi\xc3\xb6sen Banderschn\xc3\xa4tzchen!"), line);
}

TEST(FileOfLinesCoreTest, testUtf16BE)
{
    char erGriff[] = "\xfe\xff"
                     "\0E\0r\0 \0g\0r\0i\0f\0f\0 \0s\0e\0i\0n\0 \0V\0o\0r\0p\0a\0l\0s\0c\0h\0w\0e\0r\0t\0c\0h\0e\0n\0 \0z\0u\0 \0-\0-\0\n"
                     "\0L\0a\0n\0g\0 \0s\0u\0c\0h\0t\0e\0t\0 \0e\0r\0 \0d\0a\0s\0 \0m\0a\0n\0c\0h\0s\0a\0m\0'\0 \0D\0i\0n\0g\0!\0\n"
                     "\0S\0o\0.\0 \0S\0t\0e\0h\0e\0n\0d\0 \0u\0n\0t\0e\0r\0m\0 \0T\0u\0m\0t\0u\0m\0b\0a\0u\0m\0\n"
                     "\0e\0r\0 \0a\0n\0z\0u\0d\0e\0n\0k\0e\0n\0f\0i\0n\0g\0.";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(erGriff) & 1);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf16BE", erGriff, sizeof(erGriff) - 1);

    FileOfLinesCore testUtf16(tempDir.child("testUtf16BE"));
    testUtf16.open();

    bool eofRef = false;
    string line = testUtf16.get(0, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("Er griff sein Vorpalschwertchen zu --"), line);

    line = testUtf16.get(1, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("Lang suchtet er das manchsam' Ding!"), line);

    line = testUtf16.get(2, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("So. Stehend unterm Tumtumbaum"), line);

    line = testUtf16.get(3, &eofRef);
    EXPECT_TRUE(eofRef);
    EXPECT_EQ(string("er anzudenkenfing."), line);
}

TEST(FileOfLinesCoreTest, testUtf16LE)
{
    char undAls[] = "\xff\xfe"
                    "U\0n\0d\0 \0a\0l\0s\0 \0e\0r\0 \0t\0i\0e\0f\0 \0i\0n\0 \0A\0n\0d\0a\0c\0h\0t\0 \0s\0t\0a\0n\0d\0\n\0"
                    "d\0e\0s\0 \0J\0a\0m\0m\0e\0r\0w\0o\0c\0h\0e\0n\0s\0 \0A\0u\0g\0e\0n\0f\0e\0u\0e\0r\0\n\0"
                    "d\0u\0r\0c\0h\0 \0T\0u\0r\0g\0e\0n\0w\0a\0l\0d\0 \0i\0n\0 \0W\0i\0f\0f\0e\0k\0 \0k\0a\0m\0,\0\n\0"
                    "e\0i\0n\0 \0b\0u\0r\0b\0l\0e\0n\0d\0 \0U\0n\0g\0e\0h\0e\0u\0e\0r\0.\0";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(undAls) & 1);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUtf16LE", undAls, sizeof(undAls) - 1);

    FileOfLinesCore testUtf16(tempDir.child("testUtf16LE"));
    testUtf16.open();

    bool eofRef = false;
    string line = testUtf16.get(0, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("Und als er tief in Andacht stand"), line);

    line = testUtf16.get(1, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("des Jammerwochens Augenfeuer"), line);

    line = testUtf16.get(2, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("durch Turgenwald in Wiffek kam,"), line);

    line = testUtf16.get(3, &eofRef);
    EXPECT_TRUE(eofRef);
    EXPECT_EQ(string("ein burblend Ungeheuer."), line);
}

TEST(FileOfLinesCoreTest, testUcs4BE)
{
    char einsZwei[] = "\0\0\xfe\xff"
                      "\0\0\0E\0\0\0i\0\0\0n\0\0\0s\0\0\0!\0\0\0 \0\0\0Z\0\0\0w\0\0\0e\0\0\0i\0\0\0!\0\0\0 \0\0\0E\0\0\0i\0\0\0n\0\0\0s\0\0\0!\0\0\0 \0\0\0Z\0\0\0w\0\0\0e\0\0\0i\0\0\0!\0\0\0 \0\0\0u\0\0\0n\0\0\0d\0\0\0 \0\0\0d\0\0\0u\0\0\0r\0\0\0c\0\0\0h\0\0\0 \0\0\0u\0\0\0n\0\0\0d\0\0\0 \0\0\0d\0\0\0u\0\0\0r\0\0\0c\0\0\0h\0\0\0\n"
                      "\0\0\0d\0\0\0a\0\0\0s\0\0\0 \0\0\0V\0\0\0o\0\0\0r\0\0\0p\0\0\0a\0\0\0l\0\0\0s\0\0\0c\0\0\0h\0\0\0w\0\0\0e\0\0\0r\0\0\0t\0\0\0 \0\0\0z\0\0\0e\0\0\0r\0\0\0s\0\0\0c\0\0\0h\0\0\0n\0\0\0i\0\0\0e\0\0\0f\0\0\0e\0\0\0r\0\0\0s\0\0\0c\0\0\0h\0\0\0n\0\0\0\xfc\0\0\0c\0\0\0k\0\0\0.\0\0\0\n"
                      "\0\0\0S\0\0\0o\0\0\0 \0\0\0l\0\0\0i\0\0\0e\0\0\0g\0\0\0t\0\0\0 \0\0\0e\0\0\0s\0\0\0 \0\0\0t\0\0\0o\0\0\0d\0\0\0.\0\0\0 \0\0\0E\0\0\0r\0\0\0 \0\0\0K\0\0\0o\0\0\0p\0\0\0f\0\0\0 \0\0\0i\0\0\0m\0\0\0 \0\0\0H\0\0\0a\0\0\0n\0\0\0d\0\0\0\n"
                      "\0\0\0g\0\0\0e\0\0\0l\0\0\0\xe4\0\0\0u\0\0\0m\0\0\0f\0\0\0i\0\0\0g\0\0\0 \0\0\0z\0\0\0o\0\0\0g\0\0\0 \0\0\0z\0\0\0u\0\0\0r\0\0\0\xfc\0\0\0c\0\0\0k\0\0\0.";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(einsZwei) & 3);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUcs4BE", einsZwei, sizeof(einsZwei) - 1);

    FileOfLinesCore testUcs4(tempDir.child("testUcs4BE"));
    testUcs4.open();

    bool eofRef = false;
    string line = testUcs4.get(0, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("Eins! Zwei! Eins! Zwei! und durch und durch"), line);

    line = testUcs4.get(1, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("das Vorpalschwert zerschnieferschn\xc3\xbc" "ck."), line);

    line = testUcs4.get(2, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("So liegt es tod. Er Kopf im Hand"), line);

    line = testUcs4.get(3, &eofRef);
    EXPECT_TRUE(eofRef);
    EXPECT_EQ(string("gel\xc3\xa4umfig zog zur\xc3\xbc" "ck."), line);
}

TEST(FileOfLinesCoreTest, testUcs4LE)
{
    char undSchlagst[] = "\xff\xfe\0\0"
                         "\x1e\x20\0\0U\0\0\0n\0\0\0d\0\0\0 \0\0\0s\0\0\0c\0\0\0h\0\0\0l\0\0\0a\0\0\0g\0\0\0s\0\0\0t\0\0\0 \0\0\0d\0\0\0u\0\0\0,\0\0\0 \0\0\0j\0\0\0a\0\0\0,\0\0\0 \0\0\0d\0\0\0e\0\0\0n\0\0\0 \0\0\0J\0\0\0a\0\0\0m\0\0\0m\0\0\0e\0\0\0r\0\0\0w\0\0\0o\0\0\0c\0\0\0h\0\0\0?\0\0\0\n\0\0\0"
                         "U\0\0\0m\0\0\0a\0\0\0r\0\0\0m\0\0\0e\0\0\0 \0\0\0m\0\0\0i\0\0\0c\0\0\0h\0\0\0 \0\0\0m\0\0\0e\0\0\0i\0\0\0n\0\0\0 \0\0\0b\0\0\0\xf6\0\0\0h\0\0\0m\0\0\0s\0\0\0c\0\0\0h\0\0\0e\0\0\0s\0\0\0 \0\0\0K\0\0\0i\0\0\0n\0\0\0d\0\0\0!\0\0\0\n\0\0\0"
                         "O\0\0\0 \0\0\0f\0\0\0r\0\0\0e\0\0\0u\0\0\0e\0\0\0n\0\0\0 \0\0\0T\0\0\0a\0\0\0g\0\0\0!\0\0\0 \0\0\0O\0\0\0 \0\0\0h\0\0\0a\0\0\0l\0\0\0l\0\0\0o\0\0\0o\0\0\0 \0\0\0s\0\0\0c\0\0\0h\0\0\0l\0\0\0a\0\0\0g\0\0\0!\0\0\0\x1f\x20\0\0\n\0\0\0"
                         "e\0\0\0r\0\0\0 \0\0\0s\0\0\0c\0\0\0h\0\0\0o\0\0\0r\0\0\0t\0\0\0e\0\0\0l\0\0\0t\0\0\0 \0\0\0f\0\0\0r\0\0\0o\0\0\0h\0\0\0 \0\0\0g\0\0\0e\0\0\0s\0\0\0i\0\0\0n\0\0\0n\0\0\0t\0\0\0.\0\0\0";
    EXPECT_EQ(static_cast<size_t>(1), sizeof(undSchlagst) & 3);

    TempFileWrapper tempDir;
    tempDir.writeTestFile("testUcs4LE", undSchlagst, sizeof(undSchlagst) - 1);

    FileOfLinesCore testUcs4(tempDir.child("testUcs4LE"));
    testUcs4.open();

    bool eofRef = false;
    string line = testUcs4.get(0, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("\xe2\x80\x9eUnd schlagst du, ja, den Jammerwoch?"), line);

    line = testUcs4.get(1, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("Umarme mich mein b\xc3\xb6hmsches Kind!"), line);

    line = testUcs4.get(2, &eofRef);
    EXPECT_FALSE(eofRef);
    EXPECT_EQ(string("O freuen Tag! O halloo schlag!\xe2\x80\x9f"), line);

    line = testUcs4.get(3, &eofRef);
    EXPECT_TRUE(eofRef);
    EXPECT_EQ(string("er schortelt froh gesinnt."), line);
}

TEST(FileOfLinesCoreTest, testLongFile)
{
    FilePath thisDir(FilePath(__FILE__).parent());
    FileSystemPath unicodeData(thisDir.child("Unicode8Data.txt"));

    FileOfLinesCore core(unicodeData);
    core.open();

    EXPECT_FALSE(core.get(10).empty());
    EXPECT_FALSE(core.get(100).empty());
    EXPECT_FALSE(core.get(1000).empty());
    EXPECT_FALSE(core.get(50).empty());
    string l(core.get(29214));
    EXPECT_FALSE(l.empty());
    l = core.get(29215);
    EXPECT_TRUE(l.empty());
    EXPECT_FALSE(core.get(0).empty());

    int i = 29213;
    while (i > 15000)
    {
        EXPECT_FALSE(core.get(--i).empty());
    }
}

TEST(FileOfLinesCoreTest, test8bitFileWithCants)
{
    char controlEnthusiast[] = "Long t\01me t\02he m\03nxs\04me f\05e h\06 s\07ug\10t\n";
    char localHero[] = "Eins zwei drei vier f\x81nf sechs sieben acht neun zehn.";

    TempFileWrapper tempDir;
    tempDir.writeTestFile("manxsome-ctrl.txt", controlEnthusiast, sizeof(controlEnthusiast) - 1);
    tempDir.writeTestFile("im-no-hero.txt", localHero, sizeof(localHero) - 1);

    // short controls
    FileSystemPath cantOpenCtlChars(FileSystemPath(tempDir.child("manxsome-ctrl.txt")));

    try
    {
        FileOfLinesCore shouldnt(cantOpenCtlChars);
        shouldnt.open();
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, open failed"));
    }

    // short locals
    FileSystemPath antiHero(FileSystemPath(tempDir.child("im-no-hero.txt")));
    {
        auto fh = FileHandle::create(antiHero);
        fh.write(localHero, sizeof(localHero) - 1);
    }

    try
    {
        FileOfLinesCore shouldnt(antiHero);
        shouldnt.open();
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, file isn't entirely Unicode"));
    }

    FilePath thisDir(FilePath(__FILE__).parent());
    FileSystemPath penseesAscii(thisDir.child("pensees-ascii.txt"));

    // longer controls
    FileSystemPath openableCtlChars(FileSystemPath(tempDir.child("pensees+ctrls.txt")));
    openableCtlChars.copyFromFile(penseesAscii);
    {
        FileHandle fh = FileHandle::openForAppending(openableCtlChars);
        fh.write("\n", 1);
        fh.write(controlEnthusiast, sizeof(controlEnthusiast) - 1);
    }

    try
    {
        FileOfLinesCore shouldnt(openableCtlChars);
        shouldnt.open();

        int i = 0;
        for( ;; ++i )
        {
            shouldnt.get(i);
            if (i > 500)
            {
                ASSERT_TRUE(false);
            }
        }
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, open failed"));
    }

    // longer locals
    FileSystemPath openableLocals(FileSystemPath(tempDir.child("pensees+locals.txt")));
    openableLocals.copyFromFile(penseesAscii);
    {
        FileHandle fh = FileHandle::openForAppending(openableLocals);
        fh.write("\n", 1);
        fh.write(localHero, sizeof(localHero) - 1);
    }

    try
    {
        FileOfLinesCore shouldnt(openableLocals);
        shouldnt.open();

        int i = 0;
        for( ;; ++i )
        {
            shouldnt.get(i);
            if (i > 500)
            {
                ASSERT_TRUE(false);
            }
        }
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, file isn't entirely Unicode"));
    }
}

TEST(FileOfLinesCoreTest, test16bitFileWithCants)
{
    char16_t controlEnthusiast[] = u"\ufeffLong t\01me t\02he m\03nxs\04me f\05e h\06 s\07ug\10t\n";
    char16_t localHero[] = u"\ufeffEins zwei drei vier f\x81nf sechs sieben acht neun zehn.";

    TempFileWrapper tempDir;
    tempDir.writeTestFile("manxsome-ctrl.txt", reinterpret_cast<char*>(controlEnthusiast),
                                      sizeof(controlEnthusiast) - 2);
    tempDir.writeTestFile("im-no-hero.txt", reinterpret_cast<char*>(localHero), sizeof(localHero) - 2);

    // short controls
    FileSystemPath cantOpenCtlChars(FileSystemPath(tempDir.child("manxsome-ctrl.txt")));

    try
    {
        FileOfLinesCore shouldnt(cantOpenCtlChars);
        shouldnt.open();
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, open failed"));
    }

    // short locals
    FileSystemPath antiHero(FileSystemPath(tempDir.child("im-no-hero.txt")));
    try
    {
        FileOfLinesCore shouldnt(antiHero);
        shouldnt.open();
    }
    catch (FileOfLinesException& e)
    {
        string es(e.what());
        string fragment("FileOfLinesException: File could not be opened");
        EXPECT_EQ(fragment, es.substr(0, fragment.size()));
    }

    FilePath thisDir(FilePath(__FILE__).parent());
    FileSystemPath penseesAscii(thisDir.child("pensees-ascii.txt"));

    // longer controls
    FileSystemPath openableCtlChars(FileSystemPath(tempDir.child("pensees+ctrls.txt")));
    openableCtlChars.copyFromFile(penseesAscii);
    {
        FileHandle fh = FileHandle::openForAppending(openableCtlChars);
        fh.write("\n", 1);
        fh.write(reinterpret_cast<char*>(controlEnthusiast + 1), sizeof(controlEnthusiast) - 4);
    }

    try
    {
        FileOfLinesCore shouldnt(openableCtlChars);
        shouldnt.open();

        int i = 0;
        for( ;; ++i )
        {
            shouldnt.get(i);
            if (i > 500)
            {
                ASSERT_TRUE(false);
            }
        }
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, open failed"));
    }

    // longer locals
    FileSystemPath openableLocals(FileSystemPath(tempDir.child("pensees+locals.txt")));
    openableLocals.copyFromFile(penseesAscii);
    {
        FileHandle fh = FileHandle::openForAppending(openableLocals);
        fh.write("\n", 1);
        fh.write(reinterpret_cast<char*>(localHero + 1), sizeof(localHero) - 4);
    }

    try
    {
        FileOfLinesCore shouldnt(openableLocals);
        shouldnt.open();

        int i = 0;
        for( ;; ++i )
        {
            shouldnt.get(i);
            if (i > 500)
            {
                ASSERT_TRUE(false);
            }
        }
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, file isn't entirely Unicode"));
    }
}

TEST(FileOfLinesCoreTest, test32bitFileWithCants)
{
    char32_t controlEnthusiast[] = U"\U0000feffLong t\01me t\02he m\03nxs\04me f\05e h\06 s\07ug\10t\n";
    char32_t localHero[] = U"\U0000feffEins zwei drei vier f\x81nf sechs sieben acht neun zehn.";

    TempFileWrapper tempDir;
    tempDir.writeTestFile("manxsome-ctrl.txt", reinterpret_cast<char*>(controlEnthusiast),
                                      sizeof(controlEnthusiast) - 4);
    tempDir.writeTestFile("im-no-hero.txt", reinterpret_cast<char*>(localHero), sizeof(localHero) - 4);

    // short controls
    FileSystemPath cantOpenCtlChars(FileSystemPath(tempDir.child("manxsome-ctrl.txt")));

    try
    {
        FileOfLinesCore shouldnt(cantOpenCtlChars);
        shouldnt.open();
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, open failed"));
    }

    // short locals
    FileSystemPath antiHero(FileSystemPath(tempDir.child("im-no-hero.txt")));
    try
    {
        FileOfLinesCore shouldnt(antiHero);
        shouldnt.open();
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, open failed"));
    }

    FilePath thisDir(FilePath(__FILE__).parent());
    FileSystemPath penseesAscii(thisDir.child("pensees-ascii.txt"));

    // longer controls
    FileSystemPath openableCtlChars(FileSystemPath(tempDir.child("pensees+ctrls.txt")));
    openableCtlChars.copyFromFile(penseesAscii);
    {
        FileHandle fh = FileHandle::openForAppending(openableCtlChars);
        fh.write("\n", 1);
        fh.write(reinterpret_cast<char*>(controlEnthusiast + 1), sizeof(controlEnthusiast) - 8);
    }

    try
    {
        FileOfLinesCore shouldnt(openableCtlChars);
        shouldnt.open();

        int i = 0;
        for( ;; ++i )
        {
            shouldnt.get(i);
            if (i > 500)
            {
                ASSERT_TRUE(false);
            }
        }
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, open failed"));
    }

    // longer locals
    FileSystemPath openableLocals(FileSystemPath(tempDir.child("pensees+locals.txt")));
    openableLocals.copyFromFile(penseesAscii);
    {
        FileHandle fh = FileHandle::openForAppending(openableLocals);
        fh.write("\n", 1);
        fh.write(reinterpret_cast<char*>(localHero + 1), sizeof(localHero) - 8);
    }

    try
    {
        FileOfLinesCore shouldnt(openableLocals);
        shouldnt.open();

        int i = 0;
        for( ;; ++i )
        {
            shouldnt.get(i);
            if (i > 500)
            {
                ASSERT_TRUE(false);
            }
        }
    }
    catch (FileOfLinesException& e)
    {
        EXPECT_THAT(string(e.what()),
                    StartsWith("FileOfLinesException: in FileOfLinesCore::classifyFile, file isn't entirely Unicode"));
    }
}
