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
// 2017.05.17 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// sqlite_exception.cxx -- Implementation of an exception class for SQLite
//                         exception.
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_exception.hxx>
#include <runtime_exception.hxx>
#include <file_path.hxx>
#include <sstream>

using namespace std;

namespace ansak {

//===========================================================================
// public, constructor

SqliteException::SqliteException
(
    const FilePath&     dbFile,
    const char*         context
) noexcept
{
    try
    {
        ostringstream os;
        os << "SqliteException: Problem with file = " <<
              (dbFile == FilePath::invalidPath() ? "memory" : dbFile.asUtf8String());
        if (context != nullptr && *context != '\0')
        {
            os << "; context = " << context;
        }
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//===========================================================================
// public, constructor

SqliteException::SqliteException
(
    const FilePath&     dbFile,
    int                 sqliteCode,
    const char*         context
) noexcept
{
    try
    {
        ostringstream os;
        os << "SqliteException: SQLite error = " << sqliteCode
           << "; errorString = " << sqlite3_errstr(sqliteCode)
           << "; file = " <<
              (dbFile == FilePath::invalidPath() ? "memory" : dbFile.asUtf8String());
        if (context != nullptr && *context != '\0')
        {
            os << "; context = " << context;
        }
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//===========================================================================
// public, constructor

SqliteException::SqliteException
(
    const FilePath&     dbFile,
    sqlite3*            db,
    const char*         context
) noexcept
{
    try
    {
        enforce(db != nullptr, "Must have open database for sqlite3_errmsg-based exception.");
        ostringstream os;
        os << "SqliteException during database access: errorString = "
           << sqlite3_errmsg(db) << "; file = " <<
              (dbFile == FilePath::invalidPath() ? "memory" : dbFile.asUtf8String());
        if (context != nullptr && *context != '\0')
        {
            os << "; context = " << context;
        }
        m_what = os.str();
    }
    catch (...)
    {
    }
}

//===========================================================================
// public

SqliteException::~SqliteException() noexcept
{
}

const char* SqliteException::what() const noexcept
{
    try
    {
        if (!m_what.empty())
        {
            return m_what.c_str();
        }
    }
    catch (...)
    {
        static const char exceptedWhat[] = "SqliteException - throw on what()";
        return exceptedWhat;
    }
    static const char emptyWhat[] = "SqliteException: no details are available.";
    return emptyWhat;
}

}
