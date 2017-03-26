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

#include <file_system_path.hxx>
#include <file_system_primitives.hxx>
#include <runtime_exception.hxx>
#include <file_handle.hxx>
#include <file_system_primitives.hxx>

using namespace std;

namespace ansak
{

namespace {

bool checkFileSystemPrimitives()
{
    enforce(getFileSystemPrimitives() != nullptr);
    return getFileSystemPrimitives() != nullptr;
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

bool FileSystemPath::createDirectory(bool recursively)
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

        return getFileSystemPrimitives()->createDirectory(*this);
    }
}

bool FileSystemPath::createFile(bool failIfThere)
{
    auto doesItExist = exists();
    if (doesItExist)
    {
        return isFile() && !failIfThere;
    }
    else
    {
        return getFileSystemPrimitives()->createFile(*this);
    }
}

bool FileSystemPath::copyFromFile(const FileSystemPath& src)
{
    // validity check
    FileSystemPath srcPath(src);
    realize(); srcPath.realize();
    if (!isValid() || !srcPath.isValid() || m_path.isRelative() || srcPath.m_path.isRelative())
    {
        return false;
    }
    // one's not a file or source doesn't exist?
    if ((exists() && !isFile()) || (!src.exists() || !src.isFile()))
    {
        return false;
    }
    // They're already the same...
    if (m_path == srcPath.m_path)
    {
        return true;
    }
    // Make sure dest doesn't exist but can be created...
    FileSystemPath parentPath(parent());
    if (exists())
    {
        if (!getFileSystemPrimitives()->remove(m_path) || exists())
        {
            return false;
        }
    }
    else if (!parentPath.exists() || !parentPath.isDir())
    {
        return false;
    }

    auto srcH = FileHandle::openForReading(srcPath);
    auto destH = FileHandle::create(*this);

    return destH.copyFrom(srcH) != 0;
}

bool FileSystemPath::moveTo(const FileSystemPath& other) const
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

    return getFileSystemPrimitives()->move(*this, other);
}

bool FileSystemPath::remove(bool recursive)
{
    if (!exists())
    {
        return true;
    }
    else if (isFile())
    {
        return getFileSystemPrimitives()->remove(*this);
    }
    else if (isDir())
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
        return getFileSystemPrimitives()->removeDirectory(*this);
    }
    return false;
}

bool FileSystemPath::exists() const
{
    return getFileSystemPrimitives()->pathExists(m_path);
}

bool FileSystemPath::isFile() const
{
    return getFileSystemPrimitives()->pathIsFile(m_path);
}

bool FileSystemPath::isDir() const
{
    return getFileSystemPrimitives()->pathIsDir(m_path);
}

uint64_t FileSystemPath::size() const
{
    return getFileSystemPrimitives()->fileSize(m_path);
}

TimeStamp FileSystemPath::lastModTime() const
{
    return getFileSystemPrimitives()->lastModTime(m_path);
}

void FileSystemPath::realize()
{
    if (!m_path.isRelative())
    {
        return;
    }
    FilePath cwd = getFileSystemPrimitives()->getCurrentWorkingDirectory();
    auto rooted = m_path.rootPathFrom(cwd);
    m_isValid = rooted.isValid();
    if (m_isValid)
    {
        m_path = rooted;
    }
}

FileSystemPath::ChildrenRetriever::ChildrenRetriever
(
    const FileSystemPath& path
) : m_path(path),
    m_directoryReader()
{
    enforce(path.exists() && path.isDir());
    m_directoryReader.reset(getFileSystemPrimitives()->newPathIterator(path));
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
    return FilePath::invalidPath();
}

FileSystemPath::ChildrenRetriever::~ChildrenRetriever()
{
}

}
