///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, Arthur N. Klassen
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
// 2016.01.24 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_handle_test.cxx -- Tests for FileHandle in file_handle.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>

#include <file_handle.hxx>
#include <file_handle_exception.hxx>
#include <file_system_path.hxx>
#include <temp_directory.hxx>
#include <nullptr_exception.hxx>

namespace ansak {
    namespace internal {
        extern void resetThrowErrors();
    }
}

using namespace ansak;
using namespace std;
using namespace testing;

namespace {

static const string bigBadNastyMeanPath("../../../../../../../../../../../../big/bad/nasty/mean");

}

class ResetThrowOnExit {
public:
    ~ResetThrowOnExit() { ansak::internal::resetThrowErrors(); }
};

TEST(FileHandleTest, testCreate)
{
    auto tempDir = createTempDirectory();

    FileSystemPath where(tempDir->child("createTest"));
    char test0[] = "createTest";

    {
        FileHandle hNowhere;
        EXPECT_FALSE(hNowhere.isOpen());
    }

    EXPECT_FALSE(where.exists());
    {
        FileHandle hWhere(FileHandle::create(where));
        hWhere.write(test0, 10);
    }
    EXPECT_TRUE(where.exists());
}

TEST(FileHandleTest, testOpen)
{
    auto tempDir = createTempDirectory();

    FileSystemPath where(tempDir->child("openTest"));
    char test1[] = "Mikado-do-do";

    {
        FileHandle hWhere(FileHandle::create(where));
        hWhere.write(test1, 12);
    }
    EXPECT_TRUE(where.exists());
    {
        FileHandle hWhat = FileHandle::open(where);
        EXPECT_TRUE(hWhat.isOpen());
    }
}

TEST(FileHandleTest, testReadWrite)
{
    auto tempDir = createTempDirectory();
    FileSystemPath watto(tempDir->child("rw-test"));
    char test2[] = "3.1415926535897932384626433832795";

    {
        FileHandle hIn;

        hIn = FileHandle::create(watto);
        auto x = static_cast<int>(hIn.write(test2, 0));
        EXPECT_EQ(0, x);
        x = static_cast<int>(hIn.write(test2, sizeof(test2) - 1));
        EXPECT_EQ(static_cast<int>(sizeof(test2) - 1), x);
    }

    char readIn[20];
    {
        FileHandle hOut(FileHandle::open(watto));
        auto x = static_cast<int>(hOut.read(readIn, 0));
        EXPECT_EQ(0, x);
        x = static_cast<int>(hOut.read(readIn, sizeof(readIn)));
        EXPECT_EQ(20, x);
    }
    EXPECT_EQ('3', readIn[ 0]);
    EXPECT_EQ('.', readIn[ 1]);
    EXPECT_EQ('1', readIn[ 2]);
    EXPECT_EQ('4', readIn[ 3]);
    EXPECT_EQ('1', readIn[ 4]);
    EXPECT_EQ('5', readIn[ 5]);
    EXPECT_EQ('9', readIn[ 6]);
    EXPECT_EQ('2', readIn[ 7]);
    EXPECT_EQ('6', readIn[ 8]);
    EXPECT_EQ('5', readIn[ 9]);
    EXPECT_EQ('3', readIn[10]);
    EXPECT_EQ('5', readIn[11]);
    EXPECT_EQ('8', readIn[12]);
    EXPECT_EQ('9', readIn[13]);
    EXPECT_EQ('7', readIn[14]);
    EXPECT_EQ('9', readIn[15]);
    EXPECT_EQ('3', readIn[16]);
    EXPECT_EQ('2', readIn[17]);
    EXPECT_EQ('3', readIn[18]);
    EXPECT_EQ('8', readIn[19]);
}

TEST(FileHandleTest, testSeek)
{
    auto tempDir = createTempDirectory();
    FileSystemPath watto(tempDir->child("rw-test"));
    char test3[] = "3.1415926535897932384626433832795";

    {
        FileHandle hIn(FileHandle::create(watto));
        hIn.write(test3, sizeof(test3) - 1);
    }

    char readIn[5];
    {
        FileHandle hOut(FileHandle::open(watto));
        hOut.seek(10);
        auto x = static_cast<int>(hOut.read(readIn, sizeof(readIn)));
        EXPECT_EQ(5, x);
    }
    EXPECT_EQ('3', readIn[0]);
    EXPECT_EQ('5', readIn[1]);
    EXPECT_EQ('8', readIn[2]);
    EXPECT_EQ('9', readIn[3]);
    EXPECT_EQ('7', readIn[4]);
}

