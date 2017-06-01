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

#include <file_path.hxx>

#include <gtest/gtest.h>

#include <string.hxx>
#include <string_splitjoin.hxx>

#include <iostream>
#include <vector>
#include <sstream>

using namespace ansak;
using namespace std;
using namespace testing;

namespace {

const char* someUNCSharePath = "\\\\someserver\\somevolume\\one\\short\\boondoggle";

#if defined(WIN32)
const char rootPathString[] = "C:\\";
const utf8String someReasonableDir = { "C:\\Windows\\System32" };
const utf8String someForeignDir = { "/home/luser/some/directory" };
const utf8String boondogglePath = { "C:\\one\\long\\boondoggle\\to\\nowhere" };
const string shortBoondoggle = { "C:\\one\\short\\boondoggle" };
const string shortBoondoggleParent = { "C:\\one\\short" };
const char* someCwdString = "c:\\path\\to\\some\\cwd";
#else
const char rootPathString[] = "/";
const utf8String someReasonableDir = { "/home/luser/some/directory" };
const utf8String someForeignDir = { "C:\\Windows\\System32" };
const utf8String foreignShareDir = { "\\\\someserver\\someshare\\path\\to\\interesting\\file.txt" };
const utf8String boondogglePath = { "/one/long/boondoggle/to/nowhere" };
const string shortBoondoggle = { "/one/short/boondoggle" };
const string shortBoondoggleParent = { "/one/short" };
const char* someCwdString = "/path/to/some/cwd";
#endif

}

string asString(const vector<string>& src)
{
    ostringstream os;
    os << "[ ";
    bool doneOne = false;
    for (const auto& s : src)
    {
        if (doneOne)
        {
            os << ", ";
        }
        else
        {
            doneOne = true;
        }
        os << s;
    }
    os << " ]";
    return os.str();
}

TEST(FilePathTest, testConstructor)
{
    FilePath empty;
    EXPECT_EQ(".", empty.asUtf8String());
    EXPECT_EQ(u".", empty.asUtf16String());
    EXPECT_TRUE(empty.isValid());
    EXPECT_TRUE(empty.isReal());
    EXPECT_TRUE(empty.isRelative());
    EXPECT_FALSE(empty.isRoot());
    EXPECT_FALSE(empty.isUNCPath());

    FilePath otherEmpty;
    EXPECT_TRUE(empty == otherEmpty);
    EXPECT_TRUE(empty <= otherEmpty);
    EXPECT_TRUE(empty >= otherEmpty);
    EXPECT_FALSE(empty != otherEmpty);
    EXPECT_FALSE(empty < otherEmpty);
    EXPECT_FALSE(empty > otherEmpty);

    FilePath root(rootPathString);
    EXPECT_FALSE(root.asUtf8String().empty());
    EXPECT_FALSE(root.asUtf16String().empty());
    EXPECT_TRUE(root.isValid());
    EXPECT_TRUE(root.isReal());
    EXPECT_FALSE(root.isRelative());
    EXPECT_TRUE(root.isRoot());
    EXPECT_FALSE(root.isUNCPath());

    EXPECT_FALSE(root == empty);
    EXPECT_FALSE(root <= empty);
    EXPECT_TRUE(root >= empty);
    EXPECT_TRUE(root != empty);
    EXPECT_FALSE(root < empty);
    EXPECT_TRUE(root > empty);
}

TEST(FilePathTest, testForeignConstructor)
{
    FilePath foreignDirectory(someForeignDir);
    EXPECT_FALSE(foreignDirectory.asUtf8String().empty());
    EXPECT_FALSE(foreignDirectory.asUtf16String().empty());
    EXPECT_FALSE(foreignDirectory.isValid());
    EXPECT_FALSE(foreignDirectory.isReal());
    EXPECT_FALSE(foreignDirectory.isRelative());
    EXPECT_FALSE(foreignDirectory.isRoot());
    EXPECT_FALSE(foreignDirectory.isUNCPath());

#if !defined(WIN32)
    FilePath windowsShare(foreignShareDir);
    EXPECT_FALSE(windowsShare.asUtf8String().empty());
    EXPECT_FALSE(windowsShare.asUtf16String().empty());
    EXPECT_TRUE(windowsShare.isValid());
    EXPECT_FALSE(windowsShare.isReal());
    EXPECT_FALSE(windowsShare.isRelative());
    EXPECT_FALSE(windowsShare.isRoot());
    EXPECT_TRUE(windowsShare.isUNCPath());
#endif
}

