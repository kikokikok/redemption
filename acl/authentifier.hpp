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

    int tick_count;

    public:
    BackEvent_t signal;
    AclSerializer acl_serial;
    int keepalive_grace_delay;
    int max_tick;
    bool internal_domain;
    bool connected;
    bool last_module;
    uint32_t verbose;

    SessionManager(Inifile * _ini, Transport & _auth_trans, int _keepalive_grace_delay,
                   int _max_tick, bool _internal_domain, uint32_t _verbose)
        : ini(_ini)
        , tick_count(0)
        , signal(BACK_EVENT_NONE)
        , acl_serial(AclSerializer(_ini,_auth_trans,_verbose))
        , keepalive_grace_delay(_keepalive_grace_delay)
        , max_tick(_max_tick)
        , internal_domain(_internal_domain)
        , connected(false)
        , last_module(false)
        , verbose(_verbose)

    {
        if (this->verbose & 0x10){
            LOG(LOG_INFO, "auth::SessionManager");
        }
    }

    ~SessionManager()
    {
        if (this->verbose & 0x10){
            LOG(LOG_INFO, "auth::~SessionManager");
        }
    }

    void start_keep_alive(long & keepalive_time)
    {
        if (this->verbose & 0x10){
            LOG(LOG_INFO, "auth::start_keep_alive");
        }
        this->tick_count = 1;

        this->ini->context_ask(AUTHID_KEEPALIVE);
        this->acl_serial.send(AUTHID_KEEPALIVE);
        keepalive_time = ::time(NULL) + 30;
    }

    // Set AUTHCHANNEL_TARGET dict value and transmit request to sesman (then wabenginge)
    void ask_auth_channel_target(const char * target)
    {
        if (this->verbose) {
            LOG(LOG_INFO, "SessionManager::ask_auth_channel_target(%s)", target);
        }

        this->ini->context_set_value(AUTHID_AUTHCHANNEL_TARGET, target);
        this->acl_serial.send(AUTHID_AUTHCHANNEL_TARGET);
    }

    // Set AUTHCHANNEL_RESULT dict value and transmit request to sesman (then wabenginge)
    void set_auth_channel_result(const char * result)
    {
        if (this->verbose) {
            LOG(LOG_INFO, "SessionManager::set_auth_channel_result(%s)", result);
        }

        this->ini->context_set_value(AUTHID_AUTHCHANNEL_RESULT, result);
        this->acl_serial.send(AUTHID_AUTHCHANNEL_RESULT);
    }

//    bool close_on_timestamp(long & timestamp)
//    {
//        bool res = false;
//        if (MOD_STATE_DONE_CONNECTED == this->mod_state){
//            long enddate = this->ini->context.end_date_cnx;
//            if (enddate != 0 && (timestamp > enddate)) {
//                if (this->verbose & 0x10){
//                    LOG(LOG_INFO, "auth::close_on_timestamp");
//                }
//                LOG(LOG_INFO, "Session is out of allowed timeframe : stopping");
//                this->mod_state = MOD_STATE_DONE_CLOSE;
//                res = true;
//            }
//        }
//        return res;
//    }

//    bool keep_alive_checking(long & keepalive_time, long & now, Transport & trans)
//    {

//        //        LOG(LOG_INFO, "keep_alive(%lu, %lu)", keepalive_time, now);
//        if (MOD_STATE_DONE_CONNECTED == this->mod_state){
//            long enddate = this->ini->context.end_date_cnx;
//            //            LOG(LOG_INFO, "keep_alive(%lu, %lu, %lu)", keepalive_time, now, enddate));
//            if (enddate != 0 && (now > enddate)) {
//                LOG(LOG_INFO, "Session is out of allowed timeframe : closing");
//                this->mod_state = MOD_STATE_DONE_CLOSE;
//                return false;
//            }
//        }

//        if (keepalive_time == 0){
//            //            LOG(LOG_INFO, "keep_alive disabled");
//            return true;
//        }

//        TODO("we should manage a mode to disconnect on inactivity when we are on login box or on selector")
//            if (now > (keepalive_time + this->keepalive_grace_delay)){
//                LOG(LOG_INFO, "auth::keep_alive_or_inactivity Connection closed by manager (timeout)");
//                this->ini->context.auth_error_message.copy_c_str("Connection closed by manager (timeout)");
//                return false;
//            }


