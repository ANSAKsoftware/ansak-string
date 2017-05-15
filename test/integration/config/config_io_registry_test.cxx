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
// 2017.05.05 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config_io_registry_test.hxx -- unit tests for registry I/O of configuration
//
///////////////////////////////////////////////////////////////////////////

#if defined(WIN32)

#include <gtest/gtest.h>

#include <config_io.hxx>
#include <config_paths.hxx>
#include <config_atom.hxx>
#include <file_handle.hxx>
#include <file_path.hxx>
#include <file_system_path.hxx>
#include <windows_registry_key.hxx>
#include <iostream>

using namespace std;
using namespace testing;
using namespace ansak;
namespace ansak { namespace internal {

using ConfigAtomVector = vector<ConfigAtomPair>;
using ConfigAtomMap = map<string, ConfigAtom>;

extern ConfigAtomVector getAtomsFromRegLocation(const string& location);
extern ConfigAtomMap settingsToAtoms(const Config& setting);
extern bool saveAtomsToRegLocation(const string& location, const ConfigAtomMap& atoms);

} }
using namespace ansak::internal;
using namespace ansak::config;

class ConfigIORegistryFixture : public Test
{
    static const string regLocation;
    static const string parentLocation;
public:
    ConfigIORegistryFixture() : 
        m_theKey(WindowsRegKey::open(regLocation, WindowsRegKey::createIt))
    {
        clearContents();
    }

    ~ConfigIORegistryFixture()
    {
        try
        {
            m_theKey.deleteKey();
            WindowsRegKey::open(parentLocation).deleteKey();
        }
        catch (RegistryAccessException& e)
        {
            cout << "Caught RegistryAccessException during destruction: "
                 << e.what() << endl;
        }
    }

    WindowsRegKey& theKey() { return m_theKey; }
    static const string& theLocation() { return regLocation; }

    void clearContents()
    {
        for (const auto& name : m_theKey.getValueNames())
        {
            m_theKey.deleteValue(name);
        }
    }

private:
    WindowsRegKey       m_theKey;
};

const string ConfigIORegistryFixture::regLocation { "HKCU\\Software\\test-ansakkakkakk\\testProduct" };
const string ConfigIORegistryFixture::parentLocation { "HKCU\\Software\\test-ansakkakkakk" };


TEST_F(ConfigIORegistryFixture, testReadSettings)
{
    theKey().setValue("b", "true");
    theKey().setValue("i", 23u);
    theKey().setValue("f", "3.1415926535");
    theKey().setValue("d", "4.22333444455555");
    theKey().setValue("s", "Now is the time");
    theKey().setValue("pt", "3,4");
    theKey().setValue("r", "(100,60),(50,30)");

    auto atoms = getAtomsFromRegLocation(theLocation());
    unsigned done = 0;
    for (const auto& atom : atoms)
    {
        if (atom.first == "b")
        {
            EXPECT_TRUE(atom.second.isBool());
            EXPECT_TRUE(atom.second.asBool());
            done |= 1;
        }
        else if (atom.first == "i")
        {
            EXPECT_TRUE(atom.second.isInt());
            EXPECT_EQ(23, atom.second.asInt());
            done |= 2;
        }
        else if (atom.first == "f")
        {
            EXPECT_TRUE(atom.second.isFloat());
            EXPECT_TRUE(floatEqual(3.1415926535, atom.second.asFloat()));
            done |= 4;
        }
        else if (atom.first == "d")
        {
            EXPECT_TRUE(atom.second.isFloat());
            EXPECT_TRUE(floatEqual(4.22333444455555, atom.second.asFloat()));
            done |= 8;
        }
        else if (atom.first == "s")
        {
            EXPECT_TRUE(atom.second.isString());
            EXPECT_EQ("Now is the time", atom.second.asString());
            done |= 16;
        }
        else if (atom.first == "pt")
        {
            EXPECT_TRUE(atom.second.isPoint());
            EXPECT_EQ(toPoint(3,4), atom.second.asPoint());
            done |= 32;
        }
        else if (atom.first == "r")
        {
            EXPECT_TRUE(atom.second.isRect());
            EXPECT_EQ(toRect(100,60,50,30), atom.second.asRect());
            done |= 64;
        }
        else
        {
            ASSERT_FALSE(true);
        }
    }
    ASSERT_EQ(127, done);
}

TEST_F(ConfigIORegistryFixture, testWriteSettings)
{
    Config atoms;
    atoms.initValue("b", false);
    atoms.initValue("i", -23);
    atoms.initValue("f", 2.71828301);
    atoms.initValue("s", "I have a gumby cat in mind.");
    atoms.initValue("pt", toPoint(35,80));
    atoms.initValue("r", toRect(58,87,80,55));

    auto theMap = settingsToAtoms(atoms);
    EXPECT_TRUE(saveAtomsToRegLocation(theLocation(), theMap));

    string theStringVal;
    unsigned done = 0;
    for (const auto& name : theKey().getValueNames())
    {
        auto theType = theKey().getValueType(name);
        if (theType == REG_SZ)
        {
            EXPECT_TRUE(theKey().getValue(name, theStringVal));
        }
        if (name == "b")
        {
            EXPECT_EQ(REG_SZ, theType);
            EXPECT_EQ("false", theStringVal);
            done |= 1;
        }
        else if (name == "i")
        {
            EXPECT_EQ(REG_DWORD, theType);
            DWORD val;
            EXPECT_TRUE(theKey().getValue(name, val));
            EXPECT_EQ(static_cast<DWORD>(-23), val);
            done |= 2;
        }
        else if (name == "f")
        {
            EXPECT_EQ(REG_SZ, theType);
            EXPECT_EQ("2.71828", theStringVal.substr(0, 7));
            done |= 4;
        }
        else if (name == "s")
        {
            EXPECT_EQ(REG_SZ, theType);
            EXPECT_EQ("I have a gumby cat in mind.", theStringVal);
            done |= 8;
        }
        else if (name == "pt")
        {
            EXPECT_EQ(REG_SZ, theType);
            EXPECT_EQ("35,80", theStringVal);
            done |= 16;
        }
        else if (name == "r")
        {
            EXPECT_EQ(REG_SZ, theType);
            EXPECT_EQ("(58,87),(80,55)", theStringVal);
            done |= 32;
        }
        else
        {
            ASSERT_TRUE(false);
        }
    }
    ASSERT_EQ(done, 63u);
}
#endif
