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
// 2014.11.23 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines_core.hxx -- Class definition for core/implementation of
//                           file-as-vector<string> and its const_iterator
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <deque>
#include <mutex>
#include <string>

#include <file_system_path.hxx>
#include <file_handle.hxx>
#include <text_file_utilities.hxx>

namespace ansak
{

namespace internal
{

class FileOfLinesCore {

    static const int maximumLineLength = 1024;
    static const unsigned int bufferSize = 7168u;   // must exceed 1K regardless of encoding
    static const unsigned int scanLengthForTwoCharEOL =
                              bufferSize - 512;     // parsing two-char EOL close to end of
                                                    // buffer is tricky; this is cheaper
    static const unsigned long long nowhere = ~0ULL;

public:

    //=======================================================================
    // Constructor

    FileOfLinesCore
    (
        const ansak::FileSystemPath& path       // I - file path to read for lines
    );

    //=======================================================================
    // Destructor -- lock_guard(m_oneUser)

    ~FileOfLinesCore();

    FileOfLinesCore(const FileOfLinesCore& src) = delete;
    FileOfLinesCore& operator=(const FileOfLinesCore& src) = delete;

    //=======================================================================
    // open the file, set it up for reading -- lock_guard(m_oneUser)
    //
    // Throws on errors, classifies the file (UTF-8, UTF-16, UCS-4) and sets
    // up to read its contents

    void open();

    //=======================================================================
    // is the file open -- doesn't lock
    //
    // Returns true if the underlying file is open.

    bool isFileOpen() const { return m_fHandle.isOpen(); }

    //=======================================================================
    // get a string from the file and return it -- may not lock!
    //
    // Locks occur in findNextLineStart and getLine
    //
    // Throws on errors
    //
    // Returns the contents of the nth line. If string is empty and endOfFile
    // is set true, the file has been completely read. endOfFile is set false
    // otherwise.

    std::string get
    (
        unsigned int    nLine,              // I - which line to return
        bool*           endOfFile = nullptr // O - has the end-of-file been found?
    );

    //=======================================================================
    // Has the end of file been reached?
    //
    // Returns true if it has, false otherwise.

    bool isEndOfFileKnown() const { return m_foundEndOfFile; }

    //=======================================================================
    // How many lines are there in the file.
    //
    // Asserts that the end-of-file is known. Returns the line count.

    size_t lineCount() const;

private:

    // type used for scan-for-line, is-range-unicode and to-string functions --
    // 3 strategies for 3 character widths
    using GetLineLengthFunc = unsigned int (FileOfLinesCore::*)
    (
        unsigned long long startOffset,
        bool* atEof
    );
    using IsRangeUnicodeFunc = bool (FileOfLinesCore::*)
    (
        unsigned int startIndex,
        unsigned int endIndex,
        unsigned int* length
    );
    using ToStringFunc = std::string (FileOfLinesCore::*)
    (
        unsigned int startIndex,
        unsigned int endIndex
    );
    using GetNextOffsetFunc = unsigned int (FileOfLinesCore::*)
    (
        unsigned int endIndex
    );
    using EOLChecker = bool (FileOfLinesCore::*)();

    //=======================================================================
    // classifyFile -- Utility function called from within open()
    //
    // Does the dog work of figuring out what kind of text (if any is in the
    // file)
    //
    // Throws on errors.

    void classifyFile();

    //=======================================================================
    // getDataBounds -- Utility function called from within open()
    //
    // Coerces the class' noted end of file to be something consistent with
    // the size of data stored: UTF-16 data must have an even size; UCS-4 data
    // size must be a multiple of 4.
    //
    // Returns the beginning of data in the file based on presence and size of
    // BOM.

    unsigned int getDataBounds();

    //=======================================================================
    // getLine -- lock_guard(m_oneUser)
    //
    // Returns a retrieved a string from a particular (known) place in the
    // file.

    std::string getLine
    (
        unsigned long long  start       // I - offset in file for start of this string
    );

    //=======================================================================
    // findNextLine -- lock_guard(m_oneUser)
    //
    // Retrieves the offset where the next string begins in the file.
    //
    // If the file has no next line, returns false; otherwise true

    bool findNextLine();

    //=======================================================================
    // moveBuffer -- set up the buffer for a particular offset (assumes the
    // file is locked

    void moveBuffer
    (
        unsigned long long  newPosition     // I - the offset to move the buffer to
    );

    //=======================================================================
    // getMaximumBufferFilePos -- Returns the file position one bufferSize
    // before the end of the file. (inline)

    unsigned long long getMaximumBufferFilePos() const;

