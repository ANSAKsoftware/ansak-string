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
// 2017.03.24 - First Version
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

#include <file_handle.hxx>
#include <file_handle_exception.hxx>
#include <nullptr_exception.hxx>
#include <mock_operating_system.hxx>

#include <gmock/gmock.h>

using namespace ansak;
using namespace std;
using namespace testing;

namespace ansak
{

extern OperatingSystemMock* theMock;

namespace internal
{

extern void resetThrowErrors();

}

}

namespace
{

#if defined(WIN32)

const char* homeDirP = "C:\\Users\\theUser";
const char* someFileP = "C:\\Users\\theUser\\someFile";
const char* srcFileP = "C:\\Users\\theUser\\source";
const char* foreignP = "/home/theUser/someFile";

#else

const char* homeDirP = "/home/theUser";
const char* someFileP = "/home/theUser/someFile";
const char* srcFileP = "/home/theUser/source";
const char* foreignP = "C:\\Users\\theUser\\someFile";

#endif

}

class FileHandleFixture : public Test {
public:
    FileHandleFixture()
    {
        theMock = &m_osMock;
        EXPECT_CALL(m_osMock, mockGetCwd()).WillRepeatedly(Return(FilePath(homeDirP)));
    }
    ~FileHandleFixture()
    {
        theMock = nullptr;
        ansak::internal::resetThrowErrors();
    }
    OperatingSystemMock& OSMock() { return m_osMock; }

private:
    NiceMock<OperatingSystemMock>    m_osMock;
};

class OpenFileHandleFixture : public FileHandleFixture {
public:
    OpenFileHandleFixture() : FileHandleFixture()
    {
        auto& mock = OSMock();
        EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
        EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(false));
        EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillOnce(Return(123010ull));
        m_fh = FileHandle::create(FileSystemPath(someFileP));
    }

    FileHandle& uut() { return m_fh; }

private:
    FileHandle m_fh;
};

class CopyFileHandleFixture : public FileHandleFixture {
public:
    CopyFileHandleFixture() : FileHandleFixture()
    {
        auto& mock = OSMock();
        EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
        EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(false));
        EXPECT_CALL(mock, mockPathExists(Eq(FilePath(srcFileP)))).WillOnce(Return(true));
        EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(srcFileP)))).WillOnce(Return(true));
        EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillOnce(Return(123010ull));
        EXPECT_CALL(mock, mockOpen(Eq(FilePath(srcFileP)), _, _, _)).WillOnce(Return(123011ull));
        m_fhDest = FileHandle::create(FileSystemPath(someFileP));
        m_fhSrc = FileHandle::open(FileSystemPath(srcFileP));
    }

    FileHandle& uutSrc() { return m_fhSrc; }
    FileHandle& uutDest() { return m_fhDest; }

private:
    FileHandle m_fhSrc;
    FileHandle m_fhDest;
};

TEST_F(FileHandleFixture, defaultConstruct)
{
    try
    {
        FileHandle x;
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(true);
}

TEST_F(CopyFileHandleFixture, breakCloseOnMoveAssignment)
{
    EXPECT_CALL(OSMock(), mockClose(_,_)).WillRepeatedly(SetArgReferee<1>(300u));
    FileHandle::throwErrors();
    uutSrc() = move(uutDest());
}

TEST_F(FileHandleFixture, createSimple)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(false));
    EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::create(fsp);
    EXPECT_NE(FileHandle(), fh);
}

TEST_F(FileHandleFixture, closeThrowsInDestructor)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(false));
    EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::create(fsp);
    EXPECT_NE(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_CALL(mock, mockClose(Eq(123010ull), _)).WillOnce(SetArgReferee<1>(333u));
}

