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
// 2017.03.28 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// text_file_utilities_test.cxx -- unit test of the is-it-text? detecter
//
///////////////////////////////////////////////////////////////////////////

#include <text_file_utilities.hxx>
#include <mock_file_handle_exception.hxx>
#include <file_of_lines_exception.hxx>
#include <nullptr_exception.hxx>

#include <gmock/gmock.h>
#include <mock_file_system_path.hxx>
#include <mock_file_handle.hxx>
#include <iterator>

using namespace std;
using namespace ansak;
using namespace ansak::file;
using namespace testing;

namespace {

#if defined(WIN32)
FilePath validFile("c:\\Users\\jvplasmeuden\\homeworklines.txt");
FilePath invalidFile("/home/jvplasmeuden/homeworklines.txt");
#else
FilePath validFile("/home/jvplasmeuden/homeworklines.txt");
FilePath invalidFile("c:\\Users\\jvplasmeuden\\homeworklines.txt");
#endif

}

class FileIsTextFixture : public Test
{
public:
    FileIsTextFixture()
    {
    }
    ~FileIsTextFixture()
    {
    }

    void setFileIsGood()
    {
        EXPECT_CALL(m_thePathMock, exists(_)).WillRepeatedly(Return(true));
        EXPECT_CALL(m_thePathMock, isFile(_)).WillRepeatedly(Return(true));
    }

    FileSystemPathMock& PathMock() { return m_thePathMock; }
    FileHandleMock& HandleMock() { return m_theHandleMock; }
    FileHandleExceptionMock& ExceptionMock() { return m_theExceptionMock; };

private:
    FileSystemPathMock      m_thePathMock;
    FileHandleMock          m_theHandleMock;
    FileHandleExceptionMock m_theExceptionMock;
};

TEST(TextFileUtilitiesTest, testThrow)
{
    EXPECT_THROW(bufferIsText(0, 35), NullPtrException);
}

TEST(TextFileUtilitiesTest, testEmpty)
{
    const char noMessage[] = "";
    EXPECT_TRUE(bufferIsText(noMessage, 0));
}

TEST(TextFileUtilitiesTest, testEarlyOut)
{
    const char partBOMLE[] = "\xFF\x00\x00";
    EXPECT_FALSE(bufferIsText(partBOMLE, 3));
    const char partBOMBE[] = "\x00\x00\xFE";
    EXPECT_TRUE(bufferIsText(partBOMBE, 3)); // looks like early null-termination
    const char controlChars[] = "\x01\x02\x03\x04";
    EXPECT_FALSE(bufferIsText(controlChars, 4));
}

TEST(TextFileUtilitiesTest, testShortBinaries)
{
    char b0[] = { 'T', 'h', 'a', 20 };
    EXPECT_FALSE(bufferIsText(b0, sizeof(b0)));
    char b1[] = { -1, -2, 'T', 0, 'h', 0, 'i', 0, 's', 0, 3, 0 };
    EXPECT_FALSE(bufferIsText(b1, sizeof(b1)));
    char b2[] = { -2, -1, 0, 'T', 0, 'h', 0, 'i', 0, 's', 0, 3 };
    EXPECT_FALSE(bufferIsText(b2, sizeof(b2)));
    char b3[] = { -1, -2, 0, 0, 'T', 0, 0, 0, 'h', 0, 0, 0, 'i', 0, 0, 0,
                  's', 0, 0, 0, 3, 0, 0, 0 };
    EXPECT_FALSE(bufferIsText(b3, sizeof(b3)));
    char b4[] = { 0, 0, -2, -1, 0, 0, 0, 'T', 0, 0, 0, 'h', 0, 0, 0, 'i',
                  0, 0, 0, 's', 0, 0, 0, 3 };
    EXPECT_FALSE(bufferIsText(b4, sizeof(b4)));
}

