///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, Arthur N. Klassen
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
// 2016.02.01 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// runtime_exception_test.cxx -- unit tests for debug-assert-like things
//                               (used for file-of-lines, especially).
//
///////////////////////////////////////////////////////////////////////////

#include <runtime_exception.hxx>

#include <gtest/gtest.h>

using namespace std;
using namespace ansak;
using namespace testing;

TEST(RuntimeException, testFullAssert)
{
    enforce(true, "This exception should not fire.", __FILE__, __LINE__);

    unsigned int linePlus3 = __LINE__ + 3;
    try
    {
        enforce(false, "This exception should fire.", __FILE__, __LINE__);
        ASSERT_TRUE(false);
    }
    catch (ansak::RuntimeException& e)
    {
        string what(e.what());
        string where(e.fileName());
        EXPECT_NE(string::npos, where.find("runtime_exception_test.cxx"));
        EXPECT_EQ(linePlus3, e.lineNumber());
    }
}

TEST(RuntimeException, testNoLineNumberAssert)
{
    try
    {
        enforce(false, "This exception should fire.", __FILE__);
        ASSERT_TRUE(false);
    }
    catch (ansak::RuntimeException& e)
    {
        string what(e.what());
        string where(e.fileName());
        EXPECT_NE(string::npos, where.find("runtime_exception_test.cxx"));
        EXPECT_EQ(0u, e.lineNumber());
    }
}

TEST(RuntimeException, testNoFileNameAssert)
{
    try
    {
        enforce(false, "This exception should fire.");
        ASSERT_TRUE(false);
    }
    catch (ansak::RuntimeException& e)
    {
        string what(e.what());
        string where(e.fileName());
        EXPECT_EQ(string("<no filename given>"), where);
        EXPECT_EQ(0u, e.lineNumber());
    }

    try
    {
        enforce(false, "This exception should fire.", nullptr);
        ASSERT_TRUE(false);
    }
    catch (ansak::RuntimeException& e)
    {
        string what(e.what());
        string where(e.fileName());
        EXPECT_EQ(string("<no filename given>"), where);
        EXPECT_EQ(0u, e.lineNumber());
    }

    try
    {
        enforce(false, "This exception should fire.", "");
        ASSERT_TRUE(false);
    }
    catch (ansak::RuntimeException& e)
    {
        string what(e.what());
        string where(e.fileName());
        EXPECT_EQ(string("<empty-string filename given>"), where);
        EXPECT_EQ(0u, e.lineNumber());
    }
}

