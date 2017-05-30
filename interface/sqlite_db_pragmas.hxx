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