TEST(FilePathTest, testInvalidBaseName)
{
    FilePath anInvalidDirectory(someForeignDir);
    EXPECT_FALSE(anInvalidDirectory.isValid());
    EXPECT_TRUE(anInvalidDirectory.basename().empty());
}

TEST(FilePathTest, testParentOfRelative)
{
    vector<string> pathElements { "one", "long", "boondoggle", "to", "nowhere" };
    auto pathString(join(pathElements, FilePath::pathSep));
    pathElements.pop_back();
    auto relative1Path(join(pathElements, FilePath::pathSep));
    pathElements.pop_back();
    auto relative2Path(join(pathElements, FilePath::pathSep));
    pathElements.pop_back();
    auto relative3Path(join(pathElements, FilePath::pathSep));
    pathElements.pop_back();
    auto relative4Path(join(pathElements, FilePath::pathSep));

    FilePath rootRelative(pathString);
    EXPECT_TRUE(rootRelative.isValid());
    EXPECT_TRUE(rootRelative.isReal());
    EXPECT_TRUE(rootRelative.isRelative());

    FilePath relative1(rootRelative.parent());
    EXPECT_TRUE(relative1.isValid());
    EXPECT_TRUE(relative1.isReal());
    EXPECT_TRUE(relative1.isRelative());
    EXPECT_EQ(relative1Path, relative1.asUtf8String());

    FilePath relative2(relative1.parent());
    EXPECT_TRUE(relative2.isValid());
    EXPECT_TRUE(relative2.isReal());
    EXPECT_TRUE(relative2.isRelative());
    EXPECT_EQ(relative2Path, relative2.asUtf8String());

    FilePath relative3(relative2.parent());
    EXPECT_TRUE(relative3.isValid());
    EXPECT_TRUE(relative3.isReal());
    EXPECT_TRUE(relative3.isRelative());
    EXPECT_EQ(relative3Path, relative3.asUtf8String());

    FilePath relative4(relative3.parent());
    EXPECT_TRUE(relative4.isValid());
    EXPECT_TRUE(relative4.isReal());
    EXPECT_TRUE(relative4.isRelative());
    EXPECT_EQ(relative4Path, relative4.asUtf8String());

    FilePath relative5(relative4.parent());
    EXPECT_TRUE(relative5.isValid());
    EXPECT_TRUE(relative5.isReal());
    EXPECT_TRUE(relative5.isRelative());
    EXPECT_EQ(".", relative5.asUtf8String());

    FilePath relative6(relative5.parent());
    EXPECT_TRUE(relative6.isValid());
    EXPECT_TRUE(relative6.isReal());
    EXPECT_TRUE(relative6.isRelative());
    EXPECT_EQ("..", relative6.asUtf8String());

    FilePath relative7(relative6.parent());
    EXPECT_TRUE(relative7.isValid());
    EXPECT_TRUE(relative7.isReal());
    EXPECT_TRUE(relative7.isRelative());
    auto relativeComponents(split(relative7.asUtf8String(), FilePath::pathSep));
    EXPECT_EQ(2u, relativeComponents.size());
    EXPECT_EQ("..", relativeComponents[0]);
    EXPECT_EQ("..", relativeComponents[1]);
}