TEST_F(FileHandleFixture, createFailOnInvalidPath)
{
    auto fsp = FileSystemPath(foreignP);
    auto fh = FileHandle::create(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::create(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, createFailOnInvalidParent)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillRepeatedly(Return(false));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::create(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::create(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, createFailIfThere)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::create(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::create(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, createSucceedIfThere)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockOpen(Eq(FilePath(someFileP)), _, _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::create(fsp, FileHandle::CreateType::kOpenIfThere);
    EXPECT_NE(FileHandle(), fh);
}

TEST_F(FileHandleFixture, createSimpleFailed)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillRepeatedly(Return(~0ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::create(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::create(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openSimple)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockOpen(Eq(FilePath(someFileP)), _, _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::open(fsp);
    EXPECT_NE(FileHandle(), fh);
}

TEST_F(FileHandleFixture, openFailOnInvalidPath)
{
    auto fsp = FileSystemPath(foreignP);
    auto fh = FileHandle::open(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::open(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openFailIfNotThere)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::open(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::open(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openFailIfNotFile)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::open(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::open(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openSucceedIfNotThere)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::open(fsp, FileHandle::OpenType::kCreateIfNotThere);
    EXPECT_NE(FileHandle(), fh);
}

TEST_F(FileHandleFixture, openSimpleFailed)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockOpen(Eq(FilePath(someFileP)), _, _, _)).WillRepeatedly(Return(~0ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::open(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::open(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openForReadingSimple)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockOpen(Eq(FilePath(someFileP)), _, _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForReading(fsp);
    EXPECT_NE(FileHandle(), fh);
}

TEST_F(FileHandleFixture, openForReadingSimpleFailed)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockOpen(Eq(FilePath(someFileP)), _, _, _)).WillRepeatedly(Return(~0ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForReading(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForReading(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openForReadingFailOnInvalidPath)
{
    auto fsp = FileSystemPath(foreignP);
    auto fh = FileHandle::openForReading(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForReading(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openForReadingFailIfNotThere)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForReading(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForReading(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openForReadingFailIfNotFile)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForReading(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForReading(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openForAppendingSimple)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockOpen(Eq(FilePath(someFileP)), _, _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForAppending(fsp);
    EXPECT_NE(FileHandle(), fh);
}

TEST_F(FileHandleFixture, openForAppendingCreateIfNotThere)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillOnce(Return(123010ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForAppending(fsp);
    EXPECT_NE(FileHandle(), fh);
}

TEST_F(FileHandleFixture, openForAppendingSimpleFailed)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockOpen(Eq(FilePath(someFileP)), _, _, _)).WillRepeatedly(Return(~0ull));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForAppending(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForAppending(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openForAppendingFailOnInvalidPath)
{
    auto fsp = FileSystemPath(foreignP);
    auto fh = FileHandle::openForAppending(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForAppending(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, openForAppendingFailIfNotThere)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForAppending(fsp, FileHandle::OpenType::kFailIfNotThere);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForAppending(fsp, FileHandle::OpenType::kFailIfNotThere), FileHandleException);
}

TEST_F(FileHandleFixture, openForAppendingFailIfNotFile)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, mockPathIsFile(Eq(FilePath(someFileP)))).WillRepeatedly(Return(false));
    auto fsp = FileSystemPath(someFileP);
    auto fh = FileHandle::openForAppending(fsp);
    EXPECT_EQ(FileHandle(), fh);

    FileHandle::throwErrors();
    EXPECT_THROW(FileHandle::openForAppending(fsp), FileHandleException);
}

TEST_F(FileHandleFixture, doubleCloseFile)
{
    FileHandle fh;
    fh.close();

    FileHandle::throwErrors();
    EXPECT_THROW(fh.close(), FileHandleException);
}

TEST_F(OpenFileHandleFixture, size)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockFileSize(_)).WillOnce(Return(32966ull));
    EXPECT_CALL(mock, mockFileSize(_, _)).WillOnce(Return(32965ull));

    EXPECT_EQ(32965ull, uut().size());
    uut().close();
    EXPECT_EQ(32966ull, uut().size());
}

TEST_F(OpenFileHandleFixture, sizeFails)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockFileSize(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(334u), Return(~0ull)));

    EXPECT_EQ(static_cast<uint64_t>(~0ull), uut().size());

    FileHandle::throwErrors();
    EXPECT_THROW(uut().size(), FileHandleException);
}

TEST_F(OpenFileHandleFixture, seekOpenAndClose)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockSeek(_, _, _)).WillOnce(SetArgReferee<2>(0u));

    uut().seek(35u);
    uut().close();
    uut().seek(35u);

    FileHandle::throwErrors();
    EXPECT_THROW(uut().seek(35u), FileHandleException);
}

TEST_F(OpenFileHandleFixture, seekFails)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockSeek(_, _, _)).WillRepeatedly(SetArgReferee<2>(335u));

    uut().seek(35u);

    FileHandle::throwErrors();
    EXPECT_THROW(uut().seek(35u), FileHandleException);
    try
    {
        uut().seek(35u);
    }
    catch (FileHandleException& e)
    {
        EXPECT_THAT(string(e.what()), HasSubstr("FileHandleException:"));
    }
}

TEST_F(OpenFileHandleFixture, read)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockRead(_, _, _, _)).WillOnce(Return(static_cast<size_t>(422u)));

    char dest[500];
    EXPECT_EQ(static_cast<size_t>(422u), uut().read(dest, 422u));
    EXPECT_EQ(static_cast<size_t>(0u), uut().read(nullptr, 422u));
    EXPECT_EQ(static_cast<size_t>(0u), uut().read(nullptr, 0u));
    FileHandle::throwErrors();
    EXPECT_THROW(uut().read(nullptr, 422u), NullPtrException);
}

TEST_F(FileHandleFixture, readClosed)
{
    FileHandle fh;

    char dest[500];
    EXPECT_EQ(static_cast<size_t>(0u), fh.read(dest, 422u));
    FileHandle::throwErrors();
    EXPECT_THROW(fh.read(dest, 422u), FileHandleException);
}

TEST_F(OpenFileHandleFixture, readFails)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockRead(_, _, _, _)).WillRepeatedly(DoAll(SetArgReferee<3>(336u), Return(~static_cast<size_t>(0u))));

    char dest[500];
    EXPECT_EQ(0u, uut().read(dest, 422u));
    FileHandle::throwErrors();
    EXPECT_THROW(uut().read(dest, 422u), FileHandleException);
}

TEST_F(OpenFileHandleFixture, write)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockWrite(_, _, _, _)).WillOnce(Return(423u));

    char src[500];
    EXPECT_EQ(static_cast<size_t>(423u), uut().write(src, 423u));
    EXPECT_EQ(static_cast<size_t>(0u), uut().write(nullptr, 423u));
    EXPECT_EQ(static_cast<size_t>(0u), uut().write(nullptr, 0u));
    FileHandle::throwErrors();
    EXPECT_THROW(uut().write(nullptr, 423u), NullPtrException);
}

TEST_F(FileHandleFixture, writeClosed)
{
    FileHandle fh;

    char src[500];
    EXPECT_EQ(static_cast<size_t>(0u), fh.write(src, static_cast<size_t>(423u)));
    FileHandle::throwErrors();
    EXPECT_THROW(fh.write(src, 423u), FileHandleException);
}

TEST_F(OpenFileHandleFixture, writeFails)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockWrite(_, _, _, _)).WillRepeatedly(DoAll(SetArgReferee<3>(337u), Return(~static_cast<size_t>(0u))));

    char src[500];
    EXPECT_EQ(static_cast<size_t>(0u), uut().write(src, 423u));
    FileHandle::throwErrors();
    EXPECT_THROW(uut().write(src, 423u), FileHandleException);
}

TEST_F(FileHandleFixture, copyFromNotOpen)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockPathIsDir(Eq(FilePath(homeDirP)))).WillOnce(Return(true));
    EXPECT_CALL(mock, mockPathExists(Eq(FilePath(someFileP)))).WillOnce(Return(false));
    EXPECT_CALL(mock, mockCreate(Eq(FilePath(someFileP)), _, _)).WillOnce(Return(123010ull));

    FileHandle fhDest;
    FileHandle fhSrc;
    EXPECT_EQ(static_cast<size_t>(0u), fhDest.copyFrom(fhSrc, 0u, 12930u));
    fhDest = FileHandle::create(FilePath(someFileP));
    EXPECT_EQ(static_cast<size_t>(0u), fhDest.copyFrom(fhSrc, 0u, 12930u));
}

