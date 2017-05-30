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
// windows_registry_key.hxx -- a class that wraps Windows Registry access
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(WIN32)

#include <string.hxx>

#include <memory>
#include <vector>
#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ansak
{

namespace config
{

/////////////////////////////////////////////////////////////////////////////
// std::exception-derived exception when accessing the Windows Registry hasn't
// gone as expected

class RegistryAccessException : public std::exception
{
public:
    RegistryAccessException(DWORD errorCode = 0u) noexcept;
    virtual ~RegistryAccessException() noexcept override;
    virtual const char* what() const noexcept override;
    unsigned int whatCode() const noexcept { return m_code; }

private:
    std::string         m_what = std::string();
    DWORD               m_code;
};

/////////////////////////////////////////////////////////////////////////////
// RAII and a bit more brains wrapping the Win32 HKEY reference into the
// registry. Check for a key's existence, open it, by creating if neceessary.
//
// Handles only the local registry, only sets and retrieves REG_DWORD and
// REG_SZ (all you need for a map to a Unix rc settings file. Paths that do
// not start wtih a usual "HKEY_.*\>" or HK.." string constant (see windows.h)
// will fail.
//
// All access into the registry uses WCHAR (char16_t) but parameters are
// received and data sent back as UTF-8.

class WindowsRegKey {
public:

    using ValueNamesType = std::vector<utf8String>;     // a convenience type
    static const bool createIt = true;

    //=======================================================================
    // exists -- returns true if a Registry path exists, false otherwise

    static bool exists
    (
        const utf8String&           path        // I - a registry path to check
    );

    //=======================================================================
    // open -- Factory method that opens a Registry path and returns its
    // wrapper. If absent and creation is not asked for, or it is asked for
    // and creation fails, throws RegistryAccessException.
    //
    // In the case of a multi-part create where the first part succeeds and
    // later ones don't (inconceivable, but...), HKEY's will not leak but the
    // partially created path will remain in the registry.

    static WindowsRegKey open
    (
        const utf8String&   path,                   // I - a registry path to open
        bool                createIfAbsent = false  // I - to open even if not there
    );

    //=======================================================================
    // open -- Factory method that opens a Registry path and returns its
    // wrapper. If absent and creation is not asked for, or it is asked for
    // and creation fails, throws RegistryAccessException.

    ~WindowsRegKey();

    //=======================================================================
    // getValueNames - returns the value names stored in the current key

    ValueNamesType getValueNames() const;

    //=======================================================================
    // getValueType - returns the value type of a name, if it exists, REG_NONE
    // otherwise

    DWORD getValueType
    (
        const utf8String&   valueName       // I - a member value to check
    ) const;

    //=======================================================================
    // getValue - DWORD and string versions. Returns true on success, passed
    // in reference only changes if a value was successfully retreived.

    bool getValue
    (
        const utf8String&   valueName,      // I - a member value to retrieve
        DWORD&              value           // O - where to store its value
    ) const;

    bool getValue
    (
        const utf8String&   valueName,      // I - a member value to retrieve
        utf8String&         value           // O - where to store its value
    ) const;

    //=======================================================================
    // setValue - DWORD and string versions. Returns true on success.

    bool setValue
    (
        const utf8String&   valueName,      // I - a member value to set
        DWORD               value           // I - its new value
    );

    bool setValue
    (
        const utf8String&   valueName,      // I - a member value to set
        const utf8String&   value           // O - its new value
    );

    //=======================================================================
    // deleteValue - Remove one value from the current key

    void deleteValue
    (
        const utf8String&   valueName       // I - a member value to delete
    );

    //=======================================================================
    // createSubKey - Create a sub-key from the current on, return a key to
    // it. Throws RegistryAccessException if the operation fails for any
    // reason.

    WindowsRegKey createSubKey
    (
        const utf8String&   component       // I - a new sub-key to create
    );

    //=======================================================================
    // deleteKey - Closes and deletes the current key.

    void deleteKey();

    //=======================================================================
    // getPath - Returns path that can be used to open the current key.

    utf8String getPath() const;

    bool operator==(const WindowsRegKey& rhs) const;
    bool operator!=(const WindowsRegKey& rhs) const { return ! operator==(rhs); }

private:

    //=======================================================================
    // default constructor - non-open key

    WindowsRegKey() : m_rootKey(0), m_subKeyString(), m_theKey(0) {}

    //=======================================================================
    // full constructor

    WindowsRegKey
    (
        HKEY                rootKey,        // I - the Windows Root Key
        const utf16String&  subKey,         // I - the path from there to this key
        HKEY                newKey          // I - the value to wrap
    );

    //=======================================================================
    // reopenWritable - Before a set or createSub operation, make sure that
    // the current program has the necessary provision.
    //
    // Only does the reopen once, after that returns the status of m_writable,
    // set true on success.

    bool reopenWritable();

    //=======================================================================
    // getValueType - the private version, uses WCHAR string; returns the value
    // type of a name, if it exists, REG_NONE otherwise

    DWORD getValueType
    (
        const utf16String&  valueName       // I - a member value to check
    ) const;

    //=======================================================================
    // getRootKey - converts one of the standard Windows strings for root
    // keys in the registry to the associated HKEY-typed constant used by
    // registry APIs. Throws RuntimeException if the string is unrecognized.

    static HKEY getRootKey
    (
        const utf8String&   root        // I - the root value to convert
    );

    //=======================================================================
    // getRootPath - converts m_rootKey to its corresponding Windows string.

    utf8String getRootPath() const;

    //=======================================================================
    // create - Creates a sub-path, recursively if necessary returning an HKEY
    // to the resulting key created (intermediate HKEYs do not leak).

    static HKEY create
    (
        const utf8String&   subRoot,        // I - path from which this create is happening
        HKEY                subRootKey,     // I - key to the path from which this create is happening
        const utf8String&   path            // I - the path to create
    );

    bool                m_reopenTried = false;  // reopenWritable called, success or not
    bool                m_writable = false;     // reopenWritable called, succeeded
    HKEY                m_rootKey;              // a Windows magic value
    utf16String         m_subKeyString;         // the string used for sub-key
    HKEY                m_theKey;               // the HKEY being wrapped
};

}

}

#endif


