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
// 2017.05.21 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// test_sqlite_db.cxx -- Tests for sqlite_db.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_db.hxx>
#include <mock_sqlite_db.hxx>
#include <mock_sqlite_errmsg.hxx>
#include <mock_sqlite_statement.hxx>
#include <mock_file_system_path.hxx>
#include <file_path.hxx>
#include <sqlite_exception.hxx>
#include <runtime_exception.hxx>

#include <gmock/gmock.h>

using namespace ansak;
using namespace testing;

namespace {

#if defined(WIN32)

FilePath junkPath("C:\\Users\\jvplasmeuden\\data.base");
FilePath unrealPath("/home/jvplasmeuden/data/data.base");

#else

FilePath junkPath("/home/jvplasmeuden/data/data.base");
FilePath unrealPath("C:\\Users\\jvplasmeuden\\data.base");

#endif

}

TEST(SqlLiteTest, testVersionIDs)
{
    EXPECT_EQ(99, SqliteDB::getVersionNumber());
    EXPECT_THAT(SqliteDB::getVersion(true), StrEq("Vacuuous Vole"));
    EXPECT_THAT(SqliteDB::getVersion(false), StrEq("Illiberal Version"));
}

TEST(SqlLiteTest, testSimpleConstructor)
{
    SqliteDB noDB;

    EXPECT_FALSE(noDB.isMemory());
    EXPECT_FALSE(noDB.isOpen());
    EXPECT_THROW(noDB.inTransaction(), RuntimeException);
}

TEST(SqliteTest, createInMemory)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));

    SqliteDB memDB;
    memDB.create();
}

TEST(SqliteTest, throwOnClose)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Throw(RuntimeException("boo!", "bam!", 232u)));

    SqliteDB memDB;
    memDB.create();
}

TEST(SqliteTest, createInFile)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));

    FileSystemPathMock theFSMock;
    FileSystemPath parent(junkPath.parent());
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_CALL(theFSMock, parent(_)).WillOnce(Return(parent));
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(true));

    SqliteDB fileDB(junkPath);
    fileDB.create();
}

TEST(SqliteTest, pathToCreateIsUnreal)
{
    SqliteDB fileDB(unrealPath);
    EXPECT_THROW(fileDB.create(), SqliteException);
}

TEST(SqliteTest, pathExists)
{
    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));
    
    SqliteDB fileDB(junkPath);
    EXPECT_THROW(fileDB.create(), SqliteException);
}

TEST(SqliteTest, parentIsFile)
{
    FileSystemPathMock theFSMock;
    FileSystemPath parent(junkPath.parent());
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_CALL(theFSMock, parent(_)).WillOnce(Return(parent));
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    
    SqliteDB fileDB(junkPath);
    EXPECT_THROW(fileDB.create(), SqliteException);
}

TEST(SqliteTest, parentCreateDirectoryFails)
{
    FileSystemPathMock theFSMock;
    FileSystemPath parent(junkPath.parent());
    EXPECT_CALL(theFSMock, exists(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(theFSMock, parent(_)).WillOnce(Return(parent));
    EXPECT_CALL(theFSMock, createDirectory(_)).WillOnce(Return(false));

    SqliteDB fileDB(junkPath);
    EXPECT_THROW(fileDB.create(), SqliteException);
}

TEST(SqliteTest, createInFileFails)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).WillOnce(Return(3));
    SqliteErrmsgMock exceptionMock;
    EXPECT_CALL(exceptionMock, errstr(_)).WillOnce(Return("This is an error!"));


    FileSystemPathMock theFSMock;
    FileSystemPath parent(junkPath.parent());
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_CALL(theFSMock, parent(_)).WillOnce(Return(parent));
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(true));

    SqliteDB fileDB(junkPath);
    EXPECT_THROW(fileDB.create(), SqliteException);
}

TEST(SqliteTest, open)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteDB fileDB(junkPath);
    fileDB.open();
}

TEST(SqliteTest, pathToOpenIsUnreal)
{
    SqliteDB fileDB(unrealPath);
    EXPECT_THROW(fileDB.open(), SqliteException);
}

TEST(SqliteTest, pathToOpenIsDir)
{
    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(true));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteDB fileDB(junkPath);
    EXPECT_THROW(fileDB.open(), SqliteException);
}

TEST(SqliteTest, openFails)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).WillOnce(Return(35));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteErrmsgMock exceptionMock;
    EXPECT_CALL(exceptionMock, errstr(_)).WillOnce(Return("This is an error!"));

    SqliteDB fileDB(junkPath);
    EXPECT_THROW(fileDB.open(), SqliteException);
}

TEST(SqliteTest, openNonexistentPath)
{
    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(false));

    SqliteDB fileDB(FilePath::invalidPath());
    EXPECT_THROW(fileDB.open(), SqliteException);
}

TEST(SqliteTest, closeThrows)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(35));

    SqliteErrmsgMock exceptionMock;
    EXPECT_CALL(exceptionMock, errstr(_)).WillOnce(Return("This is an error!"));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteDB fileDB(junkPath);
    fileDB.open();
    EXPECT_THROW(fileDB.close(), SqliteException);
}

