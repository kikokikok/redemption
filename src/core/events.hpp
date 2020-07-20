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
   Copyright (C) Wallix 2020
   Author(s): Christophe Grosjean

*/

#pragma once

#include "utils/timeval_ops.hpp"
#include "utils/invalid_socket.hpp"
#include <vector>
#include <string>
#include <functional>

#include "utils/log.hpp"
#include "core/error.hpp"

struct Event {
    // TODO: the management of this counter may be moved to EventContainer later
    // no need to use a static lifespan, EventContainer lifespan would be fine
    // in this cas the id would be attributed when adding event to container
    static int counter;
    // event id, 0 means no event
    // used to identify some event in event queue.
    int id = 0;

    std::string name;
    void * lifespan_handle = nullptr;
    bool garbage = false;
    bool teardown = false;

    struct Trigger {
        int fd = -1;
        bool active = false;
        timeval now;
        timeval trigger_time;
        timeval start_time;
        std::chrono::microseconds grace_delay = std::chrono::seconds{0};

        // fd is stored to enabled fd events detection
        // if fd event is triggered timeout is incremented by grace delay
        void set_fd(int fd, std::chrono::microseconds grace_delay) {
            this->fd = fd;
            this->grace_delay = grace_delay;
        }

        // timeout alarm will call on_timeout once when trigger_time is reached
        // the trigger time must be reset with set_timeout
        // if we want to call the alarm again
        void set_timeout(timeval trigger_time) {
          this->active = true;
          this->trigger_time = this->start_time = this->now = trigger_time;
        }

        void reset_timeout(timeval trigger_time) {
          this->active = true;
          this->trigger_time = this->now = trigger_time;
        }

        void stop_alarm(){
            this->active = false;
        }

        bool trigger(timeval now) {
            this->now = now;
            if (not this->active) { return false; }
            if (this->now >= this->trigger_time) {
                this->active = false;
                return true;
            }
            return false;
        }
    } alarm;

    struct Actions {
        std::function<void(Event &)> on_timeout = [](Event &){};
        std::function<void(Event &)> on_action = [](Event &){};
        std::function<void(Event &)> on_teardown = [](Event &){};
    } actions;

    Event(std::string name, void * lifespan)
        : id(Event::counter++)
        , name(name)
        , lifespan_handle(lifespan)
        {
        }

    void rename(const std::string string)
    {
        this->name = string;
    }

    void exec_timeout() { this->actions.on_timeout(*this);}
    void exec_action() { this->actions.on_action(*this);}
    void exec_teardown() { this->actions.on_teardown(*this);}

};


struct Sequencer {
    struct Item {
        std::string label;
        std::function<void(Event&event,Sequencer&sequencer)> action;
    };
    bool reset = false;
    size_t current = 0;
    int verbose = true;
    std::vector<Item> sequence;
    void operator()(Event & event){
        this->reset = false;
        if (this->current >= this->sequence.size()){
            LOG_IF(this->verbose, LOG_INFO, "%s :=> sequencer_event: Sequence Terminated",
                event.name);
            event.garbage = true;
            return;
        }
        try {
            LOG_IF(this->verbose, LOG_INFO, "%s :=> sequencer_event: %s", event.name, sequence[this->current].label);
            sequence[this->current].action(event,*this);
        }
        catch(Error & error){
            LOG_IF(this->verbose, LOG_INFO, "%s :=> sequencer_event: Sequence terminated on Exception %s",
                event.name, error.errmsg());
            event.garbage = true;
            throw;
        }
        catch(...){
            LOG_IF(this->verbose, LOG_INFO, "%s :=> sequencer_event: Sequence terminated on Exception",
                event.name);
            event.garbage = true;
            throw;
        }
        if (not this->reset){
            this->current++;
            if (this->current >= this->sequence.size()){
                LOG_IF(this->verbose, LOG_INFO, "%s :=> sequencer_event: Sequence Terminated On Last Item",
                    event.name);
                    event.garbage = true;
                return;
            }
        }
        this->reset = false;
    }

    void next_state(std::string_view label){
        for(size_t i = 0 ; i < this->sequence.size() ; i++){
            if (this->sequence[i].label == label){
                this->reset = true;
                this->current = i;
                return;
            }
        }
        LOG(LOG_ERR, "Sequence item %.*s not found : ending sequencer", int(label.size()), label.data());
        assert(false);
        this->current = this->sequence.size();
    }
};

struct EventContainer {
    std::vector<Event> queue;

    void get_fds(std::function<void(int fd)> fn)
    {
        for (auto & event: this->queue){
            if ((not (event.garbage or event.teardown)) and event.alarm.fd != INVALID_SOCKET){
                fn(event.alarm.fd);
            }
        }
    }

    void get_fds_timeouts(std::function<void(timeval tv)> fn)
    {
        for (auto & event: this->queue){
            if ((not (event.garbage or event.teardown)) and event.alarm.fd != INVALID_SOCKET){
                fn(event.alarm.trigger_time);
            }
        }
    }

