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
// file_of_lines.hxx -- Class definition for file-as-vector<string> and for
//                      its const_iterator
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <exception>

namespace ansak
{

namespace internal
{
    class FileOfLinesCore;
}

class FileSystemPath;

// eats exceptions thrown by core when searching around, if it finds lines that
// are too long or contain binary bits, results in end() being returned.
// allows exceptions on individual fetches through.

//===========================================================================
// class FileOfLines -- presents a file of lines through iterators that return
//                      UTF-8 strings, regardless of what kind of Unicode data
//                      is in the file: UTF-8 (with or without BOM), UCS-2/
//                      UTF-16 (with BOM, LE or BE) or UCS-4 (with BOM, LE or
//                      BE).

class FileOfLines {

public:

    //===================================================================
    // forward declaration for begin() return

    class const_iterator;

    //===================================================================
    // short-hand typedefs

    using SharedCorePtr = std::shared_ptr<ansak::internal::FileOfLinesCore>;
    using WeakCorePtr = std::weak_ptr<ansak::internal::FileOfLinesCore>;

    //===================================================================
    // constructor -- accepts any FileSystemPath

    FileOfLines(const FileSystemPath& path);

    //===================================================================
    // begin -- creates an iterator
    //
    // throws on non-existent FileSystemPath (ENOENT),
    //           non-file FileSystemPath (PathWantedAsFile...),
    //           non-text FileSystemPath (FileIsntText) (from first 1024 byte sample)
    //
    // Returns const_iterator with the same FileOfLinesCore but wrapped in
    // a weak_ptr

    const_iterator begin() const;

    //===================================================================
    // end -- creates a "finished" iterator
    //
    // Returns const_iterator with the no FileOfLinesCore wrapped in its
    // weak_ptr

    const_iterator end() const { return const_iterator(); }

    //===================================================================
    // empty -- are there lines in this file?
    //
    // Returns true if there are lines, false otherwise.

    bool empty() const;

    //===================================================================
    // size -- how many lines are there in the file?
    //
    // Returns 0 if the file is empty(); returns accurate line count if it
    // is known, otherwise returns max unsigned size_ (so that it's always
    // more than the current line).

    size_t size() const;

    //=======================================================================
    // class const_iterator -- the iterator class that does work walking through
    //                         the file as much like an iterator as possilbe.
    // Slightly friendlier than STL-collection iterators as some actions that
    // result in core-dumps there result in RuntimeExceptions being thrown here
    // when "enforce" calls fail.

    class const_iterator
    {
        friend class ansak::FileOfLines;

        //===================================================================
        // called by FileOfLines::begin() -- the only way this can be created.

        const_iterator(SharedCorePtr core);

    public:
        const_iterator();

        const_iterator(const const_iterator& src) = default;
        const_iterator& operator=(const const_iterator& src) = default;

        const std::string& operator*() const;
        const std::string* operator->() const;

        const_iterator& operator++();
        const_iterator& operator--();
        const_iterator operator++(int);
        const_iterator operator--(int);

        const_iterator operator+(int increment);
        const_iterator& operator+=(int increment);
        const_iterator operator-(int increment);
        const_iterator& operator-=(int increment);

        size_t operator-(const_iterator src);

        bool operator==(const const_iterator& rhs) const;
        bool operator!=(const const_iterator& rhs) const { return !(*this == rhs); }

    private:

        //===================================================================
        // called after every change of current point to make sure all is well
        // and get the current string -- may change iterator to end() if the
        // result is beyond the end of the file.

        void dereference() const;

        //===================================================================
        // called from operator+ and operator-, handles different behaviour based
        // on known ends-of-the-file or not.

        const_iterator offsetBy(int increment);

        // Any access to the iterator, even if intended to be constant, may discover a change
        // to its state since, depending on what the OS does/does not allow (for local vs.
        // network files for instance), the underlying file may have changed or appeared to
        // change. Hence the core and current line contents are mutable to allow the class to
        // handle those possibilities.
        //
        // In practice this means that even for const methods, m_iterCore may continue to
        // point to the same FileOfLinesCore, or it may point to nothing (become an end()
        // iterator). m_currentLine will be refreshed and may change or (as part of becoming
        // end()) be emptied.

        mutable WeakCorePtr     m_iterCore;         // the core shared with outer class
        unsigned int            m_currentLineX;     // the current line index
        mutable std::string     m_currentLine;      // the current line contents
    };

private:

    SharedCorePtr               m_core;             // the core of this file when open
                                                    // (shared with iterators)
    mutable bool                m_isOpen;           // has begin() ever been called?
};


}