//        if (now > keepalive_time){
//            // ===================== check if no traffic =====================
//            if (this->verbose & 8){
//                LOG(LOG_INFO, "%llu bytes received in last quantum, total: %llu tick:%d",
//                    trans.last_quantum_received, trans.total_received,
//                    this->tick_count);
//            }
//            if (trans.last_quantum_received == 0){
//                this->tick_count++;
//                if (this->tick_count > this->max_tick){ // 15 minutes before closing on inactivity
//                    this->ini->context.auth_error_message.copy_c_str("Connection closed on inactivity");
//                    LOG(LOG_INFO, "Session ACL inactivity : closing");
//                    this->mod_state = MOD_STATE_DONE_CLOSE;
//                    return false;
//                }

//                keepalive_time = now + this->keepalive_grace_delay;
//            }
//            else {
//                this->tick_count = 0;
//            }
//            trans.tick();

//            // ===================== check if keepalive ======================
//            try {
//                this->acl_serial.send(AUTHID_KEEPALIVE);
//            }
//            catch (...){
//                this->ini->context.auth_error_message.copy_c_str("Connection closed by manager (ACL closed).");
//                this->mod_state = MOD_STATE_DONE_CLOSE;
//                return false;
//            }
//        }
//        return true;
//    }

//    bool keep_alive_response(long & keepalive_time, long & now)
//    {
//        if (this->verbose & 0x10){
//            LOG(LOG_INFO, "auth::keep_alive ACL incoming event");
//        }
//        try {
//            this->acl_serial.incoming();
//            if (this->ini->context_get_bool(AUTHID_KEEPALIVE)) {
//                keepalive_time = now + this->keepalive_grace_delay;
//                this->ini->context_ask(AUTHID_KEEPALIVE);
//            }
//        }
//        catch (...){
//            this->ini->context.auth_error_message.copy_c_str("Connection closed by manager (ACL closed)");
//            this->mod_state = MOD_STATE_DONE_CLOSE;
//            return false;
//        }
//        return true;
//    }
    
    
//    bool keep_alive(long & keepalive_time, long & now, Transport * trans, bool read_auth)
//    {
////        LOG(LOG_INFO, "keep_alive(%lu, %lu)", keepalive_time, now);
//        if (MOD_STATE_DONE_CONNECTED == this->mod_state){
//            long enddate = this->ini->context.end_date_cnx;
////            LOG(LOG_INFO, "keep_alive(%lu, %lu, %lu)", keepalive_time, now, enddate));
//            if (enddate != 0 && (now > enddate)) {
//                LOG(LOG_INFO, "Session is out of allowed timeframe : closing");
//                this->mod_state = MOD_STATE_DONE_CLOSE;
//                return false;
//            }
//        }

//        if (keepalive_time == 0){
////            LOG(LOG_INFO, "keep_alive disabled");
//            return true;
//        }

//        TODO("we should manage a mode to disconnect on inactivity when we are on login box or on selector")
//        if (now > (keepalive_time + this->keepalive_grace_delay)){
//            LOG(LOG_INFO, "auth::keep_alive_or_inactivity Connection closed by manager (timeout)");
//            this->ini->context.auth_error_message.copy_c_str("Connection closed by manager (timeout)");
//            return false;
//        }


//        if (now > keepalive_time){
//            // ===================== check if no traffic =====================
//            if (this->verbose & 8){
//                LOG(LOG_INFO, "%llu bytes received in last quantum, total: %llu tick:%d",
//                          trans->last_quantum_received, trans->total_received,
//                          this->tick_count);
//            }
//            if (trans->last_quantum_received == 0){
//                this->tick_count++;
//                if (this->tick_count > this->max_tick){ // 15 minutes before closing on inactivity
//                    this->ini->context.auth_error_message.copy_c_str("Connection closed on inactivity");
//                    LOG(LOG_INFO, "Session ACL inactivity : closing");
//                    this->mod_state = MOD_STATE_DONE_CLOSE;
//                    return false;
//                }

//                keepalive_time = now + this->keepalive_grace_delay;
//            }
//            else {
//                this->tick_count = 0;
//            }
//            trans->tick();

//            // ===================== check if keepalive ======================
//            try {
//                this->acl_serial.send(AUTHID_KEEPALIVE);
//            }
//            catch (...){
//                this->ini->context.auth_error_message.copy_c_str("Connection closed by manager (ACL closed).");
//                this->mod_state = MOD_STATE_DONE_CLOSE;
//                return false;
//            }
//        }

