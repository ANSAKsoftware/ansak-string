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
// 2017.04.10 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines_core_test.cxx -- unit test for file-of-lines core
//
///////////////////////////////////////////////////////////////////////////

#include <file_of_lines_core.hxx>
#include <file_of_lines_exception.hxx>

#include <file_path.hxx>
#include <mock_file_handle.hxx>
#include <mock_file_handle_exception.hxx>
#include <mock_file_system_path.hxx>
#include <mock_operating_system.hxx>
#include <array>
#include <memory>

#include <gtest/gtest.h>

using namespace ansak;
using namespace ansak::internal;
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

class FileOfLinesCoreFixture : public Test
{
public:
    FileOfLinesCoreFixture() = default;
    ~FileOfLinesCoreFixture() = default;

    FileSystemPathMock& PathMock() { return m_theFSPMock; }
    FileHandleMock& HandleMock() { return m_theHandleMock; }
    FileHandleExceptionMock& ExceptionMock() { return m_theExceptionMock; };

private:
    FileSystemPathMock      m_theFSPMock;
    FileHandleMock          m_theHandleMock;
    FileHandleExceptionMock m_theExceptionMock;
};

TEST(LinesCoreTest, construct)
{
    auto path = FileSystemPath(problemFile);
    EXPECT_FALSE(FileOfLinesCore(path).isFileOpen());
}

TEST_F(FileOfLinesCoreFixture, openZeroLength)
{
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(0));

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();
    EXPECT_FALSE(f.isFileOpen());
}

TEST_F(FileOfLinesCoreFixture, openCantClassify)
{
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(1024)).WillOnce(Return(0));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    EXPECT_THROW(f.open(), FileOfLinesException);
}

TEST_F(FileOfLinesCoreFixture, cantOpen)
{
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(1024));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    HandleMock().setFailNextOpen();

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    EXPECT_THROW(f.open(), FileOfLinesException);
}

TEST_F(FileOfLinesCoreFixture, isLocalText)
{
    string testText("How large is that in \xc5ngstroms?");
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
            SetArrayArgument<1>(testText.begin(), testText.end()),
            Return(testText.size())));

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    EXPECT_THROW(f.open(), FileOfLinesException);
}

TEST_F(FileOfLinesCoreFixture, isUtf8Text)
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
    FileOfLinesCore f(path);
    f.open();
}

TEST_F(FileOfLinesCoreFixture, isMarkedUtf8Text)
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
    FileOfLinesCore f(path);
    f.open();
}

TEST_F(FileOfLinesCoreFixture, isUtf16Text)
{
    array<char, 64> testText { {'\xff', '\xfe',
                'H', 0, 'o', 0, 'w', 0, ' ', 0, 'l', 0, 'a',    0, 'r', 0, 'g', 0,
                'e', 0, ' ', 0, 'i', 0, 's', 0, ' ', 0, 't',    0, 'h', 0, 'a', 0,
                't', 0, ' ', 0, 'i', 0, 'n', 0, ' ', 0, '\xc5', 0, 'n', 0, 'g', 0,
                's', 0, 't', 0, 'r', 0, 'o', 0, 'm', 0, 's',    0, '?', 0 } };
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size()))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.begin() + 3),
                Return(3)));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();
}

TEST_F(FileOfLinesCoreFixture, isUtf16RevText)
{
    array<char, 64> testText { {'\xfe', '\xff',
                 0, 'H', 0, 'o', 0, 'w', 0, ' ', 0, 'l', 0, 'a',    0, 'r', 0, 'g',
                 0, 'e', 0, ' ', 0, 'i', 0, 's', 0, ' ', 0, 't',    0, 'h', 0, 'a',
                 0, 't', 0, ' ', 0, 'i', 0, 'n', 0, ' ', 0, '\xc5', 0, 'n', 0, 'g',
                 0, 's', 0, 't', 0, 'r', 0, 'o', 0, 'm', 0, 's',    0, '?' } };
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size()))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.begin() + 3),
                Return(3)));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();
}