TEST(TextFileUtilitiesTest, testShortBOMLess)
{
    char ascii7[] = "Now is the time for all good men to come to the aid of the "
                    "party.";
    ansak::file::TextType t;
    EXPECT_TRUE(bufferIsText(ascii7, sizeof(ascii7) - 1, &t));
    EXPECT_EQ(ansak::file::kUtf8, t);

    char localText[] = "Now is the time for all good men to come to the \x85id of "
                       "the party.";
    EXPECT_TRUE(bufferIsText(localText, sizeof(localText) - 1, &t));
    EXPECT_EQ(ansak::file::kLocalText, t);
}

TEST(TextFileUtilitiesTest, testWithUtf8BOM)
{
    char utf8[] = "\xBF\xEF\xEFNow is the time for all good men to "
                  "come to the \xc3\xa4id of the party.";
    ansak::file::TextType t;
    EXPECT_TRUE(bufferIsText(utf8, sizeof(utf8) - 1, &t));
    EXPECT_EQ(ansak::file::kUtf8, t);

    char utf8Error[] = "\xBF\xEF\xEFNow is the time for all good men to "
                       "come to the \xe4id of the party.";
    EXPECT_FALSE(bufferIsText(utf8Error, sizeof(utf8Error) - 1));
}

TEST(TextFileUtilitiesTest, testWithUtf16LEBOM)
{
    char utf16[] = "\xff\xfeN\0o\0w\0 \0i\0s\0 \0t\0h\0e\0 \0t\0i\0m\0e\0 "
                   "\0f\0o\0r\0 \0a\0l\0l\0 \0g\0o\0o\0d\0 \0m\0e\0n\0 \0t\0o\0 "
                   "\0c\0o\0m\0e\0 \0t\0o\0 \0t\0h\0e\0 \0\xe4\0i\0d\0 \0o\0f\0 "
                   "\0t\0h\0e\0 \0p\0a\0r\0t\0y\0.\0";
    ansak::file::TextType t;
    EXPECT_TRUE(bufferIsText(utf16, sizeof(utf16) - 1, &t));
    EXPECT_EQ(ansak::file::kUtf16LE, t);

    char utf16Error[] = "\xff\xfeN\0o\0w\0 \0i\0s\0 \0t\0h\0e\0 \0t\0i\0m\0e\0 "
                        "\0f\0o\0r\0 \0a\0l\0l\0 \0g\0o\0o\0d\0 \0m\0e\0n\0 "
                        "\0t\0o\0 \0c\0o\0m\0e\0 \0t\0o\0 \0t\0h\0e\0 "
                        "\0\xe4\xdci\0d\0 \0o\0f\0 \0t\0h\0e\0 "
                        "\0p\0a\0r\0t\0y\0.\0";
    EXPECT_FALSE(bufferIsText(utf16Error, sizeof(utf16Error) - 1));
}

TEST(TextFileUtilitiesTest, testWithUtf16BEBOM)
{
    char utf16[] = "\xfe\xff\0N\0o\0w\0 \0i\0s\0 \0t\0h\0e\0 \0t\0i\0m\0e\0 "
                   "\0f\0o\0r\0 \0a\0l\0l\0 \0g\0o\0o\0d\0 \0m\0e\0n\0 \0t\0o\0 "
                   "\0c\0o\0m\0e\0 \0t\0o\0 \0t\0h\0e\0 \0\xe4\0i\0d\0 \0o\0f\0 "
                   "\0t\0h\0e\0 \0p\0a\0r\0t\0y\0.";
    ansak::file::TextType t;
    EXPECT_TRUE(bufferIsText(utf16, sizeof(utf16) - 1, &t));
    EXPECT_EQ(ansak::file::kUtf16BE, t);

    char utf16Error[] = "\xfe\xff\0N\0o\0w\0 \0i\0s\0 \0t\0h\0e\0 \0t\0i\0m\0e\0 "
                        "\0f\0o\0r\0 \0a\0l\0l\0 \0g\0o\0o\0d\0 \0m\0e\0n\0 "
                        "\0t\0o\0 \0c\0o\0m\0e\0 \0t\0o\0 \0t\0h\0e\0 "
                        "\xdc\xe4\0i\0d\0 \0o\0f\0 \0t\0h\0e\0 \0p\0a\0r\0t\0y\0.";
    EXPECT_FALSE(bufferIsText(utf16Error, sizeof(utf16Error) - 1));
}

