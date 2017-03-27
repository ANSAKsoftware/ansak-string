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
// 2015.12.25 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// file_path.cxx -- Class that wraps platform-independant file path
//                  construction and basic manipulation, entirely as string
//                  operations.
//
//                  (see file_path.hxx for a full explanation)
//
///////////////////////////////////////////////////////////////////////////

#include <file_path.hxx>
#include <string_splitjoin.hxx>

#include <algorithm>
#include <sstream>

using namespace std;

// local routines

namespace {

using namespace ansak;

#if defined(WIN32)

//==========================================================================
// hasDriveColon - returns true for a string that starts with a latin letter
//                 and a colon
bool hasDriveColon
(
    const utf8String&   path        // I - a path string to test
)
{
    return (path.size() >= 2) &&
           path[1] == ':' &&
           ((path[0] >= 'A' && path[0] <= 'Z') ||
            (path[0] >= 'a' && path[0] <= 'z'));
}

//==========================================================================
// isWindowsDriveRoot - returns true for a string that starts with a latin
//                      letter, a colon and a forward- or back-slash
bool isWindowsDriveRoot
(
    const utf8String&   path        // I - a path string to test
)
{
    return (path.size() >= 3) && hasDriveColon(path) &&
           (path[2] == FilePath::windowsPathSep || path[2] == FilePath::linuxPathSep);
}

#endif

//==========================================================================
// isWindowsUNCRoot - returns true for a string that starts with two back-
//                    slashes, a valid path component, another backslash and
//                    a second valid path component.
bool isWindowsUNCRoot
(
    const utf8String&   path        // I - a path string to test
)
{
    if (path.size() < 5 || path[0] != FilePath::windowsPathSep || path[1] != FilePath::windowsPathSep)
    {
        return false;
    }
    auto thirdBackSlash = find(path.begin() + 2, path.end(), FilePath::windowsPathSep);
    return !(thirdBackSlash == path.end() && (thirdBackSlash + 1) == path.end());
}

const string charsInvalidForWindowsPath = { "\\/:*?\"<>|" };

}