TEST_F(FileOfLinesCoreFixture, isUcs4Text)
{
    array<char, 128> testText { { '\xff', '\xfe', 0, 0,
                'H',    0, 0, 0, 'o', 0, 0, 0, 'w', 0, 0, 0, ' ', 0, 0, 0, 'l', 0, 0, 0, 'a', 0, 0, 0, 'r', 0, 0, 0,
                'g',    0, 0, 0, 'e', 0, 0, 0, ' ', 0, 0, 0, 'i', 0, 0, 0, 's', 0, 0, 0, ' ', 0, 0, 0, 't', 0, 0, 0,
                'h',    0, 0, 0, 'a', 0, 0, 0, 't', 0, 0, 0, ' ', 0, 0, 0, 'i', 0, 0, 0, 'n', 0, 0, 0, ' ', 0, 0, 0,
                '\xc5', 0, 0, 0, 'n', 0, 0, 0, 'g', 0, 0, 0, 's', 0, 0, 0, 't', 0, 0, 0, 'r', 0, 0, 0, 'o', 0, 0, 0,
                'm',    0, 0, 0, 's', 0, 0, 0, '?', 0, 0, 0 } };
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size()))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.begin() + 3),
                Return(3)));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();
}

TEST_F(FileOfLinesCoreFixture, isUcsRev4Text)
{
    array<char, 128> testText { { 0, 0, '\xfe', '\xff',
                0, 0, 0, 'H',    0, 0, 0, 'o', 0, 0, 0, 'w', 0, 0, 0, ' ', 0, 0, 0, 'l', 0, 0, 0, 'a', 0, 0, 0, 'r',
                0, 0, 0, 'g',    0, 0, 0, 'e', 0, 0, 0, ' ', 0, 0, 0, 'i', 0, 0, 0, 's', 0, 0, 0, ' ', 0, 0, 0, 't',
                0, 0, 0, 'h',    0, 0, 0, 'a', 0, 0, 0, 't', 0, 0, 0, ' ', 0, 0, 0, 'i', 0, 0, 0, 'n', 0, 0, 0, ' ',
                0, 0, 0, '\xc5', 0, 0, 0, 'n', 0, 0, 0, 'g', 0, 0, 0, 's', 0, 0, 0, 't', 0, 0, 0, 'r', 0, 0, 0, 'o',
                0, 0, 0, 'm',    0, 0, 0, 's', 0, 0, 0, '?' } };
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(testText.size()));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.end()),
                Return(testText.size()))).
            WillOnce(DoAll(
                SetArrayArgument<1>(testText.begin(), testText.begin() + 3),
                Return(3)));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();
}

TEST_F(FileOfLinesCoreFixture, fourLines1)
{
    string poem("I've never met a purple cow\x0a"
        "I never hope to meet one\x0a"
        "But judging from the milk we get\x0a"
        "There certainly must be one.");
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

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_STREQ("There certainly must be one.", f.get(3).c_str());
}

TEST_F(FileOfLinesCoreFixture, fourLines2)
{
    string poem("I've never met a purple cow\x0d"
        "I never hope to meet one\x0d"
        "But judging from the milk we get\x0d"
        "There certainly must be one.");
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

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_STREQ("There certainly must be one.", f.get(3).c_str());
}

TEST_F(FileOfLinesCoreFixture, fourLines3)
{
    string poem("I've never met a purple cow\x0d\x0a"
        "I never hope to meet one\x0d\x0a"
        "But judging from the milk we get\x0d\x0a"
        "There certainly must be one.");
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

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_STREQ("There certainly must be one.", f.get(3).c_str());
}

