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

#include <cppunit/extensions/HelperMacros.h>
#include "TempFileTestFixture.hxx"

#include "sqlite_db.hxx"
#include "exception.hxx"
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "file_path.hxx"

using namespace ansak;
using std::string;

class SqliteDBTestFixture : public TempFileTestFixture {

CPPUNIT_TEST_SUITE( SqliteDBTestFixture );
    CPPUNIT_TEST( testCheckVersion );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testCreateMemoryDB );
    CPPUNIT_TEST_EXCEPTION( testBadCreateMemory, Exception );
    CPPUNIT_TEST( testCreateFileDB );
    CPPUNIT_TEST_EXCEPTION( testCreateCloseUse, Exception );
    CPPUNIT_TEST( testCreateCloseOpen );
    CPPUNIT_TEST_EXCEPTION( testBadCreateFileRO, Exception );
    CPPUNIT_TEST_EXCEPTION( testBadCreateFileOpen, Exception );
    CPPUNIT_TEST( testOpen );
    CPPUNIT_TEST_EXCEPTION( testOpenThrowAlreadyOpen, Exception );
    CPPUNIT_TEST_EXCEPTION( testThrowOpenMemory, Exception );
    CPPUNIT_TEST( testPrepare );
CPPUNIT_TEST_SUITE_END();

public:

    void testCheckVersion();
    void testConstruction();
    void testCreateMemoryDB();
    void testBadCreateMemory();
    void testCreateFileDB();
    void testCreateCloseUse();
    void testCreateCloseOpen();
    void testBadCreateFileRO();
    void testBadCreateFileOpen();
    void testOpen();
    void testOpenThrowAlreadyOpen();
    void testThrowOpenMemory();
    void testPrepare();

    void testInTransaction();
    void testCommitTransaction();
    void testRollbackTransaction();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SqliteDBTestFixture);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SqliteDBTestFixture, "SqliteDBTests");

void SqliteDBTestFixture::testCheckVersion()
{
    CPPUNIT_ASSERT(SqliteDB::getVersionNumber() != 0);
    string version(SqliteDB::getVersion());
    string sourceId(SqliteDB::getVersion(true));

    CPPUNIT_ASSERT(!version.empty());
    CPPUNIT_ASSERT(!sourceId.empty());
}

void SqliteDBTestFixture::testConstruction()
{
    SqliteDB one;

    SqliteDB two("/tmp/two");

    FilePath cwd;
    FilePath threePath(cwd.child("three"));
    SqliteDB three(threePath());
}

void SqliteDBTestFixture::testCreateMemoryDB()
{
    {
        SqliteDB memory;
        memory.create();
        CPPUNIT_ASSERT(memory.isMemory());
        CPPUNIT_ASSERT(memory.isOpen());
    }

    {
        FilePath forgetPath(getTempDir().child("forgetMe"));
        SqliteDB namedMemory(forgetPath());
        namedMemory.create(SqliteDB::kReadWrite | SqliteDB::kMemory | SqliteDB::kNoMutex);
        CPPUNIT_ASSERT(namedMemory.isMemory());
        CPPUNIT_ASSERT(namedMemory.isOpen());
    }
}

void SqliteDBTestFixture::testBadCreateMemory()     // tested for throwing
{
    SqliteDB memory;
    memory.create(SqliteDB::kReadOnly);
}

void SqliteDBTestFixture::testCreateFileDB()
{
    FilePath forgetPath(getTempDir().child("forgetMe"));
    {
        SqliteDB fileDB(forgetPath());
        fileDB.create();
        CPPUNIT_ASSERT(!fileDB.isMemory());
        CPPUNIT_ASSERT(fileDB.isOpen());
    }
}

void SqliteDBTestFixture::testCreateCloseUse()
{
    FilePath forgetPath(getTempDir().child("reuse"));

    SqliteDB fileDB(forgetPath());
    fileDB.create();

    fileDB.close();

    fileDB.execute("select * from noTable");
}

void SqliteDBTestFixture::testCreateCloseOpen()
{
    FilePath forgetPath(getTempDir().child("reopened"));
    {
        SqliteDB fileDB(forgetPath());
        fileDB.create();

        fileDB.close();

        fileDB.open();
    }
    {
        SqliteDB fileDB(forgetPath());
        fileDB.open();
        fileDB.close();
    }
}

