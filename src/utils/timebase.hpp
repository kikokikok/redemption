/*
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Product name: redemption, a FLOSS RDP proxy
Copyright (C) Wallix 2020
Author(s):Christophe Grosjean
*/

#pragma once

#include "utils/timeval_ops.hpp"

#include <cassert>


struct TimeBase
{
    timeval current_time;

    TimeBase(timeval current_time) : current_time(current_time) {}

    void set_current_time(timeval const& now) noexcept
    {
        assert(now >= this->current_time);
        this->current_time = now;
    }

    [[nodiscard]] timeval get_current_time() const noexcept
    {
        return this->current_time;
    }
};
