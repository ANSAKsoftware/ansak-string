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
// sqlite_statement_impl.hxx -- A class to wrap individual SQLite statements
//                         once prepared against a particular database, to
//                         bind data values into it, to retrieve results out.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <sqlite_statement.hxx>
#include <sqlite_retrieval_var.hxx>
#include <sqlite_retrieval_exception.hxx>

#include <unordered_map>
#include <mutex>

namespace ansak {

class SqliteDB;

namespace internal {

///////////////////////////////////////////////////////////////////////////
// class sqliteStatement -- wrapper for a SQLITE3 statement, including
//                          binding-in, retrieving out, executing, resetting

class SqliteStatementImpl : public SqliteStatement {

    friend class ansak::SqliteDB;

    //=======================================================================
    // Constructor, private -- called only by SqliteDB::prepare

    SqliteStatementImpl
    (
        SqliteDB&           db,         // I - a pointer to the SqliteDB object
        sqlite3_stmt*       stmt        // I - the SQLite3-prepared statement
    );

public:

    //=======================================================================
    // Destructor

    ~SqliteStatementImpl();

    //=======================================================================
    // Copy Constructor, assignment deleted

    SqliteStatementImpl(const SqliteStatementImpl& src) = delete;
    SqliteStatementImpl& operator=(const SqliteStatementImpl& src) const = delete;

    //=======================================================================
    // setBeginTransaction
    //
    // Set this statement's first execution to be the one that starts a
    // transaction. No checking against current transaction states in the SQLite
    // database is done here. The caller is expected to be sensible

    void setBeginTransaction() override { m_beginTransaction = true; }

    //=======================================================================
    // setEndTransaction
    //
    // Set this statement's "done" execution to be the one that ends a
    // transaction. No checking against current transaction states in the SQLite
    // database is done here. The caller is expected to be sensible

    void setEndTransaction() override { m_endTransaction = true; }

    //=======================================================================
    // id
    //
    // Returns a serial number for the statement across the whole process.

    int id() const override { return m_id; }

    //=======================================================================
    // bind
    //
    // Bind a double, int, long long, string or 0-terminated char*,
    // vector<char> or length-terminated char* for the different data type into
    // a compiled statement.

    void bind
    (
        int         n,          // I - a 0-based index into the statement
        double      d           // I - a value to bind into the statement
    ) override;

    void bind(int n, int i) override;
    void bind(int n, long long i64) override;
    void bind(int n, const std::string& text) override;
    void bind(int n, const char* text) override;
    void bind(int n, std::vector<char>& data) override;
    void bind(int n, const char* data, int dataLength) override;

    //=======================================================================
    // setupRetrieval
    //
    // Set up the retreival of a double, int, long long, string or 0-terminated
    // char*, vector<char> or length-terminated char* for the different data
    // type out of a query's columns. Optionally, detect whether a value is
    // null.

    void setupRetrieval
    (
        int             n,              // I - a 0-based index into the columns
        double&         d,              // O - (via operator()) where to store the value
        bool*           pIsNull = 0     // O - whether a column is NULL
    ) override;

    void setupRetrieval(int n, int& i, bool* pIsNull = 0) override;
    void setupRetrieval(int n, long long& i64, bool* pIsNull = 0) override;
    void setupRetrieval(int n, std::string& text, bool* pIsNull = 0) override;
    void setupRetrieval(int n, std::vector<char>& data, bool* pIsNull = 0) override;

    //=======================================================================
    // operator()
    //
    // Executes the query for each row of the query. Pushes back true when a
    // query is done, false otherwise, optional either way.

    void operator()
    (
        bool*           done = 0        // O - sets the value true if the query has completed
    ) override;

    //=======================================================================
    // reset
    //
    // Clears the binds and retrieves and resets the execution of a query so that
    // it can be done again.

    void reset
    (
        bool minimalReset = false       // I - reset only the statement, not retrieves, binds
    ) override;

    bool hasBindings() const
    {
        return m_bindingsMade;
    }

    bool hasRetrievals() const
    {
        return !m_retrieveColumns.empty();
    }

    size_t numRetrievals() const
    {
        return m_retrieveColumns.size();
    }

    //=======================================================================
    // sql
    //
    // Returns the originally prepared SQL statement.

    std::string sql() override;

private:

    //=======================================================================
    // finalize
    //
    // Finalizes the statement as it's being destroyed -- or sooner.

    void finalize();

    // Member variables

    std::mutex      m_statementLock;        // only one thread can touch this statement
    SqliteDB&       m_db;                   // the database the statement lives in
    sqlite3_stmt*   m_stmt;                 // the prepared statement
    int             m_id;                   // a serial number for the prepared statement
    RetrievalMap    m_retrieveColumns;      // record retrieval, by column

    // execution status tracking -- a statement begins with both set to false, they are
    // set to true during operator() -- for the beginning and succesful completion
    // statuses revert to false after reset() as the statement is ready to be rerun
    bool            m_started;              // true after successful operator() call
    bool            m_done;                 // true after operator() has returned done

    bool            m_bindingsMade;         // true if any bindings have been made

    // transaction being/end control -- set for first/last statement in a transaction
    bool            m_beginTransaction;     // if true, start a transaction for the next run
    bool            m_endTransaction;       // if true, end a transaction for the next run
};

}

}

