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
// config_io_file.cxx -- implements APIs that load and store configuration data to/from
//                       permanent storage in the file system.
//
///////////////////////////////////////////////////////////////////////////

#include <config_io.hxx>

#include <file_system_path.hxx>
#include <file_path.hxx>
#include <file_handle.hxx>
#include <file_handle_exception.hxx>
#include <runtime_exception.hxx>
#include <config_atom.hxx>

#include <config_paths.hxx>

#include <sstream>
#include <string>

namespace ansak
{

using namespace std;

namespace config
{

string vendorContext;
string productContext;

#if !defined(WIN32)

//===========================================================================
// getUserConfigFilePath -- look for $HOME/.vendor/applicationrc file, assert
// that ".vendor" is not there or it's a directory, that applicationrc is not
// there, or it's a file.

FilePath getUserConfigFilePath()
{
    const char* homeVar = getenv("HOME");
    enforce(homeVar != nullptr && *homeVar != '\0', "Must have good environment.");

    FilePath homeDir(homeVar);
    string vendorDir(".");
    vendorDir += vendorContext;
    auto userVendorPath = homeDir.child(vendorDir);

    string productRC(productContext);
    productRC += "rc";
    return userVendorPath.child(productRC);
}

//===========================================================================
// getSystemConfigFilePath -- look for /etc/vendor.conf/applicationrc file, assert
// that "/etc/vendor.conf" is not there or it's a directory, that applicationrc
// is not there, or it's a file.

FilePath getSystemConfigFilePath()
{
    FilePath etcDir("/etc");

    string vendorDir(vendorContext);
    vendorDir += ".conf";
    auto sysVendorPath = etcDir.child(vendorDir);

    string productRC(productContext);
    productRC += "rc";
    return sysVendorPath.child(productRC);
}

#endif

//===========================================================================
// getFileIfThere -- Wrap a file with FileOfLines for consumption by Config ctor

FileOfLines getFileIfThere(const FilePath& src)
{
    FileSystemPath fsSrc(src);
    if (!fsSrc.exists() || !fsSrc.isFile())
    {
        throw ConfigFileNotThere();
    }
    return FileOfLines(FileSystemPath(fsSrc));
}

}

using namespace ansak::config;

//===========================================================================
// setContext -- initialize a "vendor name" and a "product name" to determine
// user and system-wide defaults for that product as provided by that vendor.

void setContext(const string& vendor, const string& product)
{
    enforce(!vendor.empty() && !product.empty() && isUtf8(vendor) && isUtf8(product),
            "Provide some strings for vendor and product");
    vendorContext = vendor;
    productContext = product;
}

#if !defined(WIN32)

//===========================================================================
// getUserConfig -- Retrieve the user config from a file.

Config getUserConfig()
{
    try
    {
        enforce(!vendorContext.empty() && !productContext.empty(),
                "getUserConfig: Provide a vendor and product context, please.");
        if (!vendorContext.empty() && !productContext.empty())
        {
            return Config(getFileIfThere(getUserConfigFilePath()));
        }
    }
    catch (std::exception&)
    {
    }
    return Config();
}

//===========================================================================
// getSystemConfig -- Retrieve the system config from a file.

Config getSystemConfig()
{
    try
    {
        enforce(!vendorContext.empty() && !productContext.empty(),
                "getSystemConfig: Provide a vendor and product context, please.");
        if (!vendorContext.empty() && !productContext.empty())
        {
            return Config(getFileIfThere(getUserConfigFilePath()));
        }
    }
    catch (std::exception&)
    {
    }
    return Config();
}

//===========================================================================
// saveUserConfig -- Store the user config to a file.

bool saveUserConfig(const Config& src)
{
    enforce(!vendorContext.empty() && !productContext.empty(),
            "saveUserConfig: Provide a vendor and product context, please.");
    auto where(getUserConfigFilePath());
    enforce(where != FileSystemPath(FilePath::invalidPath()),
            "getUserCOnfigFilePath() MUST have given us a good path.");

    return writeConfig(where, src);
}

#endif

//===========================================================================
// getFileConfig -- Given a file path, get configuration data from it.

Config getConfig(const FilePath& source)
{
    try
    {
        return Config(getFileIfThere(source));
    }
    catch (std::exception&)
    {
    }
    return Config();
}

//===========================================================================
// writeConfig -- Given a file path, save the configuration set to
// the file

bool writeConfig(const FilePath& dest, const Config& src)
{
    // don't have a real path to save to
    if (dest == FilePath::invalidPath())
    {
        return false;
    }
    FileSystemPath fsDest(dest);
    if (fsDest.exists() && !fsDest.isFile())
    {
        return false;
    }

    FileSystemPath destParent(fsDest.parent());
    // don't have an existent parent directory to save to
    if (!destParent.exists() || !destParent.isDir())
    {
        return false;
    }

    bool updating = false;
    if (fsDest.exists() && fsDest.isFile())
    {
        updating = true;
    }

    // can't create temporary file
    auto tempDestName(dest.asUtf8String());
    tempDestName += "-new";
    FileSystemPath newDest(tempDestName);
    const FileSystemPath& writeToThisOne(updating ? newDest : fsDest);

    try
    {
        FileHandle updater(FileHandle::create(writeToThisOne));
        for (const auto& p : src.getValueNames())
        {
            auto atom = src.getAtom(p);
            stringstream o;
            o << p << '=' << atom.asString() << endl;
            auto l(o.str());

            updater.write(l.c_str(), l.size());
        }
    }
    catch (FileHandleException&)
    {
        return false;
    }

    if (updating)
    {
        auto destOldName(dest.asUtf8String());
        destOldName += "-old";
        FileSystemPath oldDest(destOldName);
        fsDest.moveTo(oldDest);
        newDest.moveTo(fsDest);
        oldDest.remove();
    }

    return true;
}

}
