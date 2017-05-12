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

#include "config_io.hxx"
#include "runtime_exception.hxx"
#include <memory>
#include <vector>
#include <sstream>

#include "string_splitjoin.hxx"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ansak
{

using namespace std;

namespace config
{

extern string vendorContext;
extern string productContext;

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

using namespace ansak::config;

//===========================================================================
// getUserConfig -- Retrieve the user config from the registry

Config getUserConfig()
{
    return Config(); /*
    enforce(!vendorContext.empty() && !productContext.empty(),
            "UserConfig: Provide a vendor and product context, please.");

    auto where(getUserRegistryPath());
    if (where.empty() || !regPathExist(where))
    {
        return Config();
    }
    return Config(getAtomsFromRegLocation(where)); */
}

//===========================================================================
// getSystemConfig -- Retrieve the system config from the registry

Config getSystemConfig()
{
    return Config(); /*
    enforce(!vendorContext.empty() && !productContext.empty(),
            "UserConfig: Provide a vendor and product context, please.");

    auto where(getSystemRegistryPath());
    if (where.empty() || !regPathExist(where))
    {
        return Config();
    }
    return Config(getAtomsFromRegLocation(where));
    */
}

//===========================================================================
// saveUserConfig -- Store the user config to the registry

bool saveUserConfig(const Config& config)
{
    return false; /*
    auto where(getUserRegistryPath());
    if (where.empty() || !regPathExist(where))
    {
        return false;
    }
    return writeConfigToRegLocation(where, config); */
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
