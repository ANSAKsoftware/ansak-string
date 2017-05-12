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
// 2014.12.18 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config_atom.cxx -- Class implementation for configuration atom.
//
///////////////////////////////////////////////////////////////////////////

#include <config_atom.hxx>

#include <runtime_exception.hxx>
#include <string.hxx>
#include <string_splitjoin.hxx>

#include <ctype.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <sstream>

using namespace ansak;
using namespace std;

namespace ansak
{

namespace internal
{

namespace
{

//===========================================================================
// isAnInt -- returns true if the incoming string is an int, false otherwise

bool isAnInt
(
    const string&       src         // I - a string to examine
)
{
    if (src.empty())
    {
        return false;
    }

    auto b = src.begin();
    bool negSign = false;
    if (*b == '-')
    {
        negSign = true;
        ++b;
    }

    // check for all numbers after the sign
    bool isIt = true;
    for_each(b, src.end(), [&](char c)
    {
        isIt &= isdigit(c) != 0;
    });

    // if it's close to overflowing, let the stream validate it
    if (isIt && src.size() - negSign > 9)
    {
        istringstream s(src);
        int i;
        s >> i;
        if (!s)
        {
            return false;
        }
    }

    return isIt;
}

//===========================================================================
// toInt -- parse string as returned int; enforces that this conversion will work

int toInt
(
    const string&       src         // I - a string to parse
)
{
    enforce(!src.empty(), "We need something to parse here.");
    istringstream s(src);
    int r;
    s >> r;
    enforce(!!s, "You have to make sure this is an int string, first.");

    return r;
}

//===========================================================================
// overflowOccurred -- at least on g++, overflow in conversion from float to
// int leads to 0x80000000 -- this detects something similar, regardless of
// default int size.

inline bool overflowOccurred
(
    int         i                   // I - the result of a float-to-int cast
)
{
    return static_cast<unsigned int>(i) == (~0u & ~(~0u >> 1));
}

//===========================================================================
// isAFloat -- returns true (and the parsed float) if the incoming string is a
// floating point value; false (and nothing else) otherwise.

bool isAFloat
(
    const string&       src,        // I - a string to parse
    float&              f           // O - the value if parsed
)
{
    if (src.empty())
    {
        return false;
    }

    istringstream s(src);
    float r;
    s >> r;

    // If error, or r didn't consume the whole string, failure
    if (!s || ((s.rdstate() & istringstream::eofbit) == 0))
    {
        return false;
    }
    else
    {
        f = r;
        return true;
    }
}

}

//===========================================================================
// Constructor (from nothing)

ConfigAtom::ConfigAtom()
  : m_asString(),
    m_asInt(0),
    m_asFloat(0),
    m_asPoint(),
    m_asRect(),
    m_asBool(false),
    m_isChangeable(false),
    m_isChanged(false),
    m_properType(kNothing),
    m_availableType(kNothing)
{
}

//===========================================================================
// Constructor (from bool)

ConfigAtom::ConfigAtom
(
    bool        boolValue,          // I - a confg value (bool)
    bool        isChangeable        // I - R/W? (default false)
) : m_asString(boolValue ? "true" : "false"),
    m_asInt(boolValue),
    m_asFloat(boolValue),
    m_asPoint(),
    m_asRect(),
    m_asBool(boolValue),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kBool),
    m_availableType(kInt | kFloat | kString | kBool)
{
}

//===========================================================================
// Constructor (from int)

ConfigAtom::ConfigAtom
(
    int         intValue,           // I - a confg value (int)
    bool        isChangeable        // I - R/W? (default false)
) : m_asString(),
    m_asInt(intValue),
    m_asFloat(static_cast<float>(intValue)),
    m_asPoint(),
    m_asRect(),
    m_asBool(intValue != 0),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kInt),
    m_availableType(kInt | kFloat | kString | kBool)
{
    ostringstream o;
    o << intValue;
    m_asString = o.str();
}

//===========================================================================
// Constructor (from float)

