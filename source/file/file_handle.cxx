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
#include <operating_system_primitives.hxx>
#include <runtime_exception.hxx>
#include <nullptr_exception.hxx>

#include <sstream>

using namespace std;

namespace ansak
{

namespace internal
{

using OpenMode = OperatingSystemPrimitives::OpenMode;

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
// public, static

void FileHandle::throwErrors()
{
    m_throwErrors = true;
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
// public

bool FileHandle::operator==(const FileHandle& rhs) const
{
    if (this == &rhs)   return true;
    return m_path == rhs.m_path && m_fh == rhs.m_fh;
}

//============================================================================
// public, static

FileHandle FileHandle::create(const FileSystemPath& path, CreateType mode)
{
    if (!path.parent().isValid() || !path.parent().isDir())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "path invalid, cannot be created");
        }
        return FileHandle();
    }
    auto existsAlready = path.exists();
    if (existsAlready && mode == CreateType::kFailIfThere)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "file exists, create using FailIfThere");
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
// public, static

FileHandle FileHandle::open(const FileSystemPath& path, OpenType mode)
{
    if (!path.isValid())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "path invalid, cannot be opened");
        }
        return FileHandle();
    }
    auto exists = path.exists();
    if (!exists && mode == OpenType::kFailIfNotThere)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "file does not exist, open using FailIfNotThere");
        }
        return FileHandle();
    }
    if (exists && !path.isFile())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "path is not a file, cannot be opened");
        }
        return FileHandle();
    }

    if (!exists)
    {
        return create(path, CreateType::kFailIfThere);
    }

    FileHandle fh(path);
    if (fh.open(static_cast<int>(OperatingSystemPrimitives::OpenMode::kForReadingAndWriting)))
    {
        return fh;
    }
    else
    {
        return FileHandle();
    }
}

//============================================================================
// public, static

FileHandle FileHandle::openForReading(const FileSystemPath& path)
{
    // and this will fail if the file is not there

    if (!path.isValid())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "path invalid, cannot be opened for reading");
        }
        return FileHandle();
    }
    auto exists = path.exists();
    if (!exists)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "path does not exist, cannot be opened for reading");
        }
        return FileHandle();
    }
    if (exists && !path.isFile())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "path is not a file, cannot be opened for reading");
        }
        return FileHandle();
    }

    FileHandle fh(path);
    if (fh.open(OperatingSystemPrimitives::OpenMode::kForReading))
    {
        return fh;
    }
    else
    {
        return FileHandle();
    }
}

//============================================================================
// public, static

FileHandle FileHandle::openForAppending(const FileSystemPath& path, OpenType mode)
{
    if (!path.isValid())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "path invalid, cannot be opened for appending");
        }
        return FileHandle();
    }
    auto exists = path.exists();
    if (!exists && mode == OpenType::kFailIfNotThere)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(path, 0, "file does not exist, openForAppending using FailIfNotThere");
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
            throw FileHandleException(path, 0, "path is not a file, cannot be opened for appending");
        }
        return FileHandle();
    }

    FileHandle fh(path);
    if (fh.open(OperatingSystemPrimitives::OpenMode::kForAppending))
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

void FileHandle::close()
{
    if (isOpen())
    {
        auto h = m_fh;
        m_fh = nullHandle;
        unsigned int errorCode = 0;
        getOperatingSystemPrimitives()->close(h, errorCode);
        if (errorCode != 0 && m_throwErrors)
        {
            throw FileHandleException(m_path, errorCode, "closing file failed");
        }
    }
    else if (m_throwErrors)
    {
        throw FileHandleException(m_path, 0, "close called when file not open");
    }
}

//============================================================================
// public

uint64_t FileHandle::size()
{
    if (isOpen())
    {
        unsigned int errorCode = 0;
        auto r = getOperatingSystemPrimitives()->fileSize(m_fh, errorCode);
        if (!errorCode)
        {
            return r;
        }
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, errorCode, "checking size of open file failed");
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

void FileHandle::seek(off_t pos)
{
    if (isOpen())
    {
        unsigned int errorCode = 0;
        getOperatingSystemPrimitives()->seek(m_fh, pos, errorCode);
        if (errorCode != 0 && m_throwErrors)
        {
            stringstream os;
            os << "seeking to position " << pos << " failed";
            throw FileHandleException(m_path, errorCode, os.str());
        }
    }
    else if (m_throwErrors)
    {
        throw FileHandleException(m_path, 0, "seek called when file not open");
    }
}

//============================================================================
// public

size_t FileHandle::read(char* dest, size_t destSize)
{
    if (!isOpen())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, 0, "read called when file not open");
        }
        else
        {
            return 0;
        }
    }
    if (dest == nullptr && destSize != 0)
    {
        if (m_throwErrors)
        {
            throw NullPtrException(__FUNCTION__, __LINE__);
        }
        else
        {
            return 0;
        }
    }
    if (destSize == 0)
    {
        return 0;
    }
    unsigned int errorCode = 0;
    auto r = getOperatingSystemPrimitives()->read(m_fh, dest, destSize, errorCode);

    if (r == ~static_cast<size_t>(0u))
    {
        if (m_throwErrors)
        {
            stringstream os;
            os << "reading " << destSize << " bytes failed";
            throw FileHandleException(m_path, errorCode, os.str());
        }
        return 0;
    }
    return static_cast<size_t>(r);
}

