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
// sqlite_statement_impl.cxx -- Implementation of a class to wrap individual
//                         SQLite statements once prepared against a particular
//                         database, to bind data values into it, to retrieve
//                         results from it.
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_statement_impl.hxx>
#include <sqlite_db.hxx>
#include <sqlite_exception.hxx>
#include <runtime_exception.hxx>
#include <mutex>
#include <cstring>

using namespace std;
using namespace ansak;

namespace ansak
{

namespace internal
{

namespace
{
    // Serial number of each prepared statement for life of the process, and lock
    int nextID = 1;
    mutex nextIDLock;

    //=======================================================================
    // getNextID
    //
    // Retrieves the next ID for statements -- used in wrapping DB's map of
    // statements

    int getNextID()
    {
        lock_guard<mutex> l(nextIDLock);
        auto id = nextID++;
        if (nextID == 0) ++nextID;
        return id;
    }

}

//===========================================================================
// private, Constructor

SqliteStatementImpl::SqliteStatementImpl
(
    SqliteDB&           db,         // I - a pointer to the sqliteDB object
    sqlite3_stmt*       stmt        // I - the SQLite3-prepared statement
) : m_statementLock(),
    m_db(db),
    m_stmt(stmt),
    m_id(getNextID()),
    m_retrieveColumns(),
    m_started(false),
    m_done(false),
    m_bindingsMade(false),
    m_beginTransaction(false),
    m_endTransaction(false)
{
}

//===========================================================================
// public, Destructor

SqliteStatementImpl::~SqliteStatementImpl()
{
    enforce(m_stmt == 0, "Wrapping database finalizes before disposing statements.");
}

//===========================================================================
// public

void SqliteStatementImpl::bind
(
    int         n,          // I - a 0-based index into the statement
    double      d           // I - a value to bind into the statement
)
{
    enforce(m_stmt != 0, "No active statement being wrapped.");

    int rc = sqlite3_bind_double(m_stmt, n + 1, d);
    if (rc != SQLITE_OK)
    {
        auto e = m_db.makeException("SqliteStatementImpl::bind(int, double)");
        m_db.rollback();
        throw e;
    }

    m_bindingsMade = true;
}

//===========================================================================
// public

void SqliteStatementImpl::bind
(
    int         n,          // I - a 0-based index into the statement
    int         i           // I - a value to bind into the statement
)
{
    enforce(m_stmt != 0, "No active statement being wrapped.");

    int rc = sqlite3_bind_int(m_stmt, n + 1, i);
    if (rc != SQLITE_OK)
    {
        auto e = m_db.makeException("SqliteStatementImpl::bind(int, int)");
        m_db.rollback();
        throw e;
    }

    m_bindingsMade = true;
}

//===========================================================================
// public

void SqliteStatementImpl::bind
(
    int         n,          // I - a 0-based index into the statement
    long long   i64         // I - a value to bind into the statement
)
{
    enforce(m_stmt != 0, "No active statement being wrapped.");

    int rc = sqlite3_bind_int64(m_stmt, n + 1, i64);
    if (rc != SQLITE_OK)
    {
        auto e = m_db.makeException("SqliteStatementImpl::bind(int, long long)");
        m_db.rollback();
        throw e;
    }

    m_bindingsMade = true;
}

//===========================================================================
// public

void SqliteStatementImpl::bind
(
    int             n,          // I - a 0-based index into the statement
    const string&   text        // I - a value to bind into the statement
)
{
    enforce(m_stmt != 0, "No active statement being wrapped.");

    int rc = sqlite3_bind_text(m_stmt, n + 1, text.c_str(), static_cast<int>(text.size()), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        auto e = m_db.makeException("SqliteStatementImpl::bind(int, const string&)");
        m_db.rollback();
        throw e;
    }

    m_bindingsMade = true;
}

//===========================================================================
// public

void SqliteStatementImpl::bind
(
    int         n,          // I - a 0-based index into the statement
    const char* text        // I - a value to bind into the statement
)
{
    enforce(m_stmt != 0, "No active statement being wrapped.");

    int rc = sqlite3_bind_text(m_stmt, n + 1, text, static_cast<int>(strlen(text)), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        auto e = m_db.makeException("SqliteStatementImpl::bind(int, const char*)");
        m_db.rollback();
        throw e;
    }

    m_bindingsMade = true;
}

//===========================================================================
// public

void SqliteStatementImpl::bind
(
    int             n,          // I - a 0-based index into the statement
    vector<char>&   data        // I - a value to bind into the statement
)
{
    enforce(m_stmt != 0, "No active statement being wrapped.");

    int rc = sqlite3_bind_blob(m_stmt, n + 1, &data[0], static_cast<int>(data.size()), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        auto e = m_db.makeException("SqliteStatementImpl::bind(int, const vector<char>&)");
        m_db.rollback();
        throw e;
    }

    m_bindingsMade = true;
}

//===========================================================================
// public

void SqliteStatementImpl::bind
(
    int             n,              // I - a 0-based index into the statement
    const char*     data,           // I - a lenght-terminated value to bind
    int             dataLength      // I - length of data to bind into the statement
)
{
    enforce(m_stmt != 0, "No active statement being wrapped.");

    int rc = sqlite3_bind_blob(m_stmt, n + 1, data, dataLength, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        auto e = m_db.makeException("SqliteStatementImpl::bind(int, const char*, int)");
        m_db.rollback();
        throw e;
    }

    m_bindingsMade = true;
}

//===========================================================================
// public

void SqliteStatementImpl::setupRetrieval
(
    int             n,          // I - a 0-based index into the columns
    double&         d,          // O - (via operator()) where to store the value
    bool*           pIsNull     // O - whether a column is NULL, def 0
)
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");
    enforce(n >= 0, "SqliteStatementImpl::setupRetrieval(int, double&, bool*) requires a "
                    "whole-number index");