TEST(TextFileUtilitiesTest, testWithUcs4LEBOM)
{
    char ucs4[] = "\xff\xfe\0\0N\0\0\0o\0\0\0w\0\0\0 \0\0\0i\0\0\0s\0\0\0 "
                  "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0t\0\0\0i\0\0\0m\0\0\0e\0\0\0 "
                  "\0\0\0f\0\0\0o\0\0\0r\0\0\0 \0\0\0a\0\0\0l\0\0\0l\0\0\0 "
                  "\0\0\0g\0\0\0o\0\0\0o\0\0\0d\0\0\0 \0\0\0m\0\0\0e\0\0\0n\0\0\0 "
                  "\0\0\0t\0\0\0o\0\0\0 \0\0\0c\0\0\0o\0\0\0m\0\0\0e\0\0\0 "
                  "\0\0\0t\0\0\0o\0\0\0 \0\0\0t\0\0\0h\0\0\0e\0\0\0 "
                  "\0\0\0\xe4\0\0\0i\0\0\0d\0\0\0 \0\0\0o\0\0\0f\0\0\0 "
                  "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0p\0\0\0a\0\0\0r\0\0\0t\0\0\0y"
                  "\0\0\0.\0\0\0";
    ansak::file::TextType t;
    EXPECT_TRUE(bufferIsText(ucs4, sizeof(ucs4) - 1, &t));
    EXPECT_EQ(ansak::file::kUcs4LE, t);

    char ucs4Error[] = "\xff\xfe\0\0N\0\0\0o\0\0\0w\0\0\0 \0\0\0i\0\0\0s\0\0\0 "
                       "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0t\0\0\0i\0\0\0m\0\0\0e\0\0\0 "
                       "\0\0\0f\0\0\0o\0\0\0r\0\0\0 \0\0\0a\0\0\0l\0\0\0l\0\0\0 "
                       "\0\0\0g\0\0\0o\0\0\0o\0\0\0d\0\0\0 \0\0\0m\0\0\0e\0\0\0n\0\0\0 "
                       "\0\0\0t\0\0\0o\0\0\0 \0\0\0c\0\0\0o\0\0\0m\0\0\0e\0\0\0 "
                       "\0\0\0t\0\0\0o\0\0\0 \0\0\0t\0\0\0h\0\0\0e\0\0\0 "
                       "\0\0\0\xe4\xdc\0\0i\0\0\0d\0\0\0 \0\0\0o\0\0\0f\0\0\0 "
                       "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0p\0\0\0a\0\0\0r\0\0\0t"
                       "\0\0\0y\0\0\0.\0\0\0";
    EXPECT_FALSE(bufferIsText(ucs4Error, sizeof(ucs4Error) - 1));
}

