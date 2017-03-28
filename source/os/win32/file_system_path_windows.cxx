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
// 2016.01.02 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_system_path_windows.cxx -- Windows-specific implementations of
//          derelativize() and the ChildrenRetriever for FileSystemPath class.
//
// (see file_system_path.hxx for a full explanation)
//
///////////////////////////////////////////////////////////////////////////

#include "file_system_path.hxx"
#include "file_system_path_temp_dir_impl.hxx"

#include "time_stamp.hxx"
#include "string.hxx"

#include <windows.h>
#include <memory>
#include <sstream>
#include <atomic>

using namespace std;

namespace ansak {

namespace {

FilePath getTempDir()
{
    wchar_t buffer[MAX_PATH + 1];
    auto getEnvRC = GetEnvironmentVariableW(L"TEMP", buffer, MAX_PATH);

    if (getEnvRC > 0 && getEnvRC <= MAX_PATH)
    {
        FileSystemPath tempPath(toUtf8(reinterpret_cast<char16_t*>(buffer)));
        if (tempPath.exists() && tempPath.isDir())
        {
            return tempPath.asFilePath();
        }
    }

    getEnvRC = GetEnvironmentVariableW(L"TMP", buffer, MAX_PATH);

    if (getEnvRC > 0 && getEnvRC <= MAX_PATH)
    {
        FileSystemPath tempPath(toUtf8(reinterpret_cast<char16_t*>(buffer)));
        if (tempPath.exists() && tempPath.isDir())
        {
            return tempPath.asFilePath();
        }
    }

    FileSystemPath cTemp("C:\\TEMP");
    if (cTemp.exists() && cTemp.isDir())
    {
        return cTemp.asFilePath();
    }

    FileSystemPath cWindowsTemp("C:\\WINDOWS\\TEMP");
    return cWindowsTemp.asFilePath();
}

}

bool FileSystemPath::exists() const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    return GetFileAttributesExW(reinterpret_cast<LPCWSTR>(m_path.asUtf16String().c_str()),
                                                          GetFileExInfoStandard,
                                                          &attribs) != FALSE;
}

bool FileSystemPath::isFile() const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(m_path.asUtf16String().c_str()),
                                                          GetFileExInfoStandard,
                                                          &attribs) == FALSE)
    {
        return false;
    }
    return (attribs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool FileSystemPath::isDir() const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(m_path.asUtf16String().c_str()),
                                                          GetFileExInfoStandard,
                                                          &attribs) == FALSE)
    {
        return false;
    }
    return (attribs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

uint64_t FileSystemPath::size() const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(m_path.asUtf16String().c_str()),
                                                          GetFileExInfoStandard,
                                                          &attribs) == FALSE)
    {
        return false;
    }
    else if ((attribs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
        return 0UL;
    }
    return static_cast<uint64_t>(attribs.nFileSizeLow) +
           (static_cast<uint64_t>(attribs.nFileSizeHigh) << 32);
}

TimeStamp FileSystemPath::lastModTime() const
{
    TimeStamp r = { 0, 0, 0, 0, 0, 0 };
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(m_path.asUtf16String().c_str()),
                                                          GetFileExInfoStandard,
                                                          &attribs) != FALSE)
    {
        SYSTEMTIME lastModAsSystemTime;
        if (FileTimeToSystemTime(&attribs.ftLastWriteTime, &lastModAsSystemTime))
        {
            r.year = static_cast<unsigned int>(lastModAsSystemTime.wYear);
            r.month = static_cast<unsigned int>(lastModAsSystemTime.wMonth);
            r.day = static_cast<unsigned int>(lastModAsSystemTime.wDay);
            r.hour = static_cast<unsigned int>(lastModAsSystemTime.wHour);
            r.minute = static_cast<unsigned int>(lastModAsSystemTime.wMinute);
            r.second = static_cast<unsigned int>(lastModAsSystemTime.wSecond);
        }
    }

    return r;
}

