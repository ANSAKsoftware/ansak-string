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
// 2014.10.31 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// string_splitjoin.hxx -- templates for performing split and join (as in
//                         python) against any kind of basic_string.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <list>
#include <algorithm>
#include <functional>
#include <vector>

namespace ansak
{

//=========================================================================
// Template function split()
//
// Splits a basic_string-of-C by some delimiter, also C.

template <typename C>
std::vector< std::basic_string<C> > split(const std::basic_string<C>& src, C delim)
{
    static_assert(std::is_integral<C>::value, "split needs an integral type.");

    typedef std::basic_string<C> elem_type;
    typedef typename elem_type::size_type size_type;
    typedef std::vector< elem_type > return_type;

    if (src.empty())
    {
        return return_type();
    }

    const auto npos = elem_type::npos;

    size_type n = src.find(delim);
    if (n == npos)
    {
        return_type result;
        result.push_back(src);
        return result;
    }
    else
    {
        std::list<size_type> breaks;
        do
        {
            breaks.push_back(n);
            n = src.find(delim, n + 1);
        } while (n != npos);

        return_type result(breaks.size() + 1);
        int iResX = 0;
        result[iResX++] = src.substr(0, *breaks.begin());
        for (auto iBreak = breaks.begin(); iBreak != breaks.end(); ++iBreak)
        {
            auto nStart = *iBreak + 1;
            auto nextBreak = iBreak; ++nextBreak;
            if (nextBreak == breaks.end())
            {
                result[iResX] = src.substr(nStart);
            }
            else
            {
                result[iResX++] = src.substr(nStart, *nextBreak - nStart);
            }
        }
        return result;
    }
}

//=========================================================================
// Template function join()
//
// Joins a basic_string-of-C with some delimiter, also C.

template <typename C>
std::basic_string<C> join(const std::vector< std::basic_string<C> >& src, C delim)
{
    static_assert(std::is_integral<C>::value, "join needs an integral type.");

    typedef std::basic_string<C> return_type;

    if (src.empty())
    {
        return return_type();
    }
    else
    {
        C delimStr[2] = {0};
        delimStr[0] = delim;
        return_type r;
        bool first = true;
        for (auto s : src)
        {
            if (!first)
            {
                r.append(delimStr);
            }
            first = false;
            r.append(s);
        }

        return r;
    }
}

}

