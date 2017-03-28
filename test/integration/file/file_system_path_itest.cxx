///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015, Arthur N. Klassen
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
// 2015.12.26 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_path_test.cxx -- Tests for FilePath in file_path.cxx
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "file_system_path.hxx"
#include "string_splitjoin.hxx"

#include <iostream>
#include <vector>
#include <sstream>

using namespace ansak;
using namespace std;
using namespace testing;

TEST(FileSystemPathITest, testConstructor)
{
    // implicitly goes through FilePath
    FileSystemPath empty;
    EXPECT_FALSE(empty.asUtf8String().empty());
    EXPECT_FALSE(empty.asUtf16String().empty());
    EXPECT_TRUE(empty.isValid());
    EXPECT_FALSE(empty.isRelative());
    EXPECT_FALSE(empty.isRoot());
    EXPECT_TRUE(empty.exists());
    EXPECT_FALSE(empty.isFile());
    EXPECT_TRUE(empty.isDir());
    EXPECT_EQ(0u, empty.size());

    FileSystemPath otherEmpty;
    EXPECT_TRUE(empty == otherEmpty);
    EXPECT_TRUE(empty <= otherEmpty);
    EXPECT_TRUE(empty >= otherEmpty);
    EXPECT_FALSE(empty != otherEmpty);
    EXPECT_FALSE(empty < otherEmpty);
    EXPECT_FALSE(empty > otherEmpty);

    // straight from a string
#if defined(WIN32)
    FileSystemPath root("C:\\");
#else
    FileSystemPath root("/");
#endif
    EXPECT_FALSE(root.asUtf8String().empty());
    EXPECT_FALSE(root.asUtf16String().empty());
    EXPECT_TRUE(root.isValid());
    EXPECT_FALSE(root.isRelative());
    EXPECT_TRUE(root.isRoot());
    EXPECT_TRUE(root.exists());
    EXPECT_FALSE(root.isFile());
    EXPECT_TRUE(root.isDir());
    EXPECT_EQ(0u, root.size());

    EXPECT_FALSE(root == empty);
    EXPECT_FALSE(root >= empty);
    EXPECT_TRUE(root <= empty);
    EXPECT_TRUE(root != empty);
    EXPECT_TRUE(root < empty);
    EXPECT_FALSE(root > empty);
}

TEST(FileSystemPathITest, testBadConstructor)
{
    // implicitly goes through FilePath
    ostringstream os;
    os << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep
       << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep
       << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep
       << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep << ".." << FilePath::pathSep << "i-expect-some-file-here";

    FileSystemPath bad0(os.str());
    EXPECT_FALSE(bad0.isValid());
    EXPECT_FALSE(bad0.isRelative());
    EXPECT_FALSE(bad0.isRoot());
    EXPECT_FALSE(bad0.isFile());
    EXPECT_FALSE(bad0.exists());
    EXPECT_FALSE(bad0.isDir());
    EXPECT_EQ(0u, bad0.size());

    // yes, I'm asking the systems to parse the wrong root...
#if defined(WIN32)
    FilePath falseRoot("/");
#else
    FilePath falseRoot("C:\\");
#endif
    FileSystemPath bad1(falseRoot);
    EXPECT_FALSE(bad1.isValid());
    EXPECT_FALSE(bad1.isRelative());
    EXPECT_FALSE(bad1.isRoot());
#if defined(WIN32)
    EXPECT_TRUE(bad1.exists());
    EXPECT_TRUE(bad1.isDir());
#else
    EXPECT_FALSE(bad1.exists());
    EXPECT_FALSE(bad1.isDir());
#endif
    EXPECT_FALSE(bad1.isFile());
    EXPECT_EQ(0u, bad1.size());
}

TEST(FileSystemPathITest, testCreateDirectory)
{
    FileSystemPath thisSrcFile(__FILE__);
    FileSystemPath thisParent(thisSrcFile.parent());
    EXPECT_TRUE(thisParent.createDirectory());
    EXPECT_FALSE(thisParent.createFile());

    FilePath srcFileAsDir(thisSrcFile);
    FilePath subSrc(srcFileAsDir.child("noway"));
    FileSystemPath subSrcForSystem(subSrc);
    EXPECT_FALSE(subSrcForSystem.createDirectory());

    FilePath parentPath(thisParent);
    FilePath subDeepPath(parentPath.child("some").child("deeper").child("path"));
    FileSystemPath subDeep(subDeepPath);
    EXPECT_FALSE(subDeep.createDirectory());
    FileSystemPath subDeepParent(parentPath.child("some"));
    subDeepParent.remove(FileSystemPath::recursively);

#if !defined(WIN32)
    FilePath usr("/usr");
    FilePath dontTellMeYouCanDoThis(usr.child("bin").child("dont").child("tell").child("me").child("you").child("can"));
    FileSystemPath noneSuch(dontTellMeYouCanDoThis);
    EXPECT_FALSE(noneSuch.createDirectory(FileSystemPath::recursively));
#endif
}

