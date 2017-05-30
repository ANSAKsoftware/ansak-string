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
// 2016.02.07 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// time_stamp.hxx -- Unify file time stamps into a data type anyone can
//                   understand
//
///////////////////////////////////////////////////////////////////////////

#pragma once

namespace ansak {

/////////////////////////////////////////////////////////////////////////////
// A platform-independent TimeStamp record to be used in File attribute
// generation etc. Made for easy generation/parsing of YYYY-MM-DD and HH:mm:ss
// dates. Not suitable for precision finer than nearest-second.
//
// Does not include constructor. Use of an initializer should be sufficient.

struct TimeStamp {
    unsigned int year;              // only dealing with CE years
    unsigned int month;             // month, 1 to 12
    unsigned int day;               // days of the calendar 1 to 28, 29, 30 or 31
    unsigned int hour;              // hours 0 to 23
    unsigned int minute;            // minutes 0 to 59
    unsigned int second;            // seconds 0 to 59

    // helpful comparsion operators
    bool operator==(const TimeStamp& rhs) const;
    bool operator!=(const TimeStamp& rhs) const { return !operator==(rhs); }
    bool operator<(const TimeStamp& rhs) const;
    bool operator>(const TimeStamp& rhs) const;
    bool operator<=(const TimeStamp& rhs) const { return !operator>(rhs); }
    bool operator>=(const TimeStamp& rhs) const { return !operator<(rhs); }
};

}