TEST(FilePathTest, testParentOfNonRelative)
{
    auto components = split(boondogglePath, FilePath::pathSep);
    components.pop_back();
    string parent1Path = join(components, FilePath::pathSep);
    components.pop_back();
    string parent2Path = join(components, FilePath::pathSep);
    components.pop_back();
    string parent3Path = join(components, FilePath::pathSep);
    components.pop_back();
    string parent4Path = join(components, FilePath::pathSep);
    string root =
#if defined(WIN32)
                  components[0] + FilePath::pathSep
#else // if defined(WIN32)
                  "/"
#endif // if defined(WIN32) else
                  ;

    FilePath fullPath(boondogglePath);
    EXPECT_TRUE(fullPath.isValid());
    EXPECT_TRUE(fullPath.isReal());
    EXPECT_FALSE(fullPath.isRelative());

    FilePath relative1(fullPath.parent());
    EXPECT_TRUE(relative1.isValid());
    EXPECT_TRUE(relative1.isReal());
    EXPECT_FALSE(relative1.isRelative());
    EXPECT_EQ(parent1Path, relative1.asUtf8String());

    FilePath relative2(relative1.parent());
    EXPECT_TRUE(relative2.isValid());
    EXPECT_TRUE(relative2.isReal());
    EXPECT_FALSE(relative2.isRelative());
    EXPECT_EQ(parent2Path, relative2.asUtf8String());

    FilePath relative3(relative2.parent());
    EXPECT_TRUE(relative3.isValid());
    EXPECT_TRUE(relative3.isReal());
    EXPECT_FALSE(relative3.isRelative());
    EXPECT_EQ(parent3Path, relative3.asUtf8String());

    FilePath relative4(relative3.parent());
    EXPECT_TRUE(relative4.isValid());
    EXPECT_TRUE(relative4.isReal());
    EXPECT_FALSE(relative4.isRelative());
    EXPECT_EQ(parent4Path, relative4.asUtf8String());

    FilePath relative5(relative4.parent());
    EXPECT_TRUE(relative5.isValid());
    EXPECT_TRUE(relative5.isReal());
    EXPECT_FALSE(relative5.isRelative());
    EXPECT_EQ(root, relative5.asUtf8String());

    FilePath parentOfRoot(relative5.parent());
    EXPECT_FALSE(parentOfRoot.isValid());
    EXPECT_FALSE(parentOfRoot.isReal());
    EXPECT_FALSE(parentOfRoot.isRelative());
    EXPECT_FALSE(parentOfRoot.isRoot());
    EXPECT_FALSE(parentOfRoot.isUNCPath());
}

TEST(FilePathTest, testParentOfUNC)
{
    const string fullPathString = someUNCSharePath;
    auto fullPathComponents(split(fullPathString, FilePath::windowsPathSep));
    fullPathComponents.pop_back();
    auto parent1PathString(join(fullPathComponents, FilePath::windowsPathSep));
    fullPathComponents.pop_back();
    auto parent2PathString(join(fullPathComponents, FilePath::windowsPathSep));
    fullPathComponents.pop_back();
    auto rootPathString(join(fullPathComponents, FilePath::windowsPathSep));

    FilePath full(someUNCSharePath);
    FilePath parent1(full.parent());
    EXPECT_EQ(parent1PathString, parent1.asUtf8String());
    FilePath parent2(parent1.parent());
    EXPECT_EQ(parent2PathString, parent2.asUtf8String());
    FilePath root(parent2.parent());
    EXPECT_EQ(rootPathString, root.asUtf8String());

    FilePath invalid(root.parent());
    EXPECT_FALSE(invalid.isValid());
    EXPECT_FALSE(invalid.isReal());
    EXPECT_FALSE(invalid.isRelative());
    EXPECT_FALSE(invalid.isRoot());
    EXPECT_FALSE(invalid.isUNCPath());

    EXPECT_EQ(FilePath::invalidPath(), invalid);
}

TEST(FilePathTest, testParentCorners)
{
    auto invalid = FilePath::invalidPath();
    EXPECT_FALSE(invalid.isValid());
    EXPECT_FALSE(invalid.parent().isValid());

    FilePath dot(".");
    FilePath dotParent(dot.parent());
    EXPECT_EQ(FilePath(".."), dotParent);

    FilePath notDot("r");
    FilePath notDotParent(notDot.parent());
    EXPECT_EQ(dot, notDotParent);

#if defined(WIN32)
    FilePath twoCharRooter("\\r");
    FilePath rooter("\\");
    FilePath doubledoubleDot("..\\..");
    string threeDoubleDot("..\\..\\..");
#else
    FilePath twoCharRooter("/r");
    FilePath rooter("/");
    FilePath doubledoubleDot("../..");
    string threeDoubleDot("../../..");
#endif
    EXPECT_EQ(FilePath(doubledoubleDot), dotParent.parent());
    EXPECT_EQ(rooter, twoCharRooter.parent());
    FilePath twoChar("ab");
    EXPECT_EQ(dot, twoChar.parent());

    EXPECT_EQ(threeDoubleDot, doubledoubleDot.parent().asUtf8String());
}