TEST(FileHandleTest, testMoveOpenHandle)
{
    auto tempDir = createTempDirectory();
    FileSystemPath movePath(tempDir->child("move-it"));

    FileHandle moveHandle(FileHandle::create(movePath));
    EXPECT_TRUE(moveHandle.isOpen());
    FileHandle moveResult(FileHandle::create(FileSystemPath(tempDir->child("drop-it"))));
    EXPECT_TRUE(moveResult.isOpen());
    moveResult = static_cast<FileHandle&&>(moveHandle);
    EXPECT_FALSE(moveHandle.isOpen());
    EXPECT_TRUE(moveResult.isOpen());
    moveResult.close();
    EXPECT_FALSE(moveResult.isOpen());
}

TEST(FileHandleTest, testThrowBadCreate)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;

    auto tempDir = createTempDirectory();
    FileSystemPath subName(tempDir->child("didnt-make-it"));
    FileSystemPath cantCreate(subName.child("cantCreate"));

    try
    {
        FileHandle fh(FileHandle::create(cantCreate));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path invalid, cannot be created"));
    }

    try
    {
        FileHandle works(FileHandle::create(subName));
        FileHandle tryAgain(FileHandle::create(subName, FileHandle::CreateType::kFailIfThere));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: file exists, create using kFailIfThere"));
    }

    FileHandle tryYetAgain(FileHandle::create(subName, FileHandle::CreateType::kOpenIfThere));
    EXPECT_TRUE(tryYetAgain.isOpen());
}

TEST(FileHandleTest, testThrowBadOpen)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;

    auto tempDir = createTempDirectory();
    FileSystemPath subName(tempDir->child("didnt-make-it"));
    FileSystemPath cantOpen(subName.child("cantCreate"));

    try
    {
        FileHandle fh(FileHandle::open(FileSystemPath(FilePath(bigBadNastyMeanPath))));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path invalid, cannot be opened"));
                    // StartsWith("FileHandleException: file does not exist, open using kFailIfNotThere"));
    }

    try
    {
        FileHandle fh(FileHandle::open(cantOpen));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: file does not exist, open using kFailIfNotThere"));
    }

    try
    {
        FileSystemPath tempPath(tempDir->asFileSystemPath());
        FileHandle fh(FileHandle::open(tempPath));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        string feWhat(fe.what());
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path is not a file, cannot be opened"));
    }

    FileHandle tryYetAgain(FileHandle::open(subName, FileHandle::OpenType::kCreateIfNotThere));
    EXPECT_TRUE(tryYetAgain.isOpen());
}

TEST(FileHandleTest, testThrowBadClose)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;

    FileHandle fh;
    try
    {
        fh.close();
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: close called when file not open"));
    }
}

TEST(FileHandleTest, testThrowBadSeek)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;

    FileHandle fh;
    try
    {
        fh.seek(3290);
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: seek called when file not open"));
    }

    FileHandle fThis = FileHandle::openForReading(FileSystemPath(__FILE__));
    try
    {
        fThis.seek(-2390);
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: seeking to position -2390 failed"));
    }
}

TEST(FileHandleTest, testThrowBadRead)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;


    try
    {
        FileHandle fh;
        fh.read(nullptr, 20);
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        string feWhat(fe.what());
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: read called when file not open"));
    }

    auto tempDir = createTempDirectory();
    FileSystemPath subName(tempDir->child("not-to-read-from"));

    try
    {
        FileHandle fh = FileHandle::create(subName);
        fh.read(nullptr, 20);
        ASSERT_TRUE(false);
    }
    catch (NullPtrException& np)
    {
        EXPECT_THAT(string(np.what()), StartsWith("Unexpected Nullptr in function, "));
    }
}

TEST(FileHandleTest, testThrowBadWrite)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;


    try
    {
        FileHandle fh;
        fh.write(nullptr, 20);
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        string feWhat(fe.what());
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: write called when file not open"));
    }

    auto tempDir = createTempDirectory();
    FileSystemPath subName(tempDir->child("not-to-write-to"));

    try
    {
        FileHandle fh = FileHandle::create(subName);
        fh.write(nullptr, 20);
        ASSERT_TRUE(false);
    }
    catch (NullPtrException& np)
    {
        EXPECT_THAT(string(np.what()), StartsWith("Unexpected Nullptr in function, "));
    }
}

TEST(FileHandleTest, testOpenForReadingWithThrow)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;

    FileSystemPath here(__FILE__);
    {
        FileHandle readMe(FileHandle::openForReading(here));
    }

    try
    {
        FileHandle fh(FileHandle::openForReading(FileSystemPath(FilePath(bigBadNastyMeanPath))));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path invalid, cannot be opened for reading"));
    }

    try
    {
        FilePath notSubOfHere(here.child("there"));
        FileHandle dontReadMe(FileHandle::openForReading(FileSystemPath(notSubOfHere)));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        string feWhat(fe.what());
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path does not exist, cannot be opened for reading"));
    }

    try
    {
        FilePath parent(here.parent());
        FileHandle cantReadDir(FileHandle::openForReading(FileSystemPath(parent)));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path is not a file, cannot be opened for reading"));
    }
}

