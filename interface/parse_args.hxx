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

#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <exception>

namespace ansak {

//===========================================================================
// std::exception-derived exception when the arguments are problematic

class BadArgsException : public std::exception
{
public:
    BadArgsException(const std::string& description) noexcept;
    virtual ~BadArgsException() noexcept override;
    virtual const char* what() const noexcept override;

private:
    std::string         m_what = std::string();
};

//===========================================================================
// Class ParseArgs -- turns argc/argv into a collection of flags, settings and
//                    inputs.
//
// When preferring short flags, a "Flag" is a single-letter behind '-' (they
// can be collected there) or '/' (but not there) or a word behind "--".
//
// When preferring words, a "Flag" (or a "Setting") can be a word behind a '-'
// and multiple flags cannot be collected behind a '-'.
//
// A "Setting" is a single letter behind '/', '-' followed by a second
// parameter with its value. A Setting can also be a word behind "--" followed
// by a second parameter with its value, or its value can be separated from its
// word by a ':' or '='.
//
// An "Input" is anything else in argc/argv. If argc is greater than 20, Inputs
// are not collected after the last Flag or Setting (this can be over-riden).
// The position of the first such Input is retrievable from the class.
//
// Incoming arguemnts are considered bad if there is less than one argument
// available, if there are more than 10,000, if any argument is more than 1000
// characters long, or if the total size of the arguments is more than 10,000
// characters long.
//
// Factory methods allow selection between preferring short flags or preferring
// words; and between throwing and non-throwing behaviour.
//
// Parsed flags, settings and inputs can be added, changed or deleted and a new
// argc/argv pair generated as needed through an invocation operator for passing
// along to other initialization routines such as GTK or Qt.

class ParseArgs {

    static const int maximumDefaultParse = 20;

    ParseArgs() noexcept;

    //=======================================================================
    // Constructor, takes in argc, argv, creates flags, settings, inputs from
    // it. Ignores inputs past 20th parameter or last flag or setting, whichever
    // comes last -- except when noLimit is true. Throws an Exception for
    // TaintedArguments
    // if:
    // * fewer than 1 argument, more than 10,000 arguments
    // * any argument more than 1000 chars long
    // * total size arguments greater than 10,000 chars long.

    ParseArgs
    (
        int         argc,           // I - from argc to main()
        const char* argv[],         // I - from argv to main()
        bool        splitForFlags,  // I - -abc = -a -b -c
        int         limit           // I - maximum number of args to parse
    );

public:

    //=======================================================================
    // Factory method -- preferring short flags and throwing exceptions

    static ParseArgs preferringShortFlags
    (
        int         argc,           // I - from argc to main()
        const char* argv[],         // I - from argv to main()
        int         limit =         // I - maximum number of args to parse
                        maximumDefaultParse
    ) { return ParseArgs(argc, argv, true, limit); }

    //=======================================================================
    // Factory method -- preferring short flags and an empty object if input
    // is tainted

    static ParseArgs preferringShortFlagsNoThrow
    (
        int         argc,           // I - from argc to main()
        const char* argv[],         // I - from argv to main()
        int         limit =         // I - maximum number of args to parse
                        maximumDefaultParse
    ) noexcept;

    //=======================================================================
    // Factory method -- preferring words and throwing exceptions

    static ParseArgs preferringWords
    (
        int         argc,           // I - from argc to main()
        const char* argv[],         // I - from argv to main()
        int         limit =         // I - maximum number of args to parse
                        maximumDefaultParse
    ) { return ParseArgs(argc, argv, false, limit); }

    //=======================================================================
    // Factory method -- preferring words and an empty object if input is
    // tainted

    static ParseArgs preferringWordsNoThrow
    (
        int         argc,           // I - from argc to main()
        const char* argv[],         // I - from argv to main()
        int         limit =         // I - maximum number of args to parse
                        maximumDefaultParse
    ) noexcept;

    ~ParseArgs() = default;
    ParseArgs(const ParseArgs& src) = default;
    ParseArgs& operator=(const ParseArgs& src) = default;

    //=======================================================================
    // operator() - Retrieves an argc/argv to be passed along
    //
    // The real utility in this class is allowing the changing, adding of params
    // before sending to other libraries -- e.g. force-feeding Qt a window size.
    // This routine generates a new argc/argv pair, or returns the last (either
    // original or prepared) values.
    //
    // Return: none

    void operator()
    (
        int&                argc,           // O - the new argc value
        const char**&       argv            // O - the new argv array
    );

    //=======================================================================
    // getProcName - accessor for argv[0]
    //
    // Returns: the string value of proc being run (useful for a filePath?)

    std::string getProcName() const { return m_procName; }

    //=======================================================================
    // hasFlag -- checks for the presence of a flag by single character or
    //            by name
    //
    // Returns: true if the flag is present, false otherwise

    bool hasFlag
    (
        char                flag            // I - the flag to search for
    ) const;

    bool hasFlag
    (
        const std::string&  flag            // I - the flag to search for
    ) const;

    //=======================================================================
    // countFlags -- returns the number of unique flags detected.
    //
    // Returns: m_flags.size() cast as in int

