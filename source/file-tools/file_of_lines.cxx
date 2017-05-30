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
// 2014.12.09 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_of_lines.cxx -- Class implementation for file-as-vector<string> and
//                      for its const_iterator
//
///////////////////////////////////////////////////////////////////////////

#include <file_of_lines.hxx>
#include <file_of_lines_core.hxx>

#include <string>
#include <sstream>

#include "runtime_exception.hxx"

using namespace std;
using namespace ansak::internal;

namespace ansak
{

//===========================================================================
// Constructor

FileOfLines::FileOfLines
(
    const FileSystemPath& path
) : m_core(new FileOfLinesCore(path)),
    m_isOpen(false)
{
}

//===========================================================================
// public

FileOfLines::const_iterator FileOfLines::begin() const
{
    if (!m_isOpen)
    {
        try
        {
            m_isOpen = true;
            m_core->open();
        }
        catch(RuntimeException&)
        {
            // let "enforce" actions filter up
            throw;
        }
        catch(std::exception&)
        {
        }
    }

    if (m_core->isFileOpen())
    {
        return const_iterator(m_core);
    }
    else
    {
        return end();
    }
}

//===========================================================================
// public

size_t FileOfLines::size() const
{
    if (begin() == end())
    {
        return 0;
    }
    else
    {
        try
        {
            return m_core->lineCount();
        }
        catch (std::exception&)
        {
            return ~static_cast<size_t>(0);
        }
    }
}

//===========================================================================
// private

FileOfLines::const_iterator::const_iterator(SharedCorePtr core)
  : m_iterCore(core),
    m_currentLineX(0),
    m_currentLine()
{
}

//===========================================================================
// public

FileOfLines::const_iterator::const_iterator()
  : m_iterCore(),
    m_currentLineX(0),
    m_currentLine()
{
}

//===========================================================================
// public

const string& FileOfLines::const_iterator::operator*() const
{
    auto pCore = m_iterCore.lock();
    if (pCore.get() != nullptr)
    {
        dereference();
    }
    return m_currentLine;
}

//===========================================================================
// public

const string* FileOfLines::const_iterator::operator->() const
{
    auto pCore = m_iterCore.lock();
    if (pCore.get() != nullptr)
    {
        dereference();
    }
    return &m_currentLine;
}

//===========================================================================
// public

FileOfLines::const_iterator& FileOfLines::const_iterator::operator++()
{
    enforce(*this != const_iterator(), "Only pre-increment a non-end iterator.");

    auto pCore = m_iterCore.lock();
    if (pCore.get() != nullptr)
    {
        ++m_currentLineX;
        dereference();
    }
    return *this;
}

//===========================================================================
// public

FileOfLines::const_iterator& FileOfLines::const_iterator::operator--()
{
    enforce(*this != const_iterator(), "Only pre-decrement a non-end iterator.");
    enforce(m_currentLineX != 0, "Only pre-decrement an incremented iterator.");

    auto pCore = m_iterCore.lock();
    if (pCore.get() != nullptr)
    {
        --m_currentLineX;
        dereference();
    }
    return *this;
}

//===========================================================================
// public

FileOfLines::const_iterator FileOfLines::const_iterator::operator++(int)
{
    enforce(*this != const_iterator(), "Only post-increment a non-end iterator.");

    auto pCore = m_iterCore.lock();
    if (pCore.get() != nullptr)
    {
        auto other(*this);
        ++m_currentLineX;
        dereference();

        return other;
    }
    return const_iterator();
}

//===========================================================================
// public

FileOfLines::const_iterator FileOfLines::const_iterator::operator--(int)
{
    enforce(*this != const_iterator(), "Only post-decrement a non-end iterator.");
    enforce(m_currentLineX != 0, "Only post-decrement an incremented iterator.");

    auto pCore = m_iterCore.lock();
    if (pCore != nullptr)
    {
        auto other(*this);
        --m_currentLineX;
        dereference();

        return other;
    }
    return const_iterator();
}

//===========================================================================
// public

FileOfLines::const_iterator& FileOfLines::const_iterator::operator+=(int increment)
{
    enforce(*this != const_iterator(), "Only add-to with a non-end iterator.");

    auto pCore = m_iterCore.lock();
    if (pCore != nullptr)
    {
        m_currentLineX += increment;
        dereference();
    }
    return *this;
}

//===========================================================================
// public

FileOfLines::const_iterator& FileOfLines::const_iterator::operator-=(int increment)
{
    enforce(*this != const_iterator(), "Only subtract-from with a non-end iterator.");
    enforce(m_currentLineX > static_cast<unsigned int>(increment), "Only subtract-from less than or equal increments taken.");

    if (increment > 0 && static_cast<unsigned int>(increment) > m_currentLineX)
    {
        m_iterCore.reset();
        m_currentLineX = 0;
        m_currentLine.clear();
    }
    else
    {
        auto pCore = m_iterCore.lock();
        if (pCore != nullptr)
        {
            m_currentLineX -= increment;
            dereference();
        }
    }
    return *this;
}

//===========================================================================
// public

FileOfLines::const_iterator FileOfLines::const_iterator::operator+(int increment)
{
    enforce(*this != const_iterator(), "Only add-to with a non-end iterator.");

    return offsetBy(increment);
}

//===========================================================================
// public

FileOfLines::const_iterator FileOfLines::const_iterator::operator-(int increment)
{
    enforce(*this != const_iterator(), "Only subtract-from a non-end iterator.");

    return offsetBy(-increment);
}

//===========================================================================
// public

size_t FileOfLines::const_iterator::operator-(const FileOfLines::const_iterator src)
{
    enforce(*this != const_iterator(), "Only subtract from a non-end interator");
    enforce(src != const_iterator(), "Only subtract a non-end interator");

    auto lhsCore = m_iterCore.lock();
    auto rhsCore = src.m_iterCore.lock();
    enforce(lhsCore.get() == rhsCore.get(), "Only subtract with iterators pointing to the same file.");

    if (lhsCore.get() != nullptr && lhsCore.get() == rhsCore.get())
    {
        // underflow is not our problem
        return m_currentLineX - src.m_currentLineX;
    }

    size_t errorFrom0 = 0;
    return ~errorFrom0;
}

//===========================================================================
// public

bool FileOfLines::const_iterator::operator==(const const_iterator& rhs) const
{
    SharedCorePtr lhCore = m_iterCore.lock();
    SharedCorePtr rhCore = rhs.m_iterCore.lock();
    if (lhCore.get() != rhCore.get())  // one is null, or they point to different files
    {
        return false;
    }
    else if (lhCore.get() == nullptr && rhCore.get() == nullptr)
    {
        return true;
    }
    else
    {
        return m_currentLineX == rhs.m_currentLineX;
    }
}

//===========================================================================
// private

void FileOfLines::const_iterator::dereference() const
{
    auto p = m_iterCore.lock();
    enforce(p.get() != nullptr, "Only call dereference when you know the core is there.");
    enforce(p->isFileOpen(), "Only call dereference when you know the core is open.");

    try
    {
        bool isEOF = false;
        m_currentLine = p->get(m_currentLineX, &isEOF);
        if (m_currentLine.empty() && isEOF)
        {
            m_iterCore.reset();
            m_currentLine.clear();
        }
    }
    catch (RuntimeException&)
    {
        // let "enforce" actions filter up
        throw;
    }
    catch (std::exception&)
    {
        m_iterCore.reset();
        m_currentLine.clear();
    }
}

//===========================================================================
// private

FileOfLines::const_iterator FileOfLines::const_iterator::offsetBy(int increment)
{
    auto other(*this);
    auto p = other.m_iterCore.lock();

    enforce(p.get() != nullptr, "Only call offsetBy when you know the core is there.");
    enforce(p->isFileOpen(), "Only call offsetBy when you know the core is open.");

    if (p != nullptr)
    {
        bool moveBeforeBegin = (increment < 0) &&
                               static_cast<unsigned int>(-increment) > other.m_currentLineX;
        bool endKnown = p->isEndOfFileKnown();
        bool moveAfterEnd = (endKnown && !moveBeforeBegin) &&
                            (other.m_currentLineX + increment > p->lineCount());
        if (endKnown)
        {
            enforce(!moveBeforeBegin, "Once you know the file bounds, don't move back too far.");
            enforce(!moveAfterEnd, "Once you know the file bounds, don't move forward too far.");
        }
        if (moveBeforeBegin)
        {
            other.m_iterCore.reset();
            other.m_currentLineX = 0;
            other.m_currentLine.clear();
        }
        else
        {
            other.m_currentLineX += increment;

            try
            {
                other.dereference();
            }
            catch (RuntimeException&)
            {
                // let "enforce" actions filter up
                throw;
            }
            catch (std::exception&)
            {
                other.m_iterCore.reset();
                other.m_currentLine.clear();
                other.m_currentLineX = 0;
            }
        }
    }
    return other;
}

}