TEST(SqliteTest, closeInTransaction)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));
    EXPECT_CALL(theMock, prepare_v2(_, _, _, _, _)).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(81ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(82ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(83ul)), Return(0)));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteStatementMock theStmtMock;
    EXPECT_CALL(theStmtMock, step(_)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, reset(_)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, finalize(_)).WillRepeatedly(Return(0));

    {
        SqliteDB fileDB(junkPath);

        fileDB.open();
        fileDB.beginTransaction();
    }
}

TEST(SqliteTest, throwInBeginTransaction)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));
    EXPECT_CALL(theMock, prepare_v2(_, _, _, _, _)).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(81ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(82ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(83ul)), Return(0)));

    SqliteStatementMock theStmtMock;
    EXPECT_CALL(theStmtMock, step(_)).WillOnce(Return(3));
    EXPECT_CALL(theStmtMock, finalize(_)).WillRepeatedly(Return(0));

    SqliteErrmsgMock exceptionMock;
    EXPECT_CALL(exceptionMock, errstr(_)).WillOnce(Return("This is an error!"));

    {
        SqliteDB memDB;

        memDB.create();
        EXPECT_THROW(memDB.beginTransaction(), SqliteException);
        EXPECT_FALSE(memDB.inTransaction());
    }
}

TEST(SqliteTest, throwInRollback)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));
    EXPECT_CALL(theMock, prepare_v2(_, _, _, _, _)).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(81ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(82ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(83ul)), Return(0)));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteStatementMock theStmtMock;
    EXPECT_CALL(theStmtMock, step(_)).WillOnce(Return(0)).WillOnce(Return(3));
    EXPECT_CALL(theStmtMock, reset(_)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, finalize(_)).WillRepeatedly(Return(0));

    SqliteErrmsgMock exceptionMock;
    EXPECT_CALL(exceptionMock, errstr(_)).WillOnce(Return("This is an error!"));

    {
        SqliteDB fileDB(junkPath);

        fileDB.open();
        EXPECT_FALSE(fileDB.inTransaction());
        fileDB.beginTransaction();
        EXPECT_TRUE(fileDB.inTransaction());
        EXPECT_THROW(fileDB.rollback(), SqliteException);
        EXPECT_FALSE(fileDB.inTransaction());
    }
}

TEST(SqliteTest, noActionRollback)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));

    {
        SqliteDB memDB;

        memDB.create();
        memDB.rollback();
    }
}

TEST(SqliteTest, noActionCommit)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    {
        SqliteDB fileDB(junkPath);

        fileDB.open();
        fileDB.commit();
    }
}

TEST(SqliteTest, rollback)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));
    EXPECT_CALL(theMock, prepare_v2(_, _, _, _, _)).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(81ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(82ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(83ul)), Return(0)));

    SqliteStatementMock theStmtMock;
    EXPECT_CALL(theStmtMock, step(_)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, reset(_)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, finalize(_)).WillRepeatedly(Return(0));

    {
        SqliteDB memDB;

        memDB.create();
        memDB.beginTransaction();
        memDB.rollback();
    }
}

TEST(SqliteTest, commit)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));
    EXPECT_CALL(theMock, prepare_v2(_, _, _, _, _)).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(81ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(82ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(83ul)), Return(0)));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteStatementMock theStmtMock;
    EXPECT_CALL(theStmtMock, step(_)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, reset(_)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, finalize(_)).WillRepeatedly(Return(0));

    {
        SqliteDB fileDB(junkPath);

        fileDB.open();
        fileDB.beginTransaction();
        EXPECT_TRUE(fileDB.inTransaction());
        fileDB.commit();
        EXPECT_FALSE(fileDB.inTransaction());
    }
}

TEST(SqliteTest, throwInCommit)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));
    EXPECT_CALL(theMock, prepare_v2(_, _, _, _, _)).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(81ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(82ul)), Return(0))).
            WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(83ul)), Return(0)));

    SqliteStatementMock theStmtMock;
    EXPECT_CALL(theStmtMock, step(_)).WillOnce(Return(0)).WillOnce(Return(3)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, reset(_)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(theStmtMock, finalize(_)).WillRepeatedly(Return(0));

    SqliteErrmsgMock exceptionMock;
    EXPECT_CALL(exceptionMock, errstr(_)).WillOnce(Return("This is an error!"));

    {
        SqliteDB memDB;

        memDB.create();
        EXPECT_FALSE(memDB.inTransaction());
        memDB.beginTransaction();
        EXPECT_TRUE(memDB.inTransaction());
        EXPECT_THROW(memDB.commit(), SqliteException);
        EXPECT_TRUE(memDB.inTransaction());
    }
}

TEST(SqliteTest, disposeStatementBeforeDB)
{
    SqliteDBMock theMock;
    EXPECT_CALL(theMock, open_v2(_,_,_,_)).
            WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
    EXPECT_CALL(theMock, close(_)).WillOnce(Return(0));
    EXPECT_CALL(theMock, prepare_v2(_, _, _, _, _)).
            WillRepeatedly(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(65ul)),
                                 Return(0)));

    SqliteStatementMock theStmtMock;
    EXPECT_CALL(theStmtMock, finalize(_)).WillOnce(Return(0));

    FileSystemPathMock theFSMock;
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(false));
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(true));

    SqliteDB fileDB(junkPath);
    fileDB.open();
    {
        auto statement = fileDB.prepareStatement("Do Something");
        EXPECT_NE(statement.get(), nullptr);
    }

}
