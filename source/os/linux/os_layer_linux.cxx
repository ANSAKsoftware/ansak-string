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
// 2017.03.26 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// os_layer_linux.cxx -- Linux-specific implementations of operating system
//                       primitives
//
///////////////////////////////////////////////////////////////////////////

#include "os_layer_linux.hxx"
#include "linux_directory_scanner.hxx"

#include <string.hxx>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctime>

using namespace std;

namespace ansak {

namespace {

const LinuxPrimitives thePrimitives;

off_t wrappedSeek(int fh, off_t pos, int whence, unsigned int& errorID)
{
    auto r = ::lseek(fh, pos, whence);
    errorID = (r == static_cast<off_t>(-1)) ? errno : 0;
    return r;
}

}

/////////////////////////////////////////////////////////////////////////////

const OperatingSystemPrimitives* getOperatingSystemPrimitives()
{
    return &thePrimitives;
}

LinuxPrimitives::~LinuxPrimitives()
{
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::pathExists
(
    const FilePath& path
) const
{
    struct stat statBuf;
    return stat(path.asUtf8String().c_str(), &statBuf) == 0;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::pathIsFile
(
    const FilePath& path
) const
{
    struct stat statBuf;
    if (stat(path.asUtf8String().c_str(), &statBuf) != 0)
    {
        return false;
    }
    return (statBuf.st_mode & S_IFREG);
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::pathIsDir
(
    const FilePath& path
) const
{
    struct stat statBuf;
    if (stat(path.asUtf8String().c_str(), &statBuf) != 0)
    {
        return false;
    }
    return (statBuf.st_mode & S_IFDIR);
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

uint64_t LinuxPrimitives::fileSize
(
    const FilePath& path
) const
{
    struct stat statBuf;
    if (stat(path.asUtf8String().c_str(), &statBuf) != 0 ||
        !(statBuf.st_mode & S_IFREG))
    {
        return 0;
    }
    return static_cast<uint64_t>(statBuf.st_size);
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

TimeStamp LinuxPrimitives::lastModTime
(
    const FilePath& path
) const
{
    struct stat statBuf;
    TimeStamp r;
    if (stat(path.asUtf8String().c_str(), &statBuf) != 0)
    {
        r.year = r.month = r.day = r.hour = r.minute = r.second = 0;
    }
    else
    {
        struct std::tm modGmtTime;
        gmtime_r(&statBuf.st_mtim.tv_sec, &modGmtTime);
        r.year = modGmtTime.tm_year + 1900;
        r.month = modGmtTime.tm_mon + 1;
        r.day = modGmtTime.tm_mday;
        r.hour = modGmtTime.tm_hour;
        r.minute = modGmtTime.tm_min;
        r.second = modGmtTime.tm_sec;
    }

    return r;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

FilePath LinuxPrimitives::getCurrentWorkingDirectory() const
{
    char pathBuffer[2 * PATH_MAX + 1];
    pathBuffer[2 * PATH_MAX] = '\0';
    if (getcwd(pathBuffer, 2 * PATH_MAX) == nullptr)
    {
        return FilePath::invalidPath();
    }
    return FilePath(pathBuffer);
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::createDirectory
(
    const FilePath& path
) const
{
    mode_t fmode(umask(0));
    auto rc = mkdir(path.asUtf8String().c_str(), 0777);
    umask(fmode);

    return rc != -1;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::removeDirectory
(
    const FilePath& path
) const
{
    return rmdir(path.asUtf8String().c_str()) != -1;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::createFile
(
    const FilePath& path
) const
{
    int fd = ::open(path.asUtf8String().c_str(), O_CREAT | O_RDWR | O_TRUNC, 0660);
    if (fd == -1)
    {
        return false;
    }
    ::close(fd);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::move
(
    const FilePath&   filePath,
    const FilePath&   newName
) const
{
    return ::rename(filePath.asUtf8String().c_str(), newName.asUtf8String().c_str()) == 0;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool LinuxPrimitives::remove
(
    const FilePath&   filePath
) const
{
    return ::unlink(filePath.asUtf8String().c_str()) == 0;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

unsigned long long LinuxPrimitives::create
(
    const FilePath& path,
    int permissions,
    unsigned int& errorID
) const
{
    auto fd = ::creat(path.asUtf8String().c_str(), permissions);
    if (fd == -1)
    {
        errorID = errno;
        return ~static_cast<unsigned long long>(0);
    }
    errorID = 0;
    return static_cast<unsigned long long>(fd);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

unsigned long long LinuxPrimitives::open
(
    const FilePath& path,
    OpenMode mode,
    int permissions,
    unsigned int& errorID
) const
{
    int flags = 0;
    switch (mode)
    {
    default:
    case kForReading:           flags = O_RDONLY;           break;
    case kForAppending:         flags = O_APPEND | O_RDWR;  break;
    case kForReadingAndWriting: flags = O_RDWR;             break;
    }

    auto fd = ::open(path.asUtf8String().c_str(), flags, permissions);
    if (fd == -1)
    {
        errorID = errno;
        return ~static_cast<unsigned long long>(0);
    }
    errorID = 0;
    return static_cast<unsigned long long>(fd);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

uint64_t LinuxPrimitives::fileSize
(
    unsigned long long handle,
    unsigned int& errorID
) const
{
    auto fd = static_cast<int>(handle);
    off_t endOfFile = ~static_cast<uint64_t>(0u);
    off_t hereNow = wrappedSeek(fd, 0, SEEK_CUR, errorID);
    if (hereNow != static_cast<off_t>(-1))
    {
        endOfFile = wrappedSeek(fd, 0, SEEK_END, errorID);
        if (endOfFile != static_cast<off_t>(-1))
        {
            unsigned int throwAwayErrorID = 0;
            wrappedSeek(fd, hereNow, SEEK_SET, throwAwayErrorID);
        }
    }

    errorID = 0;
    return static_cast<uint64_t>(endOfFile);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

void LinuxPrimitives::close
(
    unsigned long long handle,
    unsigned int& errorID
) const
{
    auto fd = static_cast<int>(handle);
    auto rc = ::close(fd);
    errorID = (rc == -1) ? errno : 0;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

void LinuxPrimitives::seek
(
    unsigned long long handle,
    off_t position,
    unsigned int& errorID
) const
{
    wrappedSeek(static_cast<int>(handle), position, SEEK_SET, errorID);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

size_t LinuxPrimitives::read
(
    unsigned long long handle,
    char* destination,
    size_t destSize,
    unsigned int& errorID
) const
{
    if (destination == nullptr)
    {
        errorID = 0;
        return ~static_cast<size_t>(0);
    }
    auto fd = static_cast<int>(handle);
    auto r = ::read(fd, reinterpret_cast<void*>(destination), destSize);
    if (r == -1)
    {
        errorID = errno;
        return ~static_cast<size_t>(0u);
    }
    errorID = 0;
    return r;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

size_t LinuxPrimitives::write
(
    unsigned long long handle,
    const char* source,
    size_t srcSize,
    unsigned int& errorID
) const
{
    if (source == nullptr)
    {
        errorID = 0;
        return ~static_cast<size_t>(0);
    }
    auto fd = static_cast<int>(handle);
    auto r = ::write(fd, reinterpret_cast<const void*>(source), srcSize);
    if (r == -1)
    {
        errorID = errno;
        return ~static_cast<size_t>(0u);
    }
    errorID = 0;
    return r;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

FilePath LinuxPrimitives::getTempFilePath() const
{
    return FilePath("/tmp");
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

utf8String LinuxPrimitives::getEnvironmentVariable
(
    const char* variableName
) const
{
    if (variableName == nullptr || !*variableName)
    {
        return utf8String();
    }
    auto value = getenv(variableName);
    if (value == nullptr || !*value)
    {
        return utf8String();
    }
    return utf8String(value);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

unsigned long LinuxPrimitives::getProcessID() const
{
    return getpid();
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

utf8String LinuxPrimitives::errorAsString
(
    unsigned int errorID
) const
{
    auto s = strerror(errorID);
    return utf8String(s == nullptr ? "<no string>" : s);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

DirectoryListPrimitive* LinuxPrimitives::newPathIterator
(
    const FilePath& directory
) const
{
    return LinuxDirectoryScanner::newIterator(directory);
}

}
