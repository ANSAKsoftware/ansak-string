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
// 2017.05.24 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// test_sqlite_statement.cxx -- Tests for sqlite_statement.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_db.hxx>
#include <sqlite_statement_impl.hxx>
#include <mock_sqlite_db.hxx>
#include <mock_sqlite_errmsg.hxx>
#include <mock_sqlite_statement.hxx>
#include <file_path.hxx>
#include <sqlite_exception.hxx>
#include <runtime_exception.hxx>
#include <string.hxx>

#include <gmock/gmock.h>

using namespace ansak;
using namespace ansak::internal;
using namespace testing;
using namespace std;

class SqliteStmtFixture : public Test {
public:
    SqliteStmtFixture()
    {
        m_nextPreparedStmt = 1000000u;
    }

    void start(bool mockPrepare = true)
    {
        EXPECT_CALL(m_dbMock, open_v2(_,_,_,_)).
                WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<sqlite3*>(42)), Return(0)));
        EXPECT_CALL(m_dbMock, close(_)).WillOnce(Return(0));
        if (mockPrepare)
        {
            EXPECT_CALL(m_dbMock, prepare_v2(_, _, _, _, _)).
                    WillRepeatedly(DoAll(SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(m_nextPreparedStmt)),
                                         Return(0)));
        }

        m_uut.create();
    }

    SqliteDBMock& DBMock() { return m_dbMock; }
    SqliteErrmsgMock& ErrMsgMock() { return m_msgMock; }
    SqliteStatementMock& StatementMock() { return m_stmtMock; }

    SqliteDB& uut() { ++m_nextPreparedStmt; return m_uut; }

private:
    NiceMock<SqliteErrmsgMock> m_msgMock;
    NiceMock<SqliteDBMock> m_dbMock;
    NiceMock<SqliteStatementMock> m_stmtMock;

    SqliteDB        m_uut;
    unsigned long   m_nextPreparedStmt;
};

TEST_F(SqliteStmtFixture, executeOnClosedDB)
{
    EXPECT_THROW(uut().execute("Do Something"), RuntimeException);
}

TEST_F(SqliteStmtFixture, execute)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
            SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(7707)), Return(0)));
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    EXPECT_CALL(StatementMock(), finalize(_)).WillOnce(Return(0));

    start(false);

    uut().execute("Do Something");
}

TEST_F(SqliteStmtFixture, prepareEmpty)
{
    start(false);

    EXPECT_EQ(SqliteStatementPointer(), uut().prepareStatement(string()));
}

TEST_F(SqliteStmtFixture, prepareFailure)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(Return(35));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("Doin' Nuthin'"));
    start(false);

    EXPECT_THROW(uut().prepareStatement("Do Something"), SqliteException);
}

TEST_F(SqliteStmtFixture, prepareSuccess)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
            SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(111)),
            Return(0)));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    EXPECT_TRUE(s);
    EXPECT_NE(nullptr, dynamic_cast<SqliteStatementImpl*>(s.get()));
}

TEST_F(SqliteStmtFixture, bindABunch)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_double(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(StatementMock(), bind_int(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(StatementMock(), bind_int64(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(StatementMock(), bind_text(_, _, _, _, _)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(StatementMock(), bind_blob(_, _, _, _, _)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(StatementMock(), clear_bindings(_)).WillOnce(Return(0));
    EXPECT_CALL(StatementMock(), reset(_)).WillOnce(Return(0)).WillOnce(Return(0));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    auto si = dynamic_cast<SqliteStatementImpl*>(s.get());
    s->bind(0, 3.2910);
    s->bind(1, 2);
    s->bind(2, 19208098509128ll);
    s->bind(3, string("This is a great day for a picnic."));
    s->bind(4, "This is not a great day for a picnic");
    vector<char> data = { 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'n', 'o', 't', ' ',
                          't', 'h', 'e', ' ', 'w', 'a', 'y', ' ', 't', 'o', ' ', 'd',
                          'o', ' ', 't', 'h', 'i', 's' };
    s->bind(5, data);
    static const char* doto = "That is the way to this.";
    s->bind(6, doto, 30);
    EXPECT_TRUE(si->hasBindings());
    s->reset(true);
    EXPECT_TRUE(si->hasBindings());
    s->reset();
    EXPECT_FALSE(si->hasBindings());
}

TEST_F(SqliteStmtFixture, throwOnBindD)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_double(_, _, _)).WillOnce(Return(20));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    EXPECT_THROW(s->bind(0, 3.2910), SqliteException);
}

TEST_F(SqliteStmtFixture, throwOnBindI)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_int(_, _, _)).WillOnce(Return(20));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    EXPECT_THROW(s->bind(1, 2), SqliteException);
}

TEST_F(SqliteStmtFixture, throwOnBindI64)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_int64(_, _, _)).WillOnce(Return(20));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    EXPECT_THROW(s->bind(2, 19208098509128ll), SqliteException);
}

