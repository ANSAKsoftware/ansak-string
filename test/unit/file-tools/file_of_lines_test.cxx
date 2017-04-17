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
// 2017.04.15 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines_test.cxx -- unit test for the file-of-lines
//
///////////////////////////////////////////////////////////////////////////

#include <file_of_lines.hxx>
#include <file_of_lines_exception.hxx>
#include <runtime_exception.hxx>

#include <file_path.hxx>
#include <mock_file_handle.hxx>
#include <mock_file_handle_exception.hxx>
#include <mock_file_system_path.hxx>
#include <mock_operating_system.hxx>
#include <array>
#include <memory>

#include <gtest/gtest.h>

using namespace ansak;
using namespace std;
using namespace testing;

namespace ansak {

extern unique_ptr<OperatingSystemMock> primitive;
extern OperatingSystemMock* theMock;

}

using namespace ansak;

namespace
{
#if defined(WIN32)
FilePath problemFile("c:\\Users\\jvplasmeuden\\homeworklines.txt");
#else
FilePath problemFile("/home/jvplasmeuden/homeworklines.txt");
#endif
}

class FileOfLinesFixture : public Test
{
public:
    FileOfLinesFixture() = default;
    ~FileOfLinesFixture() = default;

    FileSystemPathMock& PathMock() { return m_theFSPMock; }
    FileHandleMock& HandleMock() { return m_theHandleMock; }
    FileHandleExceptionMock& ExceptionMock() { return m_theExceptionMock; };

private:
    FileSystemPathMock      m_theFSPMock;
    FileHandleMock          m_theHandleMock;
    FileHandleExceptionMock m_theExceptionMock;
};

class LoadedFileOfLinesFixture : public FileOfLinesFixture
{
public:
    static const string poem;

    LoadedFileOfLinesFixture()
    {
        EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(poem.size()));
        EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
        EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
        EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
            SetArrayArgument<1>(poem.begin(), poem.end()),
            Return(poem.size()))).
            WillOnce(DoAll(
                SetArrayArgument<1>(poem.begin(), poem.begin() + 3),
                Return(3))).
            WillOnce(DoAll(
                SetArrayArgument<1>(poem.begin(), poem.end()),
                Return(poem.size())));
        EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
        EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());
    }
    ~LoadedFileOfLinesFixture() = default;

};

const string LoadedFileOfLinesFixture::poem = { "I've never met a purple cow\n"
    "I never hope to meet one\n"
    "But judging from the milk we get\n"
    "There certainly must be one.\n\n" };

TEST(LinesTest, construct)
{
    auto path = FileSystemPath(problemFile);
    auto f = FileOfLines(path);
}

TEST_F(FileOfLinesFixture, openZeroLength)
{
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(0));

    auto path = FileSystemPath(problemFile);
    FileOfLines f(path);
    EXPECT_EQ(0u, f.size());
}

TEST_F(FileOfLinesFixture, openCantClassify)
{
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(1024)).WillOnce(Return(0));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));

    auto path = FileSystemPath(problemFile);
    FileOfLines f(path);
    EXPECT_EQ(f.begin(), f.end());
}

TEST_F(FileOfLinesFixture, cantOpen)
{
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(1024));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    HandleMock().setFailNextOpen();

    auto path = FileSystemPath(problemFile);
    FileOfLines f(path);
    EXPECT_EQ(f.begin(), f.end());
}

TEST_F(FileOfLinesFixture, isLocalText)
{
    string testText("How large is that in \xc5ngstroms?");
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
            SetArrayArgument<1>(testText.begin(), testText.end()),
            Return(testText.size())));

    auto path = FileSystemPath(problemFile);
    FileOfLines f(path);
    EXPECT_EQ(f.begin(), f.end());
}

TEST_F(FileOfLinesFixture, isUtf8Text)
{
    string testText("How large is that in \xc3\x85ngstroms?");
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size()))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.begin() + 3),
                Return(3))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size())));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLines f(path);
    EXPECT_EQ(string("How large is that in \xc3\x85ngstroms?"), *f.begin());
}

TEST_F(FileOfLinesFixture, isMarkedUtf8Text)
{
    string testText("\xef\xbb\xbfHow large is that in \xc3\x85ngstroms?");
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size()))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.begin() + 3),
                Return(3))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size())));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLines f(path);
    EXPECT_EQ(string("How large is that in \xc3\x85ngstroms?"), *f.begin());
}

TEST_F(LoadedFileOfLinesFixture, lastLineBlank)
{
    auto path = FileSystemPath(problemFile);
    FileOfLines f(path);
    auto i = f.begin();

    EXPECT_EQ("I've never met a purple cow", *(i++));
    EXPECT_EQ("I never hope to meet one", *i);
    EXPECT_EQ("But judging from the milk we get", *++i);
    i += 2;
    EXPECT_TRUE((i--)->empty());
    EXPECT_EQ("There certainly must be one.", *i);
    EXPECT_TRUE((++i)->empty());
    EXPECT_EQ(f.end(), ++i);

    i = f.begin();
    EXPECT_EQ(5u, f.size());
    auto j = i;
    EXPECT_NE(*i, *++j);
    auto k = i + 1;
    EXPECT_EQ(*j, *k);
    k = ++j - 2;
    EXPECT_EQ(*k, *i);
    EXPECT_THROW(k - 1, RuntimeException);
    EXPECT_THROW(i + 25, RuntimeException);

    i = f.begin();
    j = i + 1;
    k = j;
    EXPECT_EQ(*--k, *i);
    k = j;
    EXPECT_EQ(*(k--), *j);
    EXPECT_EQ(*k, *i);
}

TEST_F(LoadedFileOfLinesFixture, closedPlusPlus)
{
    FileOfLines::const_iterator x;
    {
        auto path = FileSystemPath(problemFile);
        FileOfLines f(path);
        x = f.begin();
    }
    EXPECT_THROW(x++, RuntimeException);
}