TEST_F(CopyFileHandleFixture, copyZeroBytes)
{
    EXPECT_EQ(static_cast<size_t>(0u), uutDest().copyFrom(uutSrc(), 0u, 0u));
}

TEST_F(CopyFileHandleFixture, copyAFewBytes)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockRead(Eq(123011ull), _, _, _)).WillOnce(Return(static_cast<size_t>(424u)));
    EXPECT_CALL(mock, mockWrite(Eq(123010ull), _, _, _)).WillOnce(Return(static_cast<size_t>(424u)));

    EXPECT_EQ(static_cast<size_t>(424u), uutDest().copyFrom(uutSrc(), 0u, 424u));
}

TEST_F(CopyFileHandleFixture, copyMoreBytes)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockRead(Eq(123011ull), _, _, _)).WillRepeatedly(Return(static_cast<size_t>(4096u)));
    EXPECT_CALL(mock, mockWrite(Eq(123010ull), _, _, _)).WillRepeatedly(Return(static_cast<size_t>(4096u)));

    EXPECT_EQ(static_cast<size_t>(40960u), uutDest().copyFrom(uutSrc(), 0u, 40960u));
}

TEST_F(CopyFileHandleFixture, copyTooManyBytes)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockRead(Eq(123011ull), _, _, _)).WillRepeatedly(Return(static_cast<size_t>(40960u)));
    EXPECT_CALL(mock, mockWrite(Eq(123010ull), _, _, _)).WillRepeatedly(Return(static_cast<size_t>(40960u)));

    EXPECT_EQ(static_cast<size_t>(0u), uutDest().copyFrom(uutSrc(), 0u, 40960u));

    FileHandle::throwErrors();
    EXPECT_THROW(uutDest().copyFrom(uutSrc(), 0u, 40960u), FileHandleException);
}