TEST(FilePathTest, testChildOf)
{
    string childOfHere = shortBoondoggle;
    childOfHere += FilePath::pathSep;
    childOfHere += "here";

    FilePath base(shortBoondoggle);
    FilePath noChild(base.child("."));
    EXPECT_EQ(noChild, base);
    FilePath anotherNoChild(base.child(""));
    EXPECT_EQ(anotherNoChild, base );
    FilePath oneChild(base.child("x"));
    EXPECT_NE(oneChild, base);
    FilePath twoChild(base.child("xx"));
    EXPECT_NE(twoChild, base);

    EXPECT_EQ(FilePath::invalidPath(), FilePath::invalidPath().child("someChild"));

    FilePath parent(base.child(".."));
    EXPECT_EQ(shortBoondoggleParent, parent.asUtf8String());

    FilePath here(base.child("here"));
    EXPECT_EQ(childOfHere, here.asUtf8String());
}

TEST(FilePathTest, testChildOfUNC)
{
    const string fullPathString = "\\\\someserver\\somevolume\\one\\short\\boondoggle";
    const string childOfDotDot = "\\\\someserver\\somevolume\\one\\short";

    string childOfHere = fullPathString;
    childOfHere += FilePath::windowsPathSep;
    childOfHere += "here";

    FilePath base(fullPathString);
    FilePath noChild(base.child("."));
    EXPECT_EQ(noChild, base);

    FilePath parent(base.child(".."));
    EXPECT_EQ(childOfDotDot, parent.asUtf8String());

    FilePath here(base.child("here"));
    EXPECT_EQ(childOfHere, here.asUtf8String());
}

TEST(FilePathTest, testRelativeToCWDrive)
{
    const string fullPathString = "\\some\\path\\on\\current\\drive";
    FilePath fullPath(fullPathString);
    auto components = split(fullPathString, FilePath::windowsPathSep);
    components.pop_back();
    string parent1Path = join(components, FilePath::windowsPathSep);    // "\\some\\path\\on\\current";
    components.pop_back();
    string parent2Path = join(components, FilePath::windowsPathSep);    // "\\some\\path\\on";
    components.pop_back();
    string parent3Path = join(components, FilePath::windowsPathSep);    // "\\some\\path";
    components.pop_back();
    string parent4Path = join(components, FilePath::windowsPathSep);    // "\\some";
    string root = "\\";

    EXPECT_EQ(fullPathString, fullPath.asUtf8String());
    EXPECT_FALSE(fullPath.isRoot());
    EXPECT_FALSE(fullPath.isUNCPath());

#if !defined(WIN32)
    EXPECT_FALSE(fullPath.isValid());
    EXPECT_FALSE(fullPath.isReal());
#else // if !defined(WIN32)
    EXPECT_TRUE(fullPath.isValid());
    EXPECT_TRUE(fullPath.isRelative());
    EXPECT_TRUE(fullPath.isReal());

    FilePath relative1(fullPath.parent());
    EXPECT_EQ(parent1Path, relative1.asUtf8String());
    EXPECT_TRUE(relative1.isValid());
    EXPECT_FALSE(relative1.isRoot());
    EXPECT_FALSE(relative1.isUNCPath());
    EXPECT_TRUE(relative1.isRelative());
    EXPECT_TRUE(relative1.isReal());

    FilePath relative2(relative1.parent());
    EXPECT_EQ(parent2Path, relative2.asUtf8String());
    EXPECT_TRUE(relative2.isValid());
    EXPECT_FALSE(relative2.isRoot());
    EXPECT_FALSE(relative2.isUNCPath());
    EXPECT_TRUE(relative2.isRelative());
    EXPECT_TRUE(relative2.isReal());

    FilePath relative3(relative2.parent());
    EXPECT_EQ(parent3Path, relative3.asUtf8String());
    EXPECT_TRUE(relative3.isValid());
    EXPECT_FALSE(relative3.isRoot());
    EXPECT_FALSE(relative3.isUNCPath());
    EXPECT_TRUE(relative3.isRelative());
    EXPECT_TRUE(relative3.isReal());

    FilePath relative4(relative3.parent());
    EXPECT_EQ(parent4Path, relative4.asUtf8String());
    EXPECT_TRUE(relative4.isValid());
    EXPECT_FALSE(relative4.isRoot());
    EXPECT_FALSE(relative4.isUNCPath());
    EXPECT_TRUE(relative4.isRelative());
    EXPECT_TRUE(relative4.isReal());

    FilePath relativeToRoot(relative4.parent());
    EXPECT_EQ(root, relativeToRoot.asUtf8String());
    EXPECT_TRUE(relativeToRoot.isValid());
    EXPECT_FALSE(relativeToRoot.isRoot());
    EXPECT_FALSE(relativeToRoot.isUNCPath());
    EXPECT_TRUE(relativeToRoot.isRelative());
    EXPECT_TRUE(relativeToRoot.isReal());

    FilePath parentOfRoot(relativeToRoot.parent());
    EXPECT_FALSE(parentOfRoot.isValid());
    EXPECT_FALSE(parentOfRoot.isReal());
    EXPECT_FALSE(parentOfRoot.isRelative());
    EXPECT_FALSE(parentOfRoot.isRoot());
    EXPECT_FALSE(parentOfRoot.isUNCPath());
#endif // if !defined(WIN32)
}

