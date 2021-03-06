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
   Author(s): Clément Moroldo
*/

#pragma once

#include "client_redemption/client_input_output_api/client_socket_api.hpp"


class ClientHeadlessSocket : public ClientInputSocketAPI
{

public:

    SessionReactor& session_reactor;

    ClientHeadlessSocket(SessionReactor& session_reactor)
      : session_reactor(session_reactor)
    {}

    virtual bool start_to_listen(int /*client_sck*/, mod_api * mod) override
    {
        using namespace std::chrono_literals;

        while (!mod->is_up_and_running()) {
//                 std::cout << " Early negociations...\n";
            if (int err = this->client->wait_and_draw_event(3s)) {
                std::cout << " Error: wait_and_draw_event() fail during negociation (" << err << ").\n";
            }
        }
        //std::cout << " Early negociations completes.\n";

        return true;
    }

    virtual void disconnect() override {}
};


// class ClientHeadlessInput : public ClientInputMouseKeyboardAPI
// {
//     virtual void update_keylayout() override {}
//
//     virtual void init_form() override {}
//
//
//     // TODO string_view
//     void virtual keyPressEvent(const int /*key*/, std::string const& /*text*/) override {}
//
//     // TODO string_view
//     void virtual keyReleaseEvent(const int /*key*/, std::string const& /*text*/) override {}
// };


