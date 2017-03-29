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
// windows_directory_scanner.cxx -- Linux directory scanner implementation
//
///////////////////////////////////////////////////////////////////////////

#include "windows_directory_scanner.hxx"
#include <file_system_path.hxx>

namespace ansak
{

/////////////////////////////////////////////////////////////////////////////
// destructor

WindowsDirectoryScanner::~WindowsDirectoryScanner()
{
    if (m_findHandle != 0 && m_findHandle != INVALID_HANDLE_VALUE)
    {
        closeFindHandle();
    }
}

WindowsDirectoryScanner* WindowsDirectoryScanner::newIterator(const FilePath& directory)
{
    FileSystemPath fsp(directory);
    if (!fsp.isValid() || !fsp.exists() || !fsp.isDir())
    {
        return nullptr;
    }
    if (!isUtf8(directory.asUtf8String(), ansak::kUtf16))
    {
        return nullptr;
    }
    return new WindowsDirectoryScanner(directory);
}

FilePath WindowsDirectoryScanner::operator()()
{
    if (m_findHandle == 0)
    {
        auto searchTerm = m_path.child("*.*").asUtf16String();
        m_findHandle = FindFirstFileExW(reinterpret_cast<LPCWSTR>(searchTerm.c_str()), FindExInfoStandard, &m_findData,
                                        FindExSearchNameMatch, nullptr, 0);
    }
    else if (m_findHandle != INVALID_HANDLE_VALUE)
    {
        if (!FindNextFileW(m_findHandle, &m_findData))
        {
            closeFindHandle();
        }
    }

    while (m_findHandle != INVALID_HANDLE_VALUE)
    {
        // is this "." or ".."?
        if ((m_findData.cFileName[0] == L'.' && m_findData.cFileName[1] == L'\0') ||
            (m_findData.cFileName[0] == L'.' && m_findData.cFileName[1] == L'.' &&
                m_findData.cFileName[2] == L'\0'))
        {
            if (!FindNextFileW(m_findHandle, &m_findData))
            {
                closeFindHandle();
            }
        }
        else
        {
            return m_path.child(toUtf8(reinterpret_cast<const char16_t*>(m_findData.cFileName)));
        }
    }
    return FilePath::invalidPath();
}

void WindowsDirectoryScanner::closeFindHandle()
{
    FindClose(m_findHandle);
    m_findHandle = INVALID_HANDLE_VALUE;
}

}
