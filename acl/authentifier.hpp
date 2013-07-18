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
  Copyright (C) Wallix 2010
  Author(s): Christophe Grosjean, Javier Caverni, Xavier Dunat, Raphael Zhou, Meng Tan

  Session related with ACL
  find out the next module to run from context reading
*/

#ifndef _REDEMPTION_ACL_AUTHENTIFIER_HPP_
#define _REDEMPTION_ACL_AUTHENTIFIER_HPP_

#include <unistd.h>
#include <fcntl.h>

#include "stream.hpp"
#include "config.hpp"
#include "netutils.hpp"
#include "acl_serializer.hpp"
#include "module_manager.hpp"

class SessionManager {
    Inifile * ini;

    int  tick_count;
    int  max_tick;
    int  keepalive_grace_delay;
    long keepalive_time;
    long keepalive_renew_time;

public:
    AclSerializer acl_serial;

    bool lost_acl;            // false initialy, true when connection with acl is lost
    bool internal_domain;
    bool connected;
    bool last_module;         // indicating a last module (close modules)
    bool asked_remote_answer; // false initialy, set to true when a msg is sent to acl
    bool remote_answer;       // false initialy, set to true once response is received from acl
                              // and asked_remote_answer is set to false
    time_t start_time;        // never used ?
    time_t acl_start_time;    // never used ?
    time_t close_timeout;
    uint32_t verbose;
    bool read_auth;

    SessionManager(Inifile * ini, Transport & _auth_trans, time_t start_time, time_t acl_start_time)
        : ini(ini)
        , tick_count(0)
        , max_tick(ini->globals.max_tick)
        , keepalive_grace_delay(ini->globals.keepalive_grace_delay)
        , keepalive_time(0)
        , keepalive_renew_time(0)
        , acl_serial(AclSerializer(ini, _auth_trans, ini->debug.auth))
        , lost_acl(false)
        , internal_domain(ini->globals.internal_domain)
        , connected(false)
        , last_module(false)
        , asked_remote_answer(false)
        , remote_answer(false)
        , start_time(start_time)
        , acl_start_time(acl_start_time)
        , close_timeout(0)
        , verbose(ini->debug.auth)
        , read_auth(false) {
        if (this->verbose & 0x10) {
            LOG(LOG_INFO, "auth::SessionManager");
        }
    }

    ~SessionManager() {
        if (this->verbose & 0x10) {
            LOG(LOG_INFO, "auth::~SessionManager");
        }
    }

    void start_keepalive(time_t now) {
        if (this->verbose & 0x10) {
            LOG(LOG_INFO, "auth::start_keep_alive");
        }

        this->tick_count           = 1;
        this->keepalive_time       =
        this->keepalive_renew_time = now + this->keepalive_grace_delay;

        this->ini->to_send_set.insert(AUTHID_KEEPALIVE);
        this->ini->context_ask(AUTHID_KEEPALIVE);
    }

    // Check movie start/stop/pause