TEST(FileHandleTest, testOpenForAppendingWithThrow)
{
    FileHandle::throwErrors();
    ResetThrowOnExit rtoe;

    auto tempDir = createTempDirectory();
    FileSystemPath where(tempDir->child("createTest"));
    FileSystemPath here(tempDir->child("crateTest"));
    char test0[] = "createTest";

    {
        FileHandle hWhere(FileHandle::create(where));
        hWhere.write(test0, 10);
    }

    {
        FileHandle hWhere(FileHandle::openForAppending(where));
        hWhere.write(test0, 10);
    }

    try
    {
        FileHandle hWhere(FileHandle::openForAppending(FileSystemPath(FilePath(bigBadNastyMeanPath))));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path invalid, cannot be opened for appending"));
    }

    try
    {
        FileHandle hWhere(FileHandle::openForAppending(FileSystemPath(),
                                                       FileHandle::OpenType::kFailIfNotThere));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: path is not a file, cannot be opened for appending"));
    }

    try
    {
        FileHandle hWhere(FileHandle::openForAppending(here, FileHandle::OpenType::kFailIfNotThere));
        ASSERT_TRUE(false);
    }
    catch (FileHandleException& fe)
    {
        EXPECT_THAT(string(fe.what()),
                    StartsWith("FileHandleException: file does not exist, openForAppending using kFailIfNotThere"));
    }
}

TEST(FileHandleTest, testBadCreate)
{
    auto tempDir = createTempDirectory();
    FileSystemPath subName(tempDir->child("didnt-make-it"));
    FileSystemPath cantCreate(subName.child("cantCreate"));
    FileHandle nullH;

    {
        FileHandle fh(FileHandle::create(cantCreate));
        EXPECT_EQ(fh, FileHandle());
    }

    {
        FileHandle works(FileHandle::create(subName));
        FileHandle tryAgain(FileHandle::create(subName, FileHandle::CreateType::kFailIfThere));
        EXPECT_EQ(tryAgain, FileHandle());
    }
}

TEST(FileHandleTest, testBadOpen)
{
    auto tempDir = createTempDirectory();
    FileSystemPath subName(tempDir->child("didnt-make-it"));
    FileSystemPath cantOpen(subName.child("cantCreate"));

    {
        FileHandle fh(FileHandle::open(FileSystemPath(FilePath(bigBadNastyMeanPath))));
        EXPECT_EQ(fh, FileHandle());
    }

    {
        FileHandle fh(FileHandle::open(cantOpen));
        EXPECT_EQ(fh, FileHandle());
    }

    {
        FileSystemPath tempPath(tempDir->asFileSystemPath());
        FileHandle fh(FileHandle::open(tempPath));
        EXPECT_EQ(fh, FileHandle());
    }
}

TEST(FileHandleTest, testBadClose)
{
    FileHandle fh;
    fh.close();
}

TEST(FileHandleTest, testBadSeek)
{
    FileHandle fh;
    fh.seek(3290);
}

TEST(FileHandleTest, testBadRead)
{
    {
        FileHandle fh;
        EXPECT_EQ(static_cast<size_t>(0), fh.read(nullptr, 20));
    }

    auto tempDir = createTempDirectory();
    {
        FileSystemPath subName(tempDir->child("not-to-read-from"));
        FileHandle fh = FileHandle::create(subName);

        EXPECT_EQ(static_cast<size_t>(0), fh.read(nullptr, 20));
    }
}

TEST(FileHandleTest, testBadWrite)
{
    {
        FileHandle fh;
        EXPECT_EQ(static_cast<size_t>(0), fh.write(nullptr, 20));
    }

    auto tempDir = createTempDirectory();
    FileSystemPath subName(tempDir->child("not-to-write-to"));
    {
        FileHandle fh = FileHandle::create(subName);
        EXPECT_EQ(static_cast<size_t>(0), fh.write(nullptr, 20));
    }
}