ConfigAtom::ConfigAtom
(
    float       floatValue,         // I - a confg value (float)
    bool        isChangeable        // I - R/W? (default false)
) : m_asString(),
    m_asInt(static_cast<int>(floatValue)),
    m_asFloat(floatValue),
    m_asPoint(),
    m_asRect(),
    m_asBool(floatValue != 0.0),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kFloat),
    m_availableType(kFloat | kString | kBool)
{
    ostringstream o;
    o << floatValue;
    m_asString = o.str();

    if (!overflowOccurred(m_asInt) && floatEqual(m_asInt, m_asFloat))
    {
        m_availableType |= kInt;
    }
    else
    {
        m_asInt = 0;
    }
}

//===========================================================================
// Constructor (from double)

ConfigAtom::ConfigAtom
(
    double      doubleValue,        // I - a confg value (double)
    bool        isChangeable        // I - R/W? (default false)
) : m_asString(),
    m_asInt(0),
    m_asFloat(0),
    m_asPoint(),
    m_asRect(),
    m_asBool(false),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kNothing),
    m_availableType(kNothing)
{
    if (fabs(doubleValue) < 1e38 && fabs(doubleValue) > 1e-38)
    {
        m_asFloat = static_cast<float>(doubleValue);
        m_asInt = static_cast<int>(doubleValue);
        m_asBool = !floatEqual(0.0, doubleValue);

        ostringstream o;
        o << doubleValue;
        m_asString = o.str();

        m_properType = kFloat;
        m_availableType = kFloat | kString | kBool;

        if (!overflowOccurred(m_asInt) && floatEqual(m_asInt, m_asFloat))
        {
            m_availableType |= kInt;
        }
        else
        {
            m_asInt = 0;
        }
    }
}

//===========================================================================
// Constructor (from string)

ConfigAtom::ConfigAtom
(
    const string&   stringValue,        // I - a confg value (string)
    bool            isChangeable        // I - R/W? (default false)
) : m_asString(stringValue),
    m_asInt(0),
    m_asFloat(0),
    m_asPoint(),
    m_asRect(),
    m_asBool(false),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kString),
    m_availableType(kString)
{
}

//===========================================================================
// Constructor (from string)

ConfigAtom::ConfigAtom
(
    const char*     stringValue,        // I - a confg value (string)
    bool            isChangeable        // I - R/W? (default false)
) : m_asString(stringValue == nullptr ? "" : stringValue),
    m_asInt(0),
    m_asFloat(0),
    m_asPoint(),
    m_asRect(),
    m_asBool(false),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kString),
    m_availableType(kString)
{
}

//===========================================================================
// Constructor (from Point)

ConfigAtom::ConfigAtom
(
    const Point&    point,              // I - a confg value (Point)
    bool            isChangeable        // I - R/W? (default false)
) : m_asString(),
    m_asInt(0),
    m_asFloat(0),
    m_asPoint(point),
    m_asRect(),
    m_asBool(false),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kPoint),
    m_availableType(kPoint | kString)
{
    stringstream o;
    o << x(m_asPoint) << ',' << y(m_asPoint);
    m_asString = o.str();
}

//===========================================================================
// Constructor (from Rect)

ConfigAtom::ConfigAtom
(
    const Rect&     rect,               // I - a confg value (Rect)
    bool            isChangeable        // I - R/W? (default false)
) : m_asString(),
    m_asInt(0),
    m_asFloat(0),
    m_asPoint(),
    m_asRect(rect),
    m_asBool(false),
    m_isChangeable(isChangeable),
    m_isChanged(false),
    m_properType(kRect),
    m_availableType(kRect | kString)
{
    stringstream o;
    o << '(' << left(m_asRect) << ',' << top(m_asRect) << "),(" <<
                width(m_asRect) << ',' << height(m_asRect) << ')';
    m_asString = o.str();
}

//===========================================================================
// static, public