TEST(FilePathTest, testRootPathFrom)
{
    FilePath someCwd(someCwdString);
    EXPECT_EQ("cwd", someCwd.basename());

    FilePath uncCwd("\\\\server\\share\\path\\to\\some\\cwd");

    FilePath null;
    FilePath realizedNull(null.rootPathFrom(someCwd));
    EXPECT_EQ(someCwd, realizedNull);
    FilePath dot(".");
    FilePath realizedDot(dot.rootPathFrom(someCwd));
    EXPECT_EQ(someCwd, realizedDot);
    FilePath parentOfNull("..");
    FilePath realizedParentOfNull(parentOfNull.rootPathFrom(someCwd));
    EXPECT_EQ(someCwd.parent(), realizedParentOfNull);

    FilePath child("there");
    FilePath realizedChild(child.rootPathFrom(someCwd));
    EXPECT_EQ(someCwd.child("there"), realizedChild);
    FilePath realizedUncChild(child.rootPathFrom(uncCwd));
    EXPECT_EQ(uncCwd.child("there"), realizedUncChild);

    EXPECT_FALSE(someCwd.rootPathFrom(realizedNull).isValid());
    EXPECT_FALSE(null.rootPathFrom(child).isValid());
}

TEST(FilePathTest, testUncServer)
{
    auto invalid = FilePath::invalidPath();
    FilePath uncServer("\\\\server\\share");
    FilePath dot("..");
    EXPECT_EQ(invalid, dot.rootPathFrom(uncServer));
}

#if defined(WIN32)

