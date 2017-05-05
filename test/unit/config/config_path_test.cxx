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
// 2017.05.01 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config_path_test.cxx -- Unit tests for path checkers in config
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <config_io.hxx>
#include <file_path.hxx>
#include <config_paths.hxx>

#include <string>

#if !defined(WIN32)
#include <stdlib.h>
#endif

using namespace std;
using namespace ansak;
using namespace ansak::config;

TEST(ConfigFileIOTest, context)
{
    setContext("YoyodyneCorp", "EightDimensions");
    EXPECT_EQ("YoyodyneCorp", vendorContext);
    EXPECT_EQ("EightDimensions", productContext);

    setContext("ansak", "testbed");
    EXPECT_EQ(string("ansak"), vendorContext);
    EXPECT_EQ(string("testbed"), productContext);
}

#if defined(WIN32)

TEST(ConfigFileIOTest, getPaths)
{
    setContext("paiste", "cymbals");

    EXPECT_EQ("HKCU\\Software\\paiste\\cymblas", getUserConfigFilePath());
    EXPECT_EQ("HKLM\\Software\\paiste\\cymblas", getSystemConfigFilePath());
}

#else

TEST(ConfigFileIOTest, getPaths)
{
    setContext("zildjian", "cymbals");
    auto userPath = getUserConfigFilePath();

    FilePath userRoot0(getenv("HOME"));
    auto userPath0 = userRoot0.child(".zildjian").child("cymbalsrc");
    EXPECT_EQ(userPath0, userPath);

    FilePath systemRoot0("/etc");
    auto systemPath0 = systemRoot0.child("zildjian.conf").child("cymbalsrc");
    auto systemPath = getSystemConfigFilePath();
    EXPECT_EQ(systemPath0, systemPath);
}

#endif

