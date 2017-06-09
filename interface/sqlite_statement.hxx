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
// 2014.11.01
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// sqlite_statement.hxx -- A class to wrap individual SQLite statements
//                         once prepared against a particular database, to
//                         bind data values into it, to retrieve results out.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <sqlite3.h>

#include <string>
#include <vector>
#include <memory>

namespace ansak {

class SqliteDB;

class SqliteStatement;
typedef std::shared_ptr<SqliteStatement> SqliteStatementPtr;

///////////////////////////////////////////////////////////////////////////
// class sqliteStatement -- wrapper for a SQLITE3 statement, including
//                          binding-in, retrieving out, executing, resetting

class SqliteStatement {

    friend class SqliteDB;

protected:

    //=======================================================================
    // Constructor, private -- called only by SqliteDB::prepare

    SqliteStatement();

public:

    static const bool resetOnlyStatement = true;

    //=======================================================================
    // Copy Constructor, assignment deleted

    SqliteStatement(const SqliteStatement& src) = delete;
    SqliteStatement& operator=(const SqliteStatement& src) const = delete;

    //=======================================================================
    // Destructor

    virtual ~SqliteStatement();

    //=======================================================================
    // setBeginTransaction
    //
    // Set this statement's first execution to be the one that starts a
    // transaction. No checking against current transaction states in the SQLite
    // database is done here. The caller is expected to be sensible

    virtual void setBeginTransaction() = 0;

    //=======================================================================
    // setEndTransaction
    //
    // Set this statement's "done" execution to be the one that ends a
    // transaction. No checking against current transaction states in the SQLite
    // database is done here. The caller is expected to be sensible

    virtual void setEndTransaction() = 0;

    //=======================================================================
    // id
    //
    // Returns a serial number for the statement across the whole process.

    virtual int id() const = 0;

    //=======================================================================
    // bind
    //
    // Bind a double, int, long long, string or 0-terminated char*,
    // vector<char> or length-terminated char* for the different data type into
    // a compiled statement.

    virtual void bind
    (
        int         n,          // I - a 0-based index into the statement
        double      d           // I - a value to bind into the statement
    ) = 0;

    virtual void bind(int n, int i) = 0;
    virtual void bind(int n, long long i64) = 0;
    virtual void bind(int n, const std::string& text) = 0;
    virtual void bind(int n, const char* text) = 0;
    virtual void bind(int n, std::vector<char>& data) = 0;
    virtual void bind(int n, const char* data, int dataLength) = 0;

    //=======================================================================
    // setupRetrieval
    //
    // Set up the retreival of a double, int, long long, string or 0-terminated
    // char*, vector<char> or length-terminated char* for the different data
    // type out of a query's columns. Optionally, detect whether a value is
    // null.

    virtual void setupRetrieval
    (
        int             n,              // I - a 0-based index into the columns
        double&         d,              // O - (via operator()) where to store the value
        bool*           pIsNull = 0     // O - whether a column is NULL
    ) = 0;

    virtual void setupRetrieval(int n, int& i, bool* pIsNull = 0) = 0;
    virtual void setupRetrieval(int n, long long& i64, bool* pIsNull = 0) = 0;
    virtual void setupRetrieval(int n, std::string& text, bool* pIsNull = 0) = 0;
    virtual void setupRetrieval(int n, std::vector<char>& data, bool* pIsNull = 0) = 0;

    //=======================================================================
    // operator()
    //
    // Executes the query for each row of the query. Pushes back true when a
    // query is done, false otherwise, optional either way.

    virtual void operator()
    (
        bool*           done = 0        // O - sets the value true if the query has completed
    ) = 0;

    //=======================================================================
    // reset
    //
    // Clears the binds and retrieves and resets the execution of a query so that
    // it can be done again.

    virtual void reset
    (
        bool minimalReset = false       // I - reset only the statement, not retrieves, binds
    ) = 0;

    //=======================================================================
    // sql
    //
    // Returns the originally prepared SQL statement.

    virtual std::string sql() = 0;
};

}

