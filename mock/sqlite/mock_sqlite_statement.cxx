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
// mock_sqlite_statement.cxx -- implementation of a mock to sqlite for statemnets
//
///////////////////////////////////////////////////////////////////////////

#include "mock_sqlite_statement.hxx"
#include <gmock/gmock.h>

using namespace ansak;

extern "C"
{

int sqlite3_clear_bindings(sqlite3_stmt* dbstmt)
{
    return SqliteStatementMock::getMock()->clear_bindings(dbstmt);
}

int sqlite3_bind_double(sqlite3_stmt* dbstmt, int nColumn, double value)
{
    return SqliteStatementMock::getMock()->bind_double(dbstmt, nColumn, value);
}

int sqlite3_bind_int(sqlite3_stmt* dbstmt, int nColumn, int value)
{
    return SqliteStatementMock::getMock()->bind_int(dbstmt, nColumn, value);
}

int sqlite3_bind_int64(sqlite3_stmt* dbstmt, int nColumn, sqlite3_int64 value)
{
    return SqliteStatementMock::getMock()->bind_int64(dbstmt, nColumn, value);
}

int sqlite3_bind_text(sqlite3_stmt* dbstmt, int nColumn, const char* value, int valueLength, void(*storage)(void*))
{
    return SqliteStatementMock::getMock()->bind_text(dbstmt, nColumn, value, valueLength, storage);
}

int sqlite3_bind_blob(sqlite3_stmt* dbstmt, int nColumn, const void* value, int valueLength, void(*storage)(void*))
{
    return SqliteStatementMock::getMock()->bind_blob(dbstmt, nColumn, value, valueLength, storage);
}

int sqlite3_step(sqlite3_stmt* dbstmt)
{
    return SqliteStatementMock::getMock()->step(dbstmt);
}

int sqlite3_finalize(sqlite3_stmt* dbstmt)
{
    return SqliteStatementMock::getMock()->finalize(dbstmt);
}

int sqlite3_column_count(sqlite3_stmt* dbstmt)
{
    return SqliteStatementMock::getMock()->column_count(dbstmt);
}

int sqlite3_reset(sqlite3_stmt* dbstmt)
{
    return SqliteStatementMock::getMock()->reset(dbstmt);
}

const char* sqlite3_sql(sqlite3_stmt* dbstmt)
{
    return SqliteStatementMock::getMock()->sql(dbstmt);
}

int sqlite3_column_type(sqlite3_stmt* dbstmt, int iCol)
{
    return SqliteStatementMock::getMock()->column_type(dbstmt, iCol);
}

int sqlite3_column_int(sqlite3_stmt* dbstmt, int iCol)
{
    return SqliteStatementMock::getMock()->column_int(dbstmt, iCol);
}

sqlite3_int64 sqlite3_column_int64(sqlite3_stmt* dbstmt, int iCol)
{
    return SqliteStatementMock::getMock()->column_int64(dbstmt, iCol);
}

const unsigned char* sqlite3_column_text(sqlite3_stmt* dbstmt, int iCol)
{
    return SqliteStatementMock::getMock()->column_text(dbstmt, iCol);
}

double sqlite3_column_double(sqlite3_stmt* dbstmt, int iCol)
{
    return SqliteStatementMock::getMock()->column_double(dbstmt, iCol);
}

int sqlite3_column_bytes(sqlite3_stmt* dbstmt, int iCol)
{
    return SqliteStatementMock::getMock()->column_bytes(dbstmt, iCol);
}

const void* sqlite3_column_blob(sqlite3_stmt* dbstmt, int iCol)
{
    return SqliteStatementMock::getMock()->column_blob(dbstmt, iCol);
}

}

namespace ansak
{

SqliteStatementMock* SqliteStatementMock::m_currentMock = nullptr;

SqliteStatementMock::SqliteStatementMock()
{
    m_currentMock = this;
}

SqliteStatementMock::~SqliteStatementMock()
{
    m_currentMock = nullptr;

}

}