TEST_F(CopyFileHandleFixture, writeFails)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockRead(Eq(123011ull), _, _, _)).WillRepeatedly(Return(4096u));
    EXPECT_CALL(mock, mockWrite(Eq(123010ull), _, _, _)).WillRepeatedly(Return(4086u));

    EXPECT_EQ(static_cast<size_t>(4086u), uutDest().copyFrom(uutSrc(), 0u, 40960u));

    FileHandle::throwErrors();
    EXPECT_THROW(uutDest().copyFrom(uutSrc(), 0u, 40960u), FileHandleException);
}

TEST_F(CopyFileHandleFixture, writeFailsOnCopyLimitedAndEndOfSource)
{
    auto& mock = OSMock();
    EXPECT_CALL(mock, mockRead(Eq(123011ull), _, _, _)).WillRepeatedly(Return(static_cast<size_t>(4095u)));
    EXPECT_CALL(mock, mockWrite(Eq(123010ull), _, _, _)).WillRepeatedly(Return(static_cast<size_t>(4086u)));

    EXPECT_EQ(static_cast<size_t>(4086u), uutDest().copyFrom(uutSrc(), 0u, 4096));

    FileHandle::throwErrors();
    EXPECT_THROW(uutDest().copyFrom(uutSrc(), 0u), FileHandleException);
}
