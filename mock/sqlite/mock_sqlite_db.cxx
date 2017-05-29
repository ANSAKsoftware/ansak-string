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
// mock_sqlite_db.cxx -- implementation of a mock to sqlite for err msgs
//
///////////////////////////////////////////////////////////////////////////

#include "mock_sqlite_db.hxx"
#include <sqlite3.h>

using namespace ansak;

extern "C"
{

int sqlite3_libversion_number()
{
    return 99;
}

const char* sqlite3_sourceid()
{
    return "Vacuuous Vole";
}

const char* sqlite3_libversion()
{
    return "Illiberal Version";
}

int sqlite3_open_v2(const char* filePath, sqlite3** ppDB, int flags, const char* zvfs)
{
    return SqliteDBMock::getMock()->open_v2(filePath, ppDB, flags, zvfs);
}

int sqlite3_close(sqlite3* db)
{
    return SqliteDBMock::getMock()->close(db);
}

int sqlite3_prepare_v2(sqlite3* db, const char* sql, int flags, sqlite3_stmt** ppStmt, const char** ppzTail)
{
    return SqliteDBMock::getMock()->prepare_v2(db, sql, flags, ppStmt, ppzTail);
}

}

namespace ansak
{

SqliteDBMock* SqliteDBMock::m_currentMock = nullptr;

SqliteDBMock::SqliteDBMock()
{
    m_currentMock = this;
}

SqliteDBMock::~SqliteDBMock()
{
    m_currentMock = nullptr;

}

}
