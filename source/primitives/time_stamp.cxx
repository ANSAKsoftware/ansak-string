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
// 2016.02.07
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// time_stamp.cxx -- Unify file time stamps into a data type anyone can
//                   understand
//
///////////////////////////////////////////////////////////////////////////

#include <time_stamp.hxx>

namespace ansak {

//============================================================================
// public, comparison

bool TimeStamp::operator==(const TimeStamp& rhs) const
{
    if (this == &rhs) return true;
    return year == rhs.year &&
           month == rhs.month &&
           day == rhs.day &&
           hour == rhs.hour &&
           minute == rhs.minute &&
           second == rhs.second;
}

//============================================================================
// public, comparison

bool TimeStamp::operator<(const TimeStamp& rhs) const
{
    if (this == &rhs)                               return false;
    if (year < rhs.year)                            return true;
    else if (year > rhs.year)                       return false;
    else if (month < rhs.month)                     return true;
        else if (month > rhs.month)                 return false;
        else if (day < rhs.day)                     return true;
            else if (day > rhs.day)                 return false;
            else if (hour < rhs.hour)               return true;
                else if (hour > rhs.hour)           return false;
                else if (minute < rhs.minute)       return true;
                    else if (minute > rhs.minute)   return false;
                    else if (second < rhs.second)   return true;
                        else                        return false;
}

//============================================================================
// public, comparison

bool TimeStamp::operator>(const TimeStamp& rhs) const
{
    if (this == &rhs)                               return false;
    if (year > rhs.year)                            return true;
    else if (year < rhs.year)                       return false;
    else if (month > rhs.month)                     return true;
        else if (month < rhs.month)                 return false;
        else if (day > rhs.day)                     return true;
            else if (day < rhs.day)                 return false;
            else if (hour > rhs.hour)               return true;
                else if (hour < rhs.hour)           return false;
                else if (minute > rhs.minute)       return true;
                    else if (minute < rhs.minute)   return false;
                    else if (second > rhs.second)   return true;
                        else                        return false;
}

}