TEST_F(SqliteStmtFixture, throwOnBindS1)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_text(_, _, _, _, _)).WillOnce(Return(20));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    EXPECT_THROW(s->bind(3, string("This is a great day for a picnic.")), SqliteException);
}

TEST_F(SqliteStmtFixture, throwOnBindS2)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_text(_, _, _, _, _)).WillOnce(Return(20));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    EXPECT_THROW(s->bind(4, "This is not a great day for a picnic"), SqliteException);
}

TEST_F(SqliteStmtFixture, throwOnBindB1)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_blob(_, _, _, _, _)).WillOnce(Return(20));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    vector<char> data = { 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'n', 'o', 't', ' ',
                          't', 'h', 'e', ' ', 'w', 'a', 'y', ' ', 't', 'o', ' ', 'd',
                          'o', ' ', 't', 'h', 'i', 's' };
    EXPECT_THROW(s->bind(5, data), SqliteException);
}

TEST_F(SqliteStmtFixture, throwOnBindB2)
{
    EXPECT_CALL(DBMock(), prepare_v2(_, _, _, _, _)).WillOnce(DoAll(
        SetArgPointee<3>(reinterpret_cast<sqlite3_stmt*>(8808)),
        Return(0)));
    EXPECT_CALL(StatementMock(), bind_blob(_, _, _, _, _)).WillOnce(Return(20));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start(false);

    auto s = uut().prepareStatement("Do Something");
    static const char* doto = "That is the way to this.";
    EXPECT_THROW(s->bind(6, doto, 30), SqliteException);
}

TEST_F(SqliteStmtFixture, noRetrieval)
{
    start();

    auto s = uut().prepareStatement("Do something");
    auto si = dynamic_cast<SqliteStatementImpl*>(s.get());
    EXPECT_FALSE(si->hasRetrievals());
    EXPECT_EQ(0ul, si->numRetrievals());
}

TEST_F(SqliteStmtFixture, getSql)
{
    EXPECT_CALL(StatementMock(), sql(_)).WillOnce(Return("This is the end."));
    start();
    auto s = uut().prepareStatement("This is not the end.");

    EXPECT_EQ(s->sql(), string("This is the end."));
}

TEST_F(SqliteStmtFixture, getEmptySql)
{
    EXPECT_CALL(StatementMock(), sql(_)).WillOnce(Return(nullptr));
    start();
    auto s = uut().prepareStatement("This is not the end.");

    EXPECT_TRUE(s->sql().empty());
}

