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
// 2014.12.30 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config.cxx -- defines the interface between programs and the OS-specific
//               ways configuration data is defaulted, retrieved and stored
//               for future sessions.
//
///////////////////////////////////////////////////////////////////////////

#include <config.hxx>
#include <config_atom.hxx>

#include <sstream>
#include <stdlib.h>
#include <config_atom.hxx>
#include <string_splitjoin.hxx>
#include <runtime_exception.hxx>
#include <string.hxx>
#include <string_trim.hxx>

using namespace std;
using namespace ansak::internal;

namespace ansak {

//===========================================================================
// public, constructor

Config::Config() : m_settings(new SettingsMapType())
{
}

//===========================================================================
// public, copy constructor

Config::Config(const Config& src) : m_settings(src.m_settings)
{
}

//===========================================================================
// public, assignment operator

Config& Config::operator=(const Config& src)
{
    if (this != &src)
    {
        m_settings = src.m_settings;
    }

    return *this;
}

//===========================================================================
// public

void Config::defaultWith
(
    const Config&       src                 // I - a collection of defaults
)
{
    splitIfNecessary();
    for (const auto& s : *(src.m_settings.get()))
    {
        auto p = m_settings->lower_bound(s.first);
        if (p == m_settings->end() || p->first != s.first)
        {
            m_settings->insert(p, s);
        }
    }
}

//===========================================================================
// public

void Config::overrideWith
(
    const Config&       src                 // I - a collection of over-rides
)
{
    splitIfNecessary();
    for (const auto& s : *(src.m_settings.get()))
    {
        auto p = m_settings->lower_bound(s.first);
        if (p == m_settings->end() || p->first != s.first)
        {
            m_settings->insert(p, s);
        }
        else if (p->second.isChangeable())
        {
            p->second = s.second;
        }
    }
}

//===========================================================================
// public

void Config::keepOverridesOf
(
    const Config&       src                 // I - a collection of defaults
)
{
    splitIfNecessary();
    for (const auto& s : *(src.m_settings.get()))
    {
        auto p = m_settings->lower_bound(s.first);
        if (p != m_settings->end() && p->first == s.first && p->second == s.second)
        {
            m_settings->erase(p->first);
        }
    }
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    bool                value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial bool setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial bool must not already be there.");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    int                 value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial int setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial int must not already be there.");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    float               value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial float setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial float must not already be there.");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    double              value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial float (double) setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial float (double) must not already be there.");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    const utf8String&   value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial string setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial string must not already be there.");
    enforce(isUtf8(value), "Initial string value must be UTF-8");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    const char*         value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial string (const char*) setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial string (const char*) must not already be there.");
    enforce(isUtf8(value), "Initial string (const char*) value must be UTF-8");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    Point               value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial point setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial point must not already be there.");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

void Config::initValue
(
    const utf8String&   name,               // I - the name of a value
    Rect                value,              // I - the initial value
    bool                changeable          // I - is the value changeable? def true
)
{
    enforce(!name.empty(), "Initial rect setting must have a name.");
    splitIfNecessary();
    auto insPt = m_settings->lower_bound(name);
    enforce(insPt == m_settings->end() || insPt->first != name,
            "Initial rect must not already be there.");
    m_settings->insert(insPt, make_pair(name, ConfigAtom(value, changeable)));
}

//===========================================================================
// public

bool Config::get
(
    const utf8String&   name,               // I - the name of a value
    bool&               value,              // O - the value if present
    bool*               hasChanged          // O - if the value has changed, def nullptr
) const
{
    enforce(!name.empty(), "Get bool setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return false;
    }

    ConfigAtom& setting = getPt->second;
    bool isType = setting.isBool();
    if (isType || setting.canBeBool())
    {
        value = setting.asBool();
        if (hasChanged != nullptr)
        {
            *hasChanged = !isType || setting.isChanged();
        }
        return true;
    }
    return false;
}

//===========================================================================
// public

