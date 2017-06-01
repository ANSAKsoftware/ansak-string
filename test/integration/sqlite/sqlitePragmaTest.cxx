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
// sqlitePragmaTest.cxx -- unit tests for the SQLite pragma wrapping functions
//
///////////////////////////////////////////////////////////////////////////

#include <cppunit/extensions/HelperMacros.h>
#include "TempFileTestFixture.hxx"

#include "sqlite_db_pragmas.hxx"
#include "sqlite_db.hxx"
#include "sqlite_db_pragmas.hxx"
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "file_path.hxx"

using namespace ansak;
using std::string;

class SqlitePragmaTestFixture : public TempFileTestFixture {

CPPUNIT_TEST_SUITE( SqlitePragmaTestFixture );
    CPPUNIT_TEST( testGetApplicationID );
    CPPUNIT_TEST( testSetApplicationID );
    CPPUNIT_TEST( testGetUserVersion );
    CPPUNIT_TEST( testSetUserVersion );
    CPPUNIT_TEST( testSetSecureDelete );
    CPPUNIT_TEST( testJournaling );
    CPPUNIT_TEST( testUtf8Encoding );
    CPPUNIT_TEST( testForeignKeys );
    CPPUNIT_TEST( testNoVacuuming );
    CPPUNIT_TEST( testVacuuming );
    CPPUNIT_TEST( testIncrementalVacuuming );
    CPPUNIT_TEST( testIntegrityCheck );
CPPUNIT_TEST_SUITE_END();

public:

    void testGetApplicationID();
    void testSetApplicationID();
    void testGetUserVersion();
    void testSetUserVersion();
    void testSetSecureDelete();
    void testJournaling();
    void testUtf8Encoding();
    void testForeignKeys();
    void testNoVacuuming();
    void testVacuuming();
    void testIncrementalVacuuming();
    void testIntegrityCheck();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SqlitePragmaTestFixture);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SqlitePragmaTestFixture, "SqlitePragmaTests");

void SqlitePragmaTestFixture::testGetApplicationID()
{
    FilePath appIDPath(getTempDir().child("appID"));
    SqliteDB appID(appIDPath());
    appID.create();
    int id = ansak::sqlite::getApplicationID(appID);
    CPPUNIT_ASSERT_EQUAL(0, id);
}

void SqlitePragmaTestFixture::testSetApplicationID()
{
    FilePath setAppIDPath(getTempDir().child("setAppID"));
    {
        SqliteDB setAppID(setAppIDPath());
        setAppID.create();
        ansak::sqlite::setApplicationID(setAppID, 0xBAADF00D);
    }
    SqliteDB appID(setAppIDPath());
    appID.open();
    int id = ansak::sqlite::getApplicationID(appID);
    CPPUNIT_ASSERT_EQUAL((int)0xBAADF00D, id);
}

void SqlitePragmaTestFixture::testGetUserVersion()
{
    FilePath userVersionPath(getTempDir().child("userVersion"));
    SqliteDB userVersion(userVersionPath());
    userVersion.create();
    int id = ansak::sqlite::getUserVersion(userVersion);
    CPPUNIT_ASSERT_EQUAL(0, id);
}

void SqlitePragmaTestFixture::testSetUserVersion()
{
    FilePath setUserVesionPath(getTempDir().child("setUserVesion"));
    {
        SqliteDB setUserVesion(setUserVesionPath());
        setUserVesion.create();
        ansak::sqlite::setUserVersion(setUserVesion, 32965);
    }
    SqliteDB userVesion(setUserVesionPath());
    userVesion.open();
    int id = ansak::sqlite::getUserVersion(userVesion);
    CPPUNIT_ASSERT_EQUAL(32965, id);
}

void SqlitePragmaTestFixture::testSetSecureDelete()
{
    FilePath secureDeletePath(getTempDir().child("secureDelete"));
    SqliteDB secureDelete(secureDeletePath());
    secureDelete.create();

    ansak::sqlite::setSecureDelete(secureDelete);

    SqliteDB::Statement secureStateQ(secureDelete.prepare("pragma secure_delete;"));
    int n;
    secureStateQ->setupRetrieval(0, n);
    secureStateQ();

    CPPUNIT_ASSERT_EQUAL(1, n);
}

