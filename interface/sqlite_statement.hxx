///////////////////////////////////////////////////////////////////////////
//
// 2014.11.01
//
// The author (Arthur N. Klassen) disclaims all copyright to this source code.
// In place of a legal notice, here is a blessing:
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
// (Is it too much to suggest retaining this header on this file?)
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
typedef std::shared_ptr<SqliteStatement> SqliteStatementPointer;

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
    // Copy Constructor, assignment deleted private -- called only by SqliteDB::prepare

    SqliteStatement(const SqliteStatement& src) = delete;
    SqliteStatement& operator=(const SqliteStatement& src) const = delete;

    //=======================================================================
    // Destructor

    ~SqliteStatement();

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