TEST_F(SqliteStmtFixture, retrievalSetups)
{
    EXPECT_CALL(StatementMock(), reset(_)).WillOnce(Return(0)).WillOnce(Return(0));
    start();

    auto stmt = uut().prepareStatement("select * from everything;");
    auto stmti = dynamic_cast<SqliteStatementImpl*>(stmt.get());
    double d = 0.0;
    int n = 0;
    long long n64 = 0ll;
    string s;
    vector<char> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool dIsNull = false;
    bool nIsNull = false;
    bool n64IsNull = false;
    bool textIsNull = false;
    bool blobIsNull = false;
    stmt->setupRetrieval(0, d, &dIsNull);
    stmt->setupRetrieval(1, n, &nIsNull);
    stmt->setupRetrieval(2, n64, &n64IsNull);
    stmt->setupRetrieval(3, s, &textIsNull);
    stmt->setupRetrieval(4, data, &blobIsNull);

    EXPECT_TRUE(stmti->hasRetrievals());
    EXPECT_EQ(5ul, stmti->numRetrievals());

    EXPECT_THAT(d, DoubleEq(0.0));
    EXPECT_EQ(0, n);
    EXPECT_EQ(0ll, n64);
    EXPECT_TRUE(s.empty());
    auto dConglomerate = data[ 0] | data[ 1] | data[ 2] | data[ 3] | data[ 4] | data[ 5] | data[ 6] | data[ 7] | data[ 8] | data[ 9]
                       | data[10] | data[11] | data[12] | data[13] | data[14] | data[15] | data[16] | data[17] | data[18] | data[19]
                       | data[20] | data[21] | data[22] | data[23] | data[24] | data[25] | data[26] | data[27] | data[28] | data[29]
                       | data[30] | data[31] | data[32] | data[33] | data[34] | data[35] | data[36] | data[37] | data[38] | data[39];
    EXPECT_EQ(0, dConglomerate);
    EXPECT_FALSE(dIsNull);
    EXPECT_FALSE(nIsNull);
    EXPECT_FALSE(n64IsNull);
    EXPECT_FALSE(textIsNull);
    EXPECT_FALSE(blobIsNull);

    stmt->reset(true);
    EXPECT_TRUE(stmti->hasRetrievals());
    EXPECT_EQ(5ul, stmti->numRetrievals());

    stmt->reset();
    EXPECT_FALSE(stmti->hasRetrievals());
    EXPECT_EQ(0ul, stmti->numRetrievals());
}

TEST_F(SqliteStmtFixture, retrievalsOneDone)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_DONE));
    start();

    auto stmt = uut().prepareStatement("select * from everything;");
    double d = 0.0;
    int n = 0;
    long long n64 = 0ll;
    string s;
    vector<char> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool dIsNull = false;
    bool nIsNull = false;
    bool n64IsNull = false;
    bool textIsNull = false;
    bool blobIsNull = false;
    stmt->setupRetrieval(0, d, &dIsNull);
    stmt->setupRetrieval(1, n, &nIsNull);
    stmt->setupRetrieval(2, n64, &n64IsNull);
    stmt->setupRetrieval(3, s, &textIsNull);
    stmt->setupRetrieval(4, data, &blobIsNull);

    bool done = false;
    (*stmt)(&done);
    EXPECT_TRUE(done);
}

TEST_F(SqliteStmtFixture, retrievalsOneThrows)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(35));
    EXPECT_CALL(ErrMsgMock(), errmsg(_)).WillOnce(Return("This is an error!"));
    start();

    auto stmt = uut().prepareStatement("select * from everything;");
    double d = 0.0;
    int n = 0;
    long long n64 = 0ll;
    string s;
    vector<char> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool dIsNull = false;
    bool nIsNull = false;
    bool n64IsNull = false;
    bool textIsNull = false;
    bool blobIsNull = false;
    stmt->setupRetrieval(0, d, &dIsNull);
    stmt->setupRetrieval(1, n, &nIsNull);
    stmt->setupRetrieval(2, n64, &n64IsNull);
    stmt->setupRetrieval(3, s, &textIsNull);
    stmt->setupRetrieval(4, data, &blobIsNull);

    EXPECT_THROW((*stmt)(), SqliteException);
}

TEST_F(SqliteStmtFixture, retrievalsOneTooFewColumns)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_ROW));
    EXPECT_CALL(StatementMock(), column_count(_)).WillOnce(Return(2));
    start();

    auto stmt = uut().prepareStatement("select * from everything;");
    double d = 0.0;
    int n = 0;
    long long n64 = 0ll;
    string s;
    vector<char> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool dIsNull = false;
    bool nIsNull = false;
    bool n64IsNull = false;
    bool textIsNull = false;
    bool blobIsNull = false;
    stmt->setupRetrieval(0, d, &dIsNull);
    stmt->setupRetrieval(1, n, &nIsNull);
    stmt->setupRetrieval(2, n64, &n64IsNull);
    stmt->setupRetrieval(3, s, &textIsNull);
    stmt->setupRetrieval(4, data, &blobIsNull);

    EXPECT_THROW((*stmt)(), SqliteException);
}

