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
#include <operating_system_primitives.hxx>
#include <runtime_exception.hxx>
#include <file_handle.hxx>

using namespace std;

namespace ansak
{

namespace {

bool checkFileSystemPrimitives()
{
    enforce(getOperatingSystemPrimitives() != nullptr);
    return getOperatingSystemPrimitives() != nullptr;
}

}

bool primitivesExist = checkFileSystemPrimitives();

///////////////////////////////////////////////////////////////////////////
// public, constructor

FileSystemPath::FileSystemPath
(
    const FilePath&     path        // I - an object to wrap, default FilePath()
) : m_path(path),
    m_isValid(path.isReal())
{
    if (m_isValid)
    {
        realize();
    }
}

///////////////////////////////////////////////////////////////////////////
// public, destructor

FileSystemPath::~FileSystemPath()
{
}

///////////////////////////////////////////////////////////////////////////
// public

FileSystemPath FileSystemPath::parent() const
{
    return FileSystemPath(m_path.parent());
}

///////////////////////////////////////////////////////////////////////////
// public

FileSystemPath::ChildrenRetriever FileSystemPath::children() const
{
    return ChildrenRetriever(*this);
}

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::createDirectory
(
    bool        recursively     // I - create multiple-level directory if necessary, def. false
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

        return getOperatingSystemPrimitives()->createDirectory(*this);
    }
}

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::createFile
(
    bool        failIfThere     // I - fail if the file already exists, def. false
)
{
    auto doesItExist = exists();
    if (doesItExist)
    {
        return isFile() && !failIfThere;
    }
    else
    {
        return getOperatingSystemPrimitives()->createFile(*this);
    }
}

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::copyFromFile
(
    const FileSystemPath&   src         // I - source file to duplicate to this path
)
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
        if (!getOperatingSystemPrimitives()->remove(m_path) || exists())
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

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::moveTo
(
    const FileSystemPath&   dest        // I - destination file name/location
) const
{
    if (!exists())
    {
        return false;
    }
    FileSystemPath destParent(dest.parent());
    if (!destParent.exists() || !destParent.isDir())
    {
        return false;
    }

    return getOperatingSystemPrimitives()->move(*this, dest);
}

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::remove
(
    bool        recursive       // I - if directory, remove all sub elements, def. false
)
{
    if (!exists())
    {
        return true;
    }
    else if (isFile())
    {
        return getOperatingSystemPrimitives()->remove(*this);
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
        return getOperatingSystemPrimitives()->removeDirectory(*this);
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::exists() const
{
    return getOperatingSystemPrimitives()->pathExists(m_path);
}

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::isFile() const
{
    return getOperatingSystemPrimitives()->pathIsFile(m_path);
}

///////////////////////////////////////////////////////////////////////////
// public

bool FileSystemPath::isDir() const
{
    return getOperatingSystemPrimitives()->pathIsDir(m_path);
}

///////////////////////////////////////////////////////////////////////////
// public

uint64_t FileSystemPath::size() const
{
    return getOperatingSystemPrimitives()->fileSize(m_path);
}

///////////////////////////////////////////////////////////////////////////
// public

TimeStamp FileSystemPath::lastModTime() const
{
    return getOperatingSystemPrimitives()->lastModTime(m_path);
}

///////////////////////////////////////////////////////////////////////////
// private

void FileSystemPath::realize()
{
    if (!m_path.isRelative())
    {
        return;
    }
    FilePath cwd = getOperatingSystemPrimitives()->getCurrentWorkingDirectory();
    auto rooted = m_path.rootPathFrom(cwd);
    m_isValid = rooted.isValid();
    if (m_isValid)
    {
        m_path = rooted;
    }
}

///////////////////////////////////////////////////////////////////////////
// public, constructor

FileSystemPath::ChildrenRetriever::ChildrenRetriever
(
    const FileSystemPath& path
) : m_path(path),
    m_directoryReader()
{
    enforce(path.exists() && path.isDir());
    m_directoryReader.reset(getOperatingSystemPrimitives()->newPathIterator(path));
}

///////////////////////////////////////////////////////////////////////////
// public, move constructor

FileSystemPath::ChildrenRetriever::ChildrenRetriever
(
    FileSystemPath::ChildrenRetriever&& src
) : m_path(src.m_path),
    m_directoryReader(move(src.m_directoryReader.release()))
{
}

///////////////////////////////////////////////////////////////////////////
// public, destructor

FileSystemPath::ChildrenRetriever::~ChildrenRetriever()
{
}

///////////////////////////////////////////////////////////////////////////
// public

FilePath FileSystemPath::ChildrenRetriever::operator()()
{
    if (m_directoryReader)
    {
        return (*(m_directoryReader.get()))();
    }
    return FilePath::invalidPath();
}

}