    int get_mod_from_protocol() {
        if (this->verbose & 0x10) {
            LOG(LOG_INFO, "auth::get_mod_from_protocol");
        }
        // Initialy, it no protocol known and get_value should provide "ASK".
        const char * protocol = this->ini->context.target_protocol.get_value();
        if (this->internal_domain) {
            const char * target = this->ini->globals.target_device.get_cstr();
            if (0 == strncmp(target, "autotest", 8)) {
                protocol = "INTERNAL";
            }
        }
        TODO("connected information is known by module_manager, it should be module manager that change this state when opening some modules")
        int res = MODULE_EXIT;
        if (!this->connected && 0 == strncasecmp(protocol, "RDP", 4)) {
            if (this->verbose & 0x4) {
                LOG(LOG_INFO, "auth::get_mod_from_protocol RDP");
            }
            res = MODULE_RDP;
            this->connected = true;
        }
        else if (!this->connected && 0 == strncasecmp(protocol, "APP", 4)) {
            if (this->verbose & 0x4) {
                LOG(LOG_INFO, "auth::get_mod_from_protocol APPLICATION");
            }
            res = MODULE_RDP;
            this->connected = true;
        }
        else if (!this->connected && 0 == strncasecmp(protocol, "VNC", 4)) {
            if (this->verbose & 0x4) {
                LOG(LOG_INFO, "auth::get_mod_from_protocol VNC");
            }
            res = MODULE_VNC;
            this->connected = true;
        }
        else if (!this->connected && 0 == strncasecmp(protocol, "XUP", 4)) {
            if (this->verbose & 0x4) {
                LOG(LOG_INFO, "auth::get_mod_from_protocol XUP");
            }
            res = MODULE_XUP;
            this->connected = true;
        }
        else if (strncasecmp(protocol, "INTERNAL", 8) == 0) {
            const char * target = this->ini->globals.target_device.get_cstr();
            if (this->verbose & 0x4) {
                LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL");
            }
            if (0 == strcmp(target, "bouncer2")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL bouncer2");
                }
                res = MODULE_INTERNAL_BOUNCER2;
            }
            else if (0 == strncmp(target, "autotest", 8)) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL test");
                }
                const char * user = this->ini->globals.target_user.get_cstr();
                size_t len_user = strlen(user);
                strncpy(this->ini->context.movie, user, sizeof(this->ini->context.movie));
                this->ini->context.movie[sizeof(this->ini->context.movie) - 1] = 0;
                if (0 != strcmp(".mwrm", user + len_user - 5)) {
                    strcpy(this->ini->context.movie + len_user, ".mwrm");
                }
                res = MODULE_INTERNAL_TEST;
            }
            else if (0 == strcmp(target, "selector")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL selector");
                }
                res = MODULE_INTERNAL_WIDGET2_SELECTOR;
            }
            else if (0 == strcmp(target, "login")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL login");
                }
                res = MODULE_INTERNAL_WIDGET2_LOGIN;
            }
            else if (0 == strcmp(target, "rwl_login")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL_LOGIN;
            }
            else if (0 == strcmp(target, "rwl")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL;
            }
            else if (0 == strcmp(target, "close") || 0 == strcmp(target, "widget2_close")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL close");
                }
                res = MODULE_INTERNAL_CLOSE;
                // this->last_module = true;
            }
            else if (0 == strcmp(target, "widget2_dialog")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_dialog");
                }
                res = MODULE_INTERNAL_WIDGET2_DIALOG;
            }
            else if (0 == strcmp(target, "widget2_message")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_message");
                }
                res = MODULE_INTERNAL_WIDGET2_MESSAGE;
            }
            else if (0 == strcmp(target, "widget2_login")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_login");
                }
                res = MODULE_INTERNAL_WIDGET2_LOGIN;
            }
            else if (0 == strcmp(target, "widget2_rwl")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL rwl_login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL;
            }
            else if (0 == strcmp(target, "widget2_rwl_login")) {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_rwl_login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL_LOGIN;
            }
            else {
                if (this->verbose & 0x4) {
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL card");
                }
                res = MODULE_INTERNAL_CARD;
            }
        }
        else if (this->connected) {
            if (this->verbose & 0x4) {
                LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_close");
            }
            res = MODULE_INTERNAL_CLOSE;
        }
        else {
            LOG(LOG_WARNING, "Unsupported target protocol %c%c%c%c",
                protocol[0], protocol[1], protocol[2], protocol[3]);
            res = MODULE_EXIT;
        }
        return res;
    }

protected:
    bool invoke_mod_close(MMApi & mm, const char * auth_error_message, BackEvent_t & signal) {
    
        if (this->last_module) {
            mm.mod->event.reset();
            return false;
        }
        if (auth_error_message) {
            this->ini->context.auth_error_message.copy_c_str(auth_error_message);
        }
        this->asked_remote_answer = false;
        this->last_module         = true;
        this->keepalive_time      = 0;
        TODO("Maybe the timeout of close box should be in the close box module itself");
        if (this->ini->globals.close_timeout > 0) {
            this->close_timeout   = time(NULL) + this->ini->globals.close_timeout;
            LOG(LOG_INFO, "invoke_mod_close: Ending session in %u seconds", this->ini->globals.close_timeout);
        }
        mm.remove_mod();
        mm.new_mod(MODULE_INTERNAL_CLOSE);
        signal = BACK_EVENT_NONE;
        return true;
    }

