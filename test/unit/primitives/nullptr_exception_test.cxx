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
// 2016.01.25 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// exception_test.cxx -- Test of simple exception(s).
//
///////////////////////////////////////////////////////////////////////////

#include <nullptr_exception.hxx>

#include <gtest/gtest.h>

#include <string>
#include <memory>

using namespace ansak;
using namespace std;
using namespace testing;

TEST(ExceptionTest, testException)
{
    NullPtrException e0;
    NullPtrException e1("nowhere-man");
    NullPtrException e2("much-better-than-a", 90125);

    EXPECT_EQ(string("Unexpected Nullptr."), e0.what());
    EXPECT_EQ(string("Unexpected Nullptr in function, nowhere-man."), e1.what());
    EXPECT_EQ(string("Unexpected Nullptr in function, much-better-than-a, at line #90125."), e2.what());

    unique_ptr<exception> up(static_cast<exception*>(new NullPtrException("constructed how", 32901)));
    up.reset(nullptr);

    try
    {
        throw NullPtrException();
    }
    catch (NullPtrException& e)
    {
        EXPECT_EQ(string("Unexpected Nullptr."), e.what());
    }
    catch (std::exception&)
    {
        ASSERT_FALSE(true);
    }
}

