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
// 2017.05.22 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// mock_sqlite_statement.hxx -- declaration of a mock to sqlite for statemnets
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <sqlite3.h>
#include <gmock/gmock.h>

namespace ansak
{

class SqliteStatementMock
{
public:
    static SqliteStatementMock* getMock() { return m_currentMock; }

    SqliteStatementMock();
    ~SqliteStatementMock();

    MOCK_METHOD1(clear_bindings, int(sqlite3_stmt*));
    MOCK_METHOD3(bind_double, int(sqlite3_stmt*, int, double));
    MOCK_METHOD3(bind_int, int(sqlite3_stmt*, int, int));
    MOCK_METHOD3(bind_int64, int(sqlite3_stmt*, int, sqlite3_int64));
    MOCK_METHOD5(bind_text, int(sqlite3_stmt*, int, const char*, int, void(*)(void*)));
    MOCK_METHOD5(bind_blob, int(sqlite3_stmt*, int, const void*, int, void(*)(void*)));
    MOCK_METHOD1(step, int(sqlite3_stmt*));
    MOCK_METHOD1(column_count, int(sqlite3_stmt*));
    MOCK_METHOD1(finalize, int(sqlite3_stmt*));
    MOCK_METHOD1(reset, int(sqlite3_stmt*));
    MOCK_METHOD1(sql, const char*(sqlite3_stmt*));
    MOCK_METHOD2(column_type, int(sqlite3_stmt*, int));
    MOCK_METHOD2(column_int, int(sqlite3_stmt*, int));
    MOCK_METHOD2(column_int64, sqlite3_int64(sqlite3_stmt*, int));
    MOCK_METHOD2(column_text, const unsigned char*(sqlite3_stmt*, int));
    MOCK_METHOD2(column_double, double(sqlite3_stmt*, int));
    MOCK_METHOD2(column_bytes, int(sqlite3_stmt*, int));
    MOCK_METHOD2(column_blob, const void*(sqlite3_stmt*, int));

private:

    static SqliteStatementMock* m_currentMock;
};

}
