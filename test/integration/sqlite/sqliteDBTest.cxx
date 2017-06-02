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
// 2014.11.02 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// sqliteDBTest.cxx -- unit tests for the SQLite database wrapping class
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <sqlite_db.hxx>
#include <file_path.hxx>
#include <temp_file_wrapper.hxx>
#include <runtime_exception.hxx>

using namespace ansak;
using namespace ansak::internal;
using std::string;

TEST(SqliteDBIntegration, testCheckVersion)
{
    EXPECT_NE(0, SqliteDB::getVersionNumber());
    EXPECT_FALSE(SqliteDB::getVersion().empty());
    EXPECT_FALSE(SqliteDB::getVersion(true).empty());
}

TEST(SqliteDBIntegration, testConstruction)
{
    SqliteDB one;

    SqliteDB two("/tmp/two");

    FilePath cwd;
    FilePath threePath(cwd.child("three"));
    SqliteDB three(threePath);
}

TEST(SqliteDBIntegration, testCreateMemoryDB)
{
    {
        SqliteDB memory;
        memory.create();
        EXPECT_TRUE(memory.isMemory());
        EXPECT_TRUE(memory.isOpen());
    }

    {
        TempFileWrapper tempDir;
        FilePath forgetPath(tempDir.child("forgetMe"));
        SqliteDB namedMemory(forgetPath);
        namedMemory.create(SqliteDB::kReadWrite | SqliteDB::kMemory | SqliteDB::kNoMutex);
        EXPECT_TRUE(namedMemory.isMemory());
        EXPECT_TRUE(namedMemory.isOpen());
    }
}

TEST(SqliteDBIntegration, testBadCreateMemory)     // tested for throwing
{
    SqliteDB memory;
    EXPECT_THROW(memory.create(SqliteDB::kReadOnly), RuntimeException);
}

TEST(SqliteDBIntegration, testCreateFileDB)
{
    TempFileWrapper tempDir;
    FilePath forgetPath(tempDir.child("forgetMe"));
    {
        SqliteDB fileDB(forgetPath);
        fileDB.create();
        EXPECT_FALSE(fileDB.isMemory());
        EXPECT_TRUE(fileDB.isOpen());
    }
}

TEST(SqliteDBIntegration, testCreateCloseUse)
{
    TempFileWrapper tempDir;
    FilePath forgetPath(tempDir.child("reuse"));

    SqliteDB fileDB(forgetPath);
    fileDB.create();

    fileDB.close();

    EXPECT_THROW(fileDB.execute("select * from noTable"), RuntimeException);
}

TEST(SqliteDBIntegration, testCreateCloseOpen)
{
    TempFileWrapper tempDir;
    FilePath forgetPath(tempDir.child("reopened"));
    {
        SqliteDB fileDB(forgetPath);
        fileDB.create();

        fileDB.close();

        fileDB.open();
    }
    {
        SqliteDB fileDB(forgetPath);
        fileDB.open();
        fileDB.close();
    }
}

TEST(SqliteDBIntegration, testBadCreateFileRO)     // tested for throwing
{
    TempFileWrapper tempDir;
    FilePath forgetPath(tempDir.child("forgetMe"));
    SqliteDB fileDB(forgetPath);
    EXPECT_THROW(fileDB.create(SqliteDB::kReadOnly), RuntimeException);
}

TEST(SqliteDBIntegration, testBadCreateFileOpen)       // tested for throwing
{
    TempFileWrapper tempDir;
    FilePath forgetPath(tempDir.child("forgetMe"));

    SqliteDB fileDB(forgetPath);
    fileDB.create();

    SqliteDB refileDB(forgetPath);
    EXPECT_THROW(refileDB.create(), SqliteException);
}

TEST(SqliteDBIntegration, testOpen)
{
    TempFileWrapper tempDir;
    FilePath openPath(tempDir.child("openMe"));

    {
        SqliteDB fileDB(openPath);
        fileDB.create();
    }

    {
        SqliteDB openDB(openPath);
        openDB.open();
    }
}

