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
// 2017.03.30 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// test_os_layer.cxx -- Test of the currently available OS Layer
//
///////////////////////////////////////////////////////////////////////////

#include <operating_system_primitives.hxx>
#include <file_path.hxx>

#include <gtest/gtest.h>
#if defined(WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <sstream>

using namespace ansak;
using namespace std;
using namespace testing;

namespace {

const OperatingSystemPrimitives* p = getOperatingSystemPrimitives();

string generateTempName()
{
    static int x = 0;
    ostringstream os;
    os << p->getProcessID() << "-ansakTest-" << ++x;
    return os.str();
}

#if defined(WIN32)

string getSystemRoot()
{
    return string(p->getEnvironmentVariable("SystemRoot"));
}

FilePath getWinEtc()
{
    FilePath s(getSystemRoot());
    return s.child("System32").child("drivers").child("etc");
}

#endif

#if defined(WIN32)
FilePath badPath("f:\\pink\\elephants\\on\\parade");
#else
FilePath badPath("/pink/elephants/on/parade");
#endif

}

class OSLayerFileTest : public Test
{
public:
    OSLayerFileTest() : Test()
    {
        p->createFile(m_filePath);
    }

    ~OSLayerFileTest()
    {
        p->remove(m_filePath);
    }

    const FilePath& TestFile() const { return m_filePath; }
    const string& FileName() const { return m_fileName; }

private:
    string      m_fileName = generateTempName();
    FilePath    m_filePath = p->getTempFilePath().child(m_fileName);
};

TEST(OSLayerTest, testPathExists)
{
#if defined(WIN32)
    EXPECT_TRUE(p->pathExists(FilePath(getSystemRoot()).child("win.ini")));
#elif defined(__APPLE__)
    EXPECT_TRUE(p->pathExists(FilePath("/Library/Preferences/.GlobalPreferences.plist")));
#else
    EXPECT_TRUE(p->pathExists(FilePath("/proc/cpuinfo")));
#endif
    EXPECT_FALSE(p->pathExists(badPath));
}

TEST(OSLayerTest, testPathIsFile)
{
#if defined(WIN32)
    EXPECT_TRUE(p->pathIsFile(getWinEtc().child("hosts")));
    EXPECT_FALSE(p->pathIsFile(getWinEtc()));
#else
    EXPECT_TRUE(p->pathIsFile(FilePath("/etc/hosts")));
    EXPECT_FALSE(p->pathIsFile(FilePath("/etc")));
#endif
    EXPECT_FALSE(p->pathIsFile(badPath));
}

TEST(OSLayerTest, testPathIsDir)
{
#if defined(WIN32)
    EXPECT_FALSE(p->pathIsDir(getWinEtc().child("hosts")));
    EXPECT_TRUE(p->pathIsDir(getWinEtc()));
#else
    EXPECT_FALSE(p->pathIsDir(FilePath("/etc/hosts")));
    EXPECT_TRUE(p->pathIsDir(FilePath("/etc")));
#endif
    EXPECT_FALSE(p->pathIsDir(badPath));
}

TEST(OSLayerTest, testFileSizeFromPath)
{
#if defined(WIN32)
    EXPECT_LT(128u, p->fileSize(getWinEtc().child("hosts")));
    EXPECT_EQ(0u, p->fileSize(getWinEtc()));
#else
    EXPECT_LT(128u, p->fileSize(FilePath("/etc/hosts")));
    EXPECT_EQ(0u, p->fileSize(FilePath("/etc")));
#endif
    EXPECT_EQ(0u, p->fileSize(badPath));
}

TEST(OSLayerTest, testLastModTime)
{
#if defined(WIN32)
    auto tFile = p->lastModTime(getWinEtc().child("hosts"));
    auto tDir = p->lastModTime(getWinEtc());
#else
    auto tFile = p->lastModTime(FilePath("/etc/hosts"));
    auto tDir = p->lastModTime(FilePath("/etc"));
#endif
    auto tPretend = p->lastModTime(badPath);
    TimeStamp z { 0, 0, 0, 0, 0, 0 };

    EXPECT_LT(1970u, tFile.year);
    EXPECT_LT(1970u, tDir.year);
    EXPECT_EQ(tPretend, z);
}

