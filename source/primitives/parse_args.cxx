///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2014, Arthur N. Klassen
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
// 2014.11.03 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// parse_args.hxx -- Class to turn argc/argv into a collection of flags,
//                   settings and inputs
//
///////////////////////////////////////////////////////////////////////////

#include <parse_args.hxx>

#include <string.h>
#include <algorithm>
#include <string_splitjoin.hxx>
#include <sstream>

using namespace std;

namespace ansak {

namespace {

static const int taintedArgumentCount = 10000;
static const int oneK = 1024;
static const int taintedArgumentSize = 1000;

//===========================================================================

inline bool isBareDoubleHyphen(const char* arg)
{
    return arg[0] == '-' && arg[1] == '-' && arg[2] == '\0';
}

//===========================================================================

bool isFlagOrSetting(const char* arg)
{
    if (arg[0] == '-')
    {
        return arg[1] != '\0';
    }
    return false;
}

//===========================================================================

inline bool hasDoubleHyphen(const char* arg)
{
    return arg[0] == '-' && arg[1] == '-';
}

//===========================================================================

bool hasExactlyOne(const string& s, char splitter)
{
    auto i = s.find(splitter);
    if (i == string::npos)
    {
        return false;
    }
    return s.find(splitter, i + 1) == string::npos;
}

//===========================================================================

char splitterThere(const string& s)
{
    char r = '=';
    if (hasExactlyOne(s, r))
    {
        return r;
    }
    r = ':';
    if (hasExactlyOne(s, r))
    {
        return r;
    }
    return 0;
}

//===========================================================================

void addToLine(ostringstream& line, const string& src, int& runningSize, std::set<int>&nulls)
{
    if (runningSize > 0)
    {
        nulls.insert(runningSize++);
        line << ' ';
    }
    line << src;
    runningSize += static_cast<int>(src.size());
}

//===========================================================================

void throwBadArgs
(
    int         argc,           // I - from argc to main()
    const char* argv[],         // I - from argv to main()
    int         totalSize = -1  // I - size if it's been totted up
)
{
    ostringstream line;
    line << "argc/argv were not reasonable. argc = " << argc << 
            "; argv = " << std::hex << reinterpret_cast<unsigned long long>(argv);
    if (totalSize == -1)
    {
        line << '.';
    }
    else
    {
        if (totalSize >= taintedArgumentCount)
        {
            line << "; total size was " << std::dec << totalSize << '.';
        }
        else
        {
            line << "; some argument was longer than " << taintedArgumentSize << "characters.";
        }
    }
    throw BadArgsException(line.str());
}

}

//===========================================================================
// static, public

ParseArgs ParseArgs::preferringShortFlagsNoThrow
(
    int         argc,           // I - from argc to main()
    const char* argv[],         // I - from argv to main()
    int         limit           // I - maximum number of args to parse
) noexcept
{
    try
    {
        if (!areBad(argc, argv))
        {
            return ParseArgs(argc, argv, true, limit);
        }
    }
    catch ( ... )
    {
    }

    return ParseArgs();
}

//===========================================================================
// static, public

ParseArgs ParseArgs::preferringWordsNoThrow
(
    int         argc,           // I - from argc to main()
    const char* argv[],         // I - from argv to main()
    int         limit           // I - maximum number of args to parse
) noexcept
{
    try
    {
        if (!areBad(argc, argv))
        {
            return ParseArgs(argc, argv, false, limit);
        }
    }
    catch ( ... )
    {
    }

    return ParseArgs();
}

//===========================================================================
// private, constructor

ParseArgs::ParseArgs() noexcept
  : m_procName(),
    m_flags(),
    m_settings(),
    m_inputs(),
    m_shippingArgc(0),
    m_shippingArgv(nullptr),
    m_needRewrite(true),
    m_rewrite(),
    m_argsParsed(0),
    m_allArgsParsed(false),
    m_splitForFlags(false)
{
}

//===========================================================================
// private, constructor

ParseArgs::ParseArgs
(
    int         argc,           // I - from argc to main()
    const char* argv[],         // I - from argv to main()
    bool        splitForFlags,  // I - -abc = -a -b -c
    int         limit           // I - maximum number of args to parse
)
  : m_procName((argv != 0 && argv[0] != 0) ? argv[0] : "TestProgram"),
    m_flags(),
    m_settings(),
    m_inputs(),
    m_shippingArgc(argc),
    m_shippingArgv(const_cast<char**>(argv)),
    m_needRewrite(false),
    m_rewrite(),
    m_argsParsed(0),
    m_allArgsParsed(argc <= maximumDefaultParse),
    m_splitForFlags(splitForFlags)
{
    // avoid tainted inputs
    if (argc < 1 || argc >= taintedArgumentCount)
    {
        m_allArgsParsed = false;
        m_shippingArgc = 0;
        m_shippingArgv = 0;
        throwBadArgs(argc, argv);
    }
    int maximumToScan = limit;
    int maximumToParse = limit;
    int doubleHyphenX = (~(0u) >> 1);
    int argX, maxArg = 0;
    bool tainted = false;
    int totalSize = 0;
    for (argX = 1; argX < argc; ++argX)
    {
        if (isBareDoubleHyphen(argv[argX]))
        {
            doubleHyphenX = argX;
            break;
        }
        if (isFlagOrSetting(argv[argX]))
        {
            maxArg = argX;
        }
        // no param needs to be 1K long
        auto s = strnlen(argv[argX], oneK);
        tainted |= s > taintedArgumentSize;
        totalSize += static_cast<int>(s);
    }
    // total params greater than 10000 is tainted
    tainted |= totalSize > taintedArgumentCount;
    if (tainted)
    {
        // whatever we thought we were going to do, all args were not parsed
        m_allArgsParsed = false;
        m_shippingArgc = 0;
        m_shippingArgv = 0;
        throwBadArgs(argc, argv, totalSize);
    }

    if (doubleHyphenX < maximumToParse)
    {
        maximumToParse = doubleHyphenX;
    }
    else
    {
        maximumToParse = std::min(std::max(maximumToParse, maxArg + 1), argc);
        m_allArgsParsed = argc == maximumToParse;
    }
    maximumToScan = std::min(maximumToScan, argc);

    for (argX = 1; argX < maximumToParse; ++argX)
    {
        const char* arg = argv[argX];
        if (!isFlagOrSetting(arg))
        {
            // it's definitely an input
            m_inputs.push_back(arg);
        }
        else
        {
            // starts with '-', what now?
            auto hasDouble = hasDoubleHyphen(arg);
            auto parseFrom = hasDouble ? 2 : 1;
            if (hasDouble || !m_splitForFlags)
            {
                // looking for a word -- as flag? or setting?
                string argS(arg + parseFrom);
                char splitter = splitterThere(argS);
                if (splitter != 0)
                {
                    // setting by split...
                    auto argSplit(ansak::split(argS, splitter));
                    m_settings[argSplit[0]] = argSplit[1];
                }
                else if (argX < argc - 1)
                {
                    // next arg flag or setting?
                    const char* nextArg = argv[argX + 1];
                    if (isFlagOrSetting(nextArg) || isBareDoubleHyphen(nextArg))
                    {
                        m_flags.insert(argS);
                    }
                    else
                    {
                        m_settings[argS] = string(nextArg);
                        ++argX;
                    }
                }
                else
                {
                    // it's the last one, it's a flag.
                    m_flags.insert(argS);
                }
            }
            else
            {
                if (arg[2] != '\0')
                {
                    // parse -string into -s -t -r -i -n -g
                    for (const char* p = arg + 1; *p != '\0'; ++p)
                    {
                        m_flags.insert(string(1, *p));
                    }
                }
                else
                {
                    // parse one flag or setting into a flag or setting
                    char c = arg[1];
                    if (argX < argc - 1)
                    {
                        // next arg flag or setting?
                        if (isFlagOrSetting(argv[argX + 1]) || isBareDoubleHyphen(argv[argX + 1]))
                        {
                            m_flags.insert(string(1, c));
                        }
                        else
                        {
                            m_settings[string(1, c)] = string(argv[argX + 1]);
                            ++argX;
                        }
                    }
                    else
                    {
                        // it's the last one, it's a flag.
                        m_flags.insert(string(1, c));
                    }
                }
            }
        }
    }

    // load the rest of m_inputs
    for (; argX < maximumToScan; ++argX)
    {
        if (!isBareDoubleHyphen(argv[argX]))
        {
            m_inputs.push_back(argv[argX]);
        }
    }
    m_argsParsed = maximumToParse;
}

//===========================================================================
// public

void ParseArgs::operator()
(
    int&                argc,           // O - the new argc value
    char**&             argv            // O - the new argv array
)
{
    if (m_needRewrite)
    {
        // write this into the ostringstream
        ostringstream line;
        std::set<int> nullsHere;
        int runningSize = 0;
        for (auto f : m_flags)
        {
            string fl((!m_splitForFlags || f.size() == 1) ? 1 : 2, '-'); fl += f;
            addToLine(line, fl, runningSize, nullsHere);
        }
        for (auto s : m_settings)
        {
            string st((!m_splitForFlags || s.first.size() == 1) ? 1 : 2, '-'); st += s.first;
            addToLine(line, st, runningSize, nullsHere);
            addToLine(line, s.second, runningSize, nullsHere);
        }
        bool doneDoubleDash = false;
        for (auto i : m_inputs)
        {
            if (i[0] == '-' && !doneDoubleDash)
            {
                doneDoubleDash = true;
                addToLine(line, "--", runningSize, nullsHere);
            }
            addToLine(line, i, runningSize, nullsHere);
        }

        // use the cobbled command line to size the rewrite space
        auto lineOut = line.str();
        auto lineOutSize = lineOut.size() + 1;
        auto lineOutSizePointerSizeRemainder = lineOutSize % sizeof(char*);
        if (lineOutSizePointerSizeRemainder != 0)
        {
            lineOutSize -= lineOutSizePointerSizeRemainder;
            lineOutSize += sizeof(char*);
        }
        auto rewriteSize = lineOutSize +    // size of all the parameters + '\0' + pad
                           sizeof(char*) *  (1 +   // pointers for argv[0]
                           m_flags.size() +         // all flags, once
                           m_settings.size() * 2 +  // all settings, twice
                           m_inputs.size());        // all inputs, once

        const char* argv0 = m_shippingArgv[0];
        m_rewrite = std::shared_ptr<char>(new char[rewriteSize]);
        char* wholeArray = m_rewrite.get();

        // write the command line into the rewrite space
        std::copy(lineOut.begin(), lineOut.end(), wholeArray);

        m_shippingArgv = reinterpret_cast<char**>(m_rewrite.get() + lineOutSize);
        m_shippingArgc = 1 + nullsHere.size() + 1;

        // write the argv pointers into the rewrite space AFTER the command line
        char** workingArgv = m_shippingArgv;
        *(workingArgv++) = const_cast<char*>(argv0);

        char* param = wholeArray;
        *(workingArgv++) = param;

        while (!nullsHere.empty())
        {
            auto n = nullsHere.lower_bound(0);
            char* injection = wholeArray + *n;
            *(injection++) = '\0';
            nullsHere.erase(n);
            if (!nullsHere.empty())  // don't write the pointer after the last null, it's argc+1, verboten
            {
                *(workingArgv++) = injection;
            }
        }
    }

    // write back argc/argv for the consumer
    argc = m_shippingArgc;
    argv = const_cast<char**>(m_shippingArgv);
}

//===========================================================================
// public

bool ParseArgs::hasFlag
(
    char                flag            // I - the flag to search for
) const
{
    return m_flags.find(string(1, flag)) != m_flags.end();
}

//===========================================================================
// public

bool ParseArgs::hasFlag
(
    const string&       flag            // I - the flag to search for
) const
{
    string lowerable;
    const string& realFlag = flag; // lowerIfNecessary(flag, lowerable);
    return m_flags.find(realFlag) != m_flags.end();
}

//===========================================================================
// public

vector<string> ParseArgs::getFlags() const
{
    vector<string> r;
    for (auto i : m_flags)
    {
        r.push_back(i);
    }

    return r;
}

//===========================================================================
// public

bool ParseArgs::hasSetting
(
    char                setting         // I - the setting to search for
) const
{
    return m_settings.find(string(1, setting)) != m_settings.end();
}

//===========================================================================
// public

bool ParseArgs::hasSetting
(
    const string&       setting         // I - the setting to search for
) const
{
    return m_settings.find(setting) != m_settings.end();
}

//===========================================================================
// public

string ParseArgs::getSetting
(
    char                setting         // I - the setting to search for
) const
{
    auto iSetting = m_settings.lower_bound(string(1, setting));
    if (iSetting != m_settings.end() &&
        iSetting->first.size() == 1 && iSetting->first[0] == setting)
    {
        return iSetting->second;
    }
    return string();
}

//===========================================================================
// public

string ParseArgs::getSetting
(
    const string&       setting         // I - the setting to search for
) const
{
    auto iSetting = m_settings.lower_bound(setting);
    if (iSetting != m_settings.end() && iSetting->first == setting)
    {
        return iSetting->second;
    }
    return string();
}

//===========================================================================
// public

vector<string> ParseArgs::getSettingNames() const
{
    vector<string> r;
    for (auto i : m_settings)
    {
        r.push_back(i.first);
    }

    return r;
}

//===========================================================================
// public

void ParseArgs::set
(
    char                flag            // I - a new flag to set
)
{
    string f(1, flag);
    set(f);
}

//===========================================================================
// public

void ParseArgs::set
(
    const string&       flag            // I - a new flag to set
)
{
    if (m_flags.find(flag) == m_flags.end())
    {
        m_flags.insert(flag);
        m_needRewrite = true;
    }
}

//===========================================================================
// public

void ParseArgs::unset
(
    char                flag            // I - a new flag to get rid of
)
{
    string f(1, flag);
    unset(f);
}

//===========================================================================
// public

void ParseArgs::unset
(
    const string&       flag            // I - a new flag to get rid of
)
{
    auto i = m_flags.find(flag);
    if (i != m_flags.end())
    {
        m_flags.erase(i);
        m_needRewrite = true;
    }
}

//===========================================================================
// public

void ParseArgs::set
(
    char                setting,        // I - a setting to set or replace
    const string&       value           // I - the new value
)
{
    string s(1, setting);
    set(s, value);
}

//===========================================================================
// public

void ParseArgs::set
(
    const string&       setting,        // I - a setting to set or replace
    const string&       value           // I - the new value
)
{
    auto i = m_settings.find(setting);
    if (i == m_settings.end())
    {
        if (!value.empty())
        {
            m_settings[setting] = value;
            m_needRewrite = true;
        }
    }
    else if (value.empty())
    {
        m_settings.erase(i);
        m_needRewrite = true;
    }
    else if (value != i->second)
    {
        i->second = value;
        m_needRewrite = true;
    }
}

//===========================================================================
// public

string ParseArgs::getParsedInput
(
    int                 n               // I - 0-based input to retrieve
) const
{
    if (n >= 0 && static_cast<size_t>(n) < m_inputs.size())
    {
        return m_inputs[n];
    }
    return string();
}

//===========================================================================
// public

void ParseArgs::appendInput
(
    const std::string&      input
)
{
    m_inputs.push_back(input);
    m_needRewrite = true;
}

//===========================================================================
// public

void ParseArgs::deleteParsedInput
(
    int                 n               // I - 0-based input to retrieve
)
{
    if (n >= 0 && static_cast<size_t>(n) < m_inputs.size())
    {
        m_inputs.erase(m_inputs.begin() + n);
    }
}

//===========================================================================
// private, static

bool ParseArgs::areBad
(
    int argc,
    const char* argv[]
)
{
    if (argc == 0 || argc >= taintedArgumentCount)
    {
        return true;
    }

    int totalSize = 0;
    for (auto i = 1; i < argc; ++i)
    {
        auto s = strnlen(argv[i], oneK);
        if (s > taintedArgumentSize)
        {
            return true;
        }
        totalSize += static_cast<int>(s);
    }
    return totalSize >= taintedArgumentCount;
}

//===========================================================================
// public, constructor

BadArgsException::BadArgsException(const string& description) noexcept
{
    ostringstream os;
    os << "BadArgsException: " << description;
    m_what = os.str();
}

//===========================================================================
// public, destructor

BadArgsException::~BadArgsException() noexcept
{
}

//===========================================================================
// public, virtual

const char* BadArgsException::what() const noexcept
{
    return m_what.c_str();
}

}