TEST_F(SqliteStmtFixture, retrievalsOneBlandness)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(0));
    start();

    auto stmt = uut().prepareStatement("select * from everything;");
    double d = 0.0;
    int n = 0;
    long long n64 = 0ll;
    string s;
    vector<char> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool dIsNull = false;
    bool nIsNull = false;
    bool n64IsNull = false;
    bool textIsNull = false;
    bool blobIsNull = false;
    stmt->setupRetrieval(0, d, &dIsNull);
    stmt->setupRetrieval(1, n, &nIsNull);
    stmt->setupRetrieval(2, n64, &n64IsNull);
    stmt->setupRetrieval(3, s, &textIsNull);
    stmt->setupRetrieval(4, data, &blobIsNull);

    EXPECT_THROW((*stmt)(), SqliteException);
}

TEST_F(SqliteStmtFixture, retrievalsOneRow)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(SQLITE_ROW));
    EXPECT_CALL(StatementMock(), column_count(_)).WillOnce(Return(5));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(0))).WillOnce(Return(SQLITE_FLOAT));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(1))).WillOnce(Return(SQLITE_INTEGER));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(2))).WillOnce(Return(SQLITE_INTEGER));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(3))).WillOnce(Return(SQLITE_TEXT));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(4))).WillOnce(Return(SQLITE_BLOB));
    EXPECT_CALL(StatementMock(), column_double(_, 0)).WillOnce(Return(3.14159265));
    EXPECT_CALL(StatementMock(), column_int(_, 1)).WillOnce(Return(12321));
    EXPECT_CALL(StatementMock(), column_int64(_, 2)).WillOnce(Return(0xBAADF00DEA717500));
    const unsigned char theString[] = "Now is the time for all good men to sleep!";
    EXPECT_CALL(StatementMock(), column_text(_, 3)).WillOnce(Return(&theString[0]));
    EXPECT_CALL(StatementMock(), column_bytes(_, 4)).WillOnce(Return(10));
    EXPECT_CALL(StatementMock(), column_blob(_, 4)).WillOnce(Return("0123456789"));
    start();

    auto stmt = uut().prepareStatement("select * from everything;");
    double d = 0.0;
    int n = 0;
    long long n64 = 0ll;
    string s;
    vector<char> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool dIsNull = false;
    bool nIsNull = false;
    bool n64IsNull = false;
    bool textIsNull = false;
    bool blobIsNull = false;
    stmt->setupRetrieval(0, d, &dIsNull);
    stmt->setupRetrieval(1, n, &nIsNull);
    stmt->setupRetrieval(2, n64, &n64IsNull);
    stmt->setupRetrieval(3, s, &textIsNull);
    stmt->setupRetrieval(4, data, &blobIsNull);

    (*stmt)();
    EXPECT_FALSE(dIsNull); EXPECT_FALSE(nIsNull); EXPECT_FALSE(n64IsNull); EXPECT_FALSE(textIsNull); EXPECT_FALSE(blobIsNull);
    EXPECT_THAT(d, DoubleEq(3.14159265));
    EXPECT_EQ(n, 12321);
    EXPECT_EQ(n64, static_cast<long long>(0xBAADF00DEA717500ll));
    EXPECT_EQ(s, string("Now is the time for all good men to sleep!"));
    EXPECT_EQ(data.size(), 10ul);
    EXPECT_EQ(data[0], '0'); EXPECT_EQ(data[1], '1'); EXPECT_EQ(data[2], '2'); EXPECT_EQ(data[3], '3'); EXPECT_EQ(data[4], '4');
    EXPECT_EQ(data[5], '5'); EXPECT_EQ(data[6], '6'); EXPECT_EQ(data[7], '7'); EXPECT_EQ(data[8], '8'); EXPECT_EQ(data[9], '9');
}

