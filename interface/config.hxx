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
// 2014.12.26 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config.hxx -- defines the class that collects user- and system-defaults
//               for the operation of a specific program. Facilities are
//               included for setting up default values, loading from
//               different default locations (specific to each OS) and for
//               merging different collections of settings together.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "string.hxx"

#include <vector>
#include <memory>
#include <map>

#include "config_types.hxx"

namespace ansak {

namespace internal {

class ConfigAtom;
struct ConfigAtomPair;

}

//===========================================================================
// class Config - encapsulates 

class Config
{
public:

    //=======================================================================
    // named constants for un-changeable default values and forcing type change

    static const bool immutable = false;
    static const bool forceTypeChange = true;

    //=======================================================================
    // Constructor -- creates empty config object

    Config();
    Config(const Config& src);
    Config& operator=(const Config& src);

    //=======================================================================
    // Constructor from a collection that can iterate out its values, one
    // string at a time.

    template<class Collection> Config
    (
        const Collection&       src         // I - iterable collection of lines
    ) : Config()
    {
        for (const auto& e : src)
        {
            importSetting(e);
        }
    }

    //=======================================================================
    // defaultWith -- Apply default values from another Config collection for
    // settings that aren't already set.

    void defaultWith
    (
        const Config&       src             // I - a collection of defaults
    );

    //=======================================================================
    // overrideWith -- Apply over-ride values from another Config collection
    // for any settings that it contains.

    void overrideWith
    (
        const Config&       src             // I - a collection of over-rides
    );

    //=======================================================================
    // keepOverridesOf -- Remove values from a collection that have the same
    // value as named values in another collection

    void keepOverridesOf
    (
        const Config&       src             // I - a collection of defaults
    );

    //=======================================================================
    // initValue -- Sets initial values of any type for specific names. The
    // values set must not already have settings in the collection.
    //
    // use "immutable" above for an easy-to-read over-ride of changeable

    void initValue
    (
        const utf8String&   name,               // I - the name of a value
        bool                value,              // I - the initial value
        bool                changeable = true   // I - is the value changeable?
    );

    // passim

    void initValue(const utf8String& name, int value, bool changeable = true);
    void initValue(const utf8String& name, float value, bool changeable = true);
    void initValue(const utf8String& name, double value, bool changeable = true);
    void initValue(const utf8String& name, const utf8String& value, bool changeable = true);
    void initValue(const utf8String& name, const char* value, bool changeable = true);
    void initValue(const utf8String& name, Point value, bool changeable = true);
    void initValue(const utf8String& name, Rect value, bool changeable = true);

    //=======================================================================
    // get -- Get a value (if it exists) from the collection. Also informs the
    // interested caller if a value has changed since originally being set.

    bool get
    (
        const utf8String&       name,                   // I - the name of a value
        bool&                   value,                  // O - the value if present
        bool*                   hasChanged = nullptr    // O - if the value has changed
    ) const;

    // passim

    bool get(const utf8String& name, int& value, bool* hasChanged = nullptr) const;
    bool get(const utf8String& name, float& value, bool* hasChanged = nullptr) const;
    bool get(const utf8String& name, double& value, bool* hasChanged = nullptr) const;
    bool get(const utf8String& name, utf8String& value, bool* hasChanged = nullptr) const;
    bool get(const utf8String& name, Point& value, bool* hasChanged = nullptr) const;
    bool get(const utf8String& name, Rect& value, bool* hasChanged = nullptr) const;

    //=======================================================================
    // get -- Get a value (if it exists) from the collection in its internal form

    ansak::internal::ConfigAtom getAtom
    (
        const utf8String&       name                    // I - the name of a value
    ) const;

    //=======================================================================
    // put -- Put a value into a collection. Asserts that the type is not being
    // changed unless caller selects such behaviour.
    //
    // use forceTypeChange above for an easy-to-read over-ride of forceType

    void put
    (
        const utf8String&       name,               // I - the name of a value
        bool                    value,              // I - the new value
        bool                    forceType = false   // I - force new value type
    );

    // passim

    void put(const utf8String& name, int value, bool forceType = false);
    void put(const utf8String& name, float value, bool forceType = false);
    void put(const utf8String& name, double value, bool forceType = false);
    void put(const utf8String& name, const utf8String& value, bool forceType = false);
    void put(const utf8String& name, const char* value, bool forceType = false);
    void put(const utf8String& name, Point value, bool forceType = false);
    void put(const utf8String& name, Rect value, bool forceType = false);

    //=======================================================================
    // getValueNames -- Gets the names of all the settings in the collection.

    std::vector<utf8String> getValueNames() const;

    bool operator==(const Config& rhs) const;
    bool operator!=(const Config& rhs) const { return !operator==(rhs); }

private:

    using SettingsMapType = std::map<utf8String, ansak::internal::ConfigAtom>;
    using SettingsMapPtrType = std::shared_ptr<SettingsMapType>;

    void splitIfNecessary();

    //=======================================================================
    // importSetting -- Parses one line of source material for key=value data.

    void importSetting
    (
        const std::string&      element         // I - one line to parse
    );

    //=======================================================================
    // importSetting -- Imports on ConfigAtom with its name

    void importSetting
    (
        const ansak::internal::ConfigAtomPair&   element    // I - one key-value pair
    );

    SettingsMapPtrType          m_settings;         // the collection of settings
};

}


