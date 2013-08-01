/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010
   Author(s): Christophe Grosjean, Javier Caverni, Meng Tan
   Based on xrdp Copyright (C) Jay Sorg 2004-2010

   Synchronisation objects

*/

#ifndef _REDEMPTION_CORE_WAIT_OBJ_HPP_
#define _REDEMPTION_CORE_WAIT_OBJ_HPP_

#include "error.hpp"
#include <stdint.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "difftimeval.hpp"

enum BackEvent_t {
    BACK_EVENT_NONE = 0,
    BACK_EVENT_NEXT,
    BACK_EVENT_STOP = 4,
    BACK_EVENT_REFRESH,
};


class wait_obj
{
    public:
    int obj;
    bool set_state;
    BackEvent_t signal;
    struct timeval trigger_time;
    wait_obj(int sck)
    : obj(sck)
    , set_state(false)
    , signal(BACK_EVENT_NONE)
    {
        this->trigger_time = tvtime();
    }

    ~wait_obj()
    {
        if (this->obj > 0){
            close(this->obj);
        }
    }

    void add_to_fd_set(fd_set & rfds, unsigned & max, timeval & timeout)
    {
        if (this->obj > 0){
            FD_SET(this->obj, &rfds);
            max = ((unsigned)this->obj > max)?this->obj:max;
        }
        else if (this->set_state) {
            struct timeval now;
            now = tvtime();
            timeval remain = how_long_to_wait(this->trigger_time, now);
            if (lessthantimeval(remain, timeout)){
                timeout = remain;
                // LOG(LOG_INFO, "TIMEOUT ADJUSTED TO DIFF TRIGGER TIME: %u sec, %u usec", timeout.tv_sec, timeout.tv_usec);
            }
        } //case 3
    }

    void reset()
    {
        this->set_state = false;
    }

    bool is_set(fd_set & rfds)
    {
        if (this->obj > 0){
            return FD_ISSET(this->obj, &rfds);
        }
        else{
            if (this->set_state){
                struct timeval now;
                now = tvtime();
                if ((now.tv_sec > this->trigger_time.tv_sec)
                ||  ( (now.tv_sec == this->trigger_time.tv_sec)
                    &&(now.tv_usec >= this->trigger_time.tv_usec))){
                    return true;
                }
            }
        }
        return false;
    }

    // Idle time in microsecond
    void set(uint64_t idle_usec = 0)
    {
        this->set_state = true;
        struct timeval now = tvtime();

        // uint64_t sum_usec = (now.tv_usec + idle_usec);
        // this->trigger_time.tv_sec = (sum_usec / 1000000) + now.tv_sec;
        // this->trigger_time.tv_usec = sum_usec % 1000000;
        this->trigger_time = addusectimeval(idle_usec, now);
    }

    // Idle time in microsecond
    void update(uint64_t idle_usec)
    {
        if (this->set_state) {
            struct timeval now = tvtime();
            timeval new_trigger = addusectimeval(idle_usec, now);
            if (lessthantimeval(new_trigger, this->trigger_time)) {
                this->trigger_time = new_trigger;
            }
        }
        else {
            this->set(idle_usec);
        }
    }

    bool can_recv()
    {
        fd_set rfds;
        struct timeval time;
        int rv = false;

        time.tv_sec = 0;
        time.tv_usec = 0;
        FD_ZERO(&rfds);
        if (this->obj > 0) {
            FD_SET((this->obj), &rfds);
            rv = select(this->obj + 1, &rfds, 0, 0, &time); /* don't wait */
            if (rv > 0) {
                int opt;
                unsigned int opt_len = sizeof(opt);

                if (getsockopt(this->obj, SOL_SOCKET, SO_ERROR, (char*)(&opt), &opt_len) == 0) {
                    rv = (opt == 0);
                }
            }
        }
        return rv;
    }
};

#endif