    RetrievalVar v = { kDoubleType, &d, pIsNull };
    m_retrieveColumns[n] = v;
}

//===========================================================================
// public

void SqliteStatementImpl::setupRetrieval
(
    int             n,          // I - a 0-based index into the columns
    int&            i,          // O - (via operator()) where to store the value
    bool*           pIsNull     // O - whether a column is NULL, def 0
)
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");
    enforce(n >= 0, "SqliteStatementImpl::setupRetrieval(int, int&, bool*) requires a "
                    "whole-number index");

    RetrievalVar v = { kIntType, &i, pIsNull };
    m_retrieveColumns[n] = v;
}

//===========================================================================
// public

void SqliteStatementImpl::setupRetrieval
(
    int             n,          // I - a 0-based index into the columns
    long long&      i64,        // O - (via operator()) where to store the value
    bool*           pIsNull     // O - whether a column is NULL, def 0
)
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");
    enforce(n >= 0, "SqliteStatementImpl::setupRetrieval(int, long long&, bool*) requires a "
                    "whole-number index");

    RetrievalVar v = { kInt64Type, &i64, pIsNull };
    m_retrieveColumns[n] = v;
}

//===========================================================================
// public

void SqliteStatementImpl::setupRetrieval
(
    int             n,          // I - a 0-based index into the columns
    string&         text,       // O - (via operator()) where to store the value
    bool*           pIsNull     // O - whether a column is NULL, def 0
)
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");
    enforce(n >= 0, "SqliteStatementImpl::setupRetrieval(int, string&, bool*) requires a "
                    "whole-number index");

    RetrievalVar v = { kTextType, &text, pIsNull };
    m_retrieveColumns[n] = v;
}

//===========================================================================
// public

void SqliteStatementImpl::setupRetrieval
(
    int             n,          // I - a 0-based index into the columns
    vector<char>&   data,       // O - (via operator()) where to store the value
    bool*           pIsNull     // O - whether a column is NULL, def 0
)
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");
    enforce(n >= 0, "SqliteStatementImpl::setupRetrieval(int, vector<cahr>&, bool*) requires a "
                    "whole-number index");

    RetrievalVar v = { kBlobType, &data, pIsNull };
    m_retrieveColumns[n] = v;
}

//===========================================================================
// public

void SqliteStatementImpl::operator()
(
    bool*           done    // O - sets the value true if the query has completed, def 0
)
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");
    if (!m_started && m_beginTransaction && !m_db.inTransaction())
    {
        m_db.beginTransaction();
    }
    m_started = true;

    auto rc = sqlite3_step(m_stmt);
    if (rc == SQLITE_DONE)
    {
        m_done = true;
        if (done != 0)
        {
            *done = true;
        }

        if (m_endTransaction && m_db.inTransaction())
        {
            m_db.commit();
        }
    }
    else if (rc == SQLITE_ROW)
    {
        auto nCols = sqlite3_column_count(m_stmt);
        for (auto v : m_retrieveColumns)
        {
            if (v.first >= nCols)
            {
                m_db.rollback();
                throw m_db.makeNotEnoughColumnsException(nCols, v.first);
            }
            try
            {
                v.second(m_stmt, v.first);
            }
            catch (RetrievalVarException& e)
            {
                m_db.rollback();
                throw SqliteException(m_db.getDBPath(), e.what());
            }
            catch (...)
            {
                m_db.rollback();
                throw;
            }
        }
        if (done != 0)
        {
            *done = false;
        }
    }
    else
    {
        auto e = m_db.makeException("SqliteStatementImpl::operator()(bool*)");
        m_db.rollback();
        throw e;
    }
}

//===========================================================================
// public

void SqliteStatementImpl::reset
(
    bool minimalReset       // I - reset only the statement, not retrieves, binds, def false
)
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");

    if (!minimalReset)
    {
        if (m_bindingsMade)
        {
            sqlite3_clear_bindings(m_stmt);
            m_bindingsMade = false;
        }
        m_retrieveColumns.clear();
    }

    sqlite3_reset(m_stmt);
    m_started = m_done = m_beginTransaction = m_endTransaction = false;
}

//===========================================================================
// public

string SqliteStatementImpl::sql()
{
    lock_guard<mutex> l(m_statementLock);
    enforce(m_stmt != 0, "No active statement being wrapped.");

    const char* sqlText = sqlite3_sql(m_stmt);
    if (sqlText == 0 || *sqlText == '\0')
    {
        return string();
    }

    return string(sqlText);
}

//===========================================================================
// private

void SqliteStatementImpl::finalize()
{
    lock_guard<mutex> l(m_statementLock);
    if (m_stmt != 0)
    {
        sqlite3_finalize(m_stmt);
        m_stmt = 0;
    }
}

}

}
