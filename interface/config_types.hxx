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
// 2014.12.18 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config_types.hxx -- simple types for generic layout settings
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <tuple>
#include <math.h>

namespace ansak
{

// simple position types
typedef std::pair<int, int> Point;
typedef std::pair<Point, Point> Rect;

// simple Point methods -- extract each one, construct a Point
inline int x(const Point& pt) { return pt.first; }
inline int y(const Point& pt) { return pt.second; }
inline Point toPoint(int x, int y) { Point pt; pt.first = x; pt.second = y; return pt; }

// simple Rect methods -- extract each one, construct a Rect
inline int left(const Rect& rect) { return x(rect.first); }
inline int top(const Rect& rect) { return y(rect.first); }
inline int width(const Rect& rect) { return x(rect.second); }
inline int height(const Rect& rect) { return y(rect.second); }
inline Rect toRect(int left, int top, int width, int height) {
        Rect r; r.first = toPoint(left, top); r.second = toPoint(width, height);
        return r; }

// get corner points from a rect
inline void toCornerPoints(const Rect& rect, Point& topLeft, Point& bottomRight)
{
    bottomRight = topLeft = rect.first;
    bottomRight.first += width(rect);
    bottomRight.second += height(rect);
}

// Float-equality

const float epsilon = 1.0 / 65536.0;

template<typename A, typename B> bool floatEqual(A a, B b)
{
    return fabs(a - b) < epsilon;
}

}
