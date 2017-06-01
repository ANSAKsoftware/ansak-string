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
// sqlite_db_pragmas.cxx -- Sets pragmas in a SQLite DB for common things:
//                  application ID, version, secure-delete, journaling,
//                  encoding, foreign keys, vacuuming and integrity checks.
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_db_pragmas.hxx>

#include <sqlite_db.hxx>
#include <runtime_exception.hxx>
#include <string>
#include <vector>
#include <sstream>

namespace ansak {

    namespace sqlite {

using namespace ansak;
using std::vector;
using std::string;
using std::ostringstream;

namespace {

//===========================================================================
// free function

void vacuumPragma(ansak::SqliteDB& db, int value)
{
    enforce(db.isOpen(), "vacuumPragma: called with a closed database");

    ostringstream o;
    o << "pragma auto_vacuum = " << value << ";";
    db.execute(o.str());
}

bool checkIntegrity(ansak::SqliteDB& db, bool fullCheck, vector<string>* result)
{
    string checkResult;

    // execute 'pragma xxx_check(20);' -- setupRetrieval(0, string);
    ansak::SqliteDB::Statement checkIt(db.prepare(string(
            fullCheck ? "pragma integrity_check(20);" :
                        "pragma quick_check(20);")));
    checkIt->setupRetrieval(0, checkResult);
    bool done = false;
    checkIt(&done);
    // if the First String isn't 'ok',
    if (checkResult != "ok")
    {
        if (result != 0)
        {
            // if there's a vector to stuff the strings in, do so
            do
            {
                result->push_back(checkResult);
                checkIt(&done);
            }
            while (!done);
        }

        return false;
    }
    return true;
}

}

//===========================================================================
// free function

int getApplicationID(SqliteDB& db)
{
    enforce(db.isOpen(), "getApplicationID: called with a closed database");

    SqliteDB::Statement getStatement(db.prepare("pragma application_id;"));
    int id;
    getStatement->setupRetrieval(0, id);
    getStatement();
    return id;
}

//===========================================================================
// free function

void setApplicationID(SqliteDB& db, int applicationID)
{
    enforce(db.isOpen(), "setApplicationID: called with a closed database");

    ostringstream o;
    o << "pragma application_id = " << applicationID << ";";
    db.execute(o.str());
}

//===========================================================================
// free function

int getUserVersion(SqliteDB& db)
{
    enforce(db.isOpen(), "getUserVersion: called with a closed database");

    SqliteDB::Statement getStatement(db.prepare("pragma user_version;"));
    int version;
    getStatement->setupRetrieval(0, version);
    getStatement();
    return version;
}

//===========================================================================
// free function

void setUserVersion(SqliteDB& db, int versionNumber)
{
    enforce(db.isOpen(), "setUserVersion: called with a closed database");

    ostringstream o;
    o << "pragma user_version = " << versionNumber << ";";
    db.execute(o.str());
}

//===========================================================================
// free function

void setSecureDelete(SqliteDB& db)
{
    enforce(db.isOpen(), "setSecureDelete: called with a closed database");

    db.execute("pragma secure_delete = 1;");
}

//===========================================================================
// free function

void setJournaling(SqliteDB& db, bool useJournaling)
{
    enforce(db.isOpen(), "setJournaling: called with a closed database");

    string modeWanted;
    if (!useJournaling)
    {
        modeWanted = "off;";
    }
    else if (db.isMemory())
    {
        modeWanted = "memory;";
    }
    else
    {
        modeWanted = "delete;";
    }
    string command("pragma journal_mode = ");
    command += modeWanted;
    db.execute(command);
}

//===========================================================================
// free function

bool getJournaling(SqliteDB& db)
{
    enforce(db.isOpen(), "getJournaling: called with a closed database");

    string mode;
    SqliteDB::Statement getStatement(db.prepare("pragma journal_mode;"));
    getStatement->setupRetrieval(0, mode);
    getStatement();

    if (db.isMemory())
    {
        return mode == "memory";
    }
    else
    {
        return mode != "memory" && mode != "off";
    }
}

//===========================================================================
// free function

void setUTF8Encoding(ansak::SqliteDB& db)
{
    enforce(db.isOpen(), "setUTF8Encoding: called with a closed database");

    db.execute("pragma encoding = 'UTF-8';");
}

//===========================================================================
// free function

void setForeignKeys(ansak::SqliteDB& db)
{
    enforce(db.isOpen(), "setForeignKeys: called with a closed database");

    db.execute("pragma foreign_keys = 1;");
}

//===========================================================================
// free function

void setNoVacuuming(ansak::SqliteDB& db)
{
    vacuumPragma(db, 0);
}

//===========================================================================
// free function

void setVacuuming(ansak::SqliteDB& db)
{
    vacuumPragma(db, 1);
}

//===========================================================================
// free function

void setIncrementalVacuuming(ansak::SqliteDB& db)
{
    vacuumPragma(db, 2);
}

//===========================================================================
// free function

void vacuumNow(ansak::SqliteDB& db, bool doAll)
{
    enforce(db.isOpen(), "vacuumNow: called with a closed database");

    int value = doAll ? 0 : 4;

    ostringstream o;
    o << "pragma incremental_vacuum(" << value << ");";
    db.execute(o.str());
}

//===========================================================================
// free function

bool checkIntegrity(ansak::SqliteDB& db, vector<string>* result)
{
    enforce(db.isOpen(), "checkIntegrity: called with a closed database");

    if (result != 0)
    {
        result->clear();
    }

    volatile bool minimal = checkIntegrity(db, false, result);
    volatile bool full = checkIntegrity(db, true, result);
    return minimal && full;
}

}

}