TEST(FilePathTest, testRelativeToDriveCWD)
{
    const string fullPathString = "c:some\\path\\on\\current\\drive";
    FilePath fullPath(fullPathString);
    auto components = split(fullPathString, FilePath::windowsPathSep);
    components.pop_back();
    string parent1Path = join(components, FilePath::windowsPathSep);    // "c:some\\path\\on\\current";
    components.pop_back();
    string parent2Path = join(components, FilePath::windowsPathSep);    // "c:some\\path\\on";
    components.pop_back();
    string parent3Path = join(components, FilePath::windowsPathSep);    // "c:some\\path";
    components.pop_back();
    string parent4Path = join(components, FilePath::windowsPathSep);    // "c:some";
    string relativeRoot = "c:";
    string parentOfRelativeRoot = "c:..";
    string grandParentOfRelativeRoot = "c:..\\..";

    EXPECT_EQ(fullPathString, fullPath.asUtf8String());
    EXPECT_TRUE(fullPath.isValid());
    EXPECT_FALSE(fullPath.isRoot());
    EXPECT_FALSE(fullPath.isUNCPath());
    EXPECT_TRUE(fullPath.isRelative());
    EXPECT_TRUE(fullPath.isReal());

    FilePath relative1(fullPath.parent());
    EXPECT_EQ(parent1Path, relative1.asUtf8String());
    EXPECT_TRUE(relative1.isValid());
    EXPECT_FALSE(relative1.isRoot());
    EXPECT_FALSE(relative1.isUNCPath());
    EXPECT_TRUE(relative1.isRelative());
    EXPECT_TRUE(relative1.isReal());

    FilePath relative2(relative1.parent());
    EXPECT_EQ(parent2Path, relative2.asUtf8String());
    EXPECT_TRUE(relative2.isValid());
    EXPECT_FALSE(relative2.isRoot());
    EXPECT_FALSE(relative2.isUNCPath());
    EXPECT_TRUE(relative2.isRelative());
    EXPECT_TRUE(relative2.isReal());

    FilePath relative3(relative2.parent());
    EXPECT_TRUE(relative3.asUtf8String() == parent3Path);
    EXPECT_TRUE(relative3.isValid());
    EXPECT_FALSE(relative3.isRoot());
    EXPECT_FALSE(relative3.isUNCPath());
    EXPECT_TRUE(relative3.isRelative());
    EXPECT_TRUE(relative3.isReal());

    FilePath relative4(relative3.parent());
    EXPECT_EQ(parent4Path, relative4.asUtf8String());
    EXPECT_TRUE(relative4.isValid());
    EXPECT_FALSE(relative4.isRoot());
    EXPECT_FALSE(relative4.isUNCPath());
    EXPECT_TRUE(relative4.isRelative());
    EXPECT_TRUE(relative4.isReal());

    FilePath relativeToRelativeRoot(relative4.parent());
    EXPECT_EQ(relativeRoot, relativeToRelativeRoot.asUtf8String());
    EXPECT_TRUE(relativeToRelativeRoot.isValid());
    EXPECT_FALSE(relativeToRelativeRoot.isRoot());
    EXPECT_FALSE(relativeToRelativeRoot.isUNCPath());
    EXPECT_TRUE(relativeToRelativeRoot.isRelative());
    EXPECT_TRUE(relativeToRelativeRoot.isReal());

    FilePath parentOfRelativeToRelativeRoot(relativeToRelativeRoot.parent());
    EXPECT_EQ(parentOfRelativeRoot, parentOfRelativeToRelativeRoot.asUtf8String());
    EXPECT_TRUE(parentOfRelativeToRelativeRoot.isValid());
    EXPECT_FALSE(parentOfRelativeToRelativeRoot.isRoot());
    EXPECT_FALSE(parentOfRelativeToRelativeRoot.isUNCPath());
    EXPECT_TRUE(parentOfRelativeToRelativeRoot.isRelative());
    EXPECT_TRUE(parentOfRelativeToRelativeRoot.isReal());

    FilePath grandParentOfRelativeToRelativeRoot(parentOfRelativeToRelativeRoot.parent());
    EXPECT_EQ(grandParentOfRelativeRoot, grandParentOfRelativeToRelativeRoot.asUtf8String());
    EXPECT_TRUE(grandParentOfRelativeToRelativeRoot.isValid());
    EXPECT_FALSE(grandParentOfRelativeToRelativeRoot.isRoot());
    EXPECT_FALSE(grandParentOfRelativeToRelativeRoot.isUNCPath());
    EXPECT_TRUE(grandParentOfRelativeToRelativeRoot.isRelative());
    EXPECT_TRUE(grandParentOfRelativeToRelativeRoot.isReal());

    const string cwdOfDrive = "C:.";
    const string parentOfCwdOfDrive = "C:..";
    FilePath cwdOfDrivePath(cwdOfDrive);
    FilePath parentTest(cwdOfDrivePath.parent());
    EXPECT_EQ(parentOfCwdOfDrive, parentTest.asUtf8String());
}

TEST(FilePathTest, testReversedSlashDOSPath)
{
    const string fullPathString = "c:\\some\\path\\on\\the\\current\\drive";
    const string forwardPathString = "c:/some/path/on/the/current/drive";
    FilePath understandIt(forwardPathString);
    EXPECT_EQ(fullPathString, understandIt.asUtf8String());
}

#endif
