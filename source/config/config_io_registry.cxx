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
#include "config_internals.hxx"
#include "runtime_exception.hxx"
#include <memory>
#include <vector>

#include "string_split.hxx"

namespace ansak
{

using namespace std;
using namespace ansak::config;

namespace config
{

extern string vendorContext;
extern string productContext;

class RegKey {
public:
    RegKey(const utf8String& path);

    ~RegKey();

    using ValueNamesType = std::vector<utf8String>;

    ValueNamesType getValueNames() const;

    bool getValue(const utf8String& valueName, DWORD& value);
    bool getValue(const utf8String& valueName, utf8String& value);

    bool setValue(const utf8String& valueName, DWORD value);
    bool setValue(const utf8String& valueName, const utf8String& value);

    DWORD getValueType(const utf8String& valueName);

private:

    RegKey(HKEY newKey, const RegKey* parent, bool detached);
    bool detach() { m_closeOnDestruct = false; }

    static HKEY getRootKey(const utf8String& root);

    bool                m_closeOnDestruct;
    HKEY                m_theKey;
    unique_ptr<RegKey>  m_parentKey;
};

RegKey::RegKey(const utf8String& path)
{
    auto components(split(path, '\\'));
    int currentComponent = 0;
    HKEY lastKey = nullptr;
    unique_ptr<RegKey> lastRegKeyObject;
    while (!components.empty())
    {
        auto currentKey = components.front();

        HKEY currentHKey = 0;
        if (currentComponent == 0)
        {
            currentHKey = getRootKey(currentKey);
        }
        else
        {
            auto wideKeyName(toUtf16(currentKey));
            auto rv = RegOpenKeyExW(
                    lastRegKeyObject->m_theKey, reinterpret_cast<LPCWSTR>(wideKeyName.c_str()), 0,
                    KEY_READ, &currentHKey);

            if (rv != ERROR_SUCCESS)
            {
                auto err = GetLastError();
                // throw something
            }
        }
        // never RegCloseKey the root key
        lastRegKeyObject.reset(new RegKey(currentHKey, lastRegKeyObject, currentComponent == 0));

        components.erase(components.begin(), components.begin() + 1);
        ++currentComponent;
    }
}

RegKey::~RegKey()
{
    if (m_closeOnDestruct)
    {
        RegCloseKey(m_theKey);
    }
    m_parentKey.reset(nullptr);
}

RegKey::ValueNamesType RegKey::getValueNames() const
{
    bool stillWorking = true;
    DWORD valueIndex = 0;
    wchar_t valueName[120];
    ValueNamesType result;
    for (; stillWorking; ++valueIndex)
    {
        DWORD valueSize = 120;
        auto rv = RegEnumValueW(m_theKey, valueIndex, valueName, &valueSize, 0, 0, 0);
        if (rv == ERROR_NO_MORE_ITEMS)
        {
            break;
        }
        else if ((rv == ERROR_SUCCESS) && (valueSlze < 120))
        {
            valueName[119] = L'\0';
            result.push_back(toUtf8(wstring(valueName)));
        }
        else if (rv != ERROR_SUCCESS)
        {
            auto err = GetLastError();
            // throw something
        }
    }

    return result;
}

bool RegKey::getValue(const utf8String& valueName, DWORD& value);
bool RegKey::getValue(const utf8String& valueName, utf8String& value);

bool RegKey::setValue(const utf8String& valueName, DWORD value);
bool RegKey::setValue(const utf8String& valueName, const utf8String& value);

DWORD RegKey::getValueType(const utf8String& valueName)
{
    utf16String nameIn16bit(toUtf16(valueName));
    DWORD regType;
    auto rv = RegQueryValueExW(m_theKey, reinterpret_cast<LPCWSTR>(nameIn16Bit.c_str()), NULL,
                               &regType, nullptr, nullptr);
    if (rv == ERROR_SUCCESS)
    {
        return regType;
    }
    else
    {
        return REG_NONE;
    }
}

HKEY RegKey::getRootKey(const utf8String& root)
{
    if (root == "HKCR" || root == "HKEY_CLASSES_ROOT") return HKEY_CLASSES_ROOT;
    if (root == "HKCU" || root == "HKEY_CURRENT_USER") return HKEY_CURRENT_USER;
    if (root == "HKLM" || root == "HKEY_LOCAL_MACHINE") return HKEY_LOCAL_MACHINE;
    if (root == "HKEY_USERS") return HKEY_USERS;
    if (root == "HKEY_PERFORMANCE_DATA") return HKEY_PERFORMANCE_DATA;
    if (root == "HKEY_PERFORMANCE_TEXT") return HKEY_PERFORMANCE_TEXT;
    if (root == "HKEY_PERFORMANCE_NLSTEXT") return HKEY_PERFORMANCE_NLSTEXT;
    if (root == "HKCC" || root == "HKEY_CURRENT_CONFIG") return HKEY_CURRENT_CONFIG;
    if (root == "HKEY_DYN_DATA") return HKEY_DYN_DATA;

    ansak::assert(false, "Root key was unrecognized.");
    return reinterpret_cast<HKEY>(0);
}

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

//===========================================================================
// getUserConfig -- Retrieve the user config from the registry

Config getUserConfig()
{
    enforce(!vendorContext.empty() && !productContext.empty(),
            "UserConfig: Provide a vendor and product context, please.");

    auto where(getUserRegistryPath());
    if (where.empty() || !regPathExist(where))
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
    if (where.empty() || !regPathExist(where))
    {
        return Config();
    }
    return Config(getAtomsFromRegLocation(where));
}

//===========================================================================
// saveUserConfig -- Store the user config to the registry

bool saveUserConfig(const Config& config)
{
    auto where(getUserRegistryPath());
    if (where.empty() || !regPathExist(where))
    {
        return false;
    }
    return writeConfigToRegLocation(where, config);
}

Config getConfig(const string& otherReg)
{
}

bool writeConfig(const string& dest, const Config& src)
{
}

}

#endif
