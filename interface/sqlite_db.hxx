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
// sqlite_db.hxx -- Class wrapping a connection to a SQLite Database, whether
//                  in memory or on disk.
//                  Depends on namespace ansak APIs from "base".
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "file_path.hxx"
#include "file_system_path.hxx"
#include "sqlite_statement.hxx"
#include "sqlite_exception.hxx"
#include <sqlite3.h>
#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <exception>

namespace ansak {

///////////////////////////////////////////////////////////////////////////
// class SqliteDB -- wrapper for a SQLITE3 database connection, including
//                   creation/opening and closing, statement preparation and
//                   finalization

class SqliteDB {

public:

    // flag parameters for opening a databse, a sub-set of available SQLITE3 flags
    enum {
        kReadOnly               = 0x00001,
        kReadWrite              = 0x00002,
        kMemory                 = 0x00080,
        kNoMutex                = 0x08000,
        kFullMutex              = 0x10000,
        kSharedCache            = 0x20000,
        kPrivateCache           = 0x40000,

        kAllowedFlags           = 0x78083
    };

    ///////////////////////////////////////////////////////////////////////
    // getVersionNumber -- retrieve the version number of the library
    static int getVersionNumber();

    ///////////////////////////////////////////////////////////////////////
    // getVersion -- retrieve the version string of the library
    static std::string getVersion(bool giveSourceId = false);

    ///////////////////////////////////////////////////////////////////////
    // constructor -- databases without a path string come from memory
    SqliteDB(const FilePath& path = FilePath::invalidPath());

    ///////////////////////////////////////////////////////////////////////
    // destructor -- closes if the database is open
    ~SqliteDB();

    ///////////////////////////////////////////////////////////////////////
    // suppress copies and assignments
    SqliteDB(const SqliteDB& src) = delete;
    SqliteDB operator=(const SqliteDB& src) const = delete;

    ///////////////////////////////////////////////////////////////////////
    // create -- throws if the create cannot be performed
    void create(int flags = kReadWrite | kFullMutex | kSharedCache);

    ///////////////////////////////////////////////////////////////////////
    // open -- throws if the open cannot be performed
    void open(int flags = kReadWrite | kFullMutex | kSharedCache);

    ///////////////////////////////////////////////////////////////////////
    // close -- finalizes all Statements before closing the database
    void close();

    ///////////////////////////////////////////////////////////////////////
    // execute -- prepare, execute, finalize a Statement directly
    void execute(const std::string& stmt);

    SqliteException makeException(const char* contextString);
    SqliteException makeNotEnoughColumnsException(int actualColumns, int columnX);

    ///////////////////////////////////////////////////////////////////////
    // prepareStatement -- prepare a statement that is expected to last
    //                     across multiple scopes
    SqliteStatementPtr prepareStatement(const std::string& stmt);

    ///////////////////////////////////////////////////////////////////////
    // finalizeStatement -- finalize a statement explicitly, either by pointer
    //                      or by smart pointer
    void finalizeStatement(SqliteStatement* stmt);
    void finalizeStatement(SqliteStatementPtr& stmt);

    ///////////////////////////////////////////////////////////////////////
    // accessors
    bool isMemory() const { return (m_flagsUsed & kMemory) != 0; }
    bool isOpen() const   { return m_pDB != 0; }

    ///////////////////////////////////////////////////////////////////////
    // an RAII class to wrap a statement, finalize happens on destruct
    class Statement {

        friend class SqliteDB;

        Statement(SqliteDB& pDB, SqliteStatementPtr p) : m_p(p), m_db(pDB) {}

    public:

        ~Statement() { m_db.finalizeStatement(m_p); }

        ///////////////////////////////////////////////////////////////////
        // Make this class function as much like a smart pointer as possible

        SqliteStatement* get() const noexcept { return m_p.get(); }
        SqliteStatement* operator->() const noexcept { return m_p.get(); }
        SqliteStatement& operator*() const noexcept { return *m_p.get(); }
        void operator()(bool* done = 0) { (m_p.get())->operator()(done); }

    private:
        SqliteStatementPtr      m_p;        // the statement pointer being wrapped
        SqliteDB&               m_db;       // the database associated with m_p
    };

    ///////////////////////////////////////////////////////////////////////
    // prepare -- prepare a statement and wrap it with the inner class
    SqliteDB::Statement prepare(const std::string& stmt);

    ///////////////////////////////////////////////////////////////////////
    // inTransaction -- returns true if the database is in the midst of a
    // transaction, false otherwise
    bool inTransaction();

    ///////////////////////////////////////////////////////////////////////
    // beginTransaction -- starts a transaction if one is not already
    // started
    void beginTransaction();

    ///////////////////////////////////////////////////////////////////////
    // rollback -- rolls back a transaction if one is active
    void rollback();

    ///////////////////////////////////////////////////////////////////////
    // commit -- commits a transactino if one is active
    void commit();

    ///////////////////////////////////////////////////////////////////////
    // getDBPath -- where the database is stored
    FilePath getDBPath() const;

private:

    int transactionExecute(SqliteStatementPtr& transactionQuery);

    ///////////////////////////////////////////////////////////////////////
    // Member variables

    FilePath                m_path;         // path string as filePath, if not file is invalid
    FileSystemPath          m_systemPath;   // path string as filePath, if not file is invalid
    sqlite3*                m_pDB;          // pointers! -- underlying SQLit3 database
    int                     m_flagsUsed;    // flags used during opening

    std::mutex              m_statementsLock;   // lock of statements list
    std::map<int, SqliteStatementPtr>
                            m_statementMap;     // map of active statements
    SqliteStatementPtr      m_beginTran, m_rollback, m_commit;       // transaction queries
    bool                    m_inTransaction;    // is the database in a transaction?
};

}
