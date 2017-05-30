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
#include <string.h>
#include <string.hxx>
#include <sstream>

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

    //=======================================================================
    // internal data type enums for retrieve values
    enum {
        kDoubleType = 1010,
        kIntType,
        kInt64Type,
        kTextType,
        kBlobType
    };

    const unordered_map<int, string> typesToFunctions {
        { kDoubleType, "SqliteStatementImpl::setupRetrieval(int, double&, bool*)" },
        { kIntType,    "SqliteStatementImpl::setupRetrieval(int, int&, bool*) " },
        { kInt64Type,  "SqliteStatementImpl::setupRetrieval(int, long long&, bool*) " },
        { kTextType,   "SqliteStatementImpl::setupRetrieval(int, string&, bool*)" },
        { kBlobType,   "SqliteStatementImpl::setupRetrieval(int, vector<char>&, bool*)" }
    };
}

//===========================================================================
// private, Constructor

SqliteStatementImpl::SqliteStatementImpl
(
    SqliteDB*           db,         // I - a pointer to the sqliteDB object
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
        auto e = m_db->makeException("SqliteStatementImpl::bind(int, double)");
        m_db->rollback();
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
        auto e = m_db->makeException("SqliteStatementImpl::bind(int, int)");
        m_db->rollback();
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
        auto e = m_db->makeException("SqliteStatementImpl::bind(int, long long)");
        m_db->rollback();
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

    int rc = sqlite3_bind_text(m_stmt, n + 1, text.c_str(), text.size(), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        auto e = m_db->makeException("SqliteStatementImpl::bind(int, const string&)");
        m_db->rollback();
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

    int rc = sqlite3_bind_text(m_stmt, n + 1, text, strlen(text), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        auto e = m_db->makeException("SqliteStatementImpl::bind(int, const char*)");
        m_db->rollback();
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

    int rc = sqlite3_bind_blob(m_stmt, n + 1, &data[0], data.size(), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        auto e = m_db->makeException("SqliteStatementImpl::bind(int, const vector<char>&)");
        m_db->rollback();
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
        auto e = m_db->makeException("SqliteStatementImpl::bind(int, const char*, int)");
        m_db->rollback();
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
    if (!m_started && m_beginTransaction && !m_db->inTransaction())
    {
        m_db->beginTransaction();
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

        if (m_endTransaction && m_db->inTransaction())
        {
            m_db->commit();
        }
    }
    else if (rc == SQLITE_ROW)
    {
        auto nCols = sqlite3_column_count(m_stmt);
        for (auto v : m_retrieveColumns)
        {
            if (v.first >= nCols)
            {
                m_db->rollback();
                throw m_db->makeNotEnoughColumnsException(nCols, v.first);
            }
            try
            {
                v.second(m_stmt, v.first);
            }
            catch (RetrievalVarException& e)
            {
                m_db->rollback();
                throw m_db->makeException(e.what());
            }
            catch (...)
            {
                m_db->rollback();
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
        auto e = m_db->makeException("SqliteStatementImpl::operator()(bool*)");
        m_db->rollback();
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
        m_db = 0;
    }
}

//===========================================================================
// public to private inner class

void SqliteStatementImpl::RetrievalVar::operator()
(
    sqlite3_stmt*   stmt,       // I - the statement being selected from
    int             n           // I - 0-based index into the columns
)
{
    auto t = sqlite3_column_type(stmt, n);
    if (t == SQLITE_INTEGER)
    {
        if (pIsNull != 0)
        {
            *pIsNull = false;
        }
        enforce(typeID == kIntType || typeID == kInt64Type,
                "SQLITE_INTEGER can only be retrieved into platform int and 64-bit values");
        if (typeID == kInt64Type)
        {
            *(reinterpret_cast<sqlite3_int64*>(data)) = sqlite3_column_int64(stmt, n);
        }
        else
        {
            *(reinterpret_cast<int*>(data)) = sqlite3_column_int(stmt, n);
        }
    }
    else if (t == SQLITE_TEXT)
    {
        if (pIsNull != 0)
        {
            *pIsNull = false;
        }
        enforce(typeID == kTextType,
                "SQLITE_TEXT can only be retrieved into string values");
        const char* textResult = reinterpret_cast<const char*>(sqlite3_column_text(stmt, n));
        *(reinterpret_cast<string*>(data)) = textResult == 0 ? "" : textResult;
    }
    else if (t == SQLITE_FLOAT)
    {
        if (pIsNull != 0)
        {
            *pIsNull = false;
        }
        enforce(typeID == kDoubleType,
                "SQLITE_FLOAT can only be retrieved into double values");
        *(reinterpret_cast<double*>(data)) = sqlite3_column_double(stmt, n);
    }
    else if (t == SQLITE_BLOB)
    {
        if (pIsNull != 0)
        {
            *pIsNull = false;
        }
        enforce(typeID == kBlobType,
                "SQLITE_BLOB can only be retrieved into vector<char> values");
        auto nBytes = sqlite3_column_bytes(stmt, n);
        auto pBytes = sqlite3_column_blob(stmt, n);
        vector<char>& dest = *(reinterpret_cast<vector<char>*>(data));
        dest.resize(nBytes);
        if (nBytes != 0)
        {
            memcpy(dest.data(), pBytes, nBytes);
        }
    }
    else if (t == SQLITE_NULL)
    {
        if (pIsNull != 0)
        {
            *pIsNull = true;
        }
    }
    else
    {
        throw SqliteStatementImpl::RetrievalVarException(*this, n, t);
    }
}

SqliteStatementImpl::RetrievalVarException::RetrievalVarException
(
    const SqliteStatementImpl::RetrievalVar& var,
    int columnIndex,
    int sqliteType
) noexcept
{
    try
    {
        auto iFunction = typesToFunctions.find(var.typeID);
        if (iFunction == typesToFunctions.end())
        {
            m_what = "SqliteStatementImpl retrieve variable data has been corrupted.";
        }
        else
        {
            const char* sqliteTypeName = "<unsupported type>";
            switch (sqliteType)
            {
            default:                break;
            case SQLITE_INTEGER:    sqliteTypeName = "SQLITE_INTEGER";      break;
            case SQLITE_TEXT:       sqliteTypeName = "SQLITE_TEXT";         break;
            case SQLITE_FLOAT:      sqliteTypeName = "SQLITE_FLOAT";        break;
            case SQLITE_BLOB:       sqliteTypeName = "SQLITE_BLOB";         break;
            }

            ostringstream os;
            os << iFunction->second << " called to retrieve data from column index "
               << columnIndex << " but only data of type " << sqliteTypeName
               << " was available.";
            m_what = os.str();
        }
    }
    catch (...)
    {}
}

SqliteStatementImpl::RetrievalVarException::~RetrievalVarException() noexcept {}

const char* SqliteStatementImpl::RetrievalVarException::what() const noexcept
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
        static const char exceptedWhat[] = "RetrievalVarException - throw on what()";
        return exceptedWhat;
    }
    static const char emptyWhat[] = "RetrievalVarException: no details are available.";
    return emptyWhat;
}

}

}
