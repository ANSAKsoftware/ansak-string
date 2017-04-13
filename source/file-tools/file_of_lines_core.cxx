///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2014, Arthur N. Klassen
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
// 2014.11.25 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines_core.cxx -- Implementation of core/implementation of file-
//                           as-vector<string> and its const_iterator
//
///////////////////////////////////////////////////////////////////////////

#include "file_of_lines_core.hxx"
#include "file_of_lines_exception.hxx"
#include "text_file_utilities.hxx"

#include <string>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <limits.h>

#include "file_path.hxx"
#include <file_handle_exception.hxx>
#include "endianness.hxx"
#include "string.hxx"
#include "string_decode_utf8.hxx"
#include "runtime_exception.hxx"

using namespace ansak;
using namespace ansak::file;
using namespace std;

namespace ansak
{

namespace internal
{

namespace
{

//===========================================================================
// FoundWhatEnum -- help classify what kind of previously unknown data in the
// buffer contains

enum FoundWhatEnum {
    kEndOfLine,                 // denotes finding LF, FF or CR
    kControlCharacters,         // denotes finding [NUL..SP) & [^TAB,LF,FF,CR]
    kEndOfBuffer                // denotes finding nothing but chars to e.o.buffer
};

//===========================================================================
// isEndOfLine -- regardless of width, identify character as an end-of-line
//
// Returns true for LF, FF, CR; false otherwise

template<typename C>
bool isEndOfLine(C c)
{
    return (c == ansak::file::LF || c == ansak::file::FF || c == ansak::file::CR);
}

//===========================================================================
// scanInputBuffer -- regardless of width, identify character content in a
// previously unknown part of the file.
//
// Incoming q is changed if end-of-line or control characters are found
//
// Returns kEndOfLine if LF, FF or CR was found; kControlCharacters if other
// chars less than SP and not TAB were found; kEndOfBuffer otherwise.

template<typename C>
FoundWhatEnum scanInputBuffer(C* p, C*& q)
{
    for (C* i = p; i < q; ++i)
    {
        if (*i == ansak::file::LF || *i == ansak::file::FF || *i == ansak::file::CR)
        {
            q = i - 1;
            return kEndOfLine;
        }
        else if (*i >= 0 && *i < ansak::file::SP && *i != ansak::file::TAB)
        {
            q = i;
            return kControlCharacters;
        }
    }
    return kEndOfBuffer;
}

}

//===========================================================================
// Constructor

FileOfLinesCore::FileOfLinesCore
(
    const FileSystemPath& path            // I - file path to read for lines
) : m_buffer(),
    m_path(path),
    m_fHandle(),
    m_fileSize(0),
    m_textType(),
    m_getLengthFunc(nullptr),
    m_isUnicodeFunc(nullptr),
    m_toStringFunc(nullptr),
    m_getNextOffsetFunc(nullptr),
    m_oneUser(),
    m_lineStarts(),
    m_nextScanStart(FileOfLinesCore::nowhere),
    m_foundEndOfFile(false),
    m_bufferFilePos(FileOfLinesCore::nowhere),
    m_bufferContentLength(0)
{
    // initialize it all, including the null at the end (raccoon code)
    memset(m_buffer, 0, sizeof(m_buffer));
}

//===========================================================================
// Destructor

FileOfLinesCore::~FileOfLinesCore()
{
    if (isFileOpen())
    {
        lock_guard<mutex> l(m_oneUser);
        m_fHandle.close();
    }
}

//===========================================================================
// public

void FileOfLinesCore::open()
{
    lock_guard<mutex> l(m_oneUser);

    if (!isFileOpen())
    {
        unsigned long long fileSize = m_path.size();
        if (fileSize == 0)
        {
            // it's an empty file, it has no lines, it can't be opened.
            m_foundEndOfFile = true;
            return;
        }

        classifyFile();

        m_fHandle = FileHandle::open(m_path);

        m_nextScanStart = getDataBounds();
        moveBuffer(m_nextScanStart);
    }
}

//===========================================================================
// public

string FileOfLinesCore::get
(
    unsigned int    nLine,      // I - which line to return
    bool*           endOfFile   // O - has the end-of-file been found?, def nullptr
)
{
    enforce(isFileOpen() || m_foundEndOfFile,
            "get() cannot be called on a file that is not open.");
    bool dummyEnd = true;
    bool& eofRef = endOfFile == nullptr ? dummyEnd : *endOfFile;

    if (isFileOpen())
    {
        if (!m_foundEndOfFile)
        {
            while (nLine + 1 > m_lineStarts.size() && findNextLine())
                ;
        }

        m_lineStartsSize = m_lineStarts.size();
        if (nLine + 1 < m_lineStarts.size())
        {
            string val = getLine(m_lineStarts[nLine]);
            eofRef = false;
            m_lastLineRetrieved = val;
            return val;
        }
        else if (nLine == m_lineStarts.size() - 1)
        {
            string val = getLine(m_lineStarts[nLine]);
            eofRef = m_foundEndOfFile;
            m_lastLineRetrieved = val;
            return val;
        }
    }

    eofRef = true;
    return string();
}

//===========================================================================
// public

size_t FileOfLinesCore::lineCount() const
{
    enforce(m_foundEndOfFile, "Don't call lineCount before you load them all.");

    return m_fileSize == 0 ? 0 : m_lineStarts.size();
}

//===========================================================================
// private

void FileOfLinesCore::classifyFile()
{
    if (!ansak::file::looksLikeText(m_path, &m_textType))
    {
        throw FileOfLinesException("in FileOfLinesCore::classifyFile, open failed", m_path);
    }
    switch (m_textType)
    {
    default:
    case ansak::file::TextType::kLocalText:
        {
            throw FileOfLinesException("in FileOfLinesCore::classifyFile, file isn't "
                    "entirely Unicode, contains local text", m_path);
        }
        break;

    case ansak::file::TextType::kUtf8:
        m_getLengthFunc = &FileOfLinesCore::getLineLength<char>;
        m_isUnicodeFunc = &FileOfLinesCore::isRangeUnicodeUtf8;
        m_toStringFunc = &FileOfLinesCore::toStringUtf8;
        m_getNextOffsetFunc = &FileOfLinesCore::getNextOffset<char>;
        break;

    case ansak::file::TextType::kUtf16LE:
    case ansak::file::TextType::kUtf16BE:
        m_getLengthFunc = &FileOfLinesCore::getLineLength<char16_t>;
        m_isUnicodeFunc = &FileOfLinesCore::isRangeUnicode<char16_t>;
        m_toStringFunc = &FileOfLinesCore::toStringFromWide<char16_t>;
        m_getNextOffsetFunc = &FileOfLinesCore::getNextOffset<char16_t>;
        break;

    case ansak::file::TextType::kUcs4LE:
    case ansak::file::TextType::kUcs4BE:
        m_getLengthFunc = &FileOfLinesCore::getLineLength<char32_t>;
        m_isUnicodeFunc = &FileOfLinesCore::isRangeUnicode<char32_t>;
        m_toStringFunc = &FileOfLinesCore::toStringFromWide<char32_t>;
        m_getNextOffsetFunc = &FileOfLinesCore::getNextOffset<char32_t>;
        break;
    }
}

//===========================================================================
// private

unsigned int FileOfLinesCore::getDataBounds()
{
    enforce(isFileOpen(),
            "getDataBounds() cannot be called when the underlying file is not open.");

    m_fileSize = m_path.size();

    // only denoted by BOM -- start from "2", file size must be even
    if (m_textType == ansak::file::TextType::kUtf16LE ||
        m_textType == ansak::file::TextType::kUtf16BE)
    {
        // make file size even for UTF-16
        m_fileSize &= ~1;

        return 2;
    }
    // only denoted by BOM -- start from "4", file size must be even 4-bytes
    else if (m_textType == ansak::file::TextType::kUcs4LE ||
             m_textType == ansak::file::TextType::kUcs4BE)
    {
        // make file size divisilbe by 4 for UCS-4
        m_fileSize &= ~3;
        return 4;
    }
    // might have BOM, might not, look again~
    else if (m_textType == ansak::file::TextType::kUtf8)
    {
        char bom[3];
        int n = m_fHandle.read(bom, 3);
        if (n == 3 && bom[0] == '\xef' && bom[1] == '\xbb' && bom[2] == '\xbf')
        {
            // it's a BOM, return 3
            return 3;
        }
    }
    return 0;
}

//===========================================================================
// private

string FileOfLinesCore::getLine
(
    unsigned long long  start       // I - offset in file for start of this string
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");

    lock_guard<mutex> guard(m_oneUser);

    if (m_bufferFilePos == nowhere ||           // first read   OR
        (getMaximumBufferFilePos() != 0 &&      // file is bigger than buffer and...
         (start < m_bufferFilePos ||            // current start is before buffer
          start >= m_bufferFilePos + bufferSize)))      // or after it
    {
        moveBuffer(start);
    }

    // get the next length-of-string in bytes (for which isControlCharacter() is false)
    unsigned int lengthInBytes = (this->*m_getLengthFunc)(start, nullptr);
    if (lengthInBytes == 0)
    {
        return string();
    }

    // test the unicode-validity of the bytes, length of string in code-points
    unsigned int lineStartIndex = static_cast<unsigned int>(start - m_bufferFilePos);
    unsigned int lineEndIndex = lineStartIndex + lengthInBytes;
    if (!(this->*m_isUnicodeFunc)(lineStartIndex, lineEndIndex, nullptr))
    {
        throw FileOfLinesException(
                "FileOfLinesCore::getLine discovered invalid-as-unicode characters in file",
                m_path, start);
    }

    return (this->*m_toStringFunc)(lineStartIndex, lineEndIndex);
}

//===========================================================================
// private

bool FileOfLinesCore::findNextLine()
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(!m_foundEndOfFile, "End of file has been found, no further findNextLine is needed.");
    if (m_foundEndOfFile)
    {
        return false;
    }

