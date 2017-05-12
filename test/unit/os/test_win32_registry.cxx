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
// test_win32_registry.cxx -- unit tests for registry I/O of configuration
// 
///////////////////////////////////////////////////////////////////////////

#if defined(WIN32)


#include <gtest/gtest.h>
#include <windows_registry_key.hxx>
#include <runtime_exception.hxx>
#include <string.hxx>
#include <sstream>

using namespace ansak;
using namespace ansak::config;
using namespace std;
using namespace testing;

namespace
{

utf8String vendorPath = "HKEY_CURRENT_USER\\Software\\test-ansakkakkakk";
utf8String productSub = "testKey";
utf8String fullPath = vendorPath + '\\' + productSub;

}

class TestRegistryValue : public Test {
public:
    TestRegistryValue()
      : m_theKey(WindowsRegKey::open(fullPath, true))
    {
    }

    ~TestRegistryValue()
    {
        m_theKey.deleteKey();
        WindowsRegKey::open(vendorPath).deleteKey();
    }

    WindowsRegKey& uut() { return m_theKey; }
private:
    WindowsRegKey   m_theKey;
};

TEST(Registry, open)
{
    EXPECT_THROW(WindowsRegKey::open("ILDS\\Twiddling\\Thumbs"), RuntimeException);
    try
    {
        WindowsRegKey::open("HKLM\\SOFTWARE\\Microsoft\\Blob-Blob-Blob");
    }
    catch (RegistryAccessException& e)
    {
        string message = e.what();
        EXPECT_FALSE(message.empty());
    }
    {
        auto k = WindowsRegKey::open("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion");
        auto l = WindowsRegKey::open("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion");
    }
}

TEST(Registry, enumKeys)
{
    auto k = WindowsRegKey::open("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion");
    auto names = k.getValueNames();
    EXPECT_FALSE(names.empty());
}

TEST(Registry, getValueType)
{
    auto k = WindowsRegKey::open("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\"
                                 "Explorer");
    auto names = k.getValueNames();
    bool gotDword = false;
    bool gotString = false;
    for( const auto& name : names )
    {
        auto t = k.getValueType(name);
        gotDword |= t == REG_DWORD;
        gotString |= t == REG_SZ;
    }
    EXPECT_TRUE(gotDword);
    EXPECT_TRUE(gotString);
}

TEST(Registry, getDwordValues)
{
    auto k = WindowsRegKey::open("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\"
        "Explorer");
    auto names = k.getValueNames();
    for (const auto& name : names)
    {
        if (k.getValueType(name) == REG_DWORD)
        {
            DWORD value = ~0u;
            EXPECT_TRUE(k.getValue(name, value));
            EXPECT_NE(~0, value);
        }
    }
}

TEST(Registry, getStringValues)
{
    auto k = WindowsRegKey::open("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\"
                                 "Explorer");
    auto names = k.getValueNames();
    for( const auto& name : names )
    {
        if (k.getValueType(name) == REG_SZ)
        {
            utf8String value;
            EXPECT_TRUE(k.getValue(name, value));
            EXPECT_FALSE(value.empty());
        }
    }
}

TEST(Registry, userSettingsKeyHandling)
{
    EXPECT_FALSE(WindowsRegKey::exists(fullPath));
    {
        auto userRoot = WindowsRegKey::open(fullPath, true);
    }
    EXPECT_TRUE(WindowsRegKey::exists(fullPath));
    {
        auto userRoot(WindowsRegKey::open(fullPath));
        userRoot.deleteKey();
    }
    {
        auto vendorRoot(WindowsRegKey::open(vendorPath));
        vendorRoot.deleteKey();
    }
    EXPECT_FALSE(WindowsRegKey::exists(fullPath));
    EXPECT_FALSE(WindowsRegKey::exists(vendorPath));
}

TEST_F(TestRegistryValue, setDwordValue)
{
    EXPECT_TRUE(uut().setValue("foo", 2390u));

    EXPECT_EQ(REG_DWORD, uut().getValueType("foo"));

    DWORD someReturn = 0;
    EXPECT_TRUE(uut().getValue("foo", someReturn));
    EXPECT_EQ(2390u, someReturn);
}

TEST_F(TestRegistryValue, setStringValue)
{
    EXPECT_TRUE(uut().setValue("meow", "RumTumTugger"));

    EXPECT_EQ(REG_SZ, uut().getValueType("meow"));

    utf8String someReturn;
    EXPECT_TRUE(uut().getValue("meow", someReturn));
    EXPECT_EQ("RumTumTugger", someReturn);
}

TEST_F(TestRegistryValue, deleteValue)
{
    uut().setValue("castor", "pollux");
    auto names = uut().getValueNames();
    EXPECT_FALSE(names.empty());
    EXPECT_EQ(1u, names.size());
    EXPECT_EQ("castor", names[0]);

    uut().deleteValue("pollux");
    EXPECT_EQ(1u, uut().getValueNames().size());
    uut().deleteValue("castor");
    EXPECT_TRUE(uut().getValueNames().empty());
}

TEST_F(TestRegistryValue, getNotThere)
{
    DWORD notHere = ~0u;
    utf8String notThere("squeamish ossifrage");

    EXPECT_FALSE(uut().getValue("TheManWho", notHere));
    EXPECT_FALSE(uut().getValue("WasntThere", notThere));

    EXPECT_EQ(~0u, notHere);
    EXPECT_EQ("squeamish ossifrage", notThere);
}

TEST_F(TestRegistryValue, getOtherKind)
{
    uut().setValue("hair", "raising");
    uut().setValue("one", 1u);

    DWORD notThis = ~0u;
    utf8String notThat("squeamish ossifrage");

    EXPECT_FALSE(uut().getValue("hair", notThis));
    EXPECT_FALSE(uut().getValue("one", notThat));

    EXPECT_EQ(~0u, notThis);
    EXPECT_EQ("squeamish ossifrage", notThat);
}

#endif
