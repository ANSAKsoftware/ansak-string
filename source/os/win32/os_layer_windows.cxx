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
// 2017.03.28 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// os_layer_windows.cxx -- Windows-specific implementations of operating system
//                         primitives
//
///////////////////////////////////////////////////////////////////////////

#include "os_layer_windows.hxx"
#include "windows_directory_scanner.hxx"
#include <file_system_path.hxx>

#include <string.hxx>

#include <windows.h>

using namespace std;

namespace ansak {

namespace {

const WindowsPrimitives thePrimitives;

FilePath tryOutEnvironmentForPath(const char* name)
{
	auto thePath = thePrimitives.getEnvironmentVariable(name);
	if (!thePath.empty())
	{
		FileSystemPath tempPath(thePath);
		if (tempPath.exists() && tempPath.isDir())
		{
			return tempPath.asFilePath();
		}
	}
	return FilePath::invalidPath();
}

}

/////////////////////////////////////////////////////////////////////////////

const OperatingSystemPrimitives* getOperatingSystemPrimitives()
{
    return &thePrimitives;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::pathExists
(
    const FilePath& path
) const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    return GetFileAttributesExW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
                                                          GetFileExInfoStandard,
                                                          &attribs) != FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::pathIsFile
(
    const FilePath& path
) const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
                                                       GetFileExInfoStandard,
                                                       &attribs) == FALSE)
    {
        return false;
    }
    return (attribs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::pathIsDir
(
    const FilePath& path
) const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
                                                       GetFileExInfoStandard,
                                                       &attribs) == FALSE)
    {
        return false;
    }
    return (attribs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

uint64_t WindowsPrimitives::fileSize
(
    const FilePath& path
) const
{
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
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

/////////////////////////////////////////////////////////////////////////////
// public, virtual

TimeStamp WindowsPrimitives::lastModTime
(
    const FilePath& path
) const
{
    TimeStamp r = { 0, 0, 0, 0, 0, 0 };
    WIN32_FILE_ATTRIBUTE_DATA attribs;
    if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
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

/////////////////////////////////////////////////////////////////////////////
// public, virtual

FilePath WindowsPrimitives::getCurrentWorkingDirectory() const
{
    char16_t pathBuffer[2 * MAX_PATH + 1];
    pathBuffer[2 * MAX_PATH] = static_cast<char16_t>(0);
    auto cwdReturn = GetCurrentDirectoryW(2 * MAX_PATH, reinterpret_cast<LPWSTR>(pathBuffer));
    if (cwdReturn == 0 || !isUtf16(pathBuffer, kUtf8))
    {
        return FilePath::invalidPath();
    }
    return FilePath(toUtf8(pathBuffer));
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::createDirectory
(
    const FilePath& path
) const
{
    return CreateDirectoryW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()), nullptr) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::removeDirectory
(
    const FilePath& path
) const
{
    return RemoveDirectoryW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str())) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::createFile
(
    const FilePath& path
) const
{
    auto fd = CreateFileW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
                          GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                          CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
    if (fd == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    CloseHandle(fd);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::move
(
    const FilePath&   filePath,
    const FilePath&   newName
) const
{
    return MoveFileW(reinterpret_cast<LPCWSTR>(filePath.asUtf16String().c_str()),
                     reinterpret_cast<LPCWSTR>(newName.asUtf16String().c_str())) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// public, virtual

bool WindowsPrimitives::remove
(
    const FilePath&   filePath
) const
{
    return DeleteFileW(reinterpret_cast<LPCWSTR>(filePath.asUtf16String().c_str())) != 0;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

unsigned long long WindowsPrimitives::create
(
    const FilePath& path,
    int permissions,
    unsigned int& errorID
) const
{
    auto fd = CreateFileW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
                          GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                          CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
    if (fd == INVALID_HANDLE_VALUE)
    {
        errorID = GetLastError();
        return ~static_cast<unsigned long long>(0);
    }
    errorID = 0;
    return reinterpret_cast<unsigned long long>(fd);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

unsigned long long WindowsPrimitives::open
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
    case kForReading:           flags = GENERIC_READ;                   break;
    case kForAppending:
    case kForReadingAndWriting: flags = GENERIC_READ | GENERIC_WRITE;   break;
    }

    auto fd = CreateFileW(reinterpret_cast<LPCWSTR>(path.asUtf16String().c_str()),
                          flags, FILE_SHARE_READ, nullptr,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fd == INVALID_HANDLE_VALUE)
    {
        errorID = errno;
        return ~static_cast<unsigned long long>(0);
    }
    errorID = 0;
    if (mode == kForAppending)
    {
        SetFilePointer(fd, 0, nullptr, FILE_END);
    }
    return reinterpret_cast<unsigned long long>(fd);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

uint64_t WindowsPrimitives::fileSize
(
    unsigned long long handle,
    unsigned int& errorID
) const
{
    LARGE_INTEGER result;
    result.LowPart = GetFileSize(reinterpret_cast<HANDLE>(handle), reinterpret_cast<LPDWORD>(&result.HighPart));
    if (result.LowPart == INVALID_FILE_SIZE)
    {
        errorID = GetLastError();
        return ~static_cast<uint64_t>(0u);
    }
    return static_cast<uint64_t>(result.QuadPart);
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

void WindowsPrimitives::close
(
    unsigned long long handle,
    unsigned int& errorID
) const
{
    auto rc = CloseHandle(reinterpret_cast<HANDLE>(handle));
    errorID = (rc == 0) ? GetLastError() : 0u;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

void WindowsPrimitives::seek
(
    unsigned long long handle,
    off_t position,
    unsigned int& errorID
) const
{
    auto fd = reinterpret_cast<HANDLE>(handle);
    LARGE_INTEGER pointHere;
    pointHere.QuadPart = static_cast<LONGLONG>(position);
    auto r = SetFilePointer(fd, pointHere.LowPart, &pointHere.HighPart, FILE_END);
    errorID = (r == INVALID_SET_FILE_POINTER) ? GetLastError() : 0;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

size_t WindowsPrimitives::read
(
    unsigned long long handle,
    char* destination,
    size_t destSize,
    unsigned int& errorID
) const
{
    if (destination == nullptr || destSize != static_cast<DWORD>(destSize))
    {
        return ~static_cast<size_t>(0);
    }
    auto fd = reinterpret_cast<HANDLE>(handle);
	DWORD destSizeAsDword = static_cast<DWORD>(destSize);
    DWORD destRead = 0u;
    auto r = ReadFile(fd, destination, destSizeAsDword, &destRead, nullptr);
    if (!r)
    {
        errorID = GetLastError();
        return ~static_cast<size_t>(0u);
    }
    errorID = 0;
    return r;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

size_t WindowsPrimitives::write
(
    unsigned long long handle,
    const char* source,
    size_t srcSize,
    unsigned int& errorID
) const
{
    if (source == nullptr || srcSize != static_cast<DWORD>(srcSize))
    {
        return ~static_cast<size_t>(0);
    }
    auto fd = reinterpret_cast<HANDLE>(handle);
	DWORD srcSizeAsDword = static_cast<DWORD>(srcSize);
    DWORD srcWritten = 0u;
    auto r = WriteFile(fd, source, srcSizeAsDword, &srcWritten, nullptr);
    if (!r)
    {
        errorID = errno;
        return ~static_cast<size_t>(0u);
    }
    errorID = 0;
    return r;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

FilePath WindowsPrimitives::getTempFilePath() const
{
	{
		auto result = tryOutEnvironmentForPath("TEMP");
		if (result != FilePath::invalidPath())
		{
			return result;
		}
		result = tryOutEnvironmentForPath("TMP");
		if (result != FilePath::invalidPath())
		{
			return result;
		}
	}

    FileSystemPath result("C:\\TEMP");
    if (result.exists() && result.isDir())
    {
        return result.asFilePath();
    }

    return FilePath("C:\\WINDOWS\\TEMP");
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

utf8String WindowsPrimitives::getEnvironmentVariable
(
    const char* variableName
) const
{
    if (variableName == nullptr || !*variableName)
    {
        return utf8String();
    }
    auto name = toUtf16(variableName);
    DWORD maxSize = MAX_PATH;
    vector<char16_t> buffer(maxSize, 0);
    auto getEnvRC = GetEnvironmentVariableW(reinterpret_cast<LPCWSTR>(name.c_str()), reinterpret_cast<LPWSTR>(buffer.data()), maxSize);

    if (getEnvRC == 0)
    {
        return utf8String();
    }
    else if (getEnvRC > maxSize)
    {
        maxSize = getEnvRC;
		buffer.resize(maxSize + 1, 0);
        getEnvRC = GetEnvironmentVariableW(reinterpret_cast<LPCWSTR>(name.c_str()), reinterpret_cast<LPWSTR>(buffer.data()), maxSize);
    }

    return toUtf8(buffer.data());
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

unsigned long WindowsPrimitives::getProcessID() const
{
    return static_cast<unsigned long>(GetCurrentProcessId());
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

utf8String WindowsPrimitives::errorAsString
(
    unsigned int errorID
) const
{
    LPWSTR pMessageBuffer = nullptr;
    auto messageLength = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
                                        FORMAT_MESSAGE_IGNORE_INSERTS |
                                        FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr,
                                static_cast<DWORD>(errorID),
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                reinterpret_cast<LPWSTR>(&pMessageBuffer), 0, nullptr);
    if (messageLength == 0)
    {
        return utf8String();
    }
    while (pMessageBuffer[messageLength - 1] == L' ')
    {
        pMessageBuffer[--messageLength] = L'\0';
    }
    auto rv = toUtf8(reinterpret_cast<const char16_t*>(pMessageBuffer));
    LocalFree(pMessageBuffer);
    return rv;
}

//////////////////////////////////////////////////////////////////////////////
// public, virtual

DirectoryListPrimitive* WindowsPrimitives::newPathIterator
(
    const FilePath& directory
) const
{
    return WindowsDirectoryScanner::newIterator(directory);
}

}