    lock_guard<mutex> guard(m_oneUser);

    unsigned long long lineStart = m_nextScanStart;
    bool atEndOfFile = false;
    unsigned int lengthInBytes = (this->*m_getLengthFunc)(lineStart, &atEndOfFile);
    m_foundEndOfFile = atEndOfFile;

    if (atEndOfFile && lengthInBytes == 0)
    {
        return false;
    }

    unsigned int lengthInCodePoints = 0;
    unsigned int lineStartIndex = static_cast<unsigned int>(lineStart - m_bufferFilePos);
    unsigned int lineEndIndex = lineStartIndex + lengthInBytes;
    if (!(this->*m_isUnicodeFunc)(lineStartIndex, lineEndIndex, &lengthInCodePoints))
    {
        throw FileOfLinesException("non-text encountered", m_path, lineStart);
    }

    if (lengthInCodePoints > maximumLineLength)
    {
        throw FileOfLinesException("line too long", m_path, lineStart);
    }

    unsigned int nextSearchIndex = (this->*m_getNextOffsetFunc)(lineEndIndex);

    // store new line start, take note of the next start (as file offset)
    m_lineStarts.push_back(lineStart);
    m_nextScanStart = m_bufferFilePos + nextSearchIndex;
    return true;
}

//===========================================================================
// private

void FileOfLinesCore::moveBuffer
(
    unsigned long long  newPosition // I - the offset to move the buffer to
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(newPosition != nowhere, "moveBuffer should be going somewhere, not nowhere.");
    enforce(newPosition != m_bufferFilePos,
            "The new position should be different.");
    enforce(m_bufferFilePos == nowhere || m_fileSize > bufferSize,
            "moveBuffer should only be called once when the file is smaller than the "
            "buffer.");

    // pin the seek to the end of all things
    newPosition = std::min(newPosition, getMaximumBufferFilePos());
    // and if there was no motion, do nothing more
    if (newPosition == m_bufferFilePos)
    {
        return;
    }

    // calculations around reading the data
    auto toRead = std::min(m_fileSize - newPosition,
                           static_cast<unsigned long long>(bufferSize));
    int toReadAsInt = static_cast<int>(toRead);
    int doneRead = 0;
    try
    {
        // seek the file pointer, read the data
        m_fHandle.seek(static_cast<off_t>(newPosition));
        doneRead = m_fHandle.read(&m_buffer[0], toReadAsInt);
        if (doneRead == 0 && newPosition != 0)
        {
            ostringstream os;
            os << "path could not be read; "
               << "seeking in FileOfLinesCore::moveBuffer and reading yielded 0 bytes at offest"
               << newPosition;
            throw FileOfLinesException(os.str().c_str(), m_path);
        }
    }
    catch (FileHandleException& io)
    {
        if (m_bufferFilePos == nowhere)
        {
            throw FileOfLinesException("path could not be read; in FileOfLinesCore::moveBuffer, "
                                                   "reading buffer of first-time data failed",
                                        m_path, io, newPosition);
        }
        else
        {
            ostringstream os;
            os << "path could not be read; "
               << "in FileOfLinesCore::moveBuffer, seeking and reading failed";
            throw FileOfLinesException(os.str().c_str(), m_path,
                                       io, newPosition);
        }
    }

    // set the file's state
    m_bufferContentLength = doneRead;
    m_bufferFilePos = newPosition;

    // for 2- and 4-byte data, coerce endianness
    coerceEndianness(0, toReadAsInt);

    enforce(m_bufferFilePos != nowhere, "Seeking must have happened here.");
}

//===========================================================================
// algorithm for getLineLength* template:
//
// search from here... (a)find control characters? give up; throw exception (file isn't text)
//                 ... (b)find more than bufferSize character data? give up;
//                         throw exception (line is too long)
//                 ... (c)find end-of-line? good; return end-points
//                 ... (d)find end-of-file? good; return final end-points

//===========================================================================
// private

template<typename C>
unsigned int FileOfLinesCore::getLineLength
(
    unsigned long long  startOfLine,    // I - line starting position in file
    bool*               atEndOfFile     // O - set true if at end of file (opt.)
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(m_bufferFilePos != nowhere, "Seeking must have happened by now.");

    bool eolDummy;
    bool& eolRef = atEndOfFile == nullptr ? eolDummy : *atEndOfFile;

    if (startOfLine < m_bufferFilePos)
    {
        moveBuffer(startOfLine);
    }

    unsigned int startOfLineInBuffer =
                static_cast<unsigned int>(startOfLine - m_bufferFilePos) / sizeof(C);

    C* pBufferAsC = reinterpret_cast<C*>(&m_buffer[0]);
    unsigned int endOfBuffer = bufferSize;
    if (m_fileSize < bufferSize)
    {
        // if that if succeeds this cast is fine.
        endOfBuffer = static_cast<unsigned int>(m_fileSize);
    }
    unsigned int endOfSearchArea = endOfBuffer / sizeof(C);

    for (;;)    // probably only gets run once, at most twice
    {
        C* startP = pBufferAsC + startOfLineInBuffer;
        C* endP = pBufferAsC + endOfSearchArea;
        auto found = scanInputBuffer(startP, endP);

        if (found == kEndOfLine)
        {
            // (c)
            eolRef = false;
            // cast always okay -- see buffer size (passim)
            return static_cast<unsigned int>(endP - startP + 1) * sizeof(C);
        }
        else if (found == kEndOfBuffer)
        {
            if (m_bufferFilePos >= getMaximumBufferFilePos())
            {
                // (d)
                eolRef = true;
                return static_cast<unsigned int>(endP - startP) * sizeof(C);
            }
            else if (startOfLineInBuffer != 0)
            {
                moveBuffer(startOfLine);
                // probably 0; at end of file, probably not
                startOfLineInBuffer =
                    static_cast<unsigned int>(startOfLine - m_bufferFilePos) /
                            sizeof(C);
            }
            else
            {
                // (b)
                throw FileOfLinesException("line too long", m_path, startOfLine);
            }
        }
        else // if (found == kControlCharacters)
        {
            // (a)
            throw FileOfLinesException("in FileOfLinesCore::getLineLengthUtf8, scanning UTF-8 found control characters",
                                       m_path, startOfLine);
        }
    }
}

//===========================================================================
// private

bool FileOfLinesCore::isRangeUnicodeUtf8
(
    unsigned int        startIndex,     // I - index in buffer to scan from
    unsigned int        endIndex,       // I - index in buffer to scan to
    unsigned int*       length          // O - length in code points of string
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(m_bufferFilePos != nowhere, "Seeking must have happened by now.");

    unsigned int dummyLength = 0;
    unsigned int& lengthRef = length == nullptr ? dummyLength : *length;

    if (startIndex == endIndex)
    {
        lengthRef = 0;
        return true;
    }

    auto l = unicodeLength(m_buffer + startIndex, endIndex - startIndex);
    if (l == 0)
    {
        return false;
    }
    else
    {
        const char* nextP = m_buffer + startIndex + l;
        decodeUtf8(nextP);
        if (nextP == nullptr)
        {
            return false;
        }
        lengthRef = l;
        return true;
    }
}

//===========================================================================
// private

template<typename C>
bool FileOfLinesCore::isRangeUnicode
(
    unsigned int        startIndex,     // I - index in buffer to scan from
    unsigned int        endIndex,       // I - index in buffer to scan to
    unsigned int*       length          // O - string length in code points (opt.)
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(m_bufferFilePos != nowhere, "Seeking must have happened by now.");

    enforce((startIndex & (sizeof(C) - 1)) == 0, "isRange: multi-byte strings must start on even-value boundaries.");
    enforce((endIndex & (sizeof(C) - 1)) == 0, "isRange: multi-byte strings must end on even-value boundaries.");

    unsigned int dummyLength = 0;
    unsigned int& lengthRef = length == nullptr ? dummyLength : *length;

    if (startIndex == endIndex)
    {
        lengthRef = 0;
        return true;
    }
    const C* asC = reinterpret_cast<C*>(m_buffer + startIndex);
    auto l = unicodeLength(asC, (endIndex - startIndex) / sizeof(C));
    if (l == 0)
    {
        return false;
    }
    else
    {
        lengthRef = l;
        return true;
    }
}

//===========================================================================
// private

string FileOfLinesCore::toStringUtf8
(
    unsigned int        startIndex,     // I - index in buffer to retrieve from
    unsigned int        endIndex        // I - index in buffer to retrieve to
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(m_bufferFilePos != nowhere, "Seeking must have happened by now.");

    char* endPoint = m_buffer + endIndex;
    char saveEnd = *endPoint;
    *endPoint = '\0';
    string r(m_buffer + startIndex);
    *endPoint = saveEnd;

    return r;
}

//===========================================================================
// private

template<typename C>
string FileOfLinesCore::toStringFromWide
(
    unsigned int        startIndex,     // I - index in buffer to retrieve from
    unsigned int        endIndex        // I - index in buffer to retrieve to
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(m_bufferFilePos != nowhere, "Seeking must have happened by now.");

    enforce((startIndex & (sizeof(C) - 1)) == 0, "toString: multi-byte strings must start on even-value boundaries.");
    enforce((endIndex & (sizeof(C) - 1)) == 0, "toString: multi-byte strings must end on even-value boundaries.");

    C* startPoint = reinterpret_cast<C*>(m_buffer + startIndex);
    C* endPoint = reinterpret_cast<C*>(m_buffer + endIndex);
    C saveEnd = *endPoint;
    *endPoint = static_cast<C>(0);
    string r(toUtf8(startPoint));
    *endPoint = saveEnd;
    
    return r;
}

//===========================================================================
// private

template<typename C>
unsigned int FileOfLinesCore::getNextOffset
(
    unsigned int        endOfLastIndex  // I - index of end-of-last-string
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");
    enforce(m_bufferFilePos != nowhere, "Seeking must have happened by now.");

    C* thisPoint = reinterpret_cast<C*>(m_buffer + endOfLastIndex);
    unsigned int nextStart = endOfLastIndex + sizeof(C);
    if (nextStart < bufferSize &&
            ((thisPoint[0] == ansak::file::CR && thisPoint[1] == ansak::file::LF) ||
             (thisPoint[0] == ansak::file::LF && thisPoint[1] == ansak::file::CR)))
    {
        nextStart += sizeof(C);
    }

    return nextStart;
}

//===========================================================================
// private

void FileOfLinesCore::coerceEndianness
(
    unsigned int        index,      // I - beginning of new material in buffer
    unsigned int        length      // I - length of new material in buffer
)
{
    enforce(isFileOpen(), "The file must have been opened by now.");

    if ((isLittleEndian() && (m_textType == ansak::file::TextType::kUtf16BE ||
                              m_textType == ansak::file::TextType::kUcs4BE)) ||
        (!isLittleEndian() && (m_textType == ansak::file::TextType::kUtf16LE ||
                               m_textType == ansak::file::TextType::kUcs4LE)))
    {
        if (m_textType == ansak::file::TextType::kUtf16BE ||
            m_textType == ansak::file::TextType::kUtf16LE)
        {
            enforce(length % 2 == 0,
                    "Unit of motion in UTF-16 buffer must be multiple of 2");

            char16_t* p = reinterpret_cast<char16_t*>(m_buffer + index);
            int nCount = length / 2;
            for (int i = 0; i < nCount; ++i, ++p)
            {
                char16_t c = ((*p & 0xFF) << 8) | ((*p >> 8) & 0xFF);
                *p = c;
            }
        }
        else // if (m_textType == ansak::file::TextType::kUcs4BE ||
             //     m_textType == ansak::file::TextType::kUcs4LE)
        {
            enforce(length % 4 == 0,
                    "Unit of motion in UCS-4 buffer must be multiple of 4");

            char32_t* p = reinterpret_cast<char32_t*>(m_buffer + index);
            int nCount = length / 4;
            for (int i = 0; i < nCount; ++i, ++p)
            {
                char32_t c = ((*p & 0xFF) << 24) | ((*p & 0xFF00) << 8) |
                             ((*p >> 8) & 0xFF00) | ((*p >> 24) & 0xFF);
                *p = c;
            }
        }
    }
}

}

}
