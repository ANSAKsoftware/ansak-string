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
// 2014.12.16 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config_atom.hxx -- Class definition for configuration atom.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <tuple>
#include <config_types.hxx>

namespace ansak
{

namespace internal
{

class ConfigAtom
{

public:

    static const bool changeable = true;

    //=======================================================================
    // Constructors

    ConfigAtom();
    ConfigAtom(bool boolValue, bool isChangeable = false);
    ConfigAtom(int intValue, bool isChangeable = false);
    ConfigAtom(float floatValue, bool isChangeable = false);
    ConfigAtom(double floatValue, bool isChangeable = false);
    ConfigAtom(const std::string& utf8StringValue, bool isChangeable = false);
    ConfigAtom(const char* utf8StringValue, bool isChangeable = false);
    ConfigAtom(const ansak::Point& point, bool isChangeable = false);
    ConfigAtom(const ansak::Rect& rect, bool isChangeable = false);

    //=======================================================================
    // fromString -- parses a string into whatever kind of config atom it
    // might be; const char* calls const string& value

    static ConfigAtom fromString
    (
        const std::string&  utf8StringValue,            // I - a string to parse
        bool                isChangeable = false        // I - R/W setting?
    );
    static ConfigAtom fromString
    (
        const char*         utf8StringValue,            // I - a string to parse
        bool                isChangeable = false        // I - R/W setting?
    );

    //=======================================================================
    // About the atom

    bool isChangeable() const { return m_isChangeable; }
    bool isChanged() const { return m_isChanged; }

    bool isBool() const { return m_properType == kBool; }
    bool isInt() const { return m_properType == kInt; }
    bool isFloat() const { return m_properType == kFloat; }
    bool isString() const { return m_properType == kString; }
    bool isPoint() const { return m_properType == kPoint; }
    bool isRect() const { return m_properType == kRect; }

    bool canBeBool() const { return (m_availableType & kBool) != 0; }
    bool canBeInt() const { return (m_availableType & kInt) != 0; }
    bool canBeFloat() const { return (m_availableType & kFloat) != 0; }
    bool canBeString() const { return (m_availableType & kString) != 0; }
    bool canBePoint() const { return (m_availableType & kPoint) != 0; }
    bool canBeRect() const { return (m_availableType & kRect) != 0; }

    //=======================================================================
    // Retrieves the value, if not available, enforces that caller wanted to
    // know then sets availability appropriately, returns false, 0, 0.0, "",
    // (0,0) or (0,0),(0,0) as appropriate.

    bool asBool(bool* available = nullptr) const;
    int asInt(bool* available = nullptr) const;
    float asFloat(bool* available = nullptr) const;
    std::string asString(bool* available = nullptr) const;
    ansak::Point asPoint(bool* available = nullptr) const;
    ansak::Rect asRect(bool* available = nullptr) const;

    //=======================================================================
    // Sets the value -- enforces that the value is already the same type, that
    // it is changeable, then sets the new value. "double" and "const char*"
    // versions go to "float" (enforces that it's possible), and string.

    void set(bool b);
    void set(int i);
    void set(float f);
    void set(double d);
    void set(const std::string& str);
    void set(const char* str);
    void set(const ansak::Point& pt);
    void set(const ansak::Rect& rect);

    //=======================================================================
    // Compare (equality) two atoms

    bool operator==(const ConfigAtom& rhs) const;
    bool operator!=(const ConfigAtom& rhs) const { return !operator==(rhs); }

private:

    // enumeration giving data types, both original and available
    enum AtomType {
        kNothing,
        kInt       = 1,
        kFloat     = 2,
        kString    = 4,
        kPoint     = 8,
        kRect      = 16,
        kBool      = 32
    };

    std::string             m_asString;             // a setting's string representation
    int                     m_asInt;                // a setting's int value
    float                   m_asFloat;              // a setting's float value
    ansak::Point            m_asPoint;              // a setting's Point value
    ansak::Rect             m_asRect;               // a setting's Rect value
    bool                    m_asBool;               // a setting's bool value

    bool                    m_isChangeable;         // is the setting changeable
    bool                    m_isChanged;            // has the setting been changed?
    AtomType                m_properType;           // what is the setting's proper type?
    int                     m_availableType;        // what can it be shown as?
};

struct ConfigAtomPair : public std::pair<std::string, ConfigAtom>
{
    ConfigAtomPair(const std::string& name, const ConfigAtom& atom) :
        std::pair<std::string, ConfigAtom>(std::make_pair(name, atom)) {}
};

}

}
