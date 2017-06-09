///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2014, Arthur N. Klassen
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
// 2014.11.01 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// sqlite_db.cxx -- Implementation of a class wrapping a connection to a
//                  SQLite Database, whether in memory or on disk.
//                  Depends on namespace ansak APIs from "base".
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_db.hxx>
#include <sqlite_exception.hxx>
#include <sqlite_statement_impl.hxx>
#include <file_system_path.hxx>
#include <runtime_exception.hxx>
#include <sstream>

using namespace std;
using namespace ansak::internal;

namespace ansak {

//===========================================================================
// public, static

int SqliteDB::getVersionNumber()
{
    return sqlite3_libversion_number();
}

//===========================================================================
// public, static

string SqliteDB::getVersion(bool giveSourceId)
{
    if (giveSourceId)
    {
        return string(sqlite3_sourceid());
    }
    else
    {
        return string(sqlite3_libversion());
    }
}

//===========================================================================
// public, Constructor

SqliteDB::SqliteDB
(
    const FilePath& filePath
) : m_path(filePath),
    m_systemPath(FilePath::invalidPath()),
    m_pDB(0),
    m_flagsUsed(0),
    m_statementsLock(),
    m_statementMap(),
    m_beginTran(),
    m_rollback(),
    m_commit(),
    m_inTransaction(false)
{
}

//===========================================================================
// public, Destructor

SqliteDB::~SqliteDB()
{
    if (m_pDB != 0)
    {
        try
        {
            close();
        }
        catch (...)
        {
        }
        m_pDB = 0;
    }
}

//===========================================================================
// public

void SqliteDB::create(int flags)
{
    enforce(m_pDB == 0, "SqliteDB::create called when file already open");
    enforce((flags & ~(kAllowedFlags & ~kReadOnly)) == 0,
                "SqliteDB::create flags included readonly");
    enforce((flags & (kAllowedFlags & ~kReadOnly)) != 0,
                "SqliteDB::create called without flags");
    if (m_path == FilePath::invalidPath())
    {
        flags |= kMemory;
    }
    else
    {
        if (!m_path.isReal())
        {
            throw SqliteException(m_path, "create called with OS incompatible path");
        }
        m_systemPath = FileSystemPath(m_path);
    }
    if ((flags & kMemory) == 0)
    {
        if (m_systemPath.exists())
        {
            throw SqliteException(m_path, "create found that the file exists");
        }
        auto parent = m_systemPath.parent();
        bool parentExists = parent.exists();
        if (parentExists && !parent.isDir())
        {
            throw SqliteException(m_path, "create file path parented by a file");
        }
        else if (!parentExists)
        {
            parentExists = parent.createDirectory(FileSystemPath::recursively);
        }
        if (!parentExists)
        {
            throw SqliteException(m_path, "create could not create the parent directory");
        }
    }

    int rc;
    if (m_path != FilePath::invalidPath())
    {
        rc = sqlite3_open_v2(m_path.asUtf8String().c_str(), &m_pDB,
               m_flagsUsed = flags | SQLITE_OPEN_CREATE, 0);
    }
    else
    {
        rc = sqlite3_open_v2(":memory:", &m_pDB,
               m_flagsUsed = flags | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY, 0);
    }

    if (rc != SQLITE_OK)
    {
        throw SqliteException(m_path, rc, "creating failed");
    }
}

//===========================================================================
// public

void SqliteDB::open(int flags)
{
    enforce(m_pDB == 0, "SqliteDB::open called when file already open");
    enforce((flags & kAllowedFlags) != 0,
                "SqliteDB::open called without flags");
    enforce((flags & ~kAllowedFlags) == 0,
                "SqliteDB::create flags included readonly");

    if (m_path != FilePath::invalidPath() && (flags & kMemory) == 0)
    {
        if (!m_path.isReal())
        {
            throw SqliteException(m_path, "open called with OS incompatible path");
        }
        m_systemPath = FileSystemPath(m_path);
    }

    if (m_systemPath.exists())
    {
        if (m_systemPath.isDir())
        {
            throw SqliteException(m_path, "open called with a directory");
        }
        m_flagsUsed = flags;
        auto rc = sqlite3_open_v2(m_path.asUtf8String().c_str(), &m_pDB, flags, 0);

        if (rc != SQLITE_OK)
        {
            throw SqliteException(m_path, rc, "open failed");
        }
    }
    else
    {
        throw SqliteException(m_path, "open called with non-existent file");
    }
}

//===========================================================================
// public

void SqliteDB::close()
{
    // clean up any statements that may still be open
    if (m_pDB)
    {
        if (inTransaction())
        {
            rollback();
        }
        for (auto s : m_statementMap)
        {
            auto stmtImpl = dynamic_cast<SqliteStatementImpl*>(s.second.get());
            enforce(stmtImpl != nullptr, "SqliteDB::close noticed improperly typed "
                                         "SqliteStatement*");
            stmtImpl->finalize();
        }

        sqlite3* pClosing = m_pDB;
        m_pDB = 0;

        auto rc = sqlite3_close(pClosing);
        if (rc != SQLITE_OK)
        {
            throw SqliteException(m_path, rc, "close failed");
        }
    }
}

//===========================================================================
// public

void SqliteDB::execute(const std::string& stmt)
{
    enforce(m_pDB != 0, "SqliteDB::execute called on closed database");
    if (!stmt.empty())
    {
        Statement dbStatement(prepare(stmt));
        dbStatement();
    }
}

//===========================================================================
// public

SqliteException SqliteDB::makeException(const char* contextString)
{
    enforce(m_pDB != 0, "SqliteDB::makeException called on closed database");
    return SqliteException(m_path, m_pDB, contextString);
}

//===========================================================================
// public

SqliteException SqliteDB::makeNotEnoughColumnsException(int actualColumns, int columnX)
{
    enforce(m_pDB != 0, "SqliteDB::makeNoResultException called on closed database");
    ostringstream os;
    os << "SQLite call received " << actualColumns
       << (actualColumns == 1 ? "column" : "columns")
       << ", while user wanted data at index, " << columnX << ".";
    return SqliteException(m_path, m_pDB, os.str().c_str());
}

//===========================================================================
// public

SqliteStatementPtr SqliteDB::prepareStatement(const string& stmt)
{
    enforce(m_pDB != 0, "SqliteDB::prepareStatement called on closed database");
    if (stmt.empty())
    {
        return SqliteStatementPtr();
    }

    sqlite3_stmt* theStatement = 0;
    auto rc = sqlite3_prepare_v2(m_pDB, stmt.c_str(), -1, &theStatement, 0);
    if (rc != SQLITE_OK || theStatement == nullptr)
    {
        throw SqliteException(m_path, m_pDB, "SqliteDB::prepareStatement failed");
    }
    auto stmtP = SqliteStatementPtr(new SqliteStatementImpl(*this, theStatement));

    {
        lock_guard<mutex> l(m_statementsLock);
        m_statementMap[stmtP->id()] = stmtP;
    }
    return stmtP;
}

//===========================================================================
// public

SqliteDB::Statement SqliteDB::prepare(const std::string& stmt)
{
    enforce(m_pDB != 0, "SqliteDB::prepare called on closed database");
    return Statement(*this, prepareStatement(stmt));
}

//===========================================================================
// public

void SqliteDB::finalizeStatement(SqliteStatement* stmt)
{
    enforce(m_pDB != 0, "SqliteDB::finalizeStatement called on closed database");
    auto stmtImpl = dynamic_cast<SqliteStatementImpl*>(stmt);
    enforce(stmtImpl != nullptr, "SqliteDB::finalizeStatement called on other sub-class "
                                 "than SqliteStatementImpl*");

    stmtImpl->finalize();
    lock_guard<mutex> l(m_statementsLock);
    m_statementMap.erase(stmt->id());
}

//===========================================================================
// public

void SqliteDB::finalizeStatement(SqliteStatementPtr& stmt)
{
    enforce(m_pDB != 0, "SqliteDB::finalizeStatement called on closed database");
    if (stmt.get())
    {
        finalizeStatement(stmt.get());
    }
}

//===========================================================================
// public

bool SqliteDB::inTransaction()
{
    enforce(m_pDB != 0, "SqliteDB::inTransaction called on closed database");
    return m_inTransaction;
}

//===========================================================================
// public

void SqliteDB::beginTransaction()
{
    enforce(m_pDB != 0, "SqliteDB::beginTransaction called on closed database");
    if (m_inTransaction)
    {
        return;
    }

    if (!m_beginTran.get())
    {
        m_rollback = prepareStatement("ROLLBACK TRANSACTION;");
        m_commit = prepareStatement("COMMIT TRANSACTION;");
        m_beginTran = prepareStatement("BEGIN TRANSACTION;");
    }
    auto rc = transactionExecute(m_beginTran);
    if (rc != SQLITE_OK)
    {
        throw SqliteException(m_path, rc, "SqliteDb::beginTransaction() failed");
    }
    m_inTransaction = true;
}

//===========================================================================
// public

void SqliteDB::rollback()
{
    enforce(m_pDB != 0, "SqliteDB::rollback called on closed database");

    if (inTransaction())
    {
        auto rc = transactionExecute(m_rollback);
        m_inTransaction = false;
        if (rc != SQLITE_OK)
        {
            throw SqliteException(m_path, rc, "SqliteDb::rollback() failed");
        }
    }
}

//===========================================================================
// public

void SqliteDB::commit()
{
    enforce(m_pDB != 0, "SqliteDB::commit called on closed database");

    if (inTransaction())
    {
        auto rc = transactionExecute(m_commit);
        if (rc != SQLITE_OK)
        {
            throw SqliteException(m_path, rc, "SqliteDb::beginTransaction() failed");
        }
        m_inTransaction = false;
    }
}

//===========================================================================
// public

FilePath SqliteDB::getDBPath() const
{
    if (isMemory())
    {
        return FilePath::invalidPath();
    }
    else
    {
        return m_path;
    }
}

//===========================================================================
// public

int SqliteDB::transactionExecute(SqliteStatementPtr& transactionQuery)
{
    auto stmtImpl = dynamic_cast<SqliteStatementImpl*>(transactionQuery.get());
    enforce(stmtImpl != nullptr, "SqliteDB::transactionExecute called on other sub-class "
                                 "than SqliteStatementImpl*");

    auto rc = sqlite3_step(stmtImpl->m_stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_OK)
    {
        return rc;
    }
    return sqlite3_reset(stmtImpl->m_stmt);
}

}