bool FileSystemPath::derelativize()
{
    // Expects(m_isValid);
    // Expects(m_path.isRelative());

    char16_t pathBuffer[2 * MAX_PATH + 1];
    pathBuffer[2 * MAX_PATH] = static_cast<char16_t>(0);
    auto cwdReturn = GetCurrentDirectoryW(2 * MAX_PATH, reinterpret_cast<LPWSTR>(pathBuffer));
    if (cwdReturn == 0 || !isUtf16(pathBuffer, kUtf8))
    {
        m_isValid = false;
        return false; // if this fires, you've got BIG problems...
    }

    auto rooted = m_path.rootPathFrom(FilePath(toUtf8(pathBuffer)));
    auto result = rooted.isValid();
    if (result)
    {
        m_path = rooted;
    }
    return result;
}

struct FindFirstWrapper
{
    HANDLE              findHandle = 0;
    WIN32_FIND_DATAW    findData = { 0 };
    FilePath            parentDir;
    wstring             parentBaseName;
    wstring             searchTerm;
};

inline FindFirstWrapper* asWrapper(intptr_t p)
{
    return reinterpret_cast<FindFirstWrapper*>(p);
}

bool FileSystemPath::createDirectory
(
    bool recursively            // I - create parents as needed, def. false
)
{
    if (exists())
    {
        return isDir();
    }
    else
    {
        FileSystemPath parentDir(parent());
        if (parentDir.exists())
        {
            if (!parentDir.isDir())
            {
                return false;
            }
        }
        else if (!recursively)
        {
            return false;
        }
        else
        {
            if (!parentDir.createDirectory(true))
            {
                return false;
            }
        }

        auto rc = CreateDirectoryW(reinterpret_cast<LPCWSTR>(asUtf16String().c_str()), NULL);

        if (rc != 0)
        {
            return true;
        }
        return false;
    }
}

bool FileSystemPath::createFile
(
    bool failIfThere            // I - fail if file is there, def. false
)
{
    auto doesItExist = exists();
    if (doesItExist)
    {
        return isFile() && !failIfThere;
    }
    else
    {
        FileSystemPath parentDir(parent());
        if (!parentDir.exists() || !parentDir.isDir())
        {
            return false;
        }

        auto h = CreateFileW(reinterpret_cast<LPCWSTR>(asUtf16String().c_str()),
                             GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
                             doesItExist ? OPEN_EXISTING : CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
                             NULL);

        if (h == INVALID_HANDLE_VALUE)
        {
            return false;
        }
        CloseHandle(h);
        return true;
    }
}

FileSystemPath::TempDirectoryPtr FileSystemPath::createTemporaryDirectory()
{
    unique_ptr<FileSystemPath::TempDirectory> rValue;
    ostringstream tmpSubPath;
    static atomic<int> tempdirCount { 0 };
    static FilePath tempDir = getTempDir();
    tmpSubPath << GetCurrentProcessId() << '-' << ++tempdirCount;
    FileSystemPath tmpDir(tempDir.child(tmpSubPath.str()));
    if (tmpDir.createDirectory())
    {
        rValue.reset(new TempDirectoryImpl(tmpDir));
    }
    else
    {
        rValue.reset(new TempDirectoryImpl(
                FileSystemPath(FilePath::invalidPath()))); // no privs to create tempdir? no way!
    }
    return rValue;
}

bool FileSystemPath::renameTo
(
    const FileSystemPath&   other
) const
{
    if (!exists())
    {
        return false;
    }
    FileSystemPath otherParent(other.parent());
    if (!otherParent.exists() || !otherParent.isDir())
    {
        return false;
    }

    result = MoveFile(reinterpret_cast<LPCWSTR>(asUtf16String().c_str()),
                      reinterpret_cast<LPCWSTR>(other.asUtf16String().c_str())) != 0;
    return result;
}