TEST_F(FileOfLinesCoreFixture, lastLineBlank)
{
    string poem("I've never met a purple cow\n"
        "I never hope to meet one\n"
        "But judging from the milk we get\n"
        "There certainly must be one.\n");
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

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_STREQ("There certainly must be one.", f.get(3).c_str());
    EXPECT_TRUE(f.get(4).empty());
}

TEST_F(FileOfLinesCoreFixture, innerLineBlank)
{
    string poem("I've never met a purple cow\n"
        "I never hope to meet one\n"
        "But judging from the milk we get\n\n"
        "There certainly must be one.");
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

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_TRUE(f.get(3).empty());
    EXPECT_STREQ("There certainly must be one.", f.get(4).c_str());
}

TEST_F(FileOfLinesCoreFixture, fetchBackward)
{
    string poem("I've never met a purple cow\x0a"
        "I never hope to meet one\x0a"
        "But judging from the milk we get\x0a"
        "There certainly must be one.");
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

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("There certainly must be one.", f.get(3).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
}

TEST_F(FileOfLinesCoreFixture, fourLinesUtf16)
{
    char poem[] = { '\xff', '\xfe',
    'I', 0, '\'', 0, 'v', 0, 'e', 0, ' ', 0, 'n', 0, 'e', 0, 'v', 0, 'e', 0, 'r', 0, ' ', 0, 'm', 0, 'e', 0, 't', 0, ' ', 0,
    'a', 0, ' ', 0, 'p', 0, 'u', 0, 'r', 0, 'p', 0, 'l', 0, 'e', 0, ' ', 0, 'c', 0, 'o', 0, 'w', 0, '\x0a', 0, 'I', 0,
    ' ', 0, 'n', 0, 'e', 0, 'v', 0, 'e', 0, 'r', 0, ' ', 0, 'h', 0, 'o', 0, 'p', 0, 'e', 0, ' ', 0, 't', 0, 'o', 0, ' ', 0,
    'm', 0, 'e', 0, 'e', 0, 't', 0, ' ', 0, 'o', 0, 'n', 0, 'e', 0, '\x0a', 0, 'B', 0, 'u', 0, 't', 0, ' ', 0, 'j', 0,
    'u', 0, 'd', 0, 'g', 0, 'i', 0, 'n', 0, 'g', 0, ' ', 0, 'f', 0, 'r', 0, 'o', 0, 'm', 0, ' ', 0, 't', 0, 'h', 0, 'e', 0,
    ' ', 0, 'm', 0, 'i', 0, 'l', 0, 'k', 0, ' ', 0, 'w', 0, 'e', 0, ' ', 0, 'g', 0, 'e', 0, 't', 0, '\x0a', 0, 'T', 0,
    'h', 0, 'e', 0, 'r', 0, 'e', 0, ' ', 0, 'c', 0, 'e', 0, 'r', 0, 't', 0, 'a', 0, 'i', 0, 'n', 0, 'l', 0, 'y', 0, ' ', 0,
    'm', 0, 'u', 0, 's', 0, 't', 0, ' ', 0, 'b', 0, 'e', 0, ' ', 0, 'o', 0, 'n', 0, 'e', 0, '.', 0 };
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(sizeof(poem)));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
        SetArrayArgument<1>(begin(poem), end(poem)),
        Return(sizeof(poem)))).
        WillRepeatedly(DoAll(
            SetArrayArgument<1>(begin(poem), end(poem)),
            Return(sizeof(poem))));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_STREQ("There certainly must be one.", f.get(3).c_str());
}