bool Config::get
(
    const utf8String&   name,               // I - the name of a value
    int&                value,              // O - the value if present
    bool*               hasChanged          // O - if the value has changed, def nullptr
) const
{
    enforce(!name.empty(), "Get int setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return false;
    }

    ConfigAtom& setting = getPt->second;
    bool isType = setting.isInt();
    if (isType || setting.canBeInt())
    {
        value = setting.asInt();
        if (hasChanged != nullptr)
        {
            *hasChanged = !isType || setting.isChanged();
        }
        return true;
    }
    return false;
}

//===========================================================================
// public

bool Config::get
(
    const utf8String&   name,               // I - the name of a value
    float&              value,              // O - the value if present
    bool*               hasChanged          // O - if the value has changed, def nullptr
) const
{
    enforce(!name.empty(), "Get float setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return false;
    }

    ConfigAtom& setting = getPt->second;
    bool isType = setting.isFloat();
    if (isType || setting.canBeFloat())
    {
        value = setting.asFloat();
        if (hasChanged != nullptr)
        {
            *hasChanged = !isType || setting.isChanged();
        }
        return true;
    }
    return false;
}

//===========================================================================
// public

bool Config::get
(
    const utf8String&   name,               // I - the name of a value
    double&             value,              // O - the value if present
    bool*               hasChanged          // O - if the value has changed, def nullptr
) const
{
    enforce(!name.empty(), "Get float (double) setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return false;
    }

    ConfigAtom& setting = getPt->second;
    bool isType = setting.isFloat();
    if (isType || setting.canBeFloat())
    {
        value = static_cast<double>(setting.asFloat());
        if (hasChanged != nullptr)
        {
            *hasChanged = true;
        }
        return true;
    }
    return false;
}

//===========================================================================
// public

bool Config::get
(
    const utf8String&   name,               // I - the name of a value
    utf8String&         value,              // O - the value if present, != 0
    bool*               hasChanged          // O - if the value has changed, def nullptr
) const
{
    enforce(!name.empty(), "Get string setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return false;
    }

    ConfigAtom& setting = getPt->second;
    bool isType = setting.isString();
    enforce(isType || setting.canBeString());

    value = setting.asString();
    if (hasChanged != nullptr)
    {
        *hasChanged = !isType || setting.isChanged();
    }
    return true;
}

//===========================================================================
// public

bool Config::get
(
    const utf8String&   name,               // I - the name of a value
    Point&              value,              // O - the value if present
    bool*               hasChanged          // O - if the value has changed, def nullptr
) const
{
    enforce(!name.empty(), "get Point setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return false;
    }

    ConfigAtom& setting = getPt->second;
    if (setting.isPoint())
    {
        value = setting.asPoint();
        if (hasChanged != nullptr)
        {
            *hasChanged = setting.isChanged();
        }
        return true;
    }
    return false;
}

//===========================================================================
// public

bool Config::get
(
    const utf8String&   name,               // I - the name of a value
    Rect&               value,              // O - the value if present
    bool*               hasChanged          // O - if the value has changed, def nullptr
) const
{
    enforce(!name.empty(), "get Rect setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return false;
    }

    ConfigAtom& setting = getPt->second;
    if (setting.isRect())
    {
        value = setting.asRect();
        if (hasChanged != nullptr)
        {
            *hasChanged = setting.isChanged();
        }
        return true;
    }
    return false;
}

//===========================================================================
// public

