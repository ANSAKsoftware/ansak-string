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
// sqlite_db_pragmas.hxx -- Sets pragmas in a SQLite DB for common things:
//                  application ID, version, secure-delete, journaling,
//                  encoding, foreign keys, vacuuming and integrity checks.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

namespace ansak
{

class SqliteDB;

    namespace sqlite
    {

int getApplicationID(ansak::SqliteDB& db);
void setApplicationID(ansak::SqliteDB& db, int applicationID);

int getUserVersion(ansak::SqliteDB& db);
void setUserVersion(ansak::SqliteDB& db, int versionNumber);

void setSecureDelete(ansak::SqliteDB& db);

void setJournaling(ansak::SqliteDB& db, bool useJournaling = true);
bool getJournaling(ansak::SqliteDB& db);

void setUTF8Encoding(ansak::SqliteDB& db);
void setForeignKeys(ansak::SqliteDB& db);

void setNoVacuuming(ansak::SqliteDB& db);
void setVacuuming(ansak::SqliteDB& db);
void setIncrementalVacuuming(ansak::SqliteDB& db);
void vacuumNow(ansak::SqliteDB& db, bool doAll = false);

bool checkIntegrity(ansak::SqliteDB& db, std::vector<std::string>* result = 0);

    }
}
