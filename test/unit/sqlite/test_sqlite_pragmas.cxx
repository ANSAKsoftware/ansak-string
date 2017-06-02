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
// 2017.05.30 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// test_sqlite_pragmas.cxx -- Tests for sqlite_db_pragmas.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_db_pragmas.hxx>

#include <sqlite_db.hxx>
#include <sqlite_statement_impl.hxx>
#include <mock_sqlite_db.hxx>
#include <mock_sqlite_errmsg.hxx>
#include <mock_sqlite_statement.hxx>
#include <mock_file_system_path.hxx>
#include <file_path.hxx>
#include <sqlite_exception.hxx>
#include <runtime_exception.hxx>
#include <string.hxx>
#include <memory>

#include <gmock/gmock.h>

using namespace ansak;
using namespace ansak::sqlite;
using namespace testing;
using namespace std;

namespace {

#if defined(WIN32)

FilePath junkPath("C:\\Users\\jvplasmeuden\\data.base");

#else

FilePath junkPath("/home/jvplasmeuden/data/data.base");

#endif

}


class SqliteDBPragmaFixture : public Test {
public:
    SqliteDBPragmaFixture()
    {
        m_nextPreparedStmt = 2000000u;
    }

    void start(bool mockPrepare = true, const FilePath& uutPath = FilePath::invalidPath())
    {
        m_uutPath = uutPath;
        m_uut.reset(new SqliteDB(m_uutPath));

        EXPECT_CALL(m_dbMock, open_v2(_,_,_,_)).
                WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42ul)), Return(0)));
        EXPECT_CALL(m_dbMock, close(_)).WillOnce(Return(0));
        if (mockPrepare)
        {
            unsigned long long stmtX = m_nextPreparedStmt;
            auto stmtVal = reinterpret_cast<sqlite3_stmt*>(stmtX);
            EXPECT_CALL(m_dbMock, prepare_v2(_, _, _, _, _)).
                    WillRepeatedly(DoAll(SetArgPointee<3>(stmtVal), Return(0)));
        }

        m_uut->create();
    }

    SqliteDBMock& DBMock() { return m_dbMock; }
    SqliteErrmsgMock& ErrMsgMock() { return m_msgMock; }
    SqliteStatementMock& StatementMock() { return m_stmtMock; }

    SqliteDB& uut() { ++m_nextPreparedStmt; return *(m_uut.get()); }

    sqlite3_stmt* nextStatementID() const
    {
        unsigned long long stmtX = m_nextPreparedStmt;
        return reinterpret_cast<sqlite3_stmt*>(stmtX);
    }

private:
    NiceMock<SqliteErrmsgMock> m_msgMock;
    NiceMock<SqliteDBMock> m_dbMock;
    NiceMock<SqliteStatementMock> m_stmtMock;

    FilePath                m_uutPath = FilePath::invalidPath();
    unique_ptr<SqliteDB>    m_uut;
    unsigned long           m_nextPreparedStmt;
};

TEST_F(SqliteDBPragmaFixture, getApplicationID)
{
    EXPECT_CALL(StatementMock(), column_count(_)).WillOnce(Return(1));
    EXPECT_CALL(StatementMock(), column_type(_, _)).WillOnce(Return(SQLITE_INTEGER));
    EXPECT_CALL(StatementMock(), column_int(_, _)).WillOnce(Return(42));
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_ROW));
    start();

    auto id = getApplicationID(uut());
    EXPECT_EQ(42, id);
}

TEST_F(SqliteDBPragmaFixture, setApplicationID)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, HasSubstr("35"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setApplicationID(uut(), 35);
}

TEST_F(SqliteDBPragmaFixture, getUserVersino)
{
    EXPECT_CALL(StatementMock(), column_count(_)).WillOnce(Return(1));
    EXPECT_CALL(StatementMock(), column_type(_, _)).WillOnce(Return(SQLITE_INTEGER));
    EXPECT_CALL(StatementMock(), column_int(_, _)).WillOnce(Return(88));
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_ROW));
    start();

    auto id = getUserVersion(uut());
    EXPECT_EQ(88, id);
}

TEST_F(SqliteDBPragmaFixture, setUserVersion)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, HasSubstr("89"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setUserVersion(uut(), 89);
}

TEST_F(SqliteDBPragmaFixture, setSecureDelete)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma secure_delete = 1;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setSecureDelete(uut());
}

TEST_F(SqliteDBPragmaFixture, setMemoryJournaling)
{
    EXPECT_CALL(StatementMock(), step(_)).WillRepeatedly(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma journal_mode = off;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(35ul)),
                  Return(0)));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma journal_mode = memory;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(36ul)),
                  Return(0)));
    start(false);

    setJournaling(uut(), true);
    setJournaling(uut(), false);
}

TEST_F(SqliteDBPragmaFixture, setFileJournaling)
{
    FileSystemPathMock theFSMock;
    FileSystemPath parent(junkPath.parent());
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_CALL(theFSMock, parent(_)).WillOnce(Return(parent));
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(true));

    EXPECT_CALL(StatementMock(), step(_)).WillRepeatedly(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma journal_mode = off;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(35ul)),
                  Return(0)));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma journal_mode = delete;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(36ul)),
                  Return(0)));
    start(false, junkPath);

    setJournaling(uut(), true);
    setJournaling(uut(), false);
}

