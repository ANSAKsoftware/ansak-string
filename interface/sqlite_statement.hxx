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
#include <unordered_map>
#include <memory>
#include <mutex>

namespace ansak {

class SqliteDB;

class SqliteStatement;
typedef std::shared_ptr<SqliteStatement> SqliteStatementPointer;

///////////////////////////////////////////////////////////////////////////
// class sqliteStatement -- wrapper for a SQLITE3 statement, including
//                          binding-in, retrieving out, executing, resetting

class SqliteStatement {

    friend class SqliteDB;

    //=======================================================================
    // Constructor, private -- called only by SqliteDB::prepare

    SqliteStatement
    (
        SqliteDB*           db,         // I - a pointer to the SqliteDB object
        sqlite3_stmt*       stmt        // I - the SQLite3-prepared statement
    );

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

    void setBeginTransaction() { m_beginTransaction = true; }

    //=======================================================================
    // setEndTransaction
    //
    // Set this statement's "done" execution to be the one that ends a
    // transaction. No checking against current transaction states in the SQLite
    // database is done here. The caller is expected to be sensible

    void setEndTransaction() { m_endTransaction = true; }

    //=======================================================================
    // id
    //
    // Returns a serial number for the statement across the whole process.

    int id() const { return m_id; }

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
    );

    void bind(int n, int i);
    void bind(int n, long long i64);
    void bind(int n, const std::string& text);
    void bind(int n, const char* text);
    void bind(int n, std::vector<char>& data);
    void bind(int n, const char* data, int dataLength);

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
    );

    void setupRetrieval(int n, int& i, bool* pIsNull = 0);
    void setupRetrieval(int n, long long& i64, bool* pIsNull = 0);
    void setupRetrieval(int n, std::string& text, bool* pIsNull = 0);
    void setupRetrieval(int n, std::vector<char>& data, bool* pIsNull = 0);

    //=======================================================================
    // operator()
    //
    // Executes the query for each row of the query. Pushes back true when a
    // query is done, false otherwise, optional either way.

    void operator()
    (
        bool*           done = 0        // O - sets the value true if the query has completed
    );

    //=======================================================================
    // reset
    //
    // Clears the binds and retrieves and resets the execution of a query so that
    // it can be done again.

    void reset
    (
        bool minimalReset = false       // I - reset only the statement, not retrieves, binds
    );

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

    std::string sql();

private:

    //=======================================================================
    // finalize
    //
    // Finalizes the statement as it's being destroyed -- or sooner.

    void finalize();

    class RetrievalVar;
    class RetrievalVarException : public std::exception
    {
    public:

        RetrievalVarException
        (
            const SqliteStatement::RetrievalVar& var,
            int columnIndex,
            int sqliteType
        ) noexcept;

        virtual ~RetrievalVarException() noexcept override;
        virtual const char* what() const noexcept override;
    private:
        std::string         m_what = std::string();
    };

    //=======================================================================
    // RetrievalVar
    //
    // A place that records one column's value for the current row.

    struct RetrievalVar {
        int     typeID;         // What kind of type is being noted
        void*   data;           // Where to store the result (!= 0)
        bool*   pIsNull;        // Where to store a column's null-ness (may be 0)

        //===================================================================
        // operator()
        //
        // Executes the fetch of a single column's value into the associated
        // data location

        void operator()
        (
            sqlite3_stmt*   stmt,       // I - the statement being retrieved from
            int             n           // I - 0-based index into the columns
        );

        RetrievalVarException typeMismatchException(int columnIndex, int sqliteType);
    };
    using RetrievalMap = std::unordered_map<int, RetrievalVar>;

    // Member variables

    std::mutex      m_statementLock;        // only one thread can touch this statement
    SqliteDB*       m_db;                   // the database the statement lives in
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

