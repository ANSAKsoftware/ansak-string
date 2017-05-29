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
// test_sqlite_exception.cxx -- Tests for sqlite_exception.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_exception.hxx>
#include <mock_sqlite_errmsg.hxx>
#include <file_path.hxx>

#include <gmock/gmock.h>

using namespace ansak;
using namespace std;
using namespace testing;

namespace {

#if defined(WIN32)

FilePath junkPath("C:\\Users\\jvplasmeuden\\data.base");

#else

FilePath junkPath("/home/jvplasmeuden/data/data.base");

#endif

}

TEST(SqliteException, FilePathNullContext)
{
    SqliteException uut(junkPath, nullptr);
    EXPECT_THAT(uut.what(),
                StartsWith("SqliteException: Problem with file = "));
    EXPECT_THAT(uut.what(), EndsWith("data.base"));
    SqliteException buut(junkPath, "");
    EXPECT_THAT(buut.what(),
                StartsWith("SqliteException: Problem with file = "));
    EXPECT_THAT(uut.what(), EndsWith("data.base"));
}

TEST(SqliteException, FilePathPlusContext)
{
    SqliteException uut(junkPath, "This is only a test");
    EXPECT_THAT(uut.what(),
                StartsWith("SqliteException: Problem with file = "));
    EXPECT_THAT(uut.what(),
                EndsWith("; context = This is only a test"));
}

TEST(SqliteException, FilePathPlusCodePlusContext)
{
    SqliteErrmsgMock mockIt;
    EXPECT_CALL(mockIt, errstr(_)).WillRepeatedly(Return("Potted Plants of Potatoes"));

    SqliteException uut(junkPath, 35, nullptr);
    EXPECT_THAT(uut.what(),
                StartsWith("SqliteException: SQLite error = 35; errorString = Potted Plants of Potatoes; file = "));
    EXPECT_THAT(uut.what(), EndsWith("data.base"));

    SqliteException vut(junkPath, 36, "");
    EXPECT_THAT(vut.what(),
                StartsWith("SqliteException: SQLite error = 36; errorString = Potted Plants of Potatoes; file = "));
    EXPECT_THAT(vut.what(), EndsWith("data.base"));

    SqliteException wut(junkPath, 37, "What gives here?");
    EXPECT_THAT(wut.what(),
                StartsWith("SqliteException: SQLite error = 37; errorString = Potted Plants of Potatoes; file = "));
    EXPECT_THAT(wut.what(), EndsWith("; context = What gives here?"));
}

TEST(SqliteException, FilePathPlusDBHPlusContext)
{
    SqliteErrmsgMock mockIt;
    sqlite3* db = reinterpret_cast<sqlite3*>(&mockIt);
    EXPECT_CALL(mockIt, errmsg(_)).WillRepeatedly(Return("Simpering Sinews of Silliness"));

    SqliteException uut(junkPath, db, nullptr);
    EXPECT_THAT(uut.what(),
                StartsWith("SqliteException during database access: errorString = Simpering Sinews of Silliness; file = "));
    EXPECT_THAT(uut.what(), EndsWith("data.base"));

    SqliteException vut(junkPath, db, "");
    EXPECT_THAT(vut.what(),
                StartsWith("SqliteException during database access: errorString = Simpering Sinews of Silliness; file = "));
    EXPECT_THAT(vut.what(), EndsWith("data.base"));

    SqliteException wut(junkPath, db, "What goes here?");
    EXPECT_THAT(wut.what(),
                StartsWith("SqliteException during database access: errorString = Simpering Sinews of Silliness; file = "));
    EXPECT_THAT(wut.what(), EndsWith("; context = What goes here?"));
}

TEST(SqliteException, TestExceptionDurinCtor)
{
    SqliteErrmsgMock mockIt;
    sqlite3* db = nullptr;

    SqliteException uut(junkPath, db, nullptr);
    EXPECT_STREQ("SqliteException: no details are available.", uut.what());
}