TEST_F(SqliteDBPragmaFixture, getMemoryJournaling)
{
    EXPECT_CALL(StatementMock(), step(_)).WillRepeatedly(Return(SQLITE_ROW));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma journal_mode;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(35ul)),
                  Return(0))).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(36ul)),
                  Return(0)));
    EXPECT_CALL(StatementMock(), column_count(_)).WillRepeatedly(Return(1));
    EXPECT_CALL(StatementMock(), column_type(_,_)).WillRepeatedly(Return(SQLITE_TEXT));
    EXPECT_CALL(StatementMock(), column_text(_,_)).WillOnce(Return(reinterpret_cast<const unsigned char*>("none"))).
                                                   WillOnce(Return(reinterpret_cast<const unsigned char*>("memory")));
    start(false);

    EXPECT_FALSE(getJournaling(uut()));
    EXPECT_TRUE(getJournaling(uut()));
}

TEST_F(SqliteDBPragmaFixture, getFileJournaling)
{
    FileSystemPathMock theFSMock;
    FileSystemPath parent(junkPath.parent());
    EXPECT_CALL(theFSMock, exists(_)).WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_CALL(theFSMock, parent(_)).WillOnce(Return(parent));
    EXPECT_CALL(theFSMock, isDir(_)).WillOnce(Return(true));

    EXPECT_CALL(StatementMock(), step(_)).WillRepeatedly(Return(SQLITE_ROW));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma journal_mode;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(35ul)),
                  Return(0))).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(36ul)),
                  Return(0)));
    EXPECT_CALL(StatementMock(), column_count(_)).WillRepeatedly(Return(1));
    EXPECT_CALL(StatementMock(), column_type(_,_)).WillRepeatedly(Return(SQLITE_TEXT));
    EXPECT_CALL(StatementMock(), column_text(_,_)).WillOnce(Return(reinterpret_cast<const unsigned char*>("off"))).
                                                   WillOnce(Return(reinterpret_cast<const unsigned char*>("delete")));
    start(false, junkPath);

    EXPECT_FALSE(getJournaling(uut()));
    EXPECT_TRUE(getJournaling(uut()));
}

TEST_F(SqliteDBPragmaFixture, setUtf8Encoding)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma encoding = 'UTF-8';"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setUTF8Encoding(uut());
}

TEST_F(SqliteDBPragmaFixture, setForeignKeys)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma foreign_keys = 1;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setForeignKeys(uut());
}

TEST_F(SqliteDBPragmaFixture, setNoVacuuming)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma auto_vacuum = 0;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setNoVacuuming(uut());
}

TEST_F(SqliteDBPragmaFixture, setVacuuming)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma auto_vacuum = 1;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setVacuuming(uut());
}

TEST_F(SqliteDBPragmaFixture, setIncVacuuming)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma auto_vacuum = 2;"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    setIncrementalVacuuming(uut());
}

TEST_F(SqliteDBPragmaFixture, vacuumNow)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(DBMock(), prepare_v2(_, StrEq("pragma incremental_vacuum(4);"), _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(nextStatementID())),
                  Return(0)));
    start(false);

    vacuumNow(uut(), false);
}

TEST_F(SqliteDBPragmaFixture, integrity)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_ROW))
                                         .WillOnce(Return(SQLITE_ROW))
                                         .WillOnce(Return(SQLITE_DONE))
                                         .WillOnce(Return(SQLITE_ROW))
                                         .WillOnce(Return(SQLITE_ROW))
                                         .WillOnce(Return(SQLITE_ROW))
                                         .WillOnce(Return(SQLITE_ROW))
                                         .WillOnce(Return(SQLITE_DONE))
                                         .WillOnce(Return(SQLITE_ROW))
                                         .WillOnce(Return(SQLITE_ROW));
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(44ul)), Return(0))).
            WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(46ul)), Return(0))).
            WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(47ul)), Return(0))).
            WillOnce(
            DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(48ul)), Return(0)));
    
    EXPECT_CALL(StatementMock(), column_count(_)).WillRepeatedly(Return(1));
    EXPECT_CALL(StatementMock(), column_type(_,_)).WillRepeatedly(Return(SQLITE_TEXT));
    EXPECT_CALL(StatementMock(), column_text(_,_)).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("I think therefore I am"))).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("But I am micro-managed therefore I am not."))).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("Twas brillig and the slithy toves"))).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("Did gyre and gimble in the wabe"))).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("All mimsy were the borogoves"))).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("And the mome raths outgrabe."))).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("ok"))).
            WillOnce(Return(reinterpret_cast<const unsigned char*>("ok")));
    start(false);
    vector<string> messages;

    EXPECT_FALSE(checkIntegrity(uut(), &messages));
    EXPECT_TRUE(checkIntegrity(uut()));
}