ConfigAtom ConfigAtom::fromString
(
    const string&   utf8StringValue,        // I - a config string to parse
    bool            isChangeable            // I - R/W? (default false)
)
{
    // it's not even good UTF-8 -- can't parse it, nothing's here.
    if (!isUtf8(utf8StringValue))
    {
        return ConfigAtom();
    }

    // "true" or "false"?
    if (utf8StringValue == string("true") || utf8StringValue == string("false"))
    {
        return ConfigAtom(utf8StringValue == string("true"), isChangeable);
    }

    // an int?
    else if (isAnInt(utf8StringValue))
    {
        return ConfigAtom(toInt(utf8StringValue), isChangeable);
    }
    // maybe a float?
    else
    {
        float f;
        if (isAFloat(utf8StringValue, f))
        {
            return ConfigAtom(f, isChangeable);
        }
    }

    // split by commas
    auto commaFields = split(utf8StringValue, ',');
    // is it a point?
    if (commaFields.size() == 2 && isAnInt(commaFields[0]) && isAnInt(commaFields[1]))
    {
        return ConfigAtom(toPoint(toInt(commaFields[0]), toInt(commaFields[1])),
                          isChangeable);
    }

    // is it a rect?
    if (commaFields.size() == 4)
    {
        if (*commaFields[0].begin() == '(' && *commaFields[2].begin() == '(' &&
            *commaFields[1].rbegin() == ')' && *commaFields[3].rbegin() == ')')
        {
            string leftStr(commaFields[0].begin() + 1, commaFields[0].end());
            string topStr(commaFields[1].begin(), commaFields[1].begin() +
                                                 commaFields[1].size() - 1);
            string widthStr(commaFields[2].begin() + 1, commaFields[2].end());
            string heightStr(commaFields[3].begin(), commaFields[3].begin() +
                                                 commaFields[3].size() - 1);
            if (isAnInt(leftStr) && isAnInt(topStr) && isAnInt(widthStr) &&
                isAnInt(heightStr))
            {
                return ConfigAtom(toRect(toInt(leftStr), toInt(topStr), toInt(widthStr),
                                  toInt(heightStr)), isChangeable);
            }
        }

    }

    // I give up. It's just a string
    return ConfigAtom(utf8StringValue, isChangeable);
}

//===========================================================================
// static, public

ConfigAtom ConfigAtom::fromString
(
    const char*         utf8StringValue,        // I - a string to parse
    bool                isChangeable            // I - R/W setting? (def false)
)
{
    string theValue((utf8StringValue == nullptr) ? "" : utf8StringValue);
    return fromString(theValue, isChangeable);
}

//===========================================================================
// public

bool ConfigAtom::asBool
(
    bool*           available       // O - available as this type? (def nullptr)
) const
{
    if ((m_availableType & kBool) == 0)
    {
        enforce(available != nullptr,
                "You should be checking availability on asBool call");
        *available = false;
        return false;
    }
    return m_asBool;
}

//===========================================================================
// public

int ConfigAtom::asInt
(
    bool*           available       // O - available as this type? (def nullptr)
) const
{
    if ((m_availableType & kInt) == 0)
    {
        enforce(available != nullptr,
                "You should be checking availability on asInt call");
        *available = false;
        return 0;
    }
    return m_asInt;
}

//===========================================================================
// public

float ConfigAtom::asFloat
(
    bool*           available       // O - available as this type? (def nullptr)
) const
{
    if ((m_availableType & kFloat) == 0)
    {
        enforce(available != nullptr,
                "You should be checking availability on asFloat call");
        *available = false;
        return 0;
    }
    return m_asFloat;
}

//===========================================================================
// public

string ConfigAtom::asString
(
    bool*           available       // O - available as this type? (def nullptr)
) const
{
    if ((m_availableType & kString) == 0)
    {
        enforce(available != nullptr,
                "You should be checking availability on asString call");
        *available = false;
        return string();
    }
    return m_asString;
}

//===========================================================================
// public

Point ConfigAtom::asPoint
(
    bool*           available       // O - available as this type? (def nullptr)
) const
{
    if ((m_availableType & kPoint) == 0)
    {
        enforce(available != nullptr,
                "You should be checking availability on asPoint call");
        *available = false;
        return toPoint(0, 0);
    }
    return m_asPoint;
}

//===========================================================================
// public

Rect ConfigAtom::asRect
(
    bool*           available       // O - available as this type? (def nullptr)
) const
{
    if ((m_availableType & kRect) == 0)
    {
        enforce(available != nullptr,
                "You should be checking availability on asRect call");
        *available = false;
        return toRect(0, 0, 0, 0);
    }
    return m_asRect;
}