TEST(TextFileUtilitiesTest, testWithUcs4BEBOM)
{
    char ucs4[] = "\0\0\xfe\xff\0\0\0N\0\0\0o\0\0\0w\0\0\0 \0\0\0i\0\0\0s\0\0\0 "
                  "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0t\0\0\0i\0\0\0m\0\0\0e\0\0\0 "
                  "\0\0\0f\0\0\0o\0\0\0r\0\0\0 \0\0\0a\0\0\0l\0\0\0l\0\0\0 "
                  "\0\0\0g\0\0\0o\0\0\0o\0\0\0d\0\0\0 \0\0\0m\0\0\0e\0\0\0n\0\0\0 "
                  "\0\0\0t\0\0\0o\0\0\0 \0\0\0c\0\0\0o\0\0\0m\0\0\0e\0\0\0 "
                  "\0\0\0t\0\0\0o\0\0\0 \0\0\0t\0\0\0h\0\0\0e\0\0\0 "
                  "\0\0\0\xe4\0\0\0i\0\0\0d\0\0\0 \0\0\0o\0\0\0f\0\0\0 "
                  "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0p\0\0\0a\0\0\0r\0\0\0t\0\0\0"
                  "y\0\0\0.";
    ansak::file::TextType t;
    EXPECT_TRUE(bufferIsText(ucs4, sizeof(ucs4) - 1, &t));
    EXPECT_EQ(ansak::file::kUcs4BE, t);

    char ucs4Error[] = "\0\0\xfe\xff\0\0\0N\0\0\0o\0\0\0w\0\0\0 \0\0\0i\0\0\0s\0\0\0 "
                       "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0t\0\0\0i\0\0\0m\0\0\0e\0\0\0 "
                       "\0\0\0f\0\0\0o\0\0\0r\0\0\0 \0\0\0a\0\0\0l\0\0\0l\0\0\0 "
                       "\0\0\0g\0\0\0o\0\0\0o\0\0\0d\0\0\0 \0\0\0m\0\0\0e\0\0\0n\0\0\0 "
                       "\0\0\0t\0\0\0o\0\0\0 \0\0\0c\0\0\0o\0\0\0m\0\0\0e\0\0\0 "
                       "\0\0\0t\0\0\0o\0\0\0 \0\0\0t\0\0\0h\0\0\0e\0\0\0 "
                       "\0\0\xdc\xe4\0\0\0i\0\0\0d\0\0\0 \0\0\0o\0\0\0f\0\0\0 "
                       "\0\0\0t\0\0\0h\0\0\0e\0\0\0 \0\0\0p\0\0\0a\0\0\0r\0\0\0t\0\0\0y"
                       "\0\0\0.";
    EXPECT_FALSE(bufferIsText(ucs4Error, sizeof(ucs4Error) - 1));
}

TEST(TextFileUtilitiesTest, testJunk)
{
    char badLEbom[27] = "\xff\xf2This will never be read.";
    EXPECT_FALSE(bufferIsText(badLEbom, 27));

    char hasControls[6] = { 1,2,3,4,5,6 };
    EXPECT_FALSE(bufferIsText(hasControls, 6));
}

TEST_F(FileIsTextFixture, invalidFiles)
{
    EXPECT_THROW(looksLikeText(invalidFile), FileOfLinesException);

    EXPECT_CALL(PathMock(), exists(_)).WillOnce(Return(false)).WillRepeatedly(Return(true));
    EXPECT_THROW(looksLikeText(validFile), FileOfLinesException);
    EXPECT_CALL(PathMock(), isFile(_)).WillOnce(Return(false));
    EXPECT_THROW(looksLikeText(validFile), FileOfLinesException);
}

TEST_F(FileIsTextFixture, emptyFile)
{
    setFileIsGood();
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(0u));

    EXPECT_FALSE(looksLikeText(validFile));
}

TEST_F(FileIsTextFixture, readFailed)
{
    setFileIsGood();
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(12930u));
    EXPECT_CALL(HandleMock(), mockRead(_,_,_)).WillOnce(Return(0u));

    EXPECT_THROW(looksLikeText(validFile), FileOfLinesException);
}

TEST_F(FileIsTextFixture, readThrows)
{
    setFileIsGood();
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(12930u));
    EXPECT_CALL(HandleMock(), mockRead(_,_,_)).WillOnce(Throw(FileHandleException(validFile, 35, "Can't read this time.")));
    EXPECT_CALL(ExceptionMock(), mockWhat(_)).WillOnce(Return("This is a test."));

    EXPECT_THROW(looksLikeText(validFile), FileOfLinesException);
}

TEST_F(FileIsTextFixture, textIsTested)
{
    setFileIsGood();
    char testText[] = "This is twenty chars";
    auto allButNull = sizeof(testText) - 1;
    EXPECT_CALL(PathMock(), size(_)).WillOnce(Return(allButNull));
    EXPECT_CALL(HandleMock(), mockRead(_,_,_)).
        WillOnce(DoAll(SetArrayArgument<1>(begin(testText), begin(testText) + allButNull),
                       Return(allButNull)));

    EXPECT_TRUE(looksLikeText(validFile));
}