//============================================================================
// public

size_t FileHandle::write(const char* src, size_t srcSize)
{
    if (!isOpen())
    {
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, 0, "write called when file not open");
        }
        else
        {
            return 0;
        }
    }
    if (src == nullptr && srcSize != 0)
    {
        if (m_throwErrors)
        {
            throw NullPtrException(__FUNCTION__, __LINE__);
        }
        else
        {
            return 0;
        }
    }
    if (srcSize == 0)
    {
        return 0;
    }

    unsigned int errorCode = 0;
    auto r = getOperatingSystemPrimitives()->write(m_fh, src, srcSize, errorCode);

    if (r == ~static_cast<size_t>(0u))
    {
        if (m_throwErrors)
        {
            stringstream os;
            os << "writing " << srcSize << " bytes failed";
            throw FileHandleException(m_path, errorCode, os.str());
        }
        return 0;
    }
    return static_cast<size_t>(r);
}

//============================================================================
// public

size_t FileHandle::copyFrom(FileHandle& srcFile, off_t start, size_t count)
{
    size_t bytesDone = 0;
    try
    {
        if (!isOpen())
        {
            throw FileHandleException(m_path, 0, "copyFrom destination file was not open");
        }
        if (!srcFile.isOpen())
        {
            throw FileHandleException(srcFile.m_path, 0, "copyFrom source file was not open");
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
            srcFile.seek(start);
        }
        // determine endpoint (count-or-eof, or eof)
        bool untilEOF = count == toFileEnd;
        size_t copied = 0;

        // copy until done
        bool hitEndOfFile = false;
        while (!hitEndOfFile && (copied + bufferSize < count))
        {
            auto readIn = srcFile.read(m_buffer.data(), bufferSize);
            if (readIn > bufferSize)
            {
                throw FileHandleException(srcFile.m_path, 0, "read in copyFrom returned too much returned");
            }
            hitEndOfFile |= readIn < bufferSize;
            auto writtenOut = write(m_buffer.data(), readIn);
            bytesDone += writtenOut;
            if (writtenOut != readIn)
            {
                throw FileHandleException(srcFile.m_path, 0, "read in copyFrom was incomplete");
            }
            copied += writtenOut;
        }

        if (!hitEndOfFile && !untilEOF)
        {
            auto readIn = srcFile.read(m_buffer.data(), count - copied);
            auto writtenOut = write(m_buffer.data(), readIn);
            bytesDone += writtenOut;
            if (writtenOut != readIn)
            {
                throw FileHandleException(m_path, 0, "write in copyFrom was incomplete");
            }
        }
    }
    catch (std::bad_alloc&)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, 0, "copyFrom memory allocation failed");
        }
        throw;
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
// private

bool FileHandle::create()
{
    // Expects m_path.isValid();
    // Expects !m_path.exists();
    // Expects !isOpen();
    PermissionHolder p;

    unsigned int errorNumber = 0;
    auto fd = getOperatingSystemPrimitives()->create(m_path, p.getPermission(), errorNumber);
    if (fd == ~0ull)
    {
        if (m_throwErrors)
        {
            throw FileHandleException(m_path, errorNumber, "creating file failed");
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
    auto openMode = static_cast<OperatingSystemPrimitives::OpenMode>(mode);
    enforce(openMode == OperatingSystemPrimitives::OpenMode::kForReading ||
            openMode == OperatingSystemPrimitives::OpenMode::kForAppending ||
            openMode == OperatingSystemPrimitives::OpenMode::kForReadingAndWriting,
            "Invalid mode for open");
    PermissionHolder p;

    unsigned int errorNumber = 0;
    auto fd = getOperatingSystemPrimitives()->open(m_path.asFilePath(), openMode, p.getPermission(), errorNumber);
    if (fd == ~0ull)
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
    os << "FileHandleException: " << message <<
          "file = \"" << problem.asUtf8String() << "\"; ";
    if (errorCode != 0)
    {
        os << " code = " << errorCode <<
              "; code explanation: " << getOperatingSystemPrimitives()->errorAsString(errorCode) << "; ";
    }
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
    return m_what.c_str();
}

}