TEST(FileHandleTest, testOpenForReading)
{
    FileSystemPath here(__FILE__);
    {
        FileHandle readMe(FileHandle::openForReading(here));
        EXPECT_NE(readMe, FileHandle());
    }

    FileHandle fh(FileHandle::openForReading(FileSystemPath(FilePath(bigBadNastyMeanPath))));
    EXPECT_EQ(fh, FileHandle());

    FilePath notSubOfHere(here.child("there"));
    FileHandle dontReadMe(FileHandle::openForReading(FileSystemPath(notSubOfHere)));
    EXPECT_EQ(dontReadMe, FileHandle());

    FilePath parent(here.parent());
    FileHandle cantReadDir(FileHandle::openForReading(FileSystemPath(parent)));
    EXPECT_EQ(cantReadDir, FileHandle());
}

TEST(FileHandleTest, testOpenForAppending)
{
    auto tempDir = createTempDirectory();
    FileSystemPath where(tempDir->child("createTest"));
    FileSystemPath here(tempDir->child("crateTest"));
    char test0[] = "createTest";

    {
        FileHandle hWhere(FileHandle::create(where));
        hWhere.write(test0, 10);
    }

    {
        FileHandle hWhere(FileHandle::openForAppending(where));
        EXPECT_NE(hWhere, FileHandle());
        EXPECT_EQ(static_cast<size_t>(10), hWhere.write(test0, 10));
    }

    {
        FileHandle hWhere(FileHandle::openForAppending(FileSystemPath(FilePath(bigBadNastyMeanPath))));
        EXPECT_EQ(hWhere, FileHandle());
    }

    {
        FileHandle hWhere(FileHandle::openForAppending(FileSystemPath(),
                                                       FileHandle::OpenType::kFailIfNotThere));
        EXPECT_EQ(hWhere, FileHandle());
    }

    {
        FileHandle hWhere(FileHandle::openForAppending(here, FileHandle::OpenType::kFailIfNotThere));
        EXPECT_EQ(hWhere, FileHandle());
    }

    {
        FileHandle hWhere(FileHandle::openForAppending(here, FileHandle::OpenType::kCreateIfNotThere));
        EXPECT_NE(hWhere, FileHandle());
        const char now[] = "Now is the time for all good men to come to the aid of the party.";
        hWhere.write(now, sizeof(now) - 1);
    }

    FileSystemPath whereAfter(FilePath(tempDir->asFileSystemPath().child("crateTest")));
    EXPECT_TRUE(whereAfter.exists());
    EXPECT_TRUE(whereAfter.isFile());
}

TEST(FileHandleTest, testSetPermission)
{
#if !defined(WIN32)
    auto tempDir = createTempDirectory();
    FileSystemPath where(tempDir->child("permTest"));
    FileHandle::setPermission(0400);
    auto hWhere = FileHandle::create(where);
    EXPECT_NE(hWhere, FileHandle());
#endif
}

TEST(FileHandleTest, testFileException)
{
    FileSystemPath here(__FILE__);
    FileHandleException fEx0(here, 0, "Test Exception 1");
    FileHandleException fEx1(here, 5, "Test Exception 1");

    EXPECT_THAT(string(fEx0.what()), StartsWith("FileHandleException: "));
    EXPECT_THAT(string(fEx1.what()), StartsWith("FileHandleException: "));
}

TEST(FileHandleTest, testCopyFrom)
{
    FileSystemPath here(__FILE__);
    FileHandle hHere(FileHandle::openForReading(here));
    auto tempDir(createTempDirectory());

    {
        FileHandle hThere0;
        EXPECT_EQ(static_cast<size_t>(0), hThere0.copyFrom(hHere));
        EXPECT_EQ(static_cast<size_t>(0), hHere.copyFrom(hThere0));

        hThere0 = FileHandle::create(tempDir->child("there0"));
        EXPECT_EQ(static_cast<size_t>(0), hThere0.copyFrom(hHere, 0, 0));
        EXPECT_EQ(static_cast<size_t>(100), hThere0.copyFrom(hHere, 20, 100));
        // should have copied from 20-chars in for 100 chars
    }

    size_t stuff0, fetchStuff, fetch0, fetch1;
    {
        FileHandle hThere1 = FileHandle::create(tempDir->child("there1"));
        stuff0 = hThere1.copyFrom(hHere);
        EXPECT_GT(stuff0, 20000u);
        // should have copied from 120-chars in to end of file

        hThere1.close();
        fetchStuff = static_cast<size_t>(hThere1.size());
    }

    {
        FileHandle hThere2 = FileHandle::openForReading(tempDir->child("there1"));
        fetch0 = static_cast<size_t>(hThere2.size());
        EXPECT_GT(stuff0, 20000u);
    }
    FileSystemPath there1(tempDir->child("there1"));
    fetch1 = static_cast<size_t>(there1.size());
    EXPECT_EQ(stuff0, fetchStuff);
    EXPECT_EQ(stuff0, fetch1);
    EXPECT_EQ(fetch0, fetch1);
}