TEST(SqliteDBIntegration, testOpenThrowAlreadyOpen)        // tested for throwing
{
    TempFileWrapper tempDir;
    FilePath forgetPath(tempDir.child("forgetMe"));

    {
        SqliteDB fileDB(forgetPath);
        fileDB.create();
    }

    SqliteDB fileDB(forgetPath);
    fileDB.open();
    EXPECT_THROW(fileDB.open(), RuntimeException);
}

TEST(SqliteDBIntegration, testThrowOpenMemory)     // tested for throwing
{
    SqliteDB memoryDB;
    EXPECT_THROW(memoryDB.open(), SqliteException);
}

TEST(SqliteDBIntegration, testPrepare)
{
    TempFileWrapper tempDir;
    FilePath usePath(tempDir.child("useMe"));

    {
        SqliteDB fileDB(usePath);
        fileDB.create();

        SqliteDB::Statement statement(fileDB.prepare("select count(*) from sqlite_master;"));
    }
}

TEST(SqliteDBIntegration, testInTransaction)
{
    TempFileWrapper tempDir;
    FilePath usePath(tempDir.child("trans0"));
    SqliteDB useIt(usePath);
    useIt.create();

    EXPECT_FALSE(useIt.inTransaction());
    useIt.beginTransaction();
    EXPECT_TRUE(useIt.inTransaction());
}

TEST(SqliteDBIntegration, testCommitTransaction)
{
    TempFileWrapper tempDir;
    FilePath usePath(tempDir.child("trans1"));
    SqliteDB useIt(usePath);
    useIt.create();

    useIt.execute("create table foo(x INTEGER, y TEXT);");
    useIt.execute("insert into foo(x, y) values(1, '''Twas brillig and the slithy toves');");
    useIt.execute("insert into foo(x, y) values(2, 'Did gyre and gimble in the wabe.');");
    useIt.execute("insert into foo(x, y) values(3, 'All mimsy were the borogoves');");
    useIt.execute("insert into foo(x, y) values(4, 'And the mome raths outgrabe.');");

    EXPECT_FALSE(useIt.inTransaction());
    useIt.beginTransaction();
    EXPECT_TRUE(useIt.inTransaction());
    useIt.execute("update foo set x = x + 100;");
    EXPECT_TRUE(useIt.inTransaction());
    useIt.commit();
    EXPECT_FALSE(useIt.inTransaction());

    SqliteDB::Statement sum(useIt.prepare("select sum(x) from foo;"));
    int xSum;
    sum->setupRetrieval(0, xSum);
    sum();
    EXPECT_EQ(410, xSum);
}

TEST(SqliteDBIntegration, testRollbackTransaction)
{
    TempFileWrapper tempDir;
    FilePath usePath(tempDir.child("trans2"));
    SqliteDB useIt(usePath);
    useIt.create();

    useIt.execute("create table foo(x INTEGER, y TEXT);");
    useIt.execute("insert into foo(x, y) values(1, '''Twas brillig and the slithy toves');");
    useIt.execute("insert into foo(x, y) values(2, 'Did gyre and gimble in the wabe.');");
    useIt.execute("insert into foo(x, y) values(3, 'All mimsy were the borogoves');");
    useIt.execute("insert into foo(x, y) values(4, 'And the mome raths outgrabe.');");

    EXPECT_FALSE(useIt.inTransaction());
    useIt.beginTransaction();
    EXPECT_TRUE(useIt.inTransaction());
    useIt.execute("update foo set x = x + 100;");
    EXPECT_TRUE(useIt.inTransaction());
    useIt.rollback();
    EXPECT_FALSE(useIt.inTransaction());

    SqliteDB::Statement sum(useIt.prepare("select sum(x) from foo;"));
    int xSum;
    sum->setupRetrieval(0, xSum);
    sum();
    EXPECT_EQ(10, xSum);
}