TEST(OSLayerTest, testGetCurrentWorkingDirectory)
{
    EXPECT_NE(FilePath::invalidPath(), p->getCurrentWorkingDirectory());
}

TEST(OSLayerTest, testCreateRemoveDirectory)
{
     string subDirName = generateTempName();
     FilePath subDirTemp = p->getTempFilePath().child(subDirName);
     FilePath subDirLalaLand = badPath.child(subDirName);

     EXPECT_FALSE(p->createDirectory(subDirLalaLand));
     EXPECT_TRUE(p->createDirectory(subDirTemp));
     ASSERT_TRUE(p->pathExists(subDirTemp));
     ASSERT_FALSE(p->pathIsFile(subDirTemp));
     ASSERT_TRUE(p->pathIsDir(subDirTemp));

     EXPECT_FALSE(p->removeDirectory(subDirLalaLand));
     EXPECT_TRUE(p->removeDirectory(subDirTemp));
     ASSERT_FALSE(p->pathExists(subDirTemp));
     ASSERT_FALSE(p->pathIsDir(subDirTemp));
}

TEST_F(OSLayerFileTest, testCreateRemoveFile)
{
     string fileName = generateTempName();
     FilePath fileInTemp = p->getTempFilePath().child(fileName);
     FilePath fileInLalaLand = badPath.child(fileName);

     EXPECT_FALSE(p->createFile(fileInLalaLand));
     EXPECT_TRUE(p->createFile(fileInTemp));
     ASSERT_TRUE(p->pathExists(fileInTemp));
     ASSERT_TRUE(p->pathIsFile(fileInTemp));
     ASSERT_FALSE(p->pathIsDir(fileInTemp));

     EXPECT_FALSE(p->remove(fileInLalaLand));
     EXPECT_TRUE(p->remove(fileInTemp));
}

/* TEST_F(OSLayerFileTest, testCreateEmptiesFile)
{
    // open the file, write to it, close it
    // create the file as a path, check its size
} */

TEST(OSLayerTest, testMove)
{
    string fileName1 = generateTempName();
    string fileName2 = generateTempName();
    FilePath file1InTemp = p->getTempFilePath().child(fileName1);
    FilePath file2InTemp = p->getTempFilePath().child(fileName2);
    FilePath badFileName = badPath.child(fileName1);

    p->createFile(file1InTemp);
    EXPECT_FALSE(p->move(file1InTemp, badFileName));
    EXPECT_TRUE(p->move(file1InTemp, file2InTemp));
    EXPECT_FALSE(p->move(badFileName, file1InTemp));
    ASSERT_FALSE(p->pathExists(file1InTemp));
    ASSERT_TRUE(p->pathExists(file2InTemp));
    ASSERT_TRUE(p->pathIsFile(file2InTemp));
    ASSERT_FALSE(p->pathIsDir(file2InTemp));

    p->remove(file2InTemp);
}

TEST(OSLayerTest, testCreate)
{
    string fileName = generateTempName();
    FilePath badFileName = badPath.child(fileName);
    FilePath fileInTemp = p->getTempFilePath().child(fileName);

    unsigned int errorID = 0u;
    EXPECT_EQ(~static_cast<unsigned long long>(0), p->create(badFileName, 0660, errorID));
    ASSERT_NE(0u, errorID);

    errorID = 0u;
    auto fd = p->create(fileInTemp, 0660, errorID);
    EXPECT_NE(~static_cast<unsigned long long>(0), fd);
    ASSERT_EQ(0u, errorID);

    p->close(fd, errorID);
    p->remove(fileInTemp);
}