void SqliteDBTestFixture::testBadCreateFileRO()     // tested for throwing
{
    FilePath forgetPath(getTempDir().child("forgetMe"));
    SqliteDB fileDB(forgetPath());
    fileDB.create(SqliteDB::kReadOnly);
}

void SqliteDBTestFixture::testBadCreateFileOpen()       // tested for throwing
{
    FilePath forgetPath(getTempDir().child("forgetMe"));

    SqliteDB fileDB(forgetPath());
    fileDB.create();

    SqliteDB refileDB(forgetPath());
    refileDB.create();
}

void SqliteDBTestFixture::testOpen()
{
    FilePath openPath(getTempDir().child("openMe"));

    {
        SqliteDB fileDB(openPath());
        fileDB.create();
    }

    {
        SqliteDB openDB(openPath());
        openDB.open();
    }
}

void SqliteDBTestFixture::testOpenThrowAlreadyOpen()        // tested for throwing
{
    FilePath forgetPath(getTempDir().child("forgetMe"));

    {
        SqliteDB fileDB(forgetPath());
        fileDB.create();
    }

    SqliteDB fileDB(forgetPath());
    fileDB.open();
    fileDB.open();
}

void SqliteDBTestFixture::testThrowOpenMemory()     // tested for throwing
{
    SqliteDB memoryDB;
    memoryDB.open();
}

void SqliteDBTestFixture::testPrepare()
{
    FilePath usePath(getTempDir().child("useMe"));

    {
        SqliteDB fileDB(usePath());
        fileDB.create();

        SqliteDB::Statement statement(fileDB.prepare("select count(*) from sqlite_master;"));
    }
}

void SqliteDBTestFixture::testInTransaction()
{
    FilePath usePath(getTempDir().child("trans0"));
    SqliteDB useIt(usePath());
    useIt.create();

    CPPUNIT_ASSERT(!useIt.inTransaction());
    useIt.beginTransaction();
    CPPUNIT_ASSERT(useIt.inTransaction());
}

void SqliteDBTestFixture::testCommitTransaction()
{
    FilePath usePath(getTempDir().child("trans1"));
    SqliteDB useIt(usePath());
    useIt.create();

    useIt.execute("create table foo(x INTEGER, y TEXT);");
    useIt.execute("insert into foo(x, y) values(1, '''Twas brillig and the slithy toves');");
    useIt.execute("insert into foo(x, y) values(2, 'Did gyre and gimble in the wabe.');");
    useIt.execute("insert into foo(x, y) values(3, 'All mimsy were the borogoves');");
    useIt.execute("insert into foo(x, y) values(4, 'And the mome raths outgrabe.');");

    CPPUNIT_ASSERT(!useIt.inTransaction());
    useIt.beginTransaction();
    CPPUNIT_ASSERT(useIt.inTransaction());
    useIt.execute("update foo set x = x + 100;");
    CPPUNIT_ASSERT(useIt.inTransaction());
    useIt.commit();
    CPPUNIT_ASSERT(!useIt.inTransaction());

    SqliteDB::Statement sum(useIt.prepare("select sum(x) from foo;"));
    int xSum;
    sum->setupRetrieval(0, xSum);
    sum();
    CPPUNIT_ASSERT_EQUAL(410, xSum);
}

void SqliteDBTestFixture::testRollbackTransaction()
{
    FilePath usePath(getTempDir().child("trans2"));
    SqliteDB useIt(usePath());
    useIt.create();

    useIt.execute("create table foo(x INTEGER, y TEXT);");
    useIt.execute("insert into foo(x, y) values(1, '''Twas brillig and the slithy toves');");
    useIt.execute("insert into foo(x, y) values(2, 'Did gyre and gimble in the wabe.');");
    useIt.execute("insert into foo(x, y) values(3, 'All mimsy were the borogoves');");
    useIt.execute("insert into foo(x, y) values(4, 'And the mome raths outgrabe.');");

    CPPUNIT_ASSERT(!useIt.inTransaction());
    useIt.beginTransaction();
    CPPUNIT_ASSERT(useIt.inTransaction());
    useIt.execute("update foo set x = x + 100;");
    CPPUNIT_ASSERT(useIt.inTransaction());
    useIt.rollback();
    CPPUNIT_ASSERT(!useIt.inTransaction());

    SqliteDB::Statement sum(useIt.prepare("select sum(x) from foo;"));
    int xSum;
    sum->setupRetrieval(0, xSum);
    sum();
    CPPUNIT_ASSERT_EQUAL(10, xSum);
}

