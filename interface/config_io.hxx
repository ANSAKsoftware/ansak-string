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
// config_io.hxx -- defines APIs that load and store configuration data to/from
//                  permanent storage based on whatever is available, appropriate
//                  by default on the current platform.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "config.hxx"

#include <string>

namespace ansak
{

class FilePath;

// call this entrypoint first to generate automatic locations for system-
// and user preference settings
void setContext(const std::string& vendor, const std::string& product);

// platform independent APIs that have platform dependent implementations:
// on Windows, the registry; on Linux, a /etc/... and a ~/home/.* setting
Config getUserConfig();
Config getSystemConfig();
bool saveUserConfig(const Config& config);

// Let settings, be saved to and restored from a file regardless of platform
Config getConfig(const FilePath& src);
bool writeConfig(const FilePath& dest, const Config& src);

#if defined(WIN32)
// Let settings, be saved to and restored from the registry
Config getConfig(const std::string& otherReg);
bool writeConfig(const std::string& otherReg, const Config& src);
#endif

}
