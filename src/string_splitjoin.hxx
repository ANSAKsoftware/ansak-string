///////////////////////////////////////////////////////////////////////////
//
// 2014.10.31
//
// The author (Arthur N. Klassen) disclaims all copyright to this source code.
// In place of a legal notice, here is a blessing:
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
// (Is it too much to suggest retaining this header on this file?)
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

