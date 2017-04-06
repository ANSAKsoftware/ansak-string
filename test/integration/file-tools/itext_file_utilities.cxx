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
// 2014.11.18 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// text_file_utilities_test.cxx -- unit tests for the text file utilities
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <text_file_utilities.hxx>
#include <file_of_lines_exception.hxx>
#include <nullptr_exception.hxx>
#include <file_system_path.hxx>
#include <temp_directory.hxx>
#include <file_handle.hxx>

using namespace std;
using namespace ansak;
using namespace ansak::file;

namespace {

}

TEST(TextFileUtilitiesTest, testFiles)
{
    char textSample[] = "Nothing more strangely indicates an enormous and silent "
        "evil of modern society than the extraordinary use which is made nowadays "
        "of the word \"orthodox.\" In former days the heretic was proud of not "
        "being a heretic. It was the kingdoms of the world and the police and "
        "the judges who were heretics. He was orthodox. He had no pride in having "
        "rebelled against them; they had rebelled against him. The armies with "
        "their cruel security, the kings with their cold faces, the decorous "
        "processes of State, the reasonable processes of law -- all these like "
        "sheep had gone astray. The man was proud of being orthodox, was proud of "
        "being right. If he stood alone in a howling wilderness he was more than "
        "a man; he was a church. He was the centre of the universe; it was round "
        "him that the stars swung. All the tortures torn out of forgotten hells "
        "could not make him admit that he was heretical. But a few modern phrases "
        "have made him boast of it. He says, with a conscious laugh, \"I suppose I "
        "am very heretical,\" and looks round for applause. The word \"heresy\" "
        "not only means no longer being wrong; it practically means being clear-"
        "headed and courageous. The word \"orthodoxy\" not only no longer means "
        "being right; it practically means being wrong. All this can mean one "
        "thing, and one thing only. It means that people care less for whether they "
        "are philosophically right. For obviously a man ought to confess himself "
        "crazy before he confesses himself heretical. The Bohemian, with a red tie, "
        "ought to pique himself on his orthodoxy. The dynamiter, laying a bomb, "
        "ought to feel that, whatever else he is, at least he is orthodox.\n";
    char badFragment[] = "I \1m n\2t a\3n ice \37person.";

    auto tempDir = createTempDirectory();
    FileSystemPath where(tempDir->child("goodOne"));

    auto path = tempDir->asFileSystemPath();
    try
    {
        looksLikeText(path);
        ASSERT_TRUE(false);
    }
    catch (FileOfLinesException& e)
    {
        string what(e.what());
        string exceptionString("FileOfLinesException: path is not a file; file = ");
        exceptionString += path.asUtf8String();
        exceptionString += '.';
        EXPECT_EQ(exceptionString, what);
    }

    try
    {
        looksLikeText(where);
        ASSERT_TRUE(false);
    }
    catch (FileOfLinesException& e)
    {
        string what(e.what());
        string exceptionString("FileOfLinesException: path is invalid; file = ");
        exceptionString += where.asUtf8String();
        exceptionString += '.';
        EXPECT_EQ(exceptionString, what);
    }

    {
        FileHandle fh = FileHandle::create(where);
        EXPECT_TRUE(fh.isOpen());

        auto rc = fh.write(textSample, sizeof(textSample) - 1);
        EXPECT_EQ(sizeof(textSample) - 1, rc);
    }

    // nothing but good stuff there...
    EXPECT_TRUE(looksLikeText(where));
    where.remove();

    {
        FileHandle fh = FileHandle::create(where);
        EXPECT_TRUE(fh.isOpen());

        auto rc = fh.write(textSample, sizeof(textSample) - 1);
        rc += fh.write(badFragment, sizeof(badFragment) - 1);
        EXPECT_EQ(sizeof(textSample) + sizeof(badFragment) - 2, rc);
    }

    // bad stuff happens within the first 4K
    EXPECT_FALSE(looksLikeText(where));
    where.remove();

    {
        FileHandle fh = FileHandle::create(where);
        EXPECT_TRUE(fh.isOpen());

        auto rc = fh.write(textSample, sizeof(textSample) - 1);
        rc += fh.write(textSample, sizeof(textSample) - 1);
        rc += fh.write(textSample, sizeof(textSample) - 1);
        rc += fh.write(textSample, sizeof(textSample) - 1);
        rc += fh.write(badFragment, sizeof(badFragment) - 1);
        EXPECT_EQ(4 * sizeof(textSample) + sizeof(badFragment) - 5, rc);
    }

    // bad stuff happens after the first 4K
    EXPECT_TRUE(looksLikeText(where));
}
