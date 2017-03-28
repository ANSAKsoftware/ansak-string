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
// 2016.01.08 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_system_path_temp_dir_impl.hxx -- Linux-specific implementations of realize(),
//          derelativize() and the ChildrenRetriever for
//          FileSystemPath class.
//
// (see file_system_path.hxx for a full explanation)
//
///////////////////////////////////////////////////////////////////////////

#include "file_system_path_temp_dir_impl.hxx"

namespace ansak
{

TempDirectoryImpl::TempDirectoryImpl(const FileSystemPath& tempPath) :
    m_path(tempPath)
{
}

TempDirectoryImpl::~TempDirectoryImpl()
{
    if (m_deleteOnExit)
    {
        m_path.remove(true);
    }
}

FileSystemPath TempDirectoryImpl::child(const utf8String& subPath) const
{
    // Expects(m_path.isValid());
    return FileSystemPath(m_path.asFilePath().child(subPath));
}

FileSystemPath TempDirectoryImpl::asFileSystemPath(bool detach)
{
    m_deleteOnExit &= !detach;
    return m_path;
}

}
