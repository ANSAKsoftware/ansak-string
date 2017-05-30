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
// 2016.01.02 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// temp_directory.cxx -- Implementation of class that wraps platform-
//                       dependant notions of temp file/directory use.
//
///////////////////////////////////////////////////////////////////////////

#include <temp_directory.hxx>
#include <operating_system_primitives.hxx>

#include <sstream>
#include <memory>
#include <atomic>

using namespace std;

namespace ansak {

string TempDirectory::m_tempDirectoryMarker { "ansak" };

/////////////////////////////////////////////////////////////////////////////

TempDirectoryPtr createTempDirectory()
{
    unique_ptr<TempDirectory> rValue;

    ostringstream tmpSubPath;
    static atomic<int> tempdirCount { 0 };

    tmpSubPath << getOperatingSystemPrimitives()->getProcessID() << '-'
               << TempDirectory::m_tempDirectoryMarker << '-'
               << ++tempdirCount;
    FileSystemPath tmpDir(getOperatingSystemPrimitives()->getTempFilePath().child(tmpSubPath.str()));

    if (tmpDir.createDirectory())
    {
        rValue.reset(new TempDirectory(tmpDir));
    }
    else
    {
        // no privs to create tempdir? do something reasonable anyway
        rValue.reset(new TempDirectory(FileSystemPath(FilePath::invalidPath())));
    }
    return rValue;
}

/////////////////////////////////////////////////////////////////////////////
// private, constructor

TempDirectory::TempDirectory
(
    const FileSystemPath&   parent      // I - the parent temp dir
) : m_path(parent),
    m_deleteOnExit(parent.asFilePath() != FilePath::invalidPath())
{
}

///////////////////////////////////////////////////////////////////
// public, destructor

TempDirectory::~TempDirectory()
{
    if (m_deleteOnExit)
    {
        m_path.remove(true);
    }
}

///////////////////////////////////////////////////////////////////
// public

FileSystemPath TempDirectory::child
(
    const utf8String&       subPath     // I - a dir sub-element to create
) const
{
    return FileSystemPath(m_path.asFilePath().child(subPath));
}

///////////////////////////////////////////////////////////////////
// public

FileSystemPath TempDirectory::asFileSystemPath
(
    bool        detach      // I - suppress delete on dtor, def. false
)
{
    m_deleteOnExit &= !detach;
    return m_path;
}

}
