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
   Copyright (C) Wallix 2013
   Author(s): Christophe Grosjean
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestVNC
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL
#include "vnc/vnc.hpp"
#include "test_transport.hpp"

BOOST_AUTO_TEST_CASE(TestFillEncodingTypesBuffer)
{
    {
        BStream  stream(512);
        uint16_t number_of_encodings;

        number_of_encodings = 0;
        mod_vnc::fill_encoding_types_buffer("16,2,0,1,-239", stream, number_of_encodings, 1);
        BOOST_CHECK(!memcmp(stream.get_data(),
                            "\x00\x00\x00\x10\x00\x00\x00\x02"
                            "\x00\x00\x00\x00\x00\x00\x00\x01"
                            "\xFF\xFF\xFF\x11",
                            20));
    }

    {
        BStream  stream(512);
        uint16_t number_of_encodings;

        number_of_encodings = 0;
        mod_vnc::fill_encoding_types_buffer("\t16 , 2 , 0 , 1 , -239 ", stream, number_of_encodings, 1);
        BOOST_CHECK(!memcmp(stream.get_data(),
                            "\x00\x00\x00\x10\x00\x00\x00\x02"
                            "\x00\x00\x00\x00\x00\x00\x00\x01"
                            "\xFF\xFF\xFF\x11",
                            20));
    }
}


BOOST_AUTO_TEST_CASE(TestVncMouse)
{
    char data[] =
        "\x05\x00\x00\x0a\x00\x0a"                          // move 10, 10
        "\x05\x08\x00\x0a\x00\x0a\x05\x00\x00\x0a\x00\x0a"  // scrool up
        "\x05\x01\x00\x0a\x00\x0a"                          // up left click
        "\x05\x03\x00\x0a\x00\x0a"                          // up right click
        "\x05\x02\x00\x0a\x00\x0a"                          // down left click
        "\x05\x02\x00\x0f\x00\x11"                          // move 15, 17
        "\x05\x00\x00\x0f\x00\x12"                          // down right click + move 15, 18
    ;
    CheckTransport t(data, sizeof(data)-1);
    mod_vnc::Mouse mouse;
    mouse.move(t, 10, 10);
    mouse.scroll(t, 8);
    mouse.click(t, 10, 10, 1, 1);
    mouse.click(t, 10, 10, 2, 1);
    mouse.click(t, 10, 10, 1, 0);
    mouse.move(t, 15, 17);
    mouse.click(t, 15, 18, 2, 0);
}