//        if (read_auth) {
//            if (this->verbose & 0x10){
//                LOG(LOG_INFO, "auth::keep_alive ACL incoming event");
//            }
//            try {
//                this->acl_serial.incoming();
//                if (this->ini->context_get_bool(AUTHID_KEEPALIVE)) {
//                    keepalive_time = now + this->keepalive_grace_delay;
//                    this->ini->context_ask(AUTHID_KEEPALIVE);
//                }
//            }
//            catch (...){
//                this->ini->context.auth_error_message.copy_c_str("Connection closed by manager (ACL closed)");
//                this->mod_state = MOD_STATE_DONE_CLOSE;
//                return false;
//            }
//        }

//        return true;
//    }

    int get_mod_from_protocol()
    {
        if (this->verbose & 0x10){
            LOG(LOG_INFO, "auth::get_mod_from_protocol");
        }
        const char * protocol = this->ini->context_get_value(AUTHID_TARGET_PROTOCOL, NULL, 0);
        if (this->internal_domain){
            const char * target = this->ini->context_get_value(AUTHID_TARGET_DEVICE, NULL, 0);
            if (0 == strncmp(target, "autotest", 8)){
                protocol = "INTERNAL";
            }
        }
        int res = MODULE_EXIT;
        if (!this->connected && 0 == strncasecmp(protocol, "RDP", 4)){
            if (this->verbose & 0x4){
                LOG(LOG_INFO, "auth::get_mod_from_protocol RDP");
            }
            res = MODULE_RDP;
            this->connected = true;
        }
        else if (!this->connected && 0 == strncasecmp(protocol, "VNC", 4)){
            if (this->verbose & 0x4){
                LOG(LOG_INFO, "auth::get_mod_from_protocol VNC");
            }
            res = MODULE_VNC;
            this->connected = true;
        }
        else if (!this->connected && 0 == strncasecmp(protocol, "XUP", 4)){
            if (this->verbose & 0x4){
                LOG(LOG_INFO, "auth::get_mod_from_protocol XUP");
            }
            res = MODULE_XUP;
            this->connected = true;
        }
        else if (strncasecmp(protocol, "INTERNAL", 8) == 0){
            const char * target = this->ini->context_get_value(AUTHID_TARGET_DEVICE, NULL, 0);
            if (this->verbose & 0x4){
                LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL");
            }
            if (0 == strcmp(target, "bouncer2")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL bouncer2");
                }
                res = MODULE_INTERNAL_BOUNCER2;
            }
            else if (0 == strncmp(target, "autotest", 8)){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL test");
                }
                const char * user = this->ini->context_get_value(AUTHID_TARGET_USER, NULL, 0);
                size_t len_user = strlen(user);
                strncpy(this->ini->context.movie, user, sizeof(this->ini->context.movie));
                this->ini->context.movie[sizeof(this->ini->context.movie) - 1] = 0;
                if (0 != strcmp(".mwrm", user + len_user - 5)){
                    strcpy(this->ini->context.movie + len_user, ".mwrm");
                }
                res = MODULE_INTERNAL_TEST;
            }
            else if (0 == strcmp(target, "selector")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL selector");
                }
                res = MODULE_INTERNAL_WIDGET2_SELECTOR;
            }
            else if (0 == strcmp(target, "login")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL login");
                }
                res = MODULE_INTERNAL_WIDGET2_LOGIN;
            }
            else if (0 == strcmp(target, "rwl_login")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL_LOGIN;
            }
            else if (0 == strcmp(target, "rwl")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL;
            }
            else if (0 == strcmp(target, "close")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL close");
                }
                res = MODULE_INTERNAL_CLOSE;
                this->last_module = true;
            }
            else if (0 == strcmp(target, "widget2_close")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_close");
                }
                res = MODULE_INTERNAL_WIDGET2_CLOSE;
                this->last_module = true;
            }
            else if (0 == strcmp(target, "widget2_dialog")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_dialog");
                }
                res = MODULE_INTERNAL_WIDGET2_DIALOG;
            }
            else if (0 == strcmp(target, "widget2_message")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_message");
                }
                res = MODULE_INTERNAL_WIDGET2_MESSAGE;
            }
            else if (0 == strcmp(target, "widget2_login")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_login");
                }
                res = MODULE_INTERNAL_WIDGET2_LOGIN;
            }
            else if (0 == strcmp(target, "widget2_rwl")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL rwl_login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL;
            }
            else if (0 == strcmp(target, "widget2_rwl_login")){
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_rwl_login");
                }
                res = MODULE_INTERNAL_WIDGET2_RWL_LOGIN;
            }
            else {
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL card");
                }
                res = MODULE_INTERNAL_CARD;
            }
        }
        else if (this->connected) {
                if (this->verbose & 0x4){
                    LOG(LOG_INFO, "auth::get_mod_from_protocol INTERNAL widget2_close");
                }
                res = MODULE_INTERNAL_WIDGET2_CLOSE;
                this->last_module = true;
        }
        else {
            LOG(LOG_WARNING, "Unsupported target protocol %c%c%c%c",
                protocol[0], protocol[1], protocol[2], protocol[3]);
            res = MODULE_EXIT;
        }
        return res;
    }

    bool check(){
        switch (this->signal){
        case BACK_EVENT_STOP:
            return false;
        case BACK_EVENT_NONE:
        break;
        case BACK_EVENT_REFRESH:
        case BACK_EVENT_NEXT:
            this->ask_next_module_remote();
        break;
        }
        return true;
    }

    void receive(){
        try {
            this->acl_serial.incoming();
        } catch (...) {
            this->ini->context.authenticated = false;
            this->ini->context.rejected.copy_c_str("Authentifier service failed");
        }
    }

    int next_module()
    {
        if (this->signal == BACK_EVENT_NONE) {
            LOG(LOG_INFO, "========> MODULE_RUNNING");
            return MODULE_RUNNING;
        }

        if (this->signal == BACK_EVENT_REFRESH) {
            LOG(LOG_INFO, "===========> MODULE_REFRESH");
            this->signal = BACK_EVENT_NONE;
            return MODULE_REFRESH;
        }

        this->signal = BACK_EVENT_NONE;
    
        if (this->ini->context_is_asked(AUTHID_AUTH_USER) 
        ||  this->ini->context_is_asked(AUTHID_PASSWORD)){
            LOG(LOG_INFO, "===========> MODULE_LOGIN");
            return MODULE_INTERNAL_WIDGET2_LOGIN;
        }
        // Selector Target device and Target user contains list or possible targets
        else if (!this->ini->context_is_asked(AUTHID_SELECTOR)
             &&   this->ini->context_get_bool(AUTHID_SELECTOR)
             &&  !this->ini->context_is_asked(AUTHID_TARGET_DEVICE)
             &&  !this->ini->context_is_asked(AUTHID_TARGET_USER)){
            LOG(LOG_INFO, "===============> MODULE_SELECTOR");
            return MODULE_INTERNAL_WIDGET2_SELECTOR;
        }
        // Target User or Device asked and not in selector : back to login
        else if (this->ini->context_is_asked(AUTHID_TARGET_DEVICE)
             ||  this->ini->context_is_asked(AUTHID_TARGET_USER)){
                LOG(LOG_INFO, "===============> MODULE_LOGIN (2)");
                return MODULE_INTERNAL_WIDGET2_LOGIN;
        }
        // AUTH_USER, AUTH_PASSWORD, TARGET_DEVICE, TARGET_USER known, but acl asked to show message
        else if (this->ini->context_is_asked(AUTHID_DISPLAY_MESSAGE)){
            LOG(LOG_INFO, "==================> MODULE_DISPLAY");
            return MODULE_INTERNAL_DIALOG_DISPLAY_MESSAGE;
        }
        // AUTH_USER, AUTH_PASSWORD, TARGET_DEVICE, TARGET_USER known, but acl asked to show confirmation message
        else if (this->ini->context_is_asked(AUTHID_ACCEPT_MESSAGE)){
                     LOG(LOG_INFO, "=================> MODULE_ACCEPT");
           return MODULE_INTERNAL_DIALOG_VALID_MESSAGE;
        }
        // Authenticated = true, means we have : AUTH_USER, AUTH_PASSWORD, TARGET_DEVICE, TARGET_USER, TARGET_PASSWORD
        // proceed with connection.
        else if (this->ini->context.authenticated){
//                record_video = this->ini->globals.movie;
//                keep_alive = true;
            if (this->ini->context.auth_error_message.is_empty()) {
                this->ini->context.auth_error_message.copy_c_str("End of connection");
            }
             LOG(LOG_INFO, "=================> MODULE_FROM_PROTOCOL");
            return this->get_mod_from_protocol();
        }
        // User authentication rejected : close message
        else {
            if (!this->ini->context.rejected.is_empty()) {
                this->ini->context.auth_error_message.copy_str(this->ini->context.rejected);
            }
            if (this->ini->context.auth_error_message.is_empty()) {
                this->ini->context.auth_error_message.copy_c_str("Authentifier service failed");
            }
             LOG(LOG_INFO, "MODULE_INTERNAL_CLOSE");
            return MODULE_INTERNAL_CLOSE;
        }
    }

    void ask_next_module_remote()
    {
        this->acl_serial.ask_next_module_remote();
    }

};

#endif