    //=======================================================================
    // getLineLengthUtf8, getLineLengthUtf16, getLineLengthUcs4 -- From a known
    // starting position in the file, calculate the starting position and
    // length of the next complete line when the file encoding is UTF-8, UTF-16
    // or UCS-4
    //
    // Returns length in bytes if a line (non-control chars followed by a
    // CR, LF, FF or end-of-file) of non-control characters was found. Returns 0
    // for end-of-file with no text. Sets EOF flag true or false on successful
    // exit.
    //
    // Throws FileIsntText or LineIsTooLong on error.

    template<typename C>
    unsigned int getLineLength
    (
        unsigned long long  startOfLine,    // I - line starting position in file
        bool*               atEndOfFile     // O - set true if at end of file (opt.)
    );

    //=======================================================================
    // isRangeUnicodeUtf8, isRangeUnicode -- From a range of non-control
    // characters, determine if a whole line is valid Unicode and determine its
    // length.
    //
    // Returns true if a line (non-control chars followed by a CR, LF or FF) of
    // text is Unicode; false otherwise.

    bool isRangeUnicodeUtf8
    (
        unsigned int        startIndex,     // I - index in buffer to scan from
        unsigned int        endIndex,       // I - index in buffer to scan to
        unsigned int*       length          // O - string length in code points (opt.)
    );

    template<typename C>
    bool isRangeUnicode
    (
        unsigned int        startIndex,     // I - index in buffer to scan from
        unsigned int        endIndex,       // I - index in buffer to scan to
        unsigned int*       length          // O - string length in code points (opt.)
    );

    //=======================================================================
    // toStringUtf8, toStringFromWide -- converts a range of character-buffer
    // data to a std::string of UTF-8.
    //
    // Returns the string

    std::string toStringUtf8
    (
        unsigned int        startIndex,     // I - index in buffer to retrieve from
        unsigned int        endIndex        // I - index in buffer to retrieve to
    );

    template<typename C>
    std::string toStringFromWide
    (
        unsigned int        startIndex,     // I - index in buffer to retrieve from
        unsigned int        endIndex        // I - index in buffer to retrieve to
    );

    //=======================================================================
    // getNextOffset -- starting from the end of the last string, find the
    // (probable) beginnning of the next string.
    //
    // In CR-LF files where the string happens to end one "character" before the
    // end of buffer, these routines' approach may result in detection of
    // phantom 0-length files. EWONTFIX
    //
    // Returns the string

    template<typename C>
    unsigned int getNextOffset
    (
        unsigned int        endOfLastIndex  // I - index of end-of-last-string
    );

    //=======================================================================
    // usingTwoCharEOL -- scans the beginning of the file to see if end-of-lines
    // are marked as single chars (CR or LF) or as pairs (CR-LF or LF-CR); takes
    // note of the first element in the first pair found.
    //
    // Returns true if two-char EOL is found; false otherwise

    template<typename C>
    bool usingTwoCharEOL();

    //=======================================================================
    // coerceEndianness -- For UTF-16 and UCS-4, when the endian-ness of the
    // file is different than the endian-ness of the platform, reverse byte
    // order to match. (Does not handle BE-bytes / LE-words)

    void coerceEndianness
    (
        unsigned int        index,      // I - beginning of new material in buffer
        unsigned int        length      // I - length of new material in buffer
    );

    char                        m_buffer[bufferSize + sizeof(char32_t)];
                                                    // snapshot from file + extra 0
                                                    // (defined first for alignment)

    ansak::FileSystemPath       m_path;             // path to file
    ansak::FileHandle           m_fHandle;          // open/read/lseek file handle
    unsigned long long          m_fileSize;         // file-system view of size
    ansak::file::TextType       m_textType;         // text found in file
    GetLineLengthFunc           m_getLengthFunc;    // encoding-getLineLength
    IsRangeUnicodeFunc          m_isUnicodeFunc;    // encoding-isUnicode
    ToStringFunc                m_toStringFunc;     // encoding-toString
    GetNextOffsetFunc           m_getNextOffsetFunc;// encoding-jump-line-breaks
    EOLChecker                  m_classifyEOLFunc;  // encoding-specific 2-char check
    std::mutex                  m_oneUser;          // single-user access here.

    std::deque<unsigned long long>
                                m_lineStarts;       // stored start-of-line positions
    unsigned long long          m_nextScanStart;    // where to scan from next
    bool                        m_fileUses2CharEOL; // handle DOS/Amiga encoding
    bool                        m_foundEndOfFile;   // has the end been found

    unsigned long long          m_bufferFilePos;    // position of snapshot
    int                         m_bufferContentLength;  // length of loaded contents

    std::string                 m_lastLineRetrieved = std::string();
    size_t                      m_lineStartsSize = 0;
};

//===========================================================================
// private, inline

inline unsigned long long FileOfLinesCore::getMaximumBufferFilePos() const
{
    if (!m_fHandle.isOpen() || m_fileSize < bufferSize)
    {
        return 0;
    }
    return m_fileSize - bufferSize;
}

}

}
