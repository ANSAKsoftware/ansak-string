///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, Arthur N. Klassen
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
// 2017.05.30 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// sqlite_retrieval_var.cxx -- Implementaion of methods for struct to manage
//                             data retrieve per row of a single SQLite query.
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_retrieval_var.hxx>
#include <sqlite_retrieval_exception.hxx>

#include <runtime_exception.hxx>
#include <vector>
#include <cstring>

using namespace std;

namespace ansak
{

namespace internal
{

const unordered_map<int, string> typesToFunctions {
    { kDoubleType, "SqliteStatementImpl::setupRetrieval(int, double&, bool*)" },
    { kIntType,    "SqliteStatementImpl::setupRetrieval(int, int&, bool*) " },
    { kInt64Type,  "SqliteStatementImpl::setupRetrieval(int, long long&, bool*) " },
    { kTextType,   "SqliteStatementImpl::setupRetrieval(int, string&, bool*)" },
    { kBlobType,   "SqliteStatementImpl::setupRetrieval(int, vector<char>&, bool*)" }
};

//===========================================================================
// public

void RetrievalVar::operator()
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
        throw RetrievalVarException(*this, n, t);
    }
}

}

}
