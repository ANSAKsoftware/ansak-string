///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015, Arthur N. Klassen
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
// 2015.11.23 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// encoding_check_predicate.cxx -- efficient mechanism to add encoding
//      validity checks to range validity checks, when required
//
///////////////////////////////////////////////////////////////////////////

#include <string.hxx>
#include <string_internal.hxx>

namespace ansak {

using namespace internal;

namespace {

///////////////////////////////////////////////////////////////////////////
// Local Data

EncodingTypeFlags encodingTypeToEncodingFlag[kFirstInvalidEncoding] =
    { kNoneFlag, kIsAssignedFlag, kIsPrivateFlag, kIsControlFlag, kIsWhiteSpaceFlag };

}

///////////////////////////////////////////////////////////////////////////
// Class methods

///////////////////////////////////////////////////////////////////////////
// private, constructor

EncodingCheckPredicate::EncodingCheckPredicate
(
    EncodingType    t,          // I - what range to check for
    bool            checkForIt  // I - for presence? or absence, def. presence
) : m_mask(0),
    m_value(0)
{
    if (t > kIsNone && t < kFirstInvalidEncoding)
    {
        m_value = m_mask = encodingTypeToEncodingFlag[t];
        if (!checkForIt)
        {
            m_value &= ~m_value;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// public

EncodingCheckPredicate EncodingCheckPredicate::andIf
(
    EncodingType    toCheckFor      // I - an additional range to check for
)
{
    EncodingCheckPredicate other(toCheckFor);
    m_value |= other.m_value;
    m_mask |= other.m_mask;
    return *this;
}

///////////////////////////////////////////////////////////////////////////
// public

EncodingCheckPredicate EncodingCheckPredicate::andIfNot
(
    EncodingType    toCheckFor      // I - an additional range to check for
)
{
    EncodingCheckPredicate other(toCheckFor, false);
    m_value |= other.m_value;
    m_mask |= other.m_mask;
    return *this;
}

///////////////////////////////////////////////////////////////////////////
// public, comparator

bool EncodingCheckPredicate::operator==(const EncodingCheckPredicate& other) const
{
    if (this == &other)
    {
        return true;
    }
    return m_mask == other.m_mask &&
           m_value == other.m_value;
}

///////////////////////////////////////////////////////////////////////////
// public, invoker

bool EncodingCheckPredicate::operator()(char c) const
{
    if (m_mask == 0)
    {
        return true;
    }
    auto cEncodingMask = charToEncodingTypeMask(c);
    return (cEncodingMask & m_mask) == m_value;
}

///////////////////////////////////////////////////////////////////////////
// public, invoker

bool EncodingCheckPredicate::operator()(char16_t c) const
{
    if (m_mask == 0)
    {
        return true;
    }
    auto cEncodingMask = charToEncodingTypeMask(c);
    return (cEncodingMask & m_mask) == m_value;
}

///////////////////////////////////////////////////////////////////////////
// public, invoker

bool EncodingCheckPredicate::operator()(char32_t c) const
{
    if (m_mask == 0)
    {
        return true;
    }
    auto cEncodingMask = charToEncodingTypeMask(c);
    return (cEncodingMask & m_mask) == m_value;
}

}
