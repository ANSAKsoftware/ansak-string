///////////////////////////////////////////////////////////////////////////
//
// 2015.11.23
//
// The author (Arthur N. Klassen) disclaims all copyright to this source code.
// In place of a legal notice, here is a blessing:
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
// (Is it too much to suggest retaining this header on this file?)
///////////////////////////////////////////////////////////////////////////
//
// encoding_check_predicate.cxx -- efficient mechanism to add encoding
//      validity checks to range validity checks, when required
//
///////////////////////////////////////////////////////////////////////////

#include "string.hxx"
#include "bits/string_internal.hxx"

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