    int countFlags() const { return static_cast<int>(m_flags.size()); }

    //=======================================================================
    // getFlags -- returns all flags as a vector
    //
    // Returns: a vector of string, possibly empty

    std::vector<std::string> getFlags() const;

    //=======================================================================
    // hasSetting -- checks for the presence of a setting by single character
    //               or by name
    //
    // Returns: true if the setting is present, false otherwise

    bool hasSetting
    (
        char                setting         // I - the setting to search for
    ) const;

    bool hasSetting
    (
        const std::string&  setting         // I - the setting to search for
    ) const;

    //=======================================================================
    // countSettings -- returns the number of unique settings detected.
    //
    // Returns: m_settings.size() cast as in int

    int countSettings() const { return static_cast<int>(m_settings.size()); }

    //=======================================================================
    // getSetting -- returns the value of a setting, found by single character
    //               or by name
    //
    // Returns: the setting's value as a string

    std::string getSetting
    (
        char                setting         // I - the setting to search for
    ) const;

    std::string getSetting
    (
        const std::string&  setting         // I - the setting to search for
    ) const;

    //=======================================================================
    // getSettingKeys -- returns all settings keys as a vector
    //
    // Returns: a vector of string, possibly empty

    std::vector<std::string> getSettingNames() const;

    //=======================================================================
    // set -- (over-ride for flags) add a new flag to the collection of arguments
    //
    // Returns: Nothing

    void set
    (
        char                flag            // I - a new flag to set
    );

    void set
    (
        const std::string&  flag            // I - a new flag to set
    );

    //=======================================================================
    // unset -- remove a flag from the collection of arguments
    //
    // Returns: Nothing

    void unset
    (
        char                flag            // I - a flag to get rid of
    );

    void unset
    (
        const std::string&  flag            // I - a new flag to get rid of
    );

    //=======================================================================
    // set -- change the settings.
    //
    // Use a new setting name to add to the arguments.
    // Use an existing setting name with an empty string to remove from the
    // arguments.
    // Use an existing setting name with a non-empty string to change an argument
    //
    // Returns: Nothing

    void set
    (
        char                setting,        // I - a setting to set or replace
        const std::string&  value           // I - the new value, empty() to erase
    );

    void set
    (
        const std::string&  setting,        // I - a setting to set or replace
        const std::string&  value           // I - the new value
    );

    //=======================================================================
    // countParsedInputs -- returns the number of inputs gathered for index
    //                      into argv less than maxArgs.
    //
    // Returns: the number of parsed Inputs as an int

    int countParsedInputs() const { return static_cast<int>(m_inputs.size()); }

    //=======================================================================
    // getParsedInput -- returns a parsed input by index
    //
    // Returns: an input value, or nothing if n is out of range

    std::string getParsedInput
    (
        int                 n               // I - 0-based input to retrieve
    ) const;

    //=======================================================================
    // getParsedInputs -- returns all inputs as a vector
    //
    // Returns: a vector of string, possibly empty

    std::vector<std::string> getParsedInputs() const { return m_inputs; }

    //=======================================================================
    // appendInput -- add a value to the list of inputs
    //
    // Returns: an input value, or nothing if n is out of range

    void appendInput
    (
        const std::string&      input
    );

    //=======================================================================
    // deleteInput -- add a value to the list of inputs
    //
    // Returns: an input value, or nothing if n is out of range

    void deleteParsedInput
    (
        int                 n               // I - 0-based input to retrieve
    );

    //=======================================================================
    // countParsedArgs -- returns how many values from argc/argv were collected
    //
    // Returns: how many elements were collected from argc/argv.

    int countParsedArgs() const { return m_argsParsed; }

    //=======================================================================
    // allArgsParsed -- Were all of argc/argv parsed?
    //
    // Returns: true if all arguments from argc/argv were processed (not more
    // than maxArgs above); false otherwise.

    bool allArgsParsed() const { return m_allArgsParsed; }

private:

    static bool areBad(int argc, const char* argv[]);

    void throwBadArgs
    (
        int         argc,           // I - from argc to main()
        const char* argv[],         // I - from argv to main()
        int         totalSize = -1  // I - size if it's been totted up
    );

    std::string                             m_procName;         // argv[0]
    std::set<std::string>                   m_flags;            // single values from argc/argv
    std::map<std::string, std::string>      m_settings;         // set parameters from argc/argv
    std::vector<std::string>                m_inputs;           // other parameter from argc/argvs

    int                                     m_shippingArgc;     // argc to hand out in operator()
    const char**                            m_shippingArgv;     // argv to hand out in operator()
    bool                                    m_needRewrite;      // argc/argv stale?
    std::vector<char>                       m_rewriteArgs;      // re-written argument space
    std::vector<const char*>                m_rewriteArgPs;     // re-written argv space

    int                                     m_argsParsed;       // number of args parsed
    bool                                    m_allArgsParsed;    // were all of argc/argv parsed?
    bool                                    m_splitForFlags;    // were mult-chars behind '-' split?
};

}
