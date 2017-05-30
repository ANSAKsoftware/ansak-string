///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015, Arthur N. Klassen
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
// 2015.12.24 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_path.hxx -- Class that wraps platform-independant file path
//                  construction and basic manipulation, entirely as string
//                  operations. Relative vs. Absolute paths are detected and
//                  noted. Trailing path separators are ignored.
//
//                  Child-of and Parent-of changes are handled as string
//                  manipulations. These options are not attempted on paths
//                  that are not considered valid.
//
//                  Parent-of operations on paths that are root paths return
//                  a FilePath object that has no path string, is not valid
//                  and not native to either platform.
//
//                  Parent-of operations on relative paths strip specified
//                  elements away until nothing is left, then append ".."
//                  elements on every operation (regardless of whether the
//                  resulting path points to a real location or not).
//
//                  Typically non-native paths are not considered valid. The
//                  only exception is that Linux accepts Windows UNC paths as
//                  valid, but not native. Non-native paths are not checked for
//                  whether they are relative or not.
//
//                  For Linux, paths with forward slashes are considered
//                  native. Paths that do not begin with a '/' are relative.
//
//                  For Windows, only path components without back-slash, slash,
//                  colon, asterisk, question mark, quotation mark, less than,
//                  greater than and vertical bar can be considered valid.
//                  These formats are considered native and absolute:
//                  * drive-letter + ":\"
//                  * drive-letter + ":\" + backslash-delimited path elements
//                  * "\\" + server + "\" + share + "\"
//                  * "\\" + server + "\" + share + "\" + backslash-delimited
//                    path elements
//                  * drive-letter + ":/" + forwardslash-delimited path elemnts
//                    (the '/' elements are changed to '\')
//                  These formats are considered native and relative (in one
//                  way or another) when they begin any other way (of course,
//                  forward-slash delimited paths like this are considered non-
//                  native on Windwos). The most obvious case would be bare
//                  backslash-delimited path elements but all cases that begin
//                  thus are considered relative.
//                  * backslash-delimited path elements
//                  * "\"
//                  * drive-letter + ':' (but no slash or backslash)
//                  * drive-letter + ":."
//                  * drive-letter + ":.."
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "string.hxx"
#include <vector>

namespace ansak {

///////////////////////////////////////////////////////////////////////////
// class FilePath -- mostly string operations but access to file-system-aware
//                   APIs around file path manipulations

class FilePath {

public:

    // A platform difference here so clients don't have to care. pathSep and foreignPathSep
    // are conditionally compiled appropriately
    static const char windowsPathSep;       //  = '\\'
    static const char linuxPathSep;         //  = '/'
    static const char pathSep;              // these two constants are selected
    static const char foreignPathSep;       //      at compile time from the first two

    ///////////////////////////////////////////////////////////////////////
    // Constructor -- stores it, if empty, stores current working directory
    //
    // Tests path for validity and kind of path but does not hit the file
    // system to test for existence; doesn't even normalize it. Checks for
    // relative-ness, root-ness, etc.

    FilePath
    (
        const utf8String& srcPath = utf8String()        // I - string form
    );
    FilePath(const FilePath& src) = default;
    FilePath& operator=(const FilePath& src) = default;

    ///////////////////////////////////////////////////////////////////////
    // Factory for an invalid path object
    static FilePath invalidPath();

    ///////////////////////////////////////////////////////////////////////
    // Return the parent directory or a sub-path

    FilePath parent() const;
    FilePath child(const std::string& subPath) const;
    FilePath rootPathFrom(const FilePath& cwd);

    FilePath dirname() const { return parent(); }
    utf8String basename() const;

    ///////////////////////////////////////////////////////////////////////
    // Return the underlying path string, as 8-bit or 16-bit wide string

    utf8String asUtf8String() const { return m_path; }
    utf16String asUtf16String() const { return toUtf16(m_path); }

    ///////////////////////////////////////////////////////////////////////
    // given the normalization that happens during construction of
    // FileSystemPath, string comparisions will do the right thing

    bool operator==(const FilePath& other) const { return m_path == other.m_path; }
    bool operator!=(const FilePath& other) const { return m_path != other.m_path; }
    bool operator<(const FilePath& other) const { return m_path < other.m_path; }
    bool operator<=(const FilePath& other) const { return m_path <= other.m_path; }
    bool operator>(const FilePath& other) const { return m_path > other.m_path; }
    bool operator>=(const FilePath& other) const { return m_path >= other.m_path; }

    ///////////////////////////////////////////////////////////////////////
    // Accessors for flags determined during construction:
    //     * is the path valid?
    //     * is it real (valid for the current platform)?
    //     * is it relative? (tends to be false)
    //     * is it a "root" ("/", "\\server\volume" or "c:\")
    //     * is it a Windows UNC path

    bool isValid() const { return m_isValid; }
    bool isReal() const { return m_isValid && m_isNative; }
    bool isRelative() const { return m_isValid && m_isRelative; }
    bool isRoot() const { return m_isRoot; }
    bool isUNCPath() const { return m_isUNC; }

private:

    using PathComponentsType = std::vector<utf8String>;

    ///////////////////////////////////////////////////////////////////////
    // Return a FilePath of this' parent using '..' with the right file separator

    FilePath relativeParentOfPath() const;

    ///////////////////////////////////////////////////////////////////////
    // Returns a FilePath of this' parent (when this is a UNC path), returning
    // invalidPath if the UNC path is already \\server\volume.

    FilePath parentOfUNC() const;

    ///////////////////////////////////////////////////////////////////////
    // Returns a FilePath from a set of components, honouring UNC construction
    // if necessary

    static FilePath fromFullComponents
    (
        const PathComponentsType&   components,     // I - split-ups components to build from
        bool                        fromUnc         // I - whether to start \\server\volume or not
    );

    ///////////////////////////////////////////////////////////////////////
    // Returns true if the components could be valid on Windows.
    // Characters invalid for Windows are back-slash, forward-slash, colon
    // asterisk, question mark, double quote, less than, greater than and
    // vertical bar

    bool componentsHaveNoInvalidWindowsCharacters() const;

    utf8String              m_path;                     // the string form of the path
    PathComponentsType      m_components;               // the parts of the path
    bool                    m_isRelative = false;       // is the path incomplete?
    bool                    m_isRoot = false;           // is path root? "/" for unix
                                                        //      C:\ for DOS, Windows
                                                        //      \\server\volume for Windows
    bool                    m_isNative = true;          // is the path for this platform?
    bool                    m_isValid = false;          // is path is valid?
    bool                    m_hasDriveColon = false;    // does path start with C: ?
    bool                    m_isUNC = false;            // is it a Windows Server/Volume path?
};

}
