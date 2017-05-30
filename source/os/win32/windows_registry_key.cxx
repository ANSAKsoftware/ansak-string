///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, Arthur N. Klassen
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
// 2017.05.06 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// windows_registry_key.cxx -- implementation of a class that wraps Windows
//                             Registry access
//
///////////////////////////////////////////////////////////////////////////

#if defined(WIN32)

#include <windows_registry_key.hxx>
#include <string_splitjoin.hxx>
#include <runtime_exception.hxx>
#include <operating_system_primitives.hxx>

using namespace std;

namespace ansak
{

namespace config
{

//===========================================================================
// public, static

bool WindowsRegKey::exists
(
    const utf8String&           path        // I - a registry path to check
)
{
    try
    {
        open(path);
    }
    catch (RegistryAccessException&)
    {
        return false;
    }
    return true;
}

//===========================================================================
// public, static

WindowsRegKey WindowsRegKey::open
(
    const utf8String&   path,           // I - a registry path to open
    bool                createIfAbsent  // I - to open even if not there, def false
)
{
    auto components(split(path, '\\'));
    HKEY theRootKey =  getRootKey(components[0]);

    components.erase(components.begin(), components.begin() + 1);
    auto subPath = join(components, '\\');
    auto subPath16 = toUtf16(subPath);
    HKEY theKey = nullptr;
    auto rv = RegOpenKeyExW(theRootKey, reinterpret_cast<LPCWSTR>(subPath16.c_str()),
                            0, KEY_READ, &theKey);

    if (rv != ERROR_SUCCESS)
    {
        if (!createIfAbsent)
        {
            throw RegistryAccessException(rv);
        }
        // does the root + 1 key exist? Open it
        auto recomponents(split(path, '\\'));
        recomponents.resize(2);
        auto subRootPath = join(recomponents, '\\');
        auto subRootKey = open(subRootPath);

        theKey = create(subRootPath, subRootKey.m_theKey, path);
        if (theKey == 0)
        {
            throw RegistryAccessException();
        }
    }
    return WindowsRegKey(theRootKey, subPath16, theKey);
}

//===========================================================================
// public, destructor

WindowsRegKey::~WindowsRegKey()
{
    // Don't close empty keys or magic OS provided ones
    if (((reinterpret_cast<unsigned long long>(m_theKey) & 0xFFFFFFFF) << 1) != 0 &&
        ((reinterpret_cast<unsigned long long>(m_theKey) & 0xFFFFFFFF) >> 31) == 0)
    {
        RegCloseKey(m_theKey);
    }
}

//===========================================================================
// public

WindowsRegKey::ValueNamesType WindowsRegKey::getValueNames() const
{
    enforce(*this != WindowsRegKey(), "Must have open key to get value names");
    bool stillWorking = true;
    DWORD valueIndex = 0;
    size_t bufferSize = 66;
    size_t wcharSize = bufferSize / sizeof(wchar_t);
    unique_ptr<wchar_t> valueName;
    wchar_t defaultBuffer[33];
    wchar_t* bufferP = &defaultBuffer[0];
    ValueNamesType result;
    for (; stillWorking; ++valueIndex)
    {
        DWORD valueSize = static_cast<DWORD>(wcharSize - 1);
        auto rv = RegEnumValueW(m_theKey, valueIndex, bufferP, &valueSize,
                                nullptr, nullptr, nullptr, nullptr);
        if (rv == ERROR_MORE_DATA)
        {
            if (wcharSize > 8193)
            {
                throw RegistryAccessException(rv);
            }
            bufferSize = (bufferSize - 2) * 2 + 2;
            wcharSize = bufferSize / sizeof(wchar_t);
            valueName.reset(new wchar_t[wcharSize]);
            bufferP = valueName.get();
            --valueIndex;
        }
        else if (rv == ERROR_NO_MORE_ITEMS)
        {
            break;
        }
        else if (rv == ERROR_SUCCESS)
        {
            bufferP[wcharSize - 1] = L'\0';
            result.push_back(toUtf8(reinterpret_cast<const char16_t*>(bufferP)));
        }
        else
        {
            throw RegistryAccessException(rv);
        }
    }

    return result;
}

//===========================================================================
// public

DWORD WindowsRegKey::getValueType
(
    const utf8String&   valueName       // I - a member value to check
) const
{
    enforce(*this != WindowsRegKey(), "Must have open key to get value type");
    return getValueType(toUtf16(valueName));
}

//===========================================================================
// public

bool WindowsRegKey::getValue
(
    const utf8String&   valueName,      // I - a member value to retrieve
    DWORD&              value           // O - where to store its value
) const
{
    enforce(*this != WindowsRegKey(), "Must have open key to get DWORD value.");
    utf16String nameIn16bit(toUtf16(valueName));
    if (getValueType(valueName) == REG_DWORD)
    {
        DWORD fetchValue = 0;
        DWORD fetchSize = sizeof(fetchValue);
        auto rv = RegQueryValueExW(m_theKey, reinterpret_cast<LPCWSTR>(nameIn16bit.c_str()),
                                   nullptr, nullptr, reinterpret_cast<LPBYTE>(&fetchValue), &fetchSize);
        if (rv == ERROR_SUCCESS)
        {
            value = fetchValue;
            return true;
        }
    }
    return false;
}

//===========================================================================
// public

bool WindowsRegKey::getValue
(
    const utf8String&   valueName,      // I - a member value to retrieve
    utf8String&         value           // O - where to store its value
) const
{
    enforce(*this != WindowsRegKey(), "Must have open key to get string value.");
    utf16String nameIn16bit(toUtf16(valueName));
    if (getValueType(valueName) == REG_SZ)
    {
        DWORD fetchSize = 0u;
        RegQueryValueExW(m_theKey, reinterpret_cast<LPCWSTR>(nameIn16bit.c_str()), nullptr,
                         nullptr, nullptr, &fetchSize);
        auto wcharSize = fetchSize / sizeof(wchar_t);
        unique_ptr<wchar_t> wideValue(new wchar_t[wcharSize]);
        auto rv = RegQueryValueExW(m_theKey, reinterpret_cast<LPCWSTR>(nameIn16bit.c_str()),
                                   nullptr, nullptr, reinterpret_cast<LPBYTE>(wideValue.get()),
                                   &fetchSize);
        wideValue.get()[wcharSize - 1] = L'\0';
        auto narrowValue = toUtf8(reinterpret_cast<const char16_t*>(wideValue.get()));
        if (!narrowValue.empty())
        {
            value = narrowValue;
            return true;
        }
    }
    return false;
}

//===========================================================================
// public

bool WindowsRegKey::setValue
(
    const utf8String&   valueName,      // I - a member value to set
    DWORD               value           // I - its new value
)
{
    enforce(*this != WindowsRegKey(), "Must have open key to set DWORD value.");
    auto wideName = toUtf16(valueName);
    auto currentType = getValueType(wideName);
    if ((currentType != REG_DWORD && currentType != REG_NONE ) ||
        !reopenWritable())
    {
        return false;
    }

    auto rv = RegSetValueExW(m_theKey, reinterpret_cast<LPCWSTR>(wideName.c_str()),
                             0, REG_DWORD, reinterpret_cast<const BYTE*>(&value),
                             sizeof(DWORD));
    return rv == ERROR_SUCCESS;
}

//===========================================================================
// public

bool WindowsRegKey::setValue
(
    const utf8String&   valueName,      // I - a member value to set
    const utf8String&   value           // O - its new value
)
{
    enforce(*this != WindowsRegKey(), "Must have open key to set string value.");
    auto wideName = toUtf16(valueName);
    auto currentType = getValueType(wideName);
    if ((currentType != REG_SZ && currentType != REG_NONE ) ||
        !reopenWritable())
    {
        return false;
    }

    auto wideValue = toUtf16(value);
    auto rv = RegSetValueExW(m_theKey, reinterpret_cast<LPCWSTR>(wideName.c_str()),
                             0, REG_SZ, reinterpret_cast<const BYTE*>(wideValue.c_str()),
                             static_cast<DWORD>(value.size() + 1) * sizeof(wchar_t));
    return rv == ERROR_SUCCESS;
}

//===========================================================================
// public

void WindowsRegKey::deleteValue
(
    const utf8String&   valueName       // I - a member value to delete
)
{
    enforce(*this != WindowsRegKey(), "Must have open key to delete value.");
    if (reopenWritable())
    {
        auto wideName = toUtf16(valueName);
        RegDeleteValueW(m_theKey, reinterpret_cast<LPCWSTR>(wideName.c_str()));
    }
}

//===========================================================================
// public

WindowsRegKey WindowsRegKey::createSubKey
(
    const utf8String&   component       // I - a new sub-key to create
)
{
    enforce(*this != WindowsRegKey(), "Must have open key to create sub key.");
    enforce(!component.empty());
    if (!reopenWritable())
    {
        throw RegistryAccessException(GetLastError());
    }

    HKEY newSub = 0;
    utf16String component16 = toUtf16(component);
    auto rv = RegCreateKeyExW(m_theKey, reinterpret_cast<LPCWSTR>(component16.c_str()),
                              0, nullptr, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE,
                              nullptr, &newSub, nullptr);
    if (rv != ERROR_SUCCESS)
    {
        throw RegistryAccessException(rv);
    }
    auto newSubValue = m_subKeyString;
    newSubValue += L'\\';
    newSubValue += component16;
    WindowsRegKey result(m_rootKey, newSubValue, newSub);
    result.m_writable = true;
    return result;
}

//===========================================================================
// public

void WindowsRegKey::deleteKey()
{
    if (*this != WindowsRegKey())
    {
        RegCloseKey(m_theKey);
        m_theKey = 0;
        auto rv = RegDeleteKeyExW(m_rootKey, reinterpret_cast<LPCWSTR>(m_subKeyString.c_str()), 0, 0);
        if (rv != ERROR_SUCCESS)
        {
            throw RegistryAccessException(rv);
        }
    }
}

//===========================================================================
// public

utf8String WindowsRegKey::getPath() const
{
    if (*this == WindowsRegKey())
    {
        return utf8String();
    }

    vector<utf8String> combine(1, getRootPath());
    combine.push_back(toUtf8(m_subKeyString));
    return join(combine, '\\');
}

//===========================================================================
// public

bool WindowsRegKey::operator==(const WindowsRegKey& rhs) const
{
    // only the external parts are worth checking; if one key has been
    // reopened for writing and the other not, they're still "the same"
    // as far as the public interface is concerned.
    return m_rootKey == rhs.m_rootKey &&
           m_subKeyString == rhs.m_subKeyString;
}

//===========================================================================
// private, constructor

WindowsRegKey::WindowsRegKey
(
    HKEY                rootKey,        // I - the Windows Root Key
    const utf16String&  subKey,         // I - the path from there to this key
    HKEY                newKey          // I - the value to wrap
) : m_rootKey( rootKey ),
    m_subKeyString( subKey ),
    m_theKey( newKey )
{
    enforce(*this != WindowsRegKey(), "Must not explictly create empty WindowsRegKey value.");
}

//===========================================================================
// private

bool WindowsRegKey::reopenWritable()
{
    if (!m_reopenTried)
    {
        HKEY theKey = nullptr;
        auto rv = RegOpenKeyExW(m_rootKey, reinterpret_cast<LPCWSTR>(m_subKeyString.c_str()),
                                0, KEY_READ | KEY_WRITE, &theKey);
        if (rv == ERROR_SUCCESS)
        {
            RegCloseKey(m_theKey);
            m_theKey = theKey;
            m_writable = true;
        }
        m_reopenTried = true;
    }
    return m_writable;
}

//===========================================================================
// private

DWORD WindowsRegKey::getValueType
(
    const utf16String&  valueName       // I - a member value to check
) const
{
    DWORD regType;
    auto rv = RegQueryValueExW(m_theKey, reinterpret_cast<LPCWSTR>(valueName.c_str()), nullptr,
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

//===========================================================================
// private, static

HKEY WindowsRegKey::getRootKey
(
    const utf8String&   root        // I - the root value to convert
)
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

    enforce(false, "Root key was unrecognized.");
    return reinterpret_cast<HKEY>(0);
}

//===========================================================================
// private

utf8String WindowsRegKey::getRootPath() const
{
    if      (m_rootKey == HKEY_CLASSES_ROOT)        return "HKEY_CLASSES_ROOT";
    else if (m_rootKey == HKEY_CURRENT_USER)        return "HKEY_CURRENT_USER";
    else if (m_rootKey == HKEY_LOCAL_MACHINE)       return "HKEY_LOCAL_MACHINE";
    else if (m_rootKey == HKEY_USERS)               return "HKEY_USERS";
    else if (m_rootKey == HKEY_PERFORMANCE_DATA)    return "HKEY_PERFORMANCE_DATA";
    else if (m_rootKey == HKEY_PERFORMANCE_TEXT)    return "HKEY_PERFORMANCE_TEXT";
    else if (m_rootKey == HKEY_PERFORMANCE_NLSTEXT) return "HKEY_PERFORMANCE_NLSTEXT";
    else if (m_rootKey == HKEY_CURRENT_CONFIG)      return "HKEY_CURRENT_CONFIG";
    else if (m_rootKey == HKEY_DYN_DATA)            return "HKEY_DYN_DATA";

    return utf8String();
}

//===========================================================================
// private, static

HKEY WindowsRegKey::create
(
    const utf8String&   subRoot,        // I - path from which this create is happening
    HKEY                subRootKey,     // I - key to the path from which this create is happening
    const utf8String&   path            // I - the path to create
)
{
    if (subRoot == path)
    {
        return subRootKey;
    }

    auto subRootParts = split(subRoot, '\\');
    auto pathParts = split(path, '\\');
    HKEY createFromHere = subRootKey;
    if (pathParts.size() > subRootParts.size() + 1)
    {
        auto recurseParts = pathParts;
        recurseParts.pop_back();
        createFromHere = create(subRoot, subRootKey, join(recurseParts, '\\'));
        if (createFromHere == 0)
        {
            return 0;
        }
    }
    HKEY subKey = 0;
    utf16String backPart16 = toUtf16(pathParts.back());
    auto rv = RegCreateKeyExW(createFromHere, reinterpret_cast<LPCWSTR>(backPart16.c_str()), 0,
                              nullptr, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE,
                              nullptr, &subKey, nullptr);
    if (createFromHere != subRootKey)
    {
        RegCloseKey(createFromHere);
    }
    return rv == ERROR_SUCCESS ? subKey : 0;
}

//===========================================================================
// public, constructor

RegistryAccessException::RegistryAccessException
(
    DWORD               errorCode
) noexcept
  : exception(),
    m_code(errorCode)
{
    try
    {
        m_what = getOperatingSystemPrimitives()->errorAsString(errorCode);
    }
    catch (...)
    {
    }
}

//===========================================================================
// public, destructor

RegistryAccessException::~RegistryAccessException() noexcept
{
}

//===========================================================================
// public, virtual

const char* RegistryAccessException::what() const noexcept
{
    try
    {
        if (!m_what.empty())
        {
            return m_what.c_str();
        }
    }
    catch (...)
    {
        static const char exceptedWhat[] = "RegistryAccessException - throw on what()";
        return exceptedWhat;
    }
    static const char* emptyWhat = "RegistryAccessException - no info available.";
    return emptyWhat;
}

}

}
#endif
