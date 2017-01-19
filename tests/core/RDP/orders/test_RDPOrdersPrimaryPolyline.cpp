/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2013
    Author(s): Christophe Grosjean, Raphael Zhou

    Unit test to RDP Orders coder/decoder
    Using lib boost functions for testing
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestOrderPolyline
#include "system/redemption_unit_tests.hpp"

#define LOGNULL
//#define LOGPRINT

#include "core/RDP/orders/RDPOrdersPrimaryPolyline.hpp"

#include "test_orders.hpp"

BOOST_AUTO_TEST_CASE(TestPolyline)
{
    using namespace RDP;

    {
        StaticOutStream<1000> out_stream;

        RDPOrderCommon state_common(0, Rect(0, 0, 0, 0));
        RDPPolyline state_polyline;
        RDPOrderCommon newcommon(POLYLINE, Rect(0, 0, 1024, 768));

        StaticOutStream<1024> deltaPoints;

        deltaPoints.out_sint16_le(0);
        deltaPoints.out_sint16_le(20);

        deltaPoints.out_sint16_le(160);
        deltaPoints.out_sint16_le(0);

        deltaPoints.out_sint16_le(0);
        deltaPoints.out_sint16_le(-30);

        deltaPoints.out_sint16_le(50);
        deltaPoints.out_sint16_le(50);

        deltaPoints.out_sint16_le(-50);
        deltaPoints.out_sint16_le(50);

        deltaPoints.out_sint16_le(0);
        deltaPoints.out_sint16_le(-30);

        deltaPoints.out_sint16_le(-160);
        deltaPoints.out_sint16_le(0);

        InStream deltaPoints_in(deltaPoints.get_data(), deltaPoints.get_offset());

        RDPPolyline polyline(158, 230, 0x0D, 0, RDPColor{0x000000}, 7, deltaPoints_in);


        polyline.emit(out_stream, newcommon, state_common, state_polyline);

        BOOST_CHECK_EQUAL(static_cast<uint8_t>(POLYLINE), newcommon.order);
        BOOST_CHECK_EQUAL(Rect(0, 0, 0, 0), newcommon.clip);

        uint8_t datas[] = {
            CHANGE | STANDARD,
            POLYLINE,
            0x67,                   // header fields
            0x9E, 0x00, 0xE6, 0x00, // xStart = 158, yStart = 203
            0x0D,                   // bRop2
            0x07,                   // NumDeltaEntries
            0x0D,                   // cbData
            0x98, 0x24, 0x14, 0x80, 0xA0, 0x62, 0x32, 0x32,
            0x4E, 0x32, 0x62, 0xFF, 0x60
        };
        check_datas(out_stream.get_offset(), out_stream.get_data(), sizeof(datas), datas, "Polyline 1");

        InStream in_stream(out_stream.get_data(), out_stream.get_offset());

        RDPOrderCommon common_cmd = state_common;
        uint8_t control = in_stream.in_uint8();
        BOOST_CHECK_EQUAL(true, !!(control & STANDARD));
        RDPPrimaryOrderHeader header = common_cmd.receive(in_stream, control);

        BOOST_CHECK_EQUAL(static_cast<uint8_t>(0x09), header.control);
        BOOST_CHECK_EQUAL(static_cast<uint32_t>(0x67), header.fields);
        BOOST_CHECK_EQUAL(static_cast<uint8_t>(POLYLINE), common_cmd.order);
        BOOST_CHECK_EQUAL(Rect(0, 0, 0, 0), common_cmd.clip);

        RDPPolyline cmd = state_polyline;
        cmd.receive(in_stream, header);

        deltaPoints.rewind();

        deltaPoints.out_sint16_le(0);
        deltaPoints.out_sint16_le(20);

        deltaPoints.out_sint16_le(160);
        deltaPoints.out_sint16_le(0);

        deltaPoints.out_sint16_le(0);
        deltaPoints.out_sint16_le(-30);

        deltaPoints.out_sint16_le(50);
        deltaPoints.out_sint16_le(50);

        deltaPoints.out_sint16_le(-50);
        deltaPoints.out_sint16_le(50);

        deltaPoints.out_sint16_le(0);
        deltaPoints.out_sint16_le(-30);

        deltaPoints.out_sint16_le(-160);
        deltaPoints.out_sint16_le(0);

        deltaPoints_in = InStream(deltaPoints.get_data(), deltaPoints.get_offset());

        check<RDPPolyline>(common_cmd, cmd,
            RDPOrderCommon(POLYLINE, Rect(0, 0, 0, 0)),
            RDPPolyline(158, 230, 0x0D, 0, RDPColor{0x000000}, 7, deltaPoints_in),
            "Polyline 2");
    }
}