TEST_F(OSLayerFileTest, testOpenClose)
{
#if defined(WIN32)
    FilePath onlyForReading(getWinEtc().child("hosts"));
#else
    FilePath onlyForReading("/etc/hosts");
#endif
    FilePath cantOpenAtAll(badPath);

    // test kForReading
    unsigned int errorID = 0;
    auto fdU = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForReading, 0660, errorID);
    EXPECT_NE(~static_cast<unsigned long long>(0), fdU);
    ASSERT_EQ(0u, errorID);
    auto fdS = p->open(onlyForReading, OperatingSystemPrimitives::OpenMode::kForReading, 0660, errorID);
    EXPECT_NE(~static_cast<unsigned long long>(0), fdS);
    ASSERT_EQ(0u, errorID);
    ASSERT_EQ(~static_cast<unsigned long long>(0),
              p->open(cantOpenAtAll, OperatingSystemPrimitives::kForReading,
                      0660, errorID));
    ASSERT_NE(0u, errorID);

    p->close(fdU, errorID);
    EXPECT_EQ(0u, errorID);
    p->close(fdS, errorID);
    EXPECT_EQ(0u, errorID);

    // test kForAppending
    fdU = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForAppending, 0660, errorID);
    EXPECT_NE(~static_cast<unsigned long long>(0), fdU);
    ASSERT_EQ(0u, errorID);
    fdS = p->open(onlyForReading, OperatingSystemPrimitives::OpenMode::kForAppending, 0660, errorID);
    EXPECT_EQ(~static_cast<unsigned long long>(0), fdS);
    ASSERT_NE(0u, errorID);
    ASSERT_EQ(~static_cast<unsigned long long>(0),
              p->open(cantOpenAtAll, OperatingSystemPrimitives::kForAppending,
                      0660, errorID));
    ASSERT_NE(0u, errorID);

    p->close(fdU, errorID);
    EXPECT_EQ(0u, errorID);

    // test kForReadingAndWriting
    fdU = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForReadingAndWriting, 0660, errorID);
    EXPECT_NE(~static_cast<unsigned long long>(0), fdU);
    ASSERT_EQ(0u, errorID);
    fdS = p->open(onlyForReading, OperatingSystemPrimitives::OpenMode::kForReadingAndWriting, 0660, errorID);
    EXPECT_EQ(~static_cast<unsigned long long>(0), fdS);
    ASSERT_NE(0u, errorID);
    ASSERT_EQ(~static_cast<unsigned long long>(0),
              p->open(cantOpenAtAll, OperatingSystemPrimitives::kForReadingAndWriting,
                      0660, errorID));
    ASSERT_NE(0u, errorID);

    p->close(fdU, errorID);
    EXPECT_EQ(0u, errorID);
}

/* TEST_F(OSLayerFileTest, testOpenForAppending)
{
    // create the file open, write to it, close it
    // open the file for appending to it, write to it, close it
    // open the file for reading -- make sure that the contents are correct, close it
    // open the file for appending to it, seek to 0, write to it, close it
    // open the file for reading -- make sure that the contents are correct, close it
} */

TEST_F(OSLayerFileTest, testFileSizeFromHandle)
{
    unsigned int errorID = 0;
    auto fd = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForReadingAndWriting, 0660, errorID);

    EXPECT_EQ(static_cast<uint64_t>(0u), p->fileSize(fd, errorID));
    EXPECT_EQ(0u, errorID);

    const char c[] = "A tutor who tooted a flute.";
    EXPECT_EQ(sizeof(c) - 1, p->write(fd, c, sizeof(c) - 1, errorID));
    EXPECT_EQ(static_cast<uint64_t>(sizeof(c) - 1), p->fileSize(fd, errorID));

    p->close(fd, errorID);
    EXPECT_EQ(~static_cast<uint64_t>(0u), p->fileSize(fd, errorID));
}

