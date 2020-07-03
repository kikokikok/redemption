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
Copyright (C) Wallix 2018
Author(s): Jonathan Poelen
*/

#pragma once

#include "utils/timebase.hpp"
#include "utils/select.hpp"
#include "core/events.hpp"


enum class ExecuteEventsResult
{
    Success,
    Error,
    Continue,
    Timeout,
};

inline ExecuteEventsResult execute_events(
    std::chrono::milliseconds timeout,
    TimeBase& time_base,
    EventContainer& events)
{
    unsigned max = 0;
    fd_set   rfds;
    io_fd_zero(rfds);

    auto g = [&rfds,&max](int fd){
        assert(fd != -1);
        io_fd_set(fd, rfds);
        max = std::max(max, unsigned(fd));
    };
    for (auto & event: events){
        if (event.garbage or event.teardown){
            continue;
        }
        if (event.alarm.fd != -1){
            g(event.alarm.fd);
        }
    }

    time_base.set_current_time(tvtime());
    // return a valid timeout, current_time + maxdelay if must wait more than maxdelay
    timeval tv =  timeval{0,0} + timeout;
    auto update_tv = [&](timeval const& tv2){
        if (tv2.tv_sec >= 0) {
            tv = std::min(tv, tv2);
        }
    };

    auto timer_update_tv = [&](auto& timer){
        update_tv(timer.tv);
    };

    for (auto & event: events){
        if (event.garbage or event.teardown){
            continue;
        }
        if (event.alarm.fd != -1){
            if (event.alarm.active) {
                update_tv(event.alarm.trigger_time);
            }
        }
    }


    timeval timeoutastv = tv;

    int num = select(max + 1, &rfds, nullptr, nullptr, &timeoutastv);

    if (num < 0) {
        if (errno == EINTR) {
            return ExecuteEventsResult::Continue;
        }
        return ExecuteEventsResult::Error;
    }

    time_base.set_current_time(tvtime());
    auto const end_tv = time_base.get_current_time();
    execute_events(events, end_tv, [](int /*fd*/){ return false; });

    if (num) {
        execute_events(events, end_tv, [&rfds](int fd){ return io_fd_isset(fd, rfds); });
        return ExecuteEventsResult::Success;
    }

    return ExecuteEventsResult::Timeout;
}