ConfigAtom Config::getAtom
(
    const utf8String&   name                // I - the name of a value
) const
{
    enforce(!name.empty(), "getAtom setting must have a name.");
    auto getPt = m_settings->lower_bound(name);
    if (getPt == m_settings->end() || getPt->first != name)
    {
        return ConfigAtom();
    }

    return getPt->second;
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    bool                value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put bool setting must have a name.");
    splitIfNecessary();
    auto putPt = m_settings->lower_bound(name);
    bool foundIt = putPt != m_settings->end() && putPt->first == name;

    if (foundIt)
    {
        enforce(putPt->second.isChangeable(), "Put bool must have changeable destination.");
        if (putPt->second.isBool())
        {
            putPt->second.set(value);
        }
        else
        {
            enforce(forceType, "Put bool to non-bool type must be forced.");
            putPt->second = ConfigAtom(!value, ConfigAtom::changeable);
            putPt->second.set(value);
        }
    }
    else
    {
        // Create a new ConfigAtom -- but mark it changed straightaway
        putPt = m_settings->insert(putPt,
                make_pair(name, ConfigAtom(!value, ConfigAtom::changeable)));
        putPt->second.set(value);
    }
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    int                 value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put int setting must have a name.");
    splitIfNecessary();
    auto putPt = m_settings->lower_bound(name);
    bool foundIt = putPt != m_settings->end() && putPt->first == name;

    if (foundIt)
    {
        enforce(putPt->second.isChangeable(), "Put bool must have changeable destination.");
        if (putPt->second.isInt())
        {
            putPt->second.set(value);
        }
        else
        {
            enforce(forceType, "Put int to non-int type must be forced.");
            putPt->second = ConfigAtom(~value, ConfigAtom::changeable);
            putPt->second.set(value);
        }
    }
    else
    {
        // Create a new ConfigAtom -- but mark it changed straightaway
        putPt = m_settings->insert(putPt,
                make_pair(name, ConfigAtom(~value, ConfigAtom::changeable)));
        putPt->second.set(value);
    }
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    float               value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put float setting must have a name.");
    splitIfNecessary();
    auto putPt = m_settings->lower_bound(name);
    bool foundIt = putPt != m_settings->end() && putPt->first == name;

    if (foundIt)
    {
        enforce(putPt->second.isChangeable(), "Put bool must have changeable destination.");
        if (putPt->second.isFloat())
        {
            putPt->second.set(value);
        }
        else
        {
            enforce(forceType, "Put float to non-float type must be forced.");
            putPt->second = ConfigAtom(value / 1.1 + 1.0, ConfigAtom::changeable);
            putPt->second.set(value);
        }
    }
    else
    {
        // Create a new ConfigAtom -- but mark it changed straightaway
        putPt = m_settings->insert(putPt,
                make_pair(name, ConfigAtom(value / 1.1 + 1.0, ConfigAtom::changeable)));
        putPt->second.set(value);
    }
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    double              value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put float (double) setting must have a name.");
    splitIfNecessary();
    auto putPt = m_settings->lower_bound(name);
    bool foundIt = putPt != m_settings->end() && putPt->first == name;

    if (foundIt)
    {
        enforce(putPt->second.isChangeable(),
                "Put float (double) must have changeable destination.");
        if (putPt->second.isFloat())
        {
            putPt->second.set(value);
        }
        else
        {
            enforce(forceType, "Put float (double) to non-float type must be forced.");
            putPt->second = ConfigAtom(value / 1.1 + 1.0, ConfigAtom::changeable);
            putPt->second.set(value);
        }
    }
    else
    {
        // Create a new ConfigAtom -- but mark it changed straightaway
        putPt = m_settings->insert(putPt,
                make_pair(name, ConfigAtom(value / 1.1 + 1.0, ConfigAtom::changeable)));
        putPt->second.set(value);
    }
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    const utf8String&   value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put string setting must have a name.");
    splitIfNecessary();
    auto putPt = m_settings->lower_bound(name);
    bool foundIt = putPt != m_settings->end() && putPt->first == name;

    if (foundIt)
    {
        enforce(putPt->second.isChangeable(),
                "Put string must have changeable destination.");
        if (putPt->second.isString())
        {
            putPt->second.set(value);
        }
        else
        {
            string altValue(" ");
            altValue += value;
            enforce(forceType, "Put string to non-string type must be forced.");
            putPt->second = ConfigAtom(altValue, ConfigAtom::changeable);
            putPt->second.set(value);
        }
    }
    else
    {
        // Create a new ConfigAtom -- but mark it changed straightaway
        putPt = m_settings->insert(putPt,
                make_pair(name, ConfigAtom(value + " ", ConfigAtom::changeable)));
        putPt->second.set(value);
    }
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    const char*         value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put string (const char*) setting must have a name.");
    utf8String sValue(value == nullptr ? "" : value);
    put(name, sValue, forceType);
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    Point               value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put point setting must have a name.");
    splitIfNecessary();
    auto putPt = m_settings->lower_bound(name);
    bool foundIt = putPt != m_settings->end() && putPt->first == name;

    if (foundIt)
    {
        enforce(putPt->second.isChangeable(), "Put point must have changeable destination.");
        if (putPt->second.isPoint())
        {
            putPt->second.set(value);
        }
        else
        {
            enforce(forceType, "Put point to non-point type must be forced.");
            Point pt = toPoint(x(value) + 1, y(value));
            putPt->second = ConfigAtom(pt, ConfigAtom::changeable);
            putPt->second.set(value);
        }
    }
    else
    {
        // Create a new ConfigAtom -- but mark it changed straightaway
        Point pt = toPoint(x(value) + 1, y(value));
        putPt = m_settings->insert(putPt,
                make_pair(name, ConfigAtom(pt, ConfigAtom::changeable)));
        putPt->second.set(value);
    }
}

