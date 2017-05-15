///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, Arthur N. Klassen
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
// 2016.02.08 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config_io_registry.cxx -- entrypoints to get the default user and system
//                           settings Registry keys for Windows apps to use.
//
///////////////////////////////////////////////////////////////////////////

#if defined(WIN32)

#include <config_io.hxx>
#include <config_atom.hxx>
#include <runtime_exception.hxx>
#include <memory>
#include <vector>
#include <map>
#include <sstream>

#include <string_splitjoin.hxx>
#include <windows_registry_key.hxx>

using namespace ansak::config;

namespace ansak
{

using namespace std;

namespace config
{

extern string vendorContext;
extern string productContext;

}

namespace config
{

    //===========================================================================
    // getUserRegistryPath -- in light of a "vendor name" and "product name",
    // generate a registry path for user settings.

    string getUserRegistryPath()
    {
        stringstream o;
        o << "HKCU\\Software\\" << vendorContext << '\\' << productContext;
        return o.str();
    }

    //===========================================================================
    // getSystemRegistryPath -- in light of a "vendor name" and "product name",
    // generate a registry path for system-wide settings.

    string getSystemRegistryPath()
    {
        stringstream o;
        o << "HKLM\\Software\\" << vendorContext << '\\' << productContext;
        return o.str();
    }

}

namespace internal
{

namespace {

int asInt(DWORD d)
{
    bool valNegative = ((d & 0x80000000) != 0);
    if (valNegative)
    {
        d = ~d + 1;
        return -static_cast<int>(d);
    }
    else
    {
        return static_cast<int>(d);
    }
}

DWORD asDword(int i)
{
    DWORD d = static_cast<DWORD>(abs(i));
    if (i < 0)
    {
        d = ~d + 1;
    }
    return d;
}

}



using ConfigAtomVector = vector<ConfigAtomPair>;
using ConfigAtomMap = map<string, ConfigAtom>;

ConfigAtomVector getAtomsFromRegLocation(const string& location)
{
    try
    {
        auto theKey = WindowsRegKey::open(location);
        auto theNames = theKey.getValueNames();
        ConfigAtomVector r;
        for (const auto& name : theNames)
        {
            switch (theKey.getValueType(name))
            {
            case REG_DWORD:
                {
                    DWORD dwordVal;
                    if (theKey.getValue(name, dwordVal))
                    {
                        r.push_back(ConfigAtomPair(name, ConfigAtom(asInt(dwordVal))));
                    }
                }
                break;

            case REG_SZ:
                {
                    string stringVal;
                    if (theKey.getValue(name, stringVal))
                    {
                        r.push_back(ConfigAtomPair(name, ConfigAtom::fromString(stringVal)));
                    }
                }
                break;

            default:
                break;
            }
        }

        return r;
    }
    catch ( RegistryAccessException& )
    {
        return ConfigAtomVector();
    }
}

ConfigAtomMap settingsToAtoms(const Config& setting)
{
    auto names = setting.getValueNames();
    ConfigAtomMap r;
    if (!names.empty())
    {
        for (const auto& name : names)
        {
            r[name] = setting.getAtom(name);
        }
    }

    return r;
}

bool saveAtomsToRegLocation(const string& location, const ConfigAtomMap& atoms)
{
    try
    {
        auto theKey = WindowsRegKey::open(location, WindowsRegKey::createIt);
        ConfigAtomMap inProgress = atoms;
        for (const auto& name : theKey.getValueNames())
        {
            auto inMap = inProgress.lower_bound(name);
            if (inMap == inProgress.end() || inMap->first != name)
            {
                theKey.deleteValue(name);
                continue;
            }
            switch (theKey.getValueType(name))
            {
            case REG_DWORD:
                {
                    if (!inMap->second.canBeInt())
                    {
                        return false;
                    }
                    if (!theKey.setValue(name, asDword(inMap->second.asInt())))
                    {
                        return false;
                    }
                    inProgress.erase(inMap);
                }
                break;

            case REG_SZ:
                {
                    if (!inMap->second.canBeString())
                    {
                        return false;
                    }
                    string stringVal = inMap->second.asString();
                    if (!theKey.setValue(name, stringVal))
                    {
                        return false;
                    }
                    inProgress.erase(inMap);
                }
                break;

            default:
                break;
            }
        }

        for (const auto& pair : inProgress)
        {
            if (pair.second.isInt())
            {
                if (!theKey.setValue(pair.first, asDword(pair.second.asInt())))
                {
                    return false;
                }
            }
            else if (!theKey.setValue(pair.first, pair.second.asString()))
            {
                return false;
            }
        }
    }
    catch ( RegistryAccessException& )
    {
        return false;
    }

    return true;
}

}

using namespace ansak::internal;

//===========================================================================
// getUserConfig -- Retrieve the user config from the registry

Config getUserConfig()
{
    enforce(!vendorContext.empty() && !productContext.empty(),
            "UserConfig: Provide a vendor and product context, please.");

    auto where(getUserRegistryPath());
    if (where.empty())
    {
        return Config();
    }
    return Config(getAtomsFromRegLocation(where));
}

//===========================================================================
// getSystemConfig -- Retrieve the system config from the registry

Config getSystemConfig()
{
    enforce(!vendorContext.empty() && !productContext.empty(),
            "UserConfig: Provide a vendor and product context, please.");

    auto where(getSystemRegistryPath());
    if (where.empty())
    {
        return Config();
    }
    return Config(getAtomsFromRegLocation(where));
}

//===========================================================================
// saveUserConfig -- Store the user config to the registry

bool saveUserConfig(const Config& config)
{
    return false;
    auto where(getUserRegistryPath());
    if (where.empty())
    {
        return false;
    }
    return saveAtomsToRegLocation(where, settingsToAtoms(config));
}

Config getConfig(const string& otherReg)
{
    return Config();
}

bool writeConfig(const string& dest, const Config& src)
{
    return false;
}

}

#endif