TEST_F(FileOfLinesCoreFixture, fourLinesUcs4)
{
    char poem[] = { '\xff', '\xfe', 0, 0,
    'I', 0,0,0, '\'', 0,0,0, 'v', 0,0,0, 'e', 0,0,0, ' ', 0,0,0, 'n', 0,0,0, 'e', 0,0,0, 'v', 0,0,0, 'e', 0,0,0, 'r', 0,0,0, ' ', 0,0,0, 'm', 0,0,0, 'e', 0,0,0, 't', 0,0,0, ' ', 0,0,0,
    'a', 0,0,0, ' ', 0,0,0, 'p', 0,0,0, 'u', 0,0,0, 'r', 0,0,0, 'p', 0,0,0, 'l', 0,0,0, 'e', 0,0,0, ' ', 0,0,0, 'c', 0,0,0, 'o', 0,0,0, 'w', 0,0,0, '\x0a', 0,0,0, 'I', 0,0,0,
    ' ', 0,0,0, 'n', 0,0,0, 'e', 0,0,0, 'v', 0,0,0, 'e', 0,0,0, 'r', 0,0,0, ' ', 0,0,0, 'h', 0,0,0, 'o', 0,0,0, 'p', 0,0,0, 'e', 0,0,0, ' ', 0,0,0, 't', 0,0,0, 'o', 0,0,0, ' ', 0,0,0,
    'm', 0,0,0, 'e', 0,0,0, 'e', 0,0,0, 't', 0,0,0, ' ', 0,0,0, 'o', 0,0,0, 'n', 0,0,0, 'e', 0,0,0, '\x0a', 0,0,0, 'B', 0,0,0, 'u', 0,0,0, 't', 0,0,0, ' ', 0,0,0, 'j', 0,0,0,
    'u', 0,0,0, 'd', 0,0,0, 'g', 0,0,0, 'i', 0,0,0, 'n', 0,0,0, 'g', 0,0,0, ' ', 0,0,0, 'f', 0,0,0, 'r', 0,0,0, 'o', 0,0,0, 'm', 0,0,0, ' ', 0,0,0, 't', 0,0,0, 'h', 0,0,0, 'e', 0,0,0,
    ' ', 0,0,0, 'm', 0,0,0, 'i', 0,0,0, 'l', 0,0,0, 'k', 0,0,0, ' ', 0,0,0, 'w', 0,0,0, 'e', 0,0,0, ' ', 0,0,0, 'g', 0,0,0, 'e', 0,0,0, 't', 0,0,0, '\x0a', 0,0,0, 'T', 0,0,0,
    'h', 0,0,0, 'e', 0,0,0, 'r', 0,0,0, 'e', 0,0,0, ' ', 0,0,0, 'c', 0,0,0, 'e', 0,0,0, 'r', 0,0,0, 't', 0,0,0, 'a', 0,0,0, 'i', 0,0,0, 'n', 0,0,0, 'l', 0,0,0, 'y', 0,0,0, ' ', 0,0,0,
    'm', 0,0,0, 'u', 0,0,0, 's', 0,0,0, 't', 0,0,0, ' ', 0,0,0, 'b', 0,0,0, 'e', 0,0,0, ' ', 0,0,0, 'o', 0,0,0, 'n', 0,0,0, 'e', 0,0,0, '.', 0,0,0 };
    EXPECT_CALL(PathMock(), size(_)).WillRepeatedly(Return(sizeof(poem)));
    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(true));
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(true));
    EXPECT_CALL(HandleMock(), mockRead(_, _, _)).WillOnce(DoAll(
        SetArrayArgument<1>(begin(poem), end(poem)),
        Return(sizeof(poem)))).
        WillRepeatedly(DoAll(
            SetArrayArgument<1>(begin(poem), end(poem)),
            Return(sizeof(poem))));
    EXPECT_CALL(HandleMock(), mockSeek(_, _)).WillOnce(Return());
    EXPECT_CALL(HandleMock(), mockClose(_)).WillOnce(Return());

    auto path = FileSystemPath(problemFile);
    FileOfLinesCore f(path);
    f.open();

    EXPECT_STREQ("I've never met a purple cow", f.get(0).c_str());
    EXPECT_STREQ("I never hope to meet one", f.get(1).c_str());
    EXPECT_STREQ("But judging from the milk we get", f.get(2).c_str());
    EXPECT_STREQ("There certainly must be one.", f.get(3).c_str());
}