TEST_F(OSLayerFileTest, testSeek)
{
    unsigned int errorID = 0;
    auto fd = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForReadingAndWriting, 0660, errorID);

    EXPECT_EQ(static_cast<uint64_t>(0u), p->fileSize(fd, errorID));
    EXPECT_EQ(0u, errorID);

    const char c[] = "Twas brillig and the slithy toves.";
    const char cToD[] = "ringing";
    p->write(fd, c, sizeof(c) - 1, errorID);
    errorID = 1;
    p->seek(fd, 13, errorID);
    EXPECT_EQ(0u, errorID);
    p->write(fd, cToD, sizeof(cToD) - 1, errorID);
    p->close(fd, errorID);

    fd = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForReading, 0660, errorID);
    char e[80];
    string d = "Twas brillig ringing slithy toves.";
    e[p->read(fd, e, sizeof(e) - 1, errorID)] = '\0';;
    EXPECT_EQ(d, string(e));

    p->close(fd, errorID);
}

TEST_F(OSLayerFileTest, testReadAndWrite)
{
    unsigned int errorID = 0;
    auto fd = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForReadingAndWriting, 0660, errorID);

    string s("It's a message!");
    auto written = p->write(fd, s.c_str(), s.size(), errorID);
    EXPECT_EQ(s.size(), written);
    EXPECT_EQ(0u, errorID);
    p->close(fd, errorID);

    fd = p->open(TestFile(), OperatingSystemPrimitives::OpenMode::kForReadingAndWriting, 0660, errorID);
    char b[50];
    auto readIn = p->read(fd, b, sizeof(b), errorID);
    EXPECT_EQ(s.size(), readIn);
    EXPECT_EQ(0u, errorID);
    b[readIn] = '\0';
    EXPECT_EQ(s, b);

    p->close(fd, errorID);

    written = p->write(fd, s.c_str(), s.size(), errorID);
    EXPECT_EQ(~static_cast<size_t>(0u), written);
    EXPECT_NE(0u, errorID);
    written = p->write(fd, nullptr, s.size(), errorID);
    EXPECT_EQ(~static_cast<size_t>(0u), written);
    EXPECT_EQ(0u, errorID);

    readIn = p->read(fd, b, sizeof(b), errorID);
    EXPECT_EQ(~static_cast<size_t>(0u), readIn);
    EXPECT_NE(0u, errorID);
    written = p->read(fd, nullptr, s.size(), errorID);
    EXPECT_EQ(~static_cast<size_t>(0u), written);
    EXPECT_EQ(0u, errorID);
}

TEST(OSLayerTest, testGetTempFilePath)
{
#if defined(WIN32)
    EXPECT_FALSE(p->getTempFilePath().asUtf8String().empty());
#else
    EXPECT_EQ(FilePath("/tmp"), p->getTempFilePath());
#endif
}

TEST(OSLayerTest, testGetEnvironmentVariable)
{
    EXPECT_EQ(utf8String(), p->getEnvironmentVariable(nullptr));
    EXPECT_EQ(utf8String(), p->getEnvironmentVariable(""));
    EXPECT_EQ(utf8String(), p->getEnvironmentVariable("HOPELESS_ROMANTIC_ROCKERS"));
#if defined(WIN32)
    EXPECT_FALSE(p->getEnvironmentVariable("USERNAME").empty());
#else
    EXPECT_FALSE(p->getEnvironmentVariable("USER").empty());
#endif
    EXPECT_FALSE(p->getEnvironmentVariable("PATH").empty());
}

TEST(OSLayerTest, testGetProcessID)
{
#if defined(WIN32)
    EXPECT_EQ(static_cast<unsigned long>(GetCurrentProcessId()), p->getProcessID());
#else
    EXPECT_EQ(static_cast<unsigned long>(getpid()), p->getProcessID());
#endif
}

TEST(OSLayerTest, testErrorAsString)
{
    EXPECT_FALSE(p->errorAsString(13).empty());
}