public:

    bool check(MMApi & mm, time_t now, Transport & trans, BackEvent_t & signal) {
        long enddate;
        if (!this->last_module) {
            enddate = this->ini->context.end_date_cnx.get();
        }
        else {
            enddate = this->close_timeout;
        }
        if (enddate != 0 && (now > enddate)/* && !this->last_module*/) {
            LOG(LOG_INFO, "Session is out of allowed timeframe : closing");
            return invoke_mod_close(mm, "Session is out of allowed timeframe", signal);
        }

        if (signal == BACK_EVENT_STOP) {
            mm.mod->event.reset();
            return false;
        }

        // Check if acl connection is lost.
        if (this->lost_acl && !this->last_module) {
            return invoke_mod_close(mm, "Connection closed by manager (ACL closed)", signal);
        }

        // Keep alive
        if (this->keepalive_time) {
            if (now > (this->keepalive_time + this->keepalive_grace_delay)) {
                LOG(LOG_INFO, "auth::keep_alive_or_inactivity Connection closed by manager (timeout)");
                return invoke_mod_close(mm, "Missed keepalive from ACL", signal);
            }

            if (this->read_auth) {
                if (this->verbose & 0x10) {
                    LOG(LOG_INFO, "auth::keep_alive ACL incoming event");
                }

                this->read_auth = false;

                if (this->ini->context_get_bool(AUTHID_KEEPALIVE)) {
                    // this->ini->context_ask(AUTHID_KEEPALIVE);
                    this->keepalive_time       =
                    this->keepalive_renew_time = now + this->keepalive_grace_delay;
                }
            }

            if (now > this->keepalive_renew_time) {
                this->keepalive_renew_time = now + this->keepalive_grace_delay;

                if (this->verbose & 8) {
                    LOG( LOG_INFO, "%llu bytes received in last quantum, total: %llu tick:%d"
                         , trans.last_quantum_received, trans.total_received, this->tick_count);
                }

                if (trans.last_quantum_received == 0) {
                    this->tick_count++;
                    // 15 minutes before closing on inactivity
                    if (this->tick_count > this->max_tick) {
                        LOG(LOG_INFO, "Session User inactivity : closing");
                        return invoke_mod_close(mm, "Connection closed on inactivity", signal);
                    }
                }
                else {
                    this->tick_count = 0;
                }
                LOG(LOG_INFO, "Session User inactivity : tick count: %u, disconnect on %uth tick", this->tick_count, this->max_tick);
                trans.tick();

                this->ini->context_ask(AUTHID_KEEPALIVE);
            }
        }   // if (this->keepalive_time)

        // Manage module (refresh or next)
        TODO("Check the needs and reference of this->asked_remote_answer. "
             "Maybe could be replaced by ini->check() alone ?");
        if (!this->asked_remote_answer && this->ini->check()) {
            if (signal == BACK_EVENT_REFRESH || signal == BACK_EVENT_NEXT) {
                this->asked_remote_answer = true;
                this->remote_answer       = false;
                this->ask_next_module_remote();
            }
        }
        else {
            this->asked_remote_answer = false;

            if (this->remote_answer && signal == BACK_EVENT_REFRESH) {
                LOG(LOG_INFO, "===========> MODULE_REFRESH");
                signal = BACK_EVENT_NONE;
                TODO("signal management (refresh/next) should go to ModuleManager, it's basically the same behavior. It could be implemented by closing module then opening anothe one of the same kind")
                mm.mod->refresh_context(*this->ini);
                mm.mod->event.signal = BACK_EVENT_NONE;
                mm.mod->event.set();
            }
            else if (this->remote_answer && signal == BACK_EVENT_NEXT) {
            
                LOG(LOG_INFO, "===========> MODULE_NEXT");
                signal = BACK_EVENT_NONE;
                int next_state = this->next_module();
                if (next_state == MODULE_INTERNAL_CLOSE) {
                    return invoke_mod_close(mm,NULL,signal);
                }
                mm.remove_mod();
                try {
                    mm.new_mod(next_state);
                }
                catch (Error & e) {
                    if (e.id == ERR_SOCKET_CONNECT_FAILED) {
                        return invoke_mod_close(mm, "Failed to connect to remote TCP host", signal);
                    }
                    else if (e.id == ERR_SESSION_UNKNOWN_BACKEND) {
                        return invoke_mod_close(mm, "Unknown BackEnd.", signal);
                    }
                    else {
                        TODO("is there any exception thrown by mm that should not implies opening a close box ?")
                        throw e;
                    }
                }
                this->keepalive_time = 0;
                if (this->connected) {
                    this->start_keepalive(now);

                    mm.record();
                }
            }
        }

        // send message to acl with changed values when connected to
        // a module (rdp, vnc, xup ...) and something changed
        // used for authchannel and keepalive.

        if (this->connected && this->ini->check()) {
            this->ask_next_module_remote();
        }

        // AuthCHANNEL CHECK
        // if an answer has been received, send it to
        // rdp serveur via mod (should be rdp module)
        TODO("Check if this->mod is RDP MODULE");
        if (this->connected && this->ini->globals.auth_channel[0]) {
            // Get sesman answer to AUTHCHANNEL_TARGET
            if (!this->ini->context.authchannel_answer.get().is_empty()) {
                // If set, transmit to auth_channel channel
                mm.mod->send_auth_channel_data(this->ini->context.authchannel_answer.get_cstr());
                this->ini->context.authchannel_answer.use();
                // Erase the context variable
                this->ini->context.authchannel_answer.set_empty();
            }
        }

        return true;
    }

    void receive() {
        try {
            if (!this->lost_acl) {
                this->acl_serial.incoming();
                this->read_auth     = true;
                this->remote_answer = true;
            }
        } catch (...) {
            // acl connection lost
            this->ini->context.authenticated.set(false);
            this->ini->context.rejected.set_from_cstr("Authentifier service failed");
            this->lost_acl = true;
        }
    }

    int next_module() {
        if (this->ini->context_is_asked(AUTHID_AUTH_USER)
            ||  this->ini->context_is_asked(AUTHID_PASSWORD)) {
            LOG(LOG_INFO, "===========> MODULE_LOGIN");
            return MODULE_INTERNAL_WIDGET2_LOGIN;
        }
        // Selector Target device and Target user contains list or possible targets
        else if (!this->ini->context_is_asked(AUTHID_SELECTOR)
                 &&   this->ini->context_get_bool(AUTHID_SELECTOR)
                 &&  !this->ini->context_is_asked(AUTHID_TARGET_DEVICE)
                 &&  !this->ini->context_is_asked(AUTHID_TARGET_USER)) {
            LOG(LOG_INFO, "===============> MODULE_SELECTOR");
            return MODULE_INTERNAL_WIDGET2_SELECTOR;
        }
        // Target User or Device asked and not in selector : back to login
        else if (this->ini->context_is_asked(AUTHID_TARGET_DEVICE)
                 ||  this->ini->context_is_asked(AUTHID_TARGET_USER)) {
            LOG(LOG_INFO, "===============> MODULE_LOGIN (2)");
            return MODULE_INTERNAL_WIDGET2_LOGIN;
        }
        // AUTH_USER, AUTH_PASSWORD, TARGET_DEVICE, TARGET_USER known, but acl asked to show message
        else if (this->ini->context_is_asked(AUTHID_DISPLAY_MESSAGE)) {
            LOG(LOG_INFO, "==================> MODULE_DISPLAY");
            return MODULE_INTERNAL_DIALOG_DISPLAY_MESSAGE;
        }
        // AUTH_USER, AUTH_PASSWORD, TARGET_DEVICE, TARGET_USER known, but acl asked to show confirmation message
        else if (this->ini->context_is_asked(AUTHID_ACCEPT_MESSAGE)) {
            LOG(LOG_INFO, "=================> MODULE_ACCEPT");
            return MODULE_INTERNAL_DIALOG_VALID_MESSAGE;
        }
        // Authenticated = true, means we have : AUTH_USER, AUTH_PASSWORD, TARGET_DEVICE, TARGET_USER, TARGET_PASSWORD
        // proceed with connection.
        else if (this->ini->context.authenticated.get()) {
            //                record_video = this->ini->globals.movie.get();
            //                keep_alive = true;
            if (this->ini->context.auth_error_message.is_empty()) {
                this->ini->context.auth_error_message.copy_c_str("End of connection");
            }
            LOG(LOG_INFO, "=================> MODULE_FROM_PROTOCOL");
            return this->get_mod_from_protocol();
        }
        // User authentication rejected : close message
        else {
            if (!this->ini->context.rejected.get().is_empty()) {
                this->ini->context.auth_error_message.copy_str(this->ini->context.rejected.get());
            }
            if (this->ini->context.auth_error_message.is_empty()) {
                this->ini->context.auth_error_message.copy_c_str("Authentifier service failed");
            }
            LOG(LOG_INFO, "MODULE_INTERNAL_CLOSE");
            return MODULE_INTERNAL_CLOSE;
        }
    }

    TODO("May be we should rename this method since it does not only ask for next module "
         "but it is also used for keep alive and auth channel messages acl");
    void ask_next_module_remote() {
        this->acl_serial.ask_next_module_remote();
    }

};



#endif
