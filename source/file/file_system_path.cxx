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
// file_system_path.cxx -- Class that wraps platform-dependant file operations
//                         on files as file-system objects.
//
//                         (see file_system_path.hxx for a full explanation)
//
///////////////////////////////////////////////////////////////////////////

#include "file_system_path.hxx"
#include "file_system_primitives.hxx"
#include "runtime_exception.hxx"
// #include "file_handle.hxx"

using namespace std;

namespace ansak
{

namespace {

bool checkFileSystemPrimitives()
{
    enforce(getPrimitives() != nullptr);
    return getPrimitives() != nullptr;
}

}

bool primitivesExist = checkFileSystemPrimitives();

FileSystemPath::FileSystemPath
(
    const FilePath&     path
) : m_path(path),
    m_isValid(path.isReal())
{
    if (m_isValid)
    {
        realize();
    }
}

FileSystemPath::FileSystemPath
(
    const string&       pathString
) : m_path(FilePath(pathString)),
    m_isValid(m_path.isReal())
{
    if (m_isValid)
    {
        realize();
    }
}

FileSystemPath FileSystemPath::parent() const
{
    return FileSystemPath(m_path.parent());
}


FileSystemPath::ChildrenRetriever FileSystemPath::children() const
{
    return ChildrenRetriever(*this);
}

bool FileSystemPath::copyFromFile(const FileSystemPath& ) // other)
{
    return false;
    /*
    // validity check
    FileSystemPath rhs(other);
    realize(); rhs.realize();
    if (!isValid() || !rhs.isValid() || m_path.isRelative() || rhs.m_path.isRelative())
    {
        return false;
    }
    // one's not a file or source doesn't exist?
    if ((exists() && !isFile()) || (!other.exists() || !other.isFile()))
    {
        return false;
    }
    // They're already the same...
    if (m_path == rhs.m_path)
    {
        return true;
    }
    // Make sure dest doesn't exist but can be created...
    FileSystemPath parentPath(parent());
    if (exists())
    {
        if (!remove() || exists())
        {
            return false;
        }
    }
    else if (!parentPath.exists() || !parentPath.isDir())
    {
        return false;
    }

    auto src = FileHandle::openForReading(rhs);
    auto dest = FileHandle::create(*this);

    return dest.copyFrom(src) != 0;
    */
}

void FileSystemPath::realize()
{
    if (!m_path.isRelative())
    {
        return;
    }
    FilePath cwd = getPrimitives()->getCurrentWorkingDirectory();
    auto rooted = m_path.rootPathFrom(cwd);
    m_isValid = rooted.isValid();
    if (m_isValid)
    {
        m_path = rooted;
    }
}

bool FileSystemPath::exists() const
{
    return getPrimitives()->pathExists(m_path);
}

bool FileSystemPath::isFile() const
{
    return getPrimitives()->pathIsFile(m_path);
}

bool FileSystemPath::isDir() const
{
    return getPrimitives()->pathIsDir(m_path);
}

uint64_t FileSystemPath::size() const
{
    return getPrimitives()->fileSize(m_path);
}

TimeStamp FileSystemPath::lastModTime() const
{
    return getPrimitives()->lastModTime(m_path);
}

FileSystemPath::ChildrenRetriever::ChildrenRetriever
(
    const FileSystemPath& path
) : m_path(path),
    m_directoryReader()
{
    enforce(path.exists() && path.isDir());
    m_directoryReader.reset(getPrimitives()->newPathIterator(path));
}

FileSystemPath::ChildrenRetriever::ChildrenRetriever
(
    FileSystemPath::ChildrenRetriever&& src
) : m_path(src.m_path),
    m_directoryReader(move(src.m_directoryReader.release()))
{
}

FilePath FileSystemPath::ChildrenRetriever::operator()()
{
    if (m_directoryReader)
    {
        return (*(m_directoryReader.get()))();
    }
    return FilePath();
}

FileSystemPath::ChildrenRetriever::~ChildrenRetriever()
{
}

#if 0
FileSystemPath::TempDirectory::TempDirectory()
{
}

FileSystemPath::TempDirectory::~TempDirectory()
{
}

#endif
}