//===========================================================================
// public

void ConfigAtom::set(bool b)
{
    enforce(isChangeable(), "Value is not changeable");
    enforce(isBool(), "Value is not a bool, cannot be changed with a bool value.");
    if (b != m_asBool)
    {
        m_asBool = b;
        m_asInt = b;
        m_asFloat = b;
        m_asString = b ? "true" : "false";
        m_isChanged = true;
    }
}

//===========================================================================
// public

void ConfigAtom::set(int i)
{
    enforce(isChangeable(), "Value is not changeable");
    enforce(isInt(), "Value is not a int, cannot be changed with a int value.");
    if (i != m_asInt)
    {
        m_asInt = i;
        m_asBool = i != 0;
        m_asFloat = static_cast<float>(i);

        ostringstream o;
        o << i;
        m_asString = o.str();
        m_isChanged = true;
    }
}

//===========================================================================
// public

void ConfigAtom::set(float f)
{
    enforce(isChangeable(), "Value is not changeable");
    enforce(isFloat(), "Value is not a float, cannot be changed with a float value.");
    if (!floatEqual(f, m_asFloat))
    {
        m_asFloat = f;
        m_asBool = floatEqual(0.0, f);
        m_asInt = static_cast<int>(f);
        if (overflowOccurred(m_asInt) || !floatEqual(m_asInt, m_asFloat))
        {
            m_asInt = 0;
            m_availableType &= ~kInt;
        }
        else
        {
            m_availableType |= kInt;
        }
        
        ostringstream o;
        o << f;
        m_asString = o.str();
        m_isChanged = true;
    }
}

//===========================================================================
// public

void ConfigAtom::set(double d)
{
    enforce(fabs(d) < 1e38, "No float range overflow, please");

    set(static_cast<float>(d));
}

//===========================================================================
// public

void ConfigAtom::set(const std::string& str)
{
    enforce(isChangeable(), "Value is not changeable");
    enforce(isString(), "Value is not a string, cannot be changed with a string value.");
    if (str != m_asString)
    {
        m_asString = str;
        m_availableType = kString;
        m_isChanged = true;
    }
}

//===========================================================================
// public

void ConfigAtom::set(const char* src)
{
    string theValue((src == nullptr) ? "" : src);
    set(theValue);
}

//===========================================================================
// public

void ConfigAtom::set(const ansak::Point& pt)
{
    enforce(isChangeable(), "Value is not changeable");
    enforce(isPoint(), "Value is not a Point, cannot be changed with a Point value.");
    if (pt != m_asPoint)
    {
        ostringstream o;
        m_asPoint = pt;
        o << '(' << x(m_asPoint) << ',' << y(m_asPoint) << ')';
        m_asString = o.str();
        m_isChanged = true;
    }
}

//===========================================================================
// public

void ConfigAtom::set(const ansak::Rect& rect)
{
    enforce(isChangeable(), "Value is not changeable");
    enforce(isRect(), "Value is not a Rect, cannot be changed with a Rect value.");
    if (rect != m_asRect)
    {
        ostringstream o;
        m_asRect = rect;
        o << '(' << left(m_asRect) << ',' << top(m_asRect) << "),(" <<
                    width(m_asRect) << ',' << height(m_asRect) << ')';
        m_asString = o.str();
        m_isChanged = true;
    }
}

bool ConfigAtom::operator==(const ConfigAtom& rhs) const
{
    if (m_properType != rhs.m_properType)
    {
        return false;
    }

    switch (m_properType)
    {
        case kInt:              return m_asInt == rhs.m_asInt;
        case kFloat:            return floatEqual(m_asFloat, rhs.m_asFloat);
        case kString:           return m_asString == rhs.m_asString;
        case kPoint:            return m_asPoint == rhs.m_asPoint;
        case kRect:             return m_asRect == rhs.m_asRect;
        case kBool:             return m_asBool == rhs.m_asBool;
        case kNothing:          return true;

        default:                return false;
    }
}

}

}
