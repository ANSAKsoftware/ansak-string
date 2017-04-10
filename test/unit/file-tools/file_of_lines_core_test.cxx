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
// 2017.04.10 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines_exception_test.cxx -- unit test for file-of-lines exceptions
//
///////////////////////////////////////////////////////////////////////////

#include <file_of_lines_core.hxx>

#include <file_path.hxx>
#include <file_handle_exception.hxx>
#include <mock_operating_system.hxx>
#include <mock_file_system_path.hxx>
#include <memory>

#include <gtest/gtest.h>

using namespace ansak;
using namespace ansak::internal;
using namespace std;
using namespace testing;

namespace ansak {

extern unique_ptr<OperatingSystemMock> primitive;
extern OperatingSystemMock* theMock;

}

using namespace ansak;

namespace
{
#if defined(WIN32)
FilePath problemFile("c:\\Users\\jvplasmeuden\\homeworklines.txt");
#else
FilePath problemFile("/home/jvplasmeuden/homeworklines.txt");
#endif
}

TEST(LinesCoreTest, construct)
{
    auto path = FileSystemPath(problemFile);
    EXPECT_FALSE(FileOfLinesCore(path).isFileOpen());
}