void SqlitePragmaTestFixture::testJournaling()
{
    {
        SqliteDB memo;
        memo.create();

        CPPUNIT_ASSERT(ansak::sqlite::getJournaling(memo));
        ansak::sqlite::setJournaling(memo, false);
        CPPUNIT_ASSERT(!ansak::sqlite::getJournaling(memo));
        ansak::sqlite::setJournaling(memo, true);
        CPPUNIT_ASSERT(ansak::sqlite::getJournaling(memo));
    }

    {
        SqliteDB fileO(getTempDir().child("journal0")());
        fileO.create();

        CPPUNIT_ASSERT(ansak::sqlite::getJournaling(fileO));
        ansak::sqlite::setJournaling(fileO, false);
        CPPUNIT_ASSERT(!ansak::sqlite::getJournaling(fileO));
        ansak::sqlite::setJournaling(fileO, true);
        CPPUNIT_ASSERT(ansak::sqlite::getJournaling(fileO));
    }
}

void SqlitePragmaTestFixture::testUtf8Encoding()
{
    FilePath utf8EncodingPath(getTempDir().child("utf8Encoding"));
    SqliteDB utf8Encoding(utf8EncodingPath());
    utf8Encoding.create();

    ansak::sqlite::setUTF8Encoding(utf8Encoding);

    SqliteDB::Statement stmt(utf8Encoding.prepare("pragma encoding;"));
    string encodingOut;
    stmt->setupRetrieval(0, encodingOut);
    stmt();
    CPPUNIT_ASSERT_EQUAL(string("UTF-8"), encodingOut);
}

void SqlitePragmaTestFixture::testForeignKeys()
{
    FilePath foreignKeysPath(getTempDir().child("foreignKeys"));
    SqliteDB foreignKeys(foreignKeysPath());
    foreignKeys.create();

    ansak::sqlite::setForeignKeys(foreignKeys);

    SqliteDB::Statement stmt(foreignKeys.prepare("pragma foreign_keys;"));
    int foreignKeysOut;
    stmt->setupRetrieval(0, foreignKeysOut);
    stmt();
    CPPUNIT_ASSERT_EQUAL(1, foreignKeysOut);
}

void SqlitePragmaTestFixture::testNoVacuuming()
{
    FilePath vacuumingPath(getTempDir().child("vacuuming"));
    SqliteDB vacuuming(vacuumingPath());
    vacuuming.create();

    ansak::sqlite::setNoVacuuming(vacuuming);

    SqliteDB::Statement stmt(vacuuming.prepare("pragma auto_vacuum;"));
    int vacuumingOut;
    stmt->setupRetrieval(0, vacuumingOut);
    stmt();
    CPPUNIT_ASSERT_EQUAL(0, vacuumingOut);
}

void SqlitePragmaTestFixture::testVacuuming()
{
    FilePath vacuumingPath(getTempDir().child("vacuuming"));
    SqliteDB vacuuming(vacuumingPath());
    vacuuming.create();

    ansak::sqlite::setVacuuming(vacuuming);

    SqliteDB::Statement stmt(vacuuming.prepare("pragma auto_vacuum;"));
    int vacuumingOut;
    stmt->setupRetrieval(0, vacuumingOut);
    stmt();
    CPPUNIT_ASSERT_EQUAL(1, vacuumingOut);
}

void SqlitePragmaTestFixture::testIncrementalVacuuming()
{
    FilePath vacuumingPath(getTempDir().child("vacuuming"));
    SqliteDB vacuuming(vacuumingPath());
    vacuuming.create();

    ansak::sqlite::setIncrementalVacuuming(vacuuming);

    SqliteDB::Statement stmt(vacuuming.prepare("pragma auto_vacuum;"));
    int vacuumingOut;
    stmt->setupRetrieval(0, vacuumingOut);
    stmt();
    CPPUNIT_ASSERT_EQUAL(2, vacuumingOut);
}

void SqlitePragmaTestFixture::testIntegrityCheck()
{
    FilePath checkItPath(getTempDir().child("checkIt"));
    SqliteDB checkIt(checkItPath());
    checkIt.create();

    CPPUNIT_ASSERT(ansak::sqlite::checkIntegrity(checkIt));
}