//===========================================================================
// public

void Config::put
(
    const utf8String&   name,               // I - the name of a value
    Rect                value,              // I - the new value
    bool                forceType           // I - force new value type, def false
)
{
    enforce(!name.empty(), "Put rect setting must have a name.");
    splitIfNecessary();
    auto putPt = m_settings->lower_bound(name);
    bool foundIt = putPt != m_settings->end() && putPt->first == name;

    if (foundIt)
    {
        enforce(putPt->second.isChangeable(), "Put rect must have changeable destination.");
        if (putPt->second.isRect())
        {
            putPt->second.set(value);
        }
        else
        {
            enforce(forceType, "Put rect to non-rect type must be forced.");
            Rect r = toRect(left(value) + 1, top(value), width(value), height(value));
            putPt->second = ConfigAtom(r, ConfigAtom::changeable);
            putPt->second.set(value);
        }
    }
    else
    {
        // Create a new ConfigAtom -- but mark it changed straightaway
        Rect r = toRect(left(value) + 1, top(value), width(value), height(value));
        putPt = m_settings->insert(putPt,
                make_pair(name, ConfigAtom(r, ConfigAtom::changeable)));
        putPt->second.set(value);
    }
}

//===========================================================================
// public

vector<utf8String> Config::getValueNames() const
{
    vector<utf8String> result;
    result.reserve(m_settings->size());

    for (const auto& i : (*m_settings))
    {
        result.push_back(i.first);
    }
    return result;
}

//===========================================================================
// public

bool Config::operator==(const Config& rhs) const
{
    if (this == &rhs || m_settings.get() == rhs.m_settings.get()) return true;
    else
        return *m_settings == *rhs.m_settings;
}

//===========================================================================
// private

void Config::splitIfNecessary()
{
    if (m_settings.use_count() > 1)
    {
        SettingsMapPtrType other = make_shared<SettingsMapType>(*m_settings.get());
        m_settings = other;
    }
}

//===========================================================================
// private

void Config::importSetting
(
    const std::string&      element         // I - one line to parse
)
{
    vector<string> parts(split(element, '='));
    if (parts.size() >= 2)
    {
        string key(parts[0]);
        string value;
        if (parts.size() == 2)
        {
            value = parts[1];
        }
        else
        {
            parts.erase(parts.begin());
            value = join(parts, '=');
        }
        trim(key);
        trim(value);

        if (!key.empty() && !value.empty())
        {
            m_settings->insert(
                    make_pair(key, ConfigAtom::fromString(value, true)));
        }
    }
}

//===========================================================================
// private

void Config::importSetting
(
    const ConfigAtomPair&       element     // I - one key-value pair
)
{
    if (!element.first.empty() && element.second != ConfigAtom())
    {
        m_settings->insert(element);
    }
}

}
