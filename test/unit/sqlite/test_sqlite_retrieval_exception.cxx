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
// 2017.05.30 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// test_sqlite_retrieval_exception.cxx -- Tests for sqlite_retrieval_exception.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_retrieval_exception.hxx>
#include <sqlite_retrieval_var.hxx>

#include <gmock/gmock.h>

using namespace ansak;
using namespace ansak::internal;
using namespace std;
using namespace testing;

TEST(SqliteRetrievalException, doubleAs)
{
    double d = 0.0;
    bool isNull = false;
    RetrievalVar v = { kDoubleType, &d, &isNull };

    RetrievalVarException e(v, 0, SQLITE_INTEGER);
    EXPECT_THAT(e.what(), StartsWith("RetrievalVarException: SqliteStatementImpl::setupRetrieval(int,"));
    EXPECT_THAT(e.what(), HasSubstr("SQLITE_INTEGER"));

    RetrievalVarException f(v, 0, SQLITE_TEXT);
    EXPECT_THAT(f.what(), StartsWith("RetrievalVarException: SqliteStatementImpl::setupRetrieval(int,"));
    EXPECT_THAT(f.what(), HasSubstr("SQLITE_TEXT"));

    RetrievalVarException g(v, 0, SQLITE_BLOB);
    EXPECT_THAT(g.what(), StartsWith("RetrievalVarException: SqliteStatementImpl::setupRetrieval(int,"));
    EXPECT_THAT(g.what(), HasSubstr("SQLITE_BLOB"));
}

TEST(SqliteRetrievalException, intAs)
{
    int i = 0;
    bool isNull = false;
    RetrievalVar v = { kIntType, &i, &isNull };

    RetrievalVarException e(v, 0, SQLITE_FLOAT);
    EXPECT_THAT(e.what(), StartsWith("RetrievalVarException: SqliteStatementImpl::setupRetrieval(int,"));
    EXPECT_THAT(e.what(), HasSubstr("SQLITE_FLOAT"));

    RetrievalVarException f(v, 0, SQLITE_TEXT);
    EXPECT_THAT(f.what(), StartsWith("RetrievalVarException: SqliteStatementImpl::setupRetrieval(int,"));
    EXPECT_THAT(f.what(), HasSubstr("SQLITE_TEXT"));

    RetrievalVarException g(v, 0, SQLITE_BLOB);
    EXPECT_THAT(g.what(), StartsWith("RetrievalVarException: SqliteStatementImpl::setupRetrieval(int,"));
    EXPECT_THAT(g.what(), HasSubstr("SQLITE_BLOB"));
}

TEST(SqliteRetrievalException, unknownAs)
{
    double x = 0.0;
    bool isNull = false;
    RetrievalVar v = { 80808, &x, &isNull };

    RetrievalVarException e(v, 0, SQLITE_FLOAT);
    EXPECT_THAT(e.what(), StrEq("RetrievalVarException: SqliteStatementImpl retrieve variable data has been corrupted."));
}

TEST(SqliteRetrievalException, asUnknown)
{
    double x = 0.0;
    bool isNull = false;
    RetrievalVar v = { kDoubleType, &x, &isNull };

    RetrievalVarException e(v, 0, 80907);
    EXPECT_THAT(e.what(), StartsWith("RetrievalVarException: SqliteStatementImpl::setupRetrieval(int,"));
    EXPECT_THAT(e.what(), HasSubstr("<unsupported type>"));
}
