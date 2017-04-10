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
// 2016.06.11 - First Version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// temp_file_wrapper.hxx -- A temporary-directory and file-creation wrapper
//                          for test suites.
//
///////////////////////////////////////////////////////////////////////////

#include <file_handle.hxx>
#include <file_system_path.hxx>
#include <temp_directory.hxx>

namespace ansak {
    namespace internal {

class TempFileWrapper {

public:

    void writeTestFile
    (
        const char*     fileName,       // I - file name to write
        const char*     fileContents,   // I - contents to write
        size_t          contentSize     // I - length of contents to write
    )
    {
        FileSystemPath where(child(fileName));
        auto fh = FileHandle::create(where);

        if (contentSize != 0)
        {
            fh.write(fileContents, contentSize);
        }
    }

    FileSystemPath child(const utf8String& subPath) const
    {
        return m_tempDir->child(subPath);
    }

private:

    ansak::TempDirectoryPtr    m_tempDir = ansak::createTempDirectory();
};

}

}
