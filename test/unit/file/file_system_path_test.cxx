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
// 2017.03.21 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_system_path_test.cxx -- Tests for FileSystemPath in file_system_path.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <file_system_path.hxx>
#include <file_system_primitives.hxx>
#include <runtime_exception.hxx>
#include <mock_file_system.hxx>
#include <memory>

#include <gmock/gmock.h>

using namespace ansak;
using namespace std;
using namespace testing;

namespace ansak
{

// a never-used mock of the file system to satisfy static-initialization and validation
unique_ptr<FileSystemMock> primitive;
FileSystemMock* theMock = nullptr;

const FileSystemPrimitives* getPrimitives()
{
    if (theMock == nullptr && !primitive)
    {
        // this will happen during static-initialization of the FileSystemPath class
        primitive.reset(new FileSystemMock());
    }
    return theMock == nullptr ? primitive.get() : theMock;
}

extern bool primitivesExist;

#if defined(WIN32)

const char* homeDirP = "C:\\Users\\theUser";
// const char* foreignPathP = "/usr/sbin";

#else

const char* homeDirP = "/home/theUser";
// const char* foreignPathP = "C:\\Windows\\System32";

#endif

}

class FileSystemPathFixture : public Test {
public:
    FileSystemPathFixture()
    {
        theMock = &m_fileMock;
        EXPECT_CALL(m_fileMock, mockGetCwd()).WillRepeatedly(Return(FilePath(homeDirP)));
    }
    ~FileSystemPathFixture()
    {
        theMock = nullptr;
    }
    FileSystemMock& FileMock() { return m_fileMock; }

private:
    NiceMock<FileSystemMock>    m_fileMock;
};

TEST(FileSystemPathTest, fileSystemPathChecksPrimitives)
{
    ASSERT_TRUE(ansak::primitivesExist);
    primitive.reset(nullptr);
}

TEST_F(FileSystemPathFixture, defaultConstruct)
{
    FileSystemPath uut;

    EXPECT_CALL(FileMock(), mockPathExists(_)).WillOnce(Return(true));
    EXPECT_CALL(FileMock(), mockPathIsFile(_)).WillOnce(Return(false));
    EXPECT_CALL(FileMock(), mockPathIsDir(_)).WillOnce(Return(true));

    EXPECT_TRUE(uut.isValid());
    EXPECT_TRUE(!uut.isRelative());
    EXPECT_TRUE(!uut.isRoot());
    EXPECT_EQ(homeDirP, uut.asUtf8String());
    EXPECT_TRUE(uut.exists());
    EXPECT_TRUE(!uut.isFile());
    EXPECT_TRUE(uut.isDir());
}

TEST_F(FileSystemPathFixture, nonRelativeConstruct)
{
    FilePath home(homeDirP);
    FilePath fileThere = home.child("someFile");
    FileSystemPath uut(fileThere);

    EXPECT_CALL(FileMock(), mockPathExists(_)).WillOnce(Return(true));
    EXPECT_CALL(FileMock(), mockPathIsFile(_)).WillOnce(Return(true));
    EXPECT_CALL(FileMock(), mockPathIsDir(_)).WillOnce(Return(false));
    TimeStamp t { 1909, 6, 22, 10, 0, 35 };
    EXPECT_CALL(FileMock(), mockLastModTime(_)).WillOnce(Return(t));
    EXPECT_CALL(FileMock(), mockFileSize(fileThere)).WillOnce(Return(9909ul));

    EXPECT_TRUE(uut.isValid());
    EXPECT_TRUE(!uut.isRelative());
    EXPECT_TRUE(!uut.isRoot());
    EXPECT_EQ(fileThere.asUtf8String(), uut.asUtf8String());
    EXPECT_TRUE(uut.exists());
    EXPECT_TRUE(uut.isFile());
    EXPECT_TRUE(!uut.isDir());
    EXPECT_EQ(t, uut.lastModTime());
    EXPECT_EQ(9909ul, uut.size());
}

TEST_F(FileSystemPathFixture, stringConstruct)
{
    FilePath home(homeDirP);
    FilePath fileThere = home.child("someFile");
    FileSystemPath uut(fileThere.asUtf8String());

    EXPECT_TRUE(uut.isValid());
    EXPECT_TRUE(!uut.isRelative());
    EXPECT_TRUE(!uut.isRoot());
    EXPECT_EQ(fileThere.asUtf8String(), uut.asUtf8String());
}

TEST_F(FileSystemPathFixture, getParent)
{
    FilePath home(homeDirP);
    FilePath fileThere = home.child("someFile");
    FileSystemPath someFile(fileThere);
    FileSystemPath uut(someFile.parent());

    EXPECT_TRUE(uut.isValid());
    EXPECT_TRUE(!uut.isRelative());
    EXPECT_TRUE(!uut.isRoot());
    EXPECT_EQ(home.asUtf8String(), uut.asUtf8String());
}

TEST_F(FileSystemPathFixture, getChildren)
{
    FileSystemPath homeDirSP(homeDirP);
    EXPECT_CALL(FileMock(), mockPathExists(Eq(homeDirSP))).WillOnce(Return(true));
    EXPECT_CALL(FileMock(), mockPathIsDir(Eq(homeDirSP))).WillOnce(Return(true));
    FilePath child = FilePath(homeDirP).child("alpha");

    FileSystemPath::ChildrenRetriever r = FileSystemPath(homeDirP).children();
    auto listMock = dynamic_cast<DirectoryListMock*>(FileMock().m_lister);
    enforce(listMock != nullptr);
    EXPECT_CALL(*listMock, mockInvocation()).WillOnce(Return(FilePath(child))).WillOnce(Return(FilePath()));
    EXPECT_EQ(child, r());
    EXPECT_EQ(FilePath(), r());
}

TEST_F(FileSystemPathFixture, getFromEmptyRetriever)
{
    FileSystemPath homeDirSP(homeDirP);
    EXPECT_CALL(FileMock(), mockPathExists(Eq(homeDirSP))).WillOnce(Return(true));
    EXPECT_CALL(FileMock(), mockPathIsDir(Eq(homeDirSP))).WillOnce(Return(true));
    FilePath child = FilePath(homeDirP).child("alpha");

    FileSystemPath::ChildrenRetriever r = FileSystemPath(homeDirP).children();
    auto listMock = dynamic_cast<DirectoryListMock*>(FileMock().m_lister);
    enforce(listMock != nullptr);
    EXPECT_CALL(*listMock, mockInvocation()).WillOnce(Return(FilePath(child))).WillOnce(Return(FilePath()));
    auto s = move(r);
    EXPECT_EQ(child, s());
    EXPECT_EQ(FilePath(), s());
    EXPECT_EQ(FilePath(), r());
}