    void get_timeouts(std::function<void(timeval tv)> fn)
    {
        for (auto & event: this->queue){
            if ((not (event.garbage or event.teardown)) and event.alarm.fd == INVALID_SOCKET){
                fn(event.alarm.trigger_time);
            }
        }
    }

    void execute_events(const timeval tv, const std::function<bool(int fd)> & fn, bool verbose = false)
    {
        for (size_t i = 0 ; i < this->queue.size(); i++){
            auto & event = this->queue[i];

            if (event.garbage) {
                LOG_IF(verbose, LOG_INFO, "GARBAGE EVENT '%s' (%d) timeout=%d now=%d =========",
                    event.name, event.id, int(event.alarm.trigger_time.tv_sec%1000), int(tv.tv_sec%1000));
            }
            if (event.teardown) {
                LOG_IF(verbose, LOG_INFO, "TEARDOWN EVENT '%s' (%d) timeout=%d now=%d",
                    event.name, event.id, int(event.alarm.trigger_time.tv_sec%1000), int(tv.tv_sec%1000));
            }

            if (not (event.garbage or event.teardown)) {
                if (event.alarm.fd != -1 && fn(event.alarm.fd)) {
                    LOG_IF(verbose, LOG_INFO, "FD EVENT TRIGGER '%s' (%d) timeout=%d now=%d",
                        event.name, event.id, int(event.alarm.trigger_time.tv_sec%1000), int(tv.tv_sec%1000));
                    event.alarm.set_timeout(tv+event.alarm.grace_delay);
                    event.exec_action();
                    continue;
                }
            }
            if (not (event.garbage or event.teardown)) {
                if (!event.alarm.active){
                    LOG_IF(verbose, LOG_INFO, "EXPIRED TIMEOUT EVENT '%s' (%d) timeout=%d now=%d",
                        event.name, event.id, int(event.alarm.trigger_time.tv_sec%1000), int(tv.tv_sec%1000));
                }
                if (event.alarm.trigger(tv)){
                    LOG_IF(verbose, LOG_INFO, "TIMEOUT EVENT TRIGGER '%s' (%d) timeout=%d now=%d",
                        event.name, event.id, int(event.alarm.trigger_time.tv_sec%1000), int(tv.tv_sec%1000));
                    event.exec_timeout();
                }
            }
        }
        this->exec_teardowns();
        this->garbage_collector();
        LOG_IF(verbose, LOG_INFO, "~~~~~~~~~~~~~~~~ EXECUTE EVENTS DONE ~~~~~~~~~~~~~");
    }


    void exec_teardowns() {
        for (size_t i = 0; i < this->queue.size() ; i++){
            if (not this->queue[i].garbage
            and this->queue[i].teardown){
                this->queue[i].exec_teardown();
                this->queue[i].teardown = false;
                this->queue[i].garbage = true;
            }
        }
    }


    void garbage_collector() {
        for (size_t i = 0; i < this->queue.size() ; i++){
            while ((i < this->queue.size()) && this->queue[i].garbage){
                if (i < this->queue.size() -1){
                    this->queue[i] = std::move(this->queue.back());
                }
                this->queue.pop_back();
            }
        }
    }

    void end_of_lifespan(void * lifespan)
    {
        for (auto & e: this->queue){
            if (e.lifespan_handle == lifespan){
                e.garbage = true;
            }
        }
    }

    // returns timeval{0, 0} if no alarm is set
    timeval next_timeout()
    {
       timeval ultimatum = {0, 0};
       for(auto &event: this->queue){
            if (not (event.garbage or event.teardown) and event.alarm.active){
                ultimatum = event.alarm.trigger_time;
                break;
            }
       }
       for(auto &event: this->queue){
            if (not (event.garbage or event.teardown)
            and event.alarm.active){
                ultimatum = std::min(event.alarm.trigger_time, ultimatum);
            }
        }
        return ultimatum;
    }


    int erase_event(int & event_id)
    {
        if (event_id) {
            for(auto & event: this->queue){
                if (not (event.garbage or event.teardown)
                and (event.id == event_id)){
                    event.garbage = true;
                    event.id = 0;
                    event_id = 0;
                }
            }
        }
        return event_id;
    }

    void reset_timeout(const timeval trigger_time, const int event_id)
    {
        for(auto & event: this->queue){
            if (not (event.garbage or event.teardown)
            and (event.id == event_id)){
                event.alarm.set_timeout(trigger_time);
            }
        }
    }

    void add(Event && event)
    {
        this->queue.push_back(std::move(event));
    }

    int create_event_timeout(std::string name, void * lifespan,
        timeval trigger_time,
        std::function<void(Event&)> timeout)
    {
        Event event(name, lifespan);
        event.alarm.set_timeout(trigger_time);
        event.actions.on_timeout = timeout;
        int event_id = event.id;
        this->queue.push_back(std::move(event));
        return event_id;
    }

};

