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
// file_of_lines_exception_test.cxx -- unit test for file-of-lines exceptions
//
///////////////////////////////////////////////////////////////////////////

#include <file_of_lines_exception.hxx>

#include <file_path.hxx>
#include <file_handle_exception.hxx>
#include <mock_operating_system.hxx>
#include <mock_file_handle_exception.hxx>
#include <memory>

#include <gtest/gtest.h>

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

class FileOfLinesExceptionTestFixture : public Test
{
public:
    FileOfLinesExceptionTestFixture()
    {
        // important for the first call, unimportant otherwise
        primitive.reset(nullptr);
        theMock = &m_theOSMock;
    }

    ~FileOfLinesExceptionTestFixture()
    {
        theMock = nullptr;
    }

    OperatingSystemMock& OSMock() { return m_theOSMock; }
    FileHandleExceptionMock& ExceptionMock() { return m_exceptionMock; }

private:
    OperatingSystemMock m_theOSMock;
    FileHandleExceptionMock m_exceptionMock;
};

TEST(FileOfLinesException, inComesOut)
{
    FileOfLinesException a("this is a test");
    EXPECT_STREQ(a.what(), "FileOfLinesException: this is a test.");
    FileOfLinesException b("this is another test", problemFile);
    string message("FileOfLinesException: this is another test; file = ");
    message += problemFile.asUtf8String();
    message += '.';
    EXPECT_STREQ(b.what(), message.c_str());
}


TEST_F(FileOfLinesExceptionTestFixture, withErrorCode)
{
    FileOfLinesException c("this is another test", problemFile, 35);
    string message("FileOfLinesException: this is another test; file = ");
    message += problemFile.asUtf8String();
    message += "; error occurred at or near offset 35.";
    EXPECT_STREQ(c.what(), message.c_str());
}

TEST_F(FileOfLinesExceptionTestFixture, fromFileHandleException)
{
    EXPECT_CALL(ExceptionMock(), mockWhat(_)).WillOnce(Return("MockedUp FileHandleException"));
    FileHandleException dEx(problemFile, 35, "MockedUp FileHandleException");
    FileOfLinesException d("this test wraps a file handle exception", problemFile, dEx);
    string message("FileOfLinesException: this test wraps a file handle exception; file = ");
    message += problemFile.asUtf8String();
    message += "; exception = (MockedUp FileHandleException).";
    EXPECT_STREQ(d.what(), message.c_str());
}

TEST_F(FileOfLinesExceptionTestFixture, fromFileHandleExceptionWithOffset)
{
    EXPECT_CALL(ExceptionMock(), mockWhat(_)).WillOnce(Return("MockedUp FileHandleException"));
    FileHandleException dEx(problemFile, 35, "MockedUp FileHandleException");
    FileOfLinesException e("this test wraps a file handle exception", problemFile, dEx, 98022ull);
    string message("FileOfLinesException: this test wraps a file handle exception; file = ");
    message += problemFile.asUtf8String();
    message += "; exception = (MockedUp FileHandleException); error occurred at or near offset 98022.";
    EXPECT_STREQ(e.what(), message.c_str());
}

TEST_F(FileOfLinesExceptionTestFixture, fromFileHandleExceptionWithOffsetAndLineNum)
{
    EXPECT_CALL(ExceptionMock(), mockWhat(_)).WillOnce(Return("MockedUp FileHandleException"));
    FileHandleException dEx(problemFile, 35, "MockedUp FileHandleException");
    FileOfLinesException f("this test wraps a file handle exception", problemFile, dEx, 98022ull);
    string message("FileOfLinesException: this test wraps a file handle exception; file = ");
    message += problemFile.asUtf8String();
    message += "; exception = (MockedUp FileHandleException); error occurred at or near offset 98022.";
    EXPECT_STREQ(f.what(), message.c_str());
}