TEST(FileSystemPathITest, testRemoveDirectory)
{
    FileSystemPath thisSrcFile(__FILE__);
    FileSystemPath grandParent(thisSrcFile.parent().parent());
    EXPECT_FALSE(grandParent.remove());

    FileSystemPath thisParent(thisSrcFile.parent());
    FilePath parentPath(thisParent);
    FileSystemPath sib(parentPath.child("sib"));
    sib.createDirectory();
    EXPECT_TRUE(sib.exists());
    EXPECT_TRUE(sib.isDir());
    EXPECT_TRUE(sib.remove());
    EXPECT_FALSE(sib.exists());

/*  A tempting test but I don't dare leave it in in case someone suids the test
#if !defined(WIN32)
    FileSystemPath usrbin("/usr/bin");
    EXPECT_FALSE(usrbin.remove());
#endif
*/
}

TEST(FileSystemPathITest, testCreateFile)
{
    FileSystemPath thisSrcFile(__FILE__);
    EXPECT_FALSE(thisSrcFile.createDirectory());
    EXPECT_TRUE(thisSrcFile.createFile());
    EXPECT_FALSE(thisSrcFile.createFile(FileSystemPath::failIfThere));
    EXPECT_TRUE(thisSrcFile.createFile());

    FilePath srcFileAsDir(thisSrcFile);
    FilePath subSrc(srcFileAsDir.child("noway"));
    FileSystemPath subSrcForSystem(subSrc);
    EXPECT_FALSE(subSrcForSystem.createFile());
    EXPECT_TRUE(subSrcForSystem.remove());
    EXPECT_TRUE(subSrcForSystem.remove(FileSystemPath::recursively));

#if !defined(WIN32)
    FilePath usrbin("/usr/bin");
    FileSystemPath youCantTouchThis(usrbin.child("youCantTouchThis"));
    EXPECT_FALSE(youCantTouchThis.createFile());
#endif
}

TEST(FileSystemPathITest, testThisSourceFile)
{
    FileSystemPath thisFile(__FILE__);
    EXPECT_FALSE(thisFile.asUtf8String().empty());
    EXPECT_FALSE(thisFile.asUtf16String().empty());
    EXPECT_TRUE(thisFile.isValid());
    EXPECT_FALSE(thisFile.isRelative());
    EXPECT_FALSE(thisFile.isRoot());
    EXPECT_TRUE(thisFile.exists());
    EXPECT_TRUE(thisFile.isFile());
    EXPECT_FALSE(thisFile.isDir());
    EXPECT_GT(thisFile.size(), 3072u);
    EXPECT_GT(thisFile.lastModTime().year, 2015u);
    FileSystemPath notHere("./give-me-a-break.basic");
    auto noTime(notHere.lastModTime());
    EXPECT_EQ(0u, noTime.year);
    EXPECT_EQ(0u, noTime.month);
    EXPECT_EQ(0u, noTime.day);
    EXPECT_EQ(0u, noTime.hour);
    EXPECT_EQ(0u, noTime.minute);
    EXPECT_EQ(0u, noTime.second);

    FilePath thisFileAsPath(thisFile);
    EXPECT_EQ(thisFile.asUtf8String(), thisFileAsPath.asUtf8String());
    EXPECT_EQ(thisFile.asUtf16String(), thisFileAsPath.asUtf16String());
    EXPECT_EQ(thisFile.asUtf16String(), toUtf16(thisFileAsPath.asUtf8String()));
}

TEST(FileSystemPathITest, testScanThisDir)
{
    {
        FileSystemPath p;
        auto retriever = p.children();
        int entriesFound = 0;

        for (auto c = retriever(); c.isValid(); ++entriesFound, c = retriever())
            ;
        EXPECT_LT(2, entriesFound);
    }

    {
        FileSystemPath p;
        auto retriever = p.children();

        auto c = retriever();
        c = retriever();
    }
}
