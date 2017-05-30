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
// sqlite_retrieval_var.hxx -- A struct to manage data retrieve per row of
//                             a single SQLite query.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <sqlite3.h>

#include <unordered_map>
#include <mutex>
#include <string>

namespace ansak {

class SqliteDB;

namespace internal {

//=======================================================================
// internal data type enums for retrieve values

enum {
    kDoubleType = 1010,
    kIntType,
    kInt64Type,
    kTextType,
    kBlobType
};

extern const std::unordered_map<int, std::string> typesToFunctions;

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
};

using RetrievalMap = std::unordered_map<int, RetrievalVar>;

}

}

