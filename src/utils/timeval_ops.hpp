/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2014
 *   Author(s): Christophe Grosjean, Raphael Zhou, Jonathan Poelen, Meng Tan
 */


#pragma once

#include "cxx/diagnostic.hpp"

#include <chrono>
#include <sys/time.h>

REDEMPTION_DIAGNOSTIC_PUSH
REDEMPTION_DIAGNOSTIC_CLANG_IGNORE("-Wheader-hygiene")
using namespace std::chrono_literals; // NOLINT
REDEMPTION_DIAGNOSTIC_POP

inline bool operator<(timeval const& a, timeval const& b) noexcept
{
    return a.tv_sec < b.tv_sec
       || (a.tv_sec == b.tv_sec
        && a.tv_usec < b.tv_usec);
}

inline bool operator==(timeval const& a, timeval const& b) noexcept
{
    return a.tv_sec == b.tv_sec && a.tv_usec == b.tv_usec;
}

inline bool operator!=(timeval const& a, timeval const& b) noexcept
{
    return !(a == b);
}

inline bool operator>(timeval const& a, timeval const& b) noexcept
{
    return b < a;
}

inline bool operator<=(timeval const& a, timeval const& b) noexcept
{
    return !(b < a);
}

inline bool operator>=(timeval const& a, timeval const& b) noexcept
{
    return !(a < b);
}


inline timeval to_timeval(std::chrono::seconds const& seconds)
{
    timeval tv;
    tv.tv_sec = seconds.count();
    tv.tv_usec = 0;
    return tv;
}

inline timeval to_timeval(std::chrono::milliseconds const& usec)
{
    timeval tv;
    tv.tv_sec = usec.count() / 1'000;
    tv.tv_usec = usec.count() % 1'000 * 1'000;
    return tv;
}

inline timeval to_timeval(std::chrono::microseconds const& usec)
{
    timeval tv;
    tv.tv_sec = usec.count() / 1'000'000;
    tv.tv_usec = usec.count() % 1'000'000;
    return tv;
}


inline std::chrono::milliseconds to_ms(timeval const& tv)
{
    return std::chrono::milliseconds(tv.tv_sec*1000LL + tv.tv_usec/1000L);
}

inline std::chrono::microseconds ustime(const timeval & now)
{
    return std::chrono::microseconds(now.tv_sec*1000000LL + now.tv_usec);
}


inline timeval& operator+=(timeval& tv, std::chrono::seconds const& seconds)
{
    tv.tv_sec += seconds.count();
    return tv;
}

inline timeval& operator+=(timeval& tv, std::chrono::milliseconds const& ms)
{
    auto usec = ms.count() * 1000 + tv.tv_usec;
    tv.tv_sec += usec / 1'000'000;
    tv.tv_usec = usec % 1'000'000;
    return tv;
}

inline timeval& operator+=(timeval& tv, std::chrono::microseconds const& us)
{
    auto usec = us.count() + tv.tv_usec;
    tv.tv_sec += usec / 1'000'000;
    tv.tv_usec = usec % 1'000'000;
    return tv;
}

inline timeval operator+(timeval tv, std::chrono::seconds const& seconds)
{
    tv += seconds;
    return tv;
}

inline timeval operator+(timeval tv, std::chrono::milliseconds const& ms)
{
    tv += ms;
    return tv;
}

inline timeval operator+(timeval tv, std::chrono::microseconds const& us)
{
    tv += us;
    return tv;
}


inline std::chrono::microseconds operator-(timeval const& ultimatum, timeval const& starttime)
{
    return std::chrono::seconds(ultimatum.tv_sec - starttime.tv_sec)
         + std::chrono::microseconds(ultimatum.tv_usec - starttime.tv_usec);
}
