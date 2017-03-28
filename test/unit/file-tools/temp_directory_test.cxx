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
// temp_directory_test.cxx -- declaration of a mock to FileSystemPath
//
///////////////////////////////////////////////////////////////////////////

#include <temp_directory.hxx>
#include <mock_file_system_path.hxx>
#include <mock_operating_system.hxx>
#include <file_path.hxx>
#include <gmock/gmock.h>
#include <regex>

using namespace std;
using namespace ansak;
using namespace testing;

namespace {

#if defined(WIN32)
FilePath tmpRoot("e:\\temp\\nonesuch");
string regexSrc(tmpRoot.asUtf8String() + "\\32960-[0-9]+\\guzzle");
#else
FilePath tmpRoot("/tmp/nonesuch");
string regexSrc(tmpRoot.asUtf8String() + "/32960-[0-9]+/guzzle");
#endif

}

namespace ansak {

// a never-used mock of the file system to satisfy static-initialization and validation
unique_ptr<OperatingSystemMock> primitive;
OperatingSystemMock* theMock = nullptr;

const OperatingSystemPrimitives* getOperatingSystemPrimitives()
{
    if (theMock == nullptr && !primitive)
    {
        // this will happen during static-initialization of the FileSystemPath class
        primitive.reset(new OperatingSystemMock());
    }
    return theMock == nullptr ? primitive.get() : theMock;
}

}

class TempDirectoryTestFixture : public Test
{
public:
    TempDirectoryTestFixture()
    {
        // important for the first call, unimportant otherwise
        primitive.reset(nullptr);
        theMock = &m_theOSMock;

        EXPECT_CALL(m_theMainMock, createDirectory(_)).WillOnce(Return(true));
        EXPECT_CALL(m_theOSMock, mockGetProcessID()).WillOnce(Return(32960));
        EXPECT_CALL(m_theOSMock, mockGetTempFilePath()).WillOnce(Return(tmpRoot));
    }
    ~TempDirectoryTestFixture()
    {
        theMock = nullptr;
    }

    void notDetaching()
    {
        EXPECT_CALL(m_theMainMock, remove(_, _)).WillOnce(Return(true));
    }

    FileSystemPathMock& FileSystemPath() { return m_theMainMock; }

private:
    FileSystemPathMock m_theMainMock;
    OperatingSystemMock m_theOSMock;
};

TEST_F(TempDirectoryTestFixture, detached)
{
    TempDirectoryPtr p = createTempDirectory();
    ASSERT_NE(nullptr, p.get());
    p->asFileSystemPath(TempDirectory::detachIt);
}

TEST_F(TempDirectoryTestFixture, attached)
{
    notDetaching();
    TempDirectoryPtr p = createTempDirectory();
    ASSERT_NE(nullptr, p.get());
    p->asFileSystemPath(TempDirectory::dontDetachIt);
}
