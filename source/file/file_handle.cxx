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
// 2016.01.23 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_handle.cxx -- Class that wraps platform-dependant file handle. I'd
//                    use the open/lseek/close library but they won't do
//                    internationalized names in Windows. Gotta abstract
//                    it away...
//
///////////////////////////////////////////////////////////////////////////

#include <file_handle.hxx>
#include <file_system_primitives.hxx>
#include <runtime_exception.hxx>

#include <sstream>

using namespace std;

namespace ansak
{

namespace internal
{

using OpenMode = FileSystemPrimitives::OpenMode;

namespace {

    bool throwErrors = false;

}

// facility to be used for unit tests only
void resetThrowErrors() { throwErrors = false; }

}

bool& FileHandle::m_throwErrors = ansak::internal::throwErrors;

namespace {

    const int defaultPermission = 0660;
    int nextPermission = defaultPermission;

    //============================================================================
    // class PermissionHolder, used to change a permission on the fly if needed.
    // Permission changes are assumed to be one-offs.

    class PermissionHolder {
    public:
        PermissionHolder() : thePermission(nextPermission)
            { nextPermission = defaultPermission; }

        int getPermission() const { return thePermission; }
    private:
        int thePermission;
    };

}

//============================================================================
// public, destructor

FileHandle::~FileHandle()
{
    if (isOpen())
    {
        try
        {
            close();
        }
        catch (...) { }
    }
}

//============================================================================
// public, move constructor

FileHandle::FileHandle(FileHandle&& src) : m_path(FileSystemPath())
{
    m_path = src.m_path;    src.m_path = FileSystemPath();
    m_fh   = src.m_fh;      src.m_fh = nullHandle;
}

//============================================================================
// public, move assignment

FileHandle& FileHandle::operator=(FileHandle&& src)
{
    if (this != &src)
    {
        if (isOpen())
        {
            try
            {
                close();
            }
            catch (...) { }
        }
        m_path = src.m_path;    src.m_path = FileSystemPath();
        m_fh = src.m_fh;        src.m_fh = nullHandle;
    }

    return *this;
}

//============================================================================
// static, public

FileHandle FileHandle::create(const FileSystemPath& path, CreateType mode)
{
    if (!path.parent().isValid() || !path.parent().isDir())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't create an invalid path.");
        }
        return FileHandle();
    }
    auto existsAlready = path.exists();
    if (existsAlready && mode == CreateType::kFailIfThere)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't create existing file.");
        }
        return FileHandle();
    }

    if (existsAlready)
    {
        return open(path, OpenType::kFailIfNotThere);
    }

    FileHandle fh(path);
    if (fh.create())
    {
        return fh;
    }
    else
    {
        return FileHandle();
    }
}

//============================================================================
// static, public

FileHandle FileHandle::open(const FileSystemPath& path, OpenType mode)
{
    if (!path.isValid())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't open an invalid path.");
        }
        return FileHandle();
    }
    auto exists = path.exists();
    if (!exists && mode == OpenType::kFailIfNotThere)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't open a non-existent file.");
        }
        return FileHandle();
    }
    if (exists && !path.isFile())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't open a non-file.");
        }
        return FileHandle();
    }

    if (!exists)
    {
        return create(path, CreateType::kFailIfThere);
    }

    FileHandle fh(path);
    if (fh.open(static_cast<int>(FileSystemPrimitives::OpenMode::kForReadingAndWriting)))
    {
        return fh;
    }
    else
    {
        return FileHandle();
    }
}

//============================================================================
// static, public

FileHandle FileHandle::openForReading(const FileSystemPath& path)
{
    // and this will fail if the file is not there

    if (!path.isValid())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't read an invalid path.");
        }
        return FileHandle();
    }
    auto exists = path.exists();
    if (!exists)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't read a non-existent path.");
        }
        return FileHandle();
    }
    if (exists && !path.isFile())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't read a non-file.");
        }
        return FileHandle();
    }

    FileHandle fh(path);
    if (fh.open(FileSystemPrimitives::OpenMode::kForReading))
    {
        return fh;
    }
    else
    {
        return FileHandle();
    }
}

//============================================================================
// static, public

FileHandle FileHandle::openForAppending(const FileSystemPath& path, OpenType mode)
{
    if (!path.isValid())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't append to an invalid path.");
        }
        return FileHandle();
    }
    auto exists = path.exists();
    if (!exists && mode == OpenType::kFailIfNotThere)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't append to a non-existent path.");
        }
        return FileHandle();
    }

    if (!exists)
    {
        return create(path, kFailIfThere);
    }
    else if (!path.isFile())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "Can't append to a non-file.");
        }
        return FileHandle();
    }

    FileHandle fh(path);
    if (fh.open(FileSystemPrimitives::OpenMode::kForAppending))
    {
        return fh;
    }
    else
    {
        return FileHandle();
    }
}

//============================================================================
// public