bool FileSystemPath::remove
(
    bool        recursive       // I - rm -rf, default false
)
{
    if (!exists())
    {
        return true;
    }
    else if (isFile())
    {
        auto result = DeleteFileW(reinterpret_cast<LPCWSTR>(asUtf16String().c_str())) != 0;
        return result;
    }
    else
    {
        ChildrenRetriever finder(children());
        FileSystemPath p(finder());
        vector<FileSystemPath> victims;
        if (p.isValid())
        {
            if (!recursive)
            {
                return false;
            }
            else
            {
                while (p.isValid())
                {
                    victims.push_back(p);
                    p = finder();
                }
            }
        }

        for (auto& v : victims)
        {
            v.remove(recursive);
        }
        bool result = RemoveDirectoryW(reinterpret_cast<LPCWSTR>(asUtf16String().c_str())) != 0;
        return result;
    }
}

FileSystemPath::ChildrenRetriever::ChildrenRetriever
(
    const FileSystemPath& path
) : m_path(path),
    m_innerThat(0)
{
    if (m_path.isDir())
    {
        m_innerThat = reinterpret_cast<intptr_t>(new FindFirstWrapper());

        innerThat->parentDir = path.asFilePath();
        innerThat->parentBaseName =
            wstring(reinterpret_cast<const wchar_t*>(
                    toUtf16(innerThat->parentDir.basename()).c_str() ));
        innerThat->searchTerm =
            reinterpret_cast<const wchar_t*>(innerThat->parentDir.child("*").asUtf16String().c_str());
    }
}

FileSystemPath::ChildrenRetriever::~ChildrenRetriever()
{
    auto wrapper = asWrapper(m_innerThat);
    if (wrapper)
    {
        if (wrapper->findHandle != 0)
        {
            FindClose(wrapper->findHandle);
        }
        delete wrapper;
    }
}

namespace {

bool isDotOrDotDot(const wchar_t* p)
{
    // Expects p != nullptr
    if (*p == L'\0')    return false;
    if (*p == L'.')
    {
        if (p[1] == L'\0')                      return true;
        else if (p[1] == L'.' && p[2] == '\0')  return true;
    }
    return false;
}

}

FileSystemPath FileSystemPath::ChildrenRetriever::operator()()
{
    if (m_innerThat)
    {
        auto wrapper = asWrapper(m_innerThat);
        if (wrapper->findHandle == 0)
        {
            wrapper->findHandle = FindFirstFileExW(wrapper->searchTerm.c_str(),
                                                   FindExInfoStandard, &wrapper->findData,
                                                   FindExSearchNameMatch, nullptr, 0);
            if (wrapper->findHandle == INVALID_HANDLE_VALUE ||
                !isUtf16(reinterpret_cast<const char16_t*>(wrapper->findData.cFileName), kUtf8))
            {
                delete wrapper;
                m_innerThat = 0;
                return FileSystemPath(FilePath::invalidPath());
            }
            else if (isDotOrDotDot(wrapper->findData.cFileName) ||
                     wstring(wrapper->findData.cFileName) == wrapper->parentBaseName)
            {
                return this->operator()();
            }
            else
            {
                FilePath result(FilePath(m_path.asUtf8String()).child(
                            toUtf8(reinterpret_cast<const char16_t*>(wrapper->findData.cFileName)) ));
                return FileSystemPath(result);
            }
        }
        else
        {
            if (!!FindNextFileW(wrapper->findHandle, &wrapper->findData))
            {
                FilePath result(FilePath(m_path.asUtf8String()).child(
                        toUtf8(reinterpret_cast<const char16_t*>(wrapper->findData.cFileName)) ));
                if (isDotOrDotDot(wrapper->findData.cFileName))
                {
                    return this->operator()();
                }
                else
                {
                    return FileSystemPath(result);
                }
            }
            else
            {
                FindClose(wrapper->findHandle);
                delete wrapper;
                m_innerThat = 0;
                return FileSystemPath(FilePath::invalidPath());
            }
        }
    }
    return FileSystemPath(FilePath::invalidPath());
}

}