TEST_F(SqliteStmtFixture, retrievalsTwoAndDone)
{
    EXPECT_CALL(StatementMock(), step(_)).WillOnce(Return(0)).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_DONE)).WillOnce(Return(0));
    EXPECT_CALL(StatementMock(), column_count(_)).WillRepeatedly(Return(5));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(0))).WillRepeatedly(Return(SQLITE_FLOAT));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(1))).WillRepeatedly(Return(SQLITE_INTEGER));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(2))).WillRepeatedly(Return(SQLITE_INTEGER));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(3))).WillRepeatedly(Return(SQLITE_TEXT));
    EXPECT_CALL(StatementMock(), column_type(_, Eq(4))).WillOnce(Return(SQLITE_BLOB)).WillOnce(Return(SQLITE_NULL));;
    EXPECT_CALL(StatementMock(), column_double(_, 0)).WillOnce(Return(3.14159265)).WillOnce(Return(2.7182818301));
    EXPECT_CALL(StatementMock(), column_int(_, 1)).WillOnce(Return(12321)).WillOnce(Return(32123));
    EXPECT_CALL(StatementMock(), column_int64(_, 2)).WillOnce(Return(0xBAADF00DEA717500)).WillOnce(Return(0));;
    const unsigned char theString[] = "Now is the time for all good men to sleep!";
    const unsigned char theOtherString[] = "Sing ho for bath at close of day that washes the weary mud away!";
    EXPECT_CALL(StatementMock(), column_text(_, 3)).WillOnce(Return(&theString[0])).WillOnce(Return(&theOtherString[0]));
    EXPECT_CALL(StatementMock(), column_bytes(_, 4)).WillOnce(Return(10));
    EXPECT_CALL(StatementMock(), column_blob(_, 4)).WillOnce(Return("0123456789"));
    start();

    auto stmt = uut().prepareStatement("select * from everything;");
    stmt->setBeginTransaction();
    stmt->setEndTransaction();
    double d = 0.0;
    int n = 0;
    long long n64 = 0ll;
    string s;
    vector<char> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool dIsNull = false;
    bool nIsNull = false;
    bool n64IsNull = false;
    bool textIsNull = false;
    bool blobIsNull = false;
    stmt->setupRetrieval(0, d, &dIsNull);
    stmt->setupRetrieval(1, n, &nIsNull);
    stmt->setupRetrieval(2, n64, &n64IsNull);
    stmt->setupRetrieval(3, s, &textIsNull);
    stmt->setupRetrieval(4, data, &blobIsNull);

    bool done = false;
    (*stmt)(&done);
    EXPECT_FALSE(done);
    EXPECT_FALSE(dIsNull); EXPECT_FALSE(nIsNull); EXPECT_FALSE(n64IsNull); EXPECT_FALSE(textIsNull); EXPECT_FALSE(blobIsNull);
    EXPECT_THAT(d, DoubleEq(3.14159265));
    EXPECT_EQ(n, 12321);
    EXPECT_EQ(n64, static_cast<long long>(0xBAADF00DEA717500ll));
    EXPECT_EQ(s, string("Now is the time for all good men to sleep!"));
    EXPECT_EQ(data.size(), 10ul);
    EXPECT_EQ(data[0], '0'); EXPECT_EQ(data[1], '1'); EXPECT_EQ(data[2], '2'); EXPECT_EQ(data[3], '3'); EXPECT_EQ(data[4], '4');
    EXPECT_EQ(data[5], '5'); EXPECT_EQ(data[6], '6'); EXPECT_EQ(data[7], '7'); EXPECT_EQ(data[8], '8'); EXPECT_EQ(data[9], '9');

    (*stmt)(&done);
    EXPECT_FALSE(done);
    EXPECT_FALSE(dIsNull); EXPECT_FALSE(nIsNull); EXPECT_FALSE(n64IsNull); EXPECT_FALSE(textIsNull); EXPECT_TRUE(blobIsNull);
    EXPECT_THAT(d, DoubleEq(2.7182818301));
    EXPECT_EQ(n, 32123);
    EXPECT_EQ(n64, static_cast<long long>(0));
    EXPECT_EQ(s, string("Sing ho for bath at close of day that washes the weary mud away!"));

    (*stmt)(&done);
    EXPECT_TRUE(done);
}
