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
// sqlite_retrieval_exception.cxx -- An exception to throw over retrieval
//                                   failures.
//
///////////////////////////////////////////////////////////////////////////

#include <sqlite_retrieval_exception.hxx>
#include <sqlite_retrieval_var.hxx>

#include <sstream>

using namespace std;

namespace ansak
{

namespace internal
{

//===========================================================================
// public

RetrievalVarException::RetrievalVarException
(
    const RetrievalVar&     var,            // I - the problem variable
    int                     columnIndex,    // I - its 0-based column index
    int                     sqliteType      // I - the column's SQLite type
) noexcept
{
    try
    {
        auto iFunction = typesToFunctions.find(var.typeID);
        if (iFunction == typesToFunctions.end())
        {
            m_what = "RetrievalVarException: SqliteStatementImpl retrieve variable data has been corrupted.";
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
            os << "RetrievalVarException: ";
            os << iFunction->second << " called to retrieve data from column index "
               << columnIndex << " but only data of type " << sqliteTypeName
               << " (" << sqliteType << ") was available.";
            m_what = os.str();
        }
    }
    catch (...)
    {}
}

//===========================================================================
// public, virtual

RetrievalVarException::~RetrievalVarException() noexcept
{
}

//===========================================================================
// public, virtual

const char* RetrievalVarException::what() const noexcept
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
        static const char exceptedWhat[] = "RetrievalVarException: throw on what()";
        return exceptedWhat;
    }
    static const char emptyWhat[] = "RetrievalVarException: no details are available.";
    return emptyWhat;
}

}

}
