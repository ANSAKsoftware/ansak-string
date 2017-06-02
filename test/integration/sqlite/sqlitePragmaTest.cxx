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

#include <gmock/gmock.h>

#include <sqlite_db_pragmas.hxx>
#include <sqlite_db.hxx>
#include <sqlite_db_pragmas.hxx>
#include <file_path.hxx>
#include <temp_file_wrapper.hxx>
#include <runtime_exception.hxx>

using namespace ansak;
using namespace ansak::internal;
using std::string;
using namespace testing;

TEST(SqlitePragmaTest, testGetApplicationID)
{
    TempFileWrapper tempDir;
    FilePath appIDPath(tempDir.child("appID"));
    SqliteDB appID(appIDPath);
    appID.create();
    int id = ansak::sqlite::getApplicationID(appID);
    EXPECT_EQ(0, id);
}

TEST(SqlitePragmaTest, testSetApplicationID)
{
    TempFileWrapper tempDir;
    FilePath setAppIDPath(tempDir.child("setAppID"));
    {
        SqliteDB setAppID(setAppIDPath);
        setAppID.create();
        ansak::sqlite::setApplicationID(setAppID, 0xBAADF00D);
    }
    SqliteDB appID(setAppIDPath);
    appID.open();
    int id = ansak::sqlite::getApplicationID(appID);
    EXPECT_EQ((int)0xBAADF00D, id);
}

TEST(SqlitePragmaTest, testGetUserVersion)
{
    TempFileWrapper tempDir;
    FilePath userVersionPath(tempDir.child("userVersion"));
    SqliteDB userVersion(userVersionPath);
    userVersion.create();
    int id = ansak::sqlite::getUserVersion(userVersion);
    EXPECT_EQ(0, id);
}

TEST(SqlitePragmaTest, testSetUserVersion)
{
    TempFileWrapper tempDir;
    FilePath setUserVesionPath(tempDir.child("setUserVesion"));
    {
        SqliteDB setUserVesion(setUserVesionPath);
        setUserVesion.create();
        ansak::sqlite::setUserVersion(setUserVesion, 32965);
    }
    SqliteDB userVesion(setUserVesionPath);
    userVesion.open();
    int id = ansak::sqlite::getUserVersion(userVesion);
    EXPECT_EQ(32965, id);
}

TEST(SqlitePragmaTest, testSetSecureDelete)
{
    TempFileWrapper tempDir;
    FilePath secureDeletePath(tempDir.child("secureDelete"));
    SqliteDB secureDelete(secureDeletePath);
    secureDelete.create();

    ansak::sqlite::setSecureDelete(secureDelete);

    SqliteDB::Statement secureStateQ(secureDelete.prepare("pragma secure_delete;"));
    int n;
    secureStateQ->setupRetrieval(0, n);
    secureStateQ();

    EXPECT_EQ(1, n);
}

TEST(SqlitePragmaTest, testJournaling)
{
    TempFileWrapper tempDir;
    {
        SqliteDB memo;
        memo.create();

        EXPECT_TRUE(ansak::sqlite::getJournaling(memo));
        ansak::sqlite::setJournaling(memo, false);
        EXPECT_FALSE(ansak::sqlite::getJournaling(memo));
        ansak::sqlite::setJournaling(memo, true);
        EXPECT_TRUE(ansak::sqlite::getJournaling(memo));
    }

    {
        SqliteDB fileO(tempDir.child("journal0"));
        fileO.create();

        EXPECT_TRUE(ansak::sqlite::getJournaling(fileO));
        ansak::sqlite::setJournaling(fileO, false);
        EXPECT_FALSE(ansak::sqlite::getJournaling(fileO));
        ansak::sqlite::setJournaling(fileO, true);
        EXPECT_TRUE(ansak::sqlite::getJournaling(fileO));
    }
}

TEST(SqlitePragmaTest, testUtf8Encoding)
{
    TempFileWrapper tempDir;
    FilePath utf8EncodingPath(tempDir.child("utf8Encoding"));
    SqliteDB utf8Encoding(utf8EncodingPath);
    utf8Encoding.create();

    ansak::sqlite::setUTF8Encoding(utf8Encoding);

    SqliteDB::Statement stmt(utf8Encoding.prepare("pragma encoding;"));
    string encodingOut;
    stmt->setupRetrieval(0, encodingOut);
    stmt();
    EXPECT_THAT(encodingOut, StrEq("UTF-8"));
}

TEST(SqlitePragmaTest, testForeignKeys)
{
    TempFileWrapper tempDir;
    FilePath foreignKeysPath(tempDir.child("foreignKeys"));
    SqliteDB foreignKeys(foreignKeysPath);
    foreignKeys.create();

    ansak::sqlite::setForeignKeys(foreignKeys);

    SqliteDB::Statement stmt(foreignKeys.prepare("pragma foreign_keys;"));
    int foreignKeysOut;
    stmt->setupRetrieval(0, foreignKeysOut);
    stmt();
    EXPECT_EQ(1, foreignKeysOut);
}

TEST(SqlitePragmaTest, testNoVacuuming)
{
    TempFileWrapper tempDir;
    FilePath vacuumingPath(tempDir.child("vacuuming"));
    SqliteDB vacuuming(vacuumingPath);
    vacuuming.create();

    ansak::sqlite::setNoVacuuming(vacuuming);

    SqliteDB::Statement stmt(vacuuming.prepare("pragma auto_vacuum;"));
    int vacuumingOut;
    stmt->setupRetrieval(0, vacuumingOut);
    stmt();
    EXPECT_EQ(0, vacuumingOut);
}

TEST(SqlitePragmaTest, testVacuuming)
{
    TempFileWrapper tempDir;
    FilePath vacuumingPath(tempDir.child("vacuuming"));
    SqliteDB vacuuming(vacuumingPath);
    vacuuming.create();

    ansak::sqlite::setVacuuming(vacuuming);

    SqliteDB::Statement stmt(vacuuming.prepare("pragma auto_vacuum;"));
    int vacuumingOut;
    stmt->setupRetrieval(0, vacuumingOut);
    stmt();
    EXPECT_EQ(1, vacuumingOut);
}

TEST(SqlitePragmaTest, testIncrementalVacuuming)
{
    TempFileWrapper tempDir;
    FilePath vacuumingPath(tempDir.child("vacuuming"));
    SqliteDB vacuuming(vacuumingPath);
    vacuuming.create();

    ansak::sqlite::setIncrementalVacuuming(vacuuming);

    SqliteDB::Statement stmt(vacuuming.prepare("pragma auto_vacuum;"));
    int vacuumingOut;
    stmt->setupRetrieval(0, vacuumingOut);
    stmt();
    EXPECT_EQ(2, vacuumingOut);
}

TEST(SqlitePragmaTest, testIntegrityCheck)
{
    TempFileWrapper tempDir;
    FilePath checkItPath(tempDir.child("checkIt"));
    SqliteDB checkIt(checkItPath);
    checkIt.create();

    EXPECT_TRUE(ansak::sqlite::checkIntegrity(checkIt));
}