size_t FileHandle::copyFrom(FileHandle& otherFile, off_t start, size_t count)
{
    size_t bytesDone = 0;
    try
    {
        if (!isOpen())
        {
            throw FileHandleException(m_path, 0, "Can't copyFrom to a non-file.");
        }
        if (!otherFile.isOpen())
        {
            throw FileHandleException(otherFile.m_path, 0, "Can't copyFrom from a non-file.");
        }
        if (count == 0)
        {
            return bytesDone;
        }

        if (m_buffer.empty())
        {
            m_buffer.resize(bufferSize);    // throws std::bad_alloc on failure
        }

        // set/determine starting point
        if (start >= 0)
        {
            otherFile.seek(start);
        }
        // determine endpoint (count-or-eof, or eof)
        bool untilEOF = count == toFileEnd;
        size_t copied = 0;

        // copy until done
        bool hitEndOfFile = false;
        while (!hitEndOfFile && (copied + bufferSize < count))
        {
            auto readIn = otherFile.read(m_buffer.data(), bufferSize);
            hitEndOfFile |= readIn < bufferSize;
            auto writtenOut = write(m_buffer.data(), readIn);
            bytesDone += writtenOut;
            if (writtenOut != readIn)
            {
                throw FileHandleException(m_path, 0, "Write in copyFrom didn't complete.");
            }
            copied += writtenOut;
        }

        if (!hitEndOfFile && !untilEOF)
        {
            auto readIn = otherFile.read(m_buffer.data(), count - copied);
            auto writtenOut = write(m_buffer.data(), readIn);
            if (writtenOut != readIn)
            {
                throw FileHandleException(m_path, 0, "Write in copyFrom didn't complete.");
            }
            bytesDone += writtenOut;
        }
    }
    catch (std::bad_alloc& e)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, 0, "Can't copyFrom, memory allocation problem");
        }
    }
    catch (FileHandleException& e)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(e, bytesDone);
        }
    }

    return bytesDone;
}

//============================================================================
// public

bool FileHandle::operator==(const FileHandle& rhs) const
{
    if (this == &rhs)   return true;
    return m_path == rhs.m_path && m_fh == rhs.m_fh;
}

//============================================================================
// static, public

void FileHandle::throwErrors()
{
    m_throwErrors = true;
}

//============================================================================
// private

bool FileHandle::create()
{
    // Expects m_path.isValid();
    // Expects !m_path.exists();
    // Expects !isOpen();
    PermissionHolder p;

    unsigned int errorNumber = 0;
    auto fd = getPrimitives()->create(m_path, p.getPermission(), errorNumber);
    if (fd == ~0u)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, errorNumber, "Can't create this file.");
        }
        return false;
    }
    m_fh = static_cast<unsigned long long>(fd);
    return true;
}

//============================================================================
// private

bool FileHandle::open(int mode)
{
    // Expects m_path.isValid();
    // Expects m_path.exists();
    // Expects m_path.isFile();
    // Expects !m_path.isDir();
    // Expects !isOpen();
    auto openMode = static_cast<FileSystemPrimitives::OpenMode>(mode);
    enforce(openMode == FileSystemPrimitives::OpenMode::kForReading ||
            openMode == FileSystemPrimitives::OpenMode::kForAppending ||
            openMode == FileSystemPrimitives::OpenMode::kForReadingAndWriting,
            "Invalid mode for open");
    PermissionHolder p;

    unsigned int errorNumber = 0;
    auto fd = getPrimitives()->open(m_path.asFilePath(), openMode, p.getPermission(), errorNumber);
    if (fd == ~0u)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, errorNumber, "opening file failed");
        }
        return false;
    }
    m_fh = static_cast<unsigned long long>(fd);
    return true;
}

//============================================================================
// public

uint64_t FileHandle::size()
{
    if (isOpen())
    {
        unsigned int errorCode = 0;
        auto r = getPrimitives()->fileSize(m_fh, errorCode);
        if (!errorCode)
        {
            return r;
        }
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, errorCode, "checking open file's size failed");
        }
    }
    else
    {
        return m_path.size();
    }

    return static_cast<uint64_t>(-1);
}

//============================================================================
// public

void FileHandle::close()
{
    if (isOpen())
    {
        auto h = m_fh;
        m_fh = nullHandle;
        unsigned int errorCode = 0;
        getPrimitives()->close(h, errorCode);
        if (errorCode != 0 && m_throwErrors)
        {
            throw FileHandleException(m_path, errorCode, "closing file failed");
        }
    }
    else if (m_throwErrors)
    {
        throw FileHandleException(m_path, 0, "closing file twice");
    }
}

//============================================================================
// public, constructor

FileHandleException::FileHandleException
(
    const FileSystemPath&   problem,
    unsigned int            errorCode,
    const std::string&      message
) noexcept :
    m_code(errorCode)
{
    ostringstream os;
    os << "FileHandleException: " << message;
    if (errorCode == 0)
    {
        os << " no related OS error; ";
    }
    else
    {
        os << " code = " << errorCode <<
              "; error explanation: " << getPrimitives()->errorAsString(errorCode) << "; ";
    }
    os << "file = \"" << problem.asUtf8String() << "\".";
    m_what = os.str();
}

//============================================================================
// public, constructor (almost copy)

FileHandleException::FileHandleException(const FileHandleException& src, size_t inProgress) :
    m_what(src.m_what),
    m_code(src.m_code),
    m_inProgress(inProgress)
{
    ostringstream os;
    os << " (progress: " << inProgress << ')';
    m_what += os.str();
}

//============================================================================
// public, destructor

FileHandleException::~FileHandleException() noexcept
{
}

//============================================================================
// virtual, public

const char* FileHandleException::what() const noexcept
{
    enforce(!m_what.empty(), "The only constructor always writes m_what");
    return m_what.c_str();
}

}