namespace ansak {

const char FilePath::windowsPathSep = '\\';
const char FilePath::linuxPathSep = '/';

#if defined(WIN32)
const char FilePath::pathSep = windowsPathSep;
const char FilePath::foreignPathSep = linuxPathSep;
#else
const char FilePath::pathSep = linuxPathSep;
const char FilePath::foreignPathSep = windowsPathSep;
#endif

//==========================================================================
// public, constructor

FilePath::FilePath
(
    const utf8String& srcPath        // I - string form, def empty
) : m_path(srcPath),
    m_components(split(srcPath, pathSep))
{
    if (m_path.empty())
    {
        m_path = ".";
        m_components.push_back(m_path);
    }
    if (isWindowsUNCRoot(m_path))
    {
        m_isUNC = true;
#if !defined(WIN32)
        auto otherComponents = split(m_path, foreignPathSep);
        m_components.swap(otherComponents);
        m_isNative = false;
#endif
        // collapse first few elements into UNC root
        m_components.erase(m_components.begin(), m_components.begin() + 2);
        m_isValid = componentsHaveNoInvalidWindowsCharacters();
        m_isRoot = m_components.size() == 2;
    }
    else
    {
        // if we have the wrong kind of path separators
        if (m_components.size() <= 1)
        {
            auto otherComponents = split(m_path, foreignPathSep);
            if (otherComponents.size() > 1)
            {
                m_components.swap(otherComponents);
                m_isNative = false;
            }
        }
        // if we have a native type of path
        if (m_isNative)
        {
            // get rid of trailing empty component
            if (!m_components.empty() && m_components.rbegin()->empty() && !m_path.empty())
            {
                // for multi-part paths, get rid of last empty component
                m_components.resize(m_components.size() - 1);
            }
#if defined(WIN32) // && !defined(CYGWIN) ??
            m_hasDriveColon = hasDriveColon(m_path);
            auto hasDriveColon = isWindowsDriveRoot(m_path);
            m_isRelative = m_path.empty() || (!hasDriveColon && !isWindowsUNCRoot(m_path));
            m_isValid = componentsHaveNoInvalidWindowsCharacters();
            m_isRoot = m_path.size() == 3 && !m_isRelative;
#else // if defined(WIN32)
            m_isRelative = m_path.empty() || !m_components.begin()->empty();
            m_isRoot = (m_path.size() == 1) && !m_isRelative;
            // If this is a root path, the zero-length first element is no longer needed
            if (!m_isRelative)
            {
                m_components.erase(m_components.begin());
            }
            m_isValid = true;
#endif // if defined(WIN32) else
        }
#if defined(WIN32)
        else
        {
            // for forward-slash delineated paths that may be valid (as for VIM and CMake)
            const auto& firstComponent = *(m_components.begin());
            if (firstComponent.size() == 2 && firstComponent[1] == ':' &&
                    ((firstComponent[0] >= 'A' && firstComponent[0] <= 'Z') ||
                     (firstComponent[0] >= 'a' && firstComponent[0] <= 'z')))
            {
                m_isNative = true;
                m_path = join(m_components, pathSep);
                m_isValid = componentsHaveNoInvalidWindowsCharacters();
            }
        }
#endif // if defined(WIN32)
    }
}

//==========================================================================
// public -- factory method for an invalid FilePath

FilePath FilePath::invalidPath()
{
    FilePath parentOfRoot;
    parentOfRoot.m_isRelative = parentOfRoot.m_isRoot =
        parentOfRoot.m_isNative = parentOfRoot.m_isValid =
        parentOfRoot.m_isUNC = false;
    return parentOfRoot;
}

//==========================================================================
// public

FilePath FilePath::parent() const
{
    if (!isValid())
    {
        return invalidPath();
    }

    switch(m_path.size())
    {
    // case 0:     return FilePath(".."); empty-string paths become "."
    case 1:
        if (m_path[0] == '.')
        {
            return FilePath("..");
        }
        else if (m_path[0] == pathSep || m_path[0] == foreignPathSep)
        {
            return invalidPath();
        }
        else
        {
            return FilePath(".");
        }

    case 2:
        if (m_path[0] == '.' && m_path[1] == '.')
        {
            return relativeParentOfPath();
        }
        else if (m_path[0] == pathSep || m_path[0] == foreignPathSep)
        {
            return FilePath(string(1, m_path[0]));
        }
        break;

#if defined(WIN32)
    case 3:
        if (!m_isRelative && !m_isUNC)
        {
            return invalidPath();
        }
        break;
#endif

    default:
        break;
    }

    if (*m_components.rbegin() == "..")
    {
        return relativeParentOfPath();
    }
    else if (m_isUNC)
    {
        return parentOfUNC();
    }
#if defined(WIN32)
    else if (!m_isRelative && m_components.size() == 2)
    {
        return FilePath(m_components[0] + pathSep);
    }
    else if (m_hasDriveColon && m_isRelative && m_components.size() == 1)
    {
        // handle parent-of-drive-plus-relative-path, 2 cases
        switch (m_path.size())
        {
        case 2: // "C:" case --> "C:.."
            return FilePath(m_path + "..");

        case 3: // make sure of "C:." case --> "C:.."
            if (m_path[2] == '.')
            {
                return FilePath(m_path + ".");
            }
            else
            {
                // otherwise --> "C:"
                return FilePath(utf8String(m_path, 0, 2));
            }

        case 4: // make sure of "C:.." case --> "C:..\.."
            if (m_path[2] == '.' && m_path[3] == '.')
            {
                return relativeParentOfPath();
            }
            else
            {
                // otherwise --> "C:"
                return FilePath(utf8String(m_path, 0, 2));
            }

        default: // "C:someSubDir" --> "C:"
            return FilePath(utf8String(m_path, 0, 2));
        }
    }
#endif

    char sep = m_isNative ? pathSep : foreignPathSep;
    auto lastSep = m_path.find_last_of(sep);
    if (lastSep == string::npos)
    {
        return FilePath();
    }
    else if  (lastSep == 0)
    {
        return FilePath(string(1, sep));
    }
    else
    {
        return FilePath(string(m_path.begin(), m_path.begin() + lastSep));
    }
}

//==========================================================================
// public

FilePath FilePath::child(const std::string& subPath) const
{
    if (!isValid())
    {
        return invalidPath();
    }

    switch (subPath.size())
    {
    case 0: return *this;
    case 1: if (subPath[0] == '.')      return *this;
            else                        break;
    case 2: if (subPath[0] == '.' && subPath[1] == '.')
                                        return parent();
            else                        break;
    default:
        break;
    }

    char sep = m_isNative ? pathSep : foreignPathSep;
    ostringstream newPath;
    newPath << m_path << sep << subPath;
    return FilePath(newPath.str());
}

//==========================================================================
// public

FilePath FilePath::rootPathFrom(const FilePath& cwd)
{
    if (!isRelative() || cwd.isRelative() || !isValid() || !cwd.isValid())
    {
        return invalidPath();
    }
    else if (m_components.empty() ||
             (m_components.size() == 1 && m_components[0] == "."))
    {
        return cwd;
    }

#if defined(WIN32)
    bool firstComponentIsDriveLetter = false;
    // check start of relative path, if it's a drive-letter + :, if it's not the
    // same as the incoming cwd, we can't rootFrom it
    if (hasDriveColon(m_path))
    {
        if (m_path[0] != cwd.m_path[0])
        {
            return invalidPath();
        }
        firstComponentIsDriveLetter = true;
    }
#endif  // if defined(WIN32)

    auto workRelative = m_components;
    auto workCwd = cwd.m_components;

#if defined(WIN32)
    if (firstComponentIsDriveLetter)
    {
        workRelative[0].erase(0, 2);
        if (workRelative[0].size() == 1 && workRelative[0] == ".")
        {
            workRelative.erase(workRelative.begin(), workRelative.begin() + 1);
        }
    }
#endif

    while (!workRelative.empty() && !workCwd.empty())
    {
        if (workRelative[0] == "..")
        {
            workRelative.erase(workRelative.begin(), workRelative.begin() + 1);
            workCwd.pop_back();
        }
        else
        {
            break;
        }
    }

    if ((!workRelative.empty() && workRelative[0] == "..") || (cwd.isUNCPath() && workCwd.size() < 2))
    {
        // we're here because we ran out of CWD components -- can't derelativize
        return invalidPath();
    }
    workCwd.insert(workCwd.end(), workRelative.begin(), workRelative.end());

    return fromFullComponents(workCwd, cwd.isUNCPath());
}

//==========================================================================
// public

utf8String FilePath::basename() const
{
    if (!isValid())
    {
        return utf8String();
    }
    else
    {
        return *(m_components.rbegin());
    }

}

//==========================================================================
// private -- factory method for a parent of a relative path (only doable
// through dot-dot component)

FilePath FilePath::relativeParentOfPath() const
{
    ostringstream newPath;
    newPath << m_path << (m_isNative ? pathSep : foreignPathSep) << "..";
    return FilePath(newPath.str());
}

//==========================================================================
// private -- factory method for a parent of a UNC path (returns an invalid
// FilePath if the UNC path is already a root path)

FilePath FilePath::parentOfUNC() const
{
    if (m_components.size() > 2)
    {
        return FilePath(
            string(m_path.begin(),
                   m_path.begin() + m_path.find_last_of(windowsPathSep)));
    }
    else
    {
        return invalidPath();
    }
}

//==========================================================================
// private -- returns true if a component could be a valid windows file path
// component

bool FilePath::componentsHaveNoInvalidWindowsCharacters() const
{
    if (!isRelative())
    {
        return find_if(m_components.begin() + 1, m_components.end(), [](const utf8String& c)
                { return c.find_first_of(charsInvalidForWindowsPath) != utf8String::npos; } )
            == m_components.end();
    }
#if defined(WIN32)
    else if (m_hasDriveColon)
    {
        bool firstValid = m_components[0].find_first_of(charsInvalidForWindowsPath, 2) ==
                            utf8String::npos;
        return firstValid &&
            find_if(m_components.begin() + 1, m_components.end(), [](const utf8String& c)
                { return c.find_first_of(charsInvalidForWindowsPath) !=
                            utf8String::npos; } ) 
                    == m_components.end();

    }
    return find_if(m_components.begin(), m_components.end(), [](const utf8String& c)
            { return c.find_first_of(charsInvalidForWindowsPath) !=
                        utf8String::npos; } )
                == m_components.end();
#else
    return true;    // should never be called -- relative windows paths are not welcome in Linux
#endif
}

//==========================================================================
// private

FilePath FilePath::fromFullComponents(const PathComponentsType& components, bool fromUnc)
{
    ostringstream pathBuilder;

    if (fromUnc)
    {
        pathBuilder << windowsPathSep << windowsPathSep;
    }
#if !defined(WIN32)
    else
    {
        pathBuilder << pathSep;
    }
#endif

    char fullSep = fromUnc ? windowsPathSep : pathSep;
    bool firstDone = false;
    for (auto& c : components)
    {
        if (!firstDone)
        {
            firstDone = true;
        }
        else
        {
            pathBuilder << fullSep;
        }
        pathBuilder << c;
    }

    return FilePath(pathBuilder.str());
}

}
