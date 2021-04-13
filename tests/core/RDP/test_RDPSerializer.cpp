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
   Copyright (C) Wallix 2013
   Author(s): Christophe Grosjean

*/

#include "test_only/test_framework/redemption_unit_tests.hpp"

#include "core/RDP/caches/bmpcache.hpp"
#include "core/RDP/caches/glyphcache.hpp"
#include "core/RDP/RDPSerializer.hpp"

class FakeSerializer : public RDPSerializer
{
public:
    using RDPSerializer::RDPSerializer;

protected:
    void flush_orders() override {}
    void flush_bitmaps() override {}

    void new_pointer(gdi::CachePointerIndex cache_idx, const RdpPointerView & cursor) override
    {
        (void)cache_idx;
        (void)cursor;
    }

    void cached_pointer(gdi::CachePointerIndex cache_idx) override
    {
        (void)cache_idx;
    }
};

RED_AUTO_TEST_CASE(TestXXX)
{
    uint8_t ori_bmp_data[] = {
/* 0000 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0010 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0020 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0030 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0040 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0050 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0060 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0070 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0080 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0090 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 00a0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 00b0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 00c0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 00d0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 00e0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 00f0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0100 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0110 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0120 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0130 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0140 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0150 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0160 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0170 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0180 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0190 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 01a0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 01b0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 01c0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 01d0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 01e0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 01f0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0200 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0210 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0220 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0230 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0240 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0250 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0260 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0270 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0280 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0290 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 02a0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 02b0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 02c0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 02d0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 02e0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 02f0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0300 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0310 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0320 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0330 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0340 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0350 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0360 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0370 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0380 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0390 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 03a0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 03b0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 03c0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 03d0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 03e0 */ "\x46\x29\x25\x21\x46\x31\x25\x21\x45\x29\x25\x21\x46\x31\x45\x21" // F)%!F1%!E)%!F1E!
/* 03f0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0400 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0410 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0420 */ "\x86\x31\xa7\x31\x05\x21\x45\x29\x04\x21\x45\x21\x05\x21\x45\x29" // .1.1.!E).!E!.!E)
/* 0430 */ "\x04\x21\x45\x21\x05\x21\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .!E!.!.1.1.1.1.1
/* 0440 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0450 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0460 */ "\xf7\xbd\x9a\xd6\x79\xce\x9a\xd6\x79\xce\x9a\xd6\x79\xce\x18\xc6" // ....y...y...y...
/* 0470 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0480 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0490 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 04a0 */ "\xa7\x39\xa7\x31\x9a\xd6\x7a\xd6\x9a\xd6\x7a\xd6\x9a\xd6\x7a\xd6" // .9.1..z...z...z.
/* 04b0 */ "\x9a\xd6\x7a\xd6\x9a\xd6\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // ..z....1.9.1.1.1
/* 04c0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 04d0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 04e0 */ "\x9a\xd6\x7a\xd6\x9a\xd6\x7a\xd6\x9a\xd6\x7a\xd6\x9a\xd6\x7a\xd6" // ..z...z...z...z.
/* 04f0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0500 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0510 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0520 */ "\x87\x31\xa7\x31\x79\xce\x9a\xd6\x79\xce\x9a\xd6\x79\xce\x9a\xd6" // .1.1y...y...y...
/* 0530 */ "\x79\xce\x9a\xd6\x79\xce\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // y...y..1.1.1.1.1
/* 0540 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0550 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0560 */ "\x79\xce\x9a\xd6\x86\x31\xa7\x31\x87\x31\xa7\x31\x79\xce\x9a\xd6" // y....1.1.1.1y...
/* 0570 */ "\x05\x21\x45\x29\x24\x21\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .!E)$!.1.1.1.1.1
/* 0580 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0590 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 05a0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 05b0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 05c0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 05d0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 05e0 */ "\x9a\xd6\x7a\xd6\x46\x31\x25\x21\x45\x29\x25\x21\x9a\xd6\x7a\xd6" // ..z.F1%!E)%!..z.
/* 05f0 */ "\x9a\xd6\x7a\xd6\x14\xa5\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // ..z....1.1.1.9.1
/* 0600 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0610 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0620 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0630 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0640 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0650 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0660 */ "\x79\xce\x9a\xd6\x79\xce\x9a\xd6\x79\xce\x9a\xd6\x79\xce\x9a\xd6" // y...y...y...y...
/* 0670 */ "\x79\xce\x9a\xd6\x79\xce\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // y...y..1.1.1.1.1
/* 0680 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0690 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 06a0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 06b0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 06c0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 06d0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 06e0 */ "\x39\xce\x7a\xd6\x9a\xd6\x7a\xd6\x9a\xd6\x7a\xd6\x9a\xd6\x18\xc6" // 9.z...z...z.....
/* 06f0 */ "\xa7\x39\x7a\xd6\x9a\xd6\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9z....1.9.1.1.1
/* 0700 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0710 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0720 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0730 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0740 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0750 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0760 */ "\x87\x31\xa7\x31\x86\x31\x9a\xd6\x79\xce\x45\x29\x04\x21\x45\x21" // .1.1.1..y.E).!E!
/* 0770 */ "\x05\x21\x9a\xd6\x79\xce\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .!..y..1.1.1.1.1
/* 0780 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0790 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 07a0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 07b0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 07c0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 07d0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 07e0 */ "\xa7\x31\xa7\x31\xa7\x39\x7a\xd6\x9a\xd6\x7a\xd6\x9a\xd6\x7a\xd6" // .1.1.9z...z...z.
/* 07f0 */ "\x9a\xd6\x7a\xd6\x9a\xd6\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // ..z....1.1.1.9.1
/* 0800 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0810 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0820 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0830 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0840 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0850 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0860 */ "\x86\x31\xa7\x31\x67\x31\x55\xad\x79\xce\x9a\xd6\x79\xce\x9a\xd6" // .1.1g1U.y...y...
/* 0870 */ "\x79\xce\x9a\xd6\x35\xa5\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // y...5..1.1.1.1.1
/* 0880 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0890 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 08a0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 08b0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 08c0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 08d0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 08e0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 08f0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0900 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0910 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0920 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0930 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0940 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0950 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0960 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0970 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0980 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0990 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 09a0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 09b0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 09c0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 09d0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 09e0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 09f0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0a00 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a10 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a20 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a30 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a40 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a50 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a60 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a70 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0a80 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0a90 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0aa0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0ab0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0ac0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0ad0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0ae0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0af0 */ "\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31" // .9.1.1.1.9.1.1.1
/* 0b00 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b10 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b20 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b30 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b40 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b50 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b60 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b70 */ "\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0b80 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0b90 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0ba0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0bb0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0bc0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0bd0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0be0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0bf0 */ "\xa7\x31\xa7\x31\xa7\x39\xa7\x31\xa7\x31\xa7\x31\xa7\x39\xa7\x31" // .1.1.9.1.1.1.9.1
/* 0c00 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0c10 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0c20 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0c30 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0c40 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0c50 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0c60 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
/* 0c70 */ "\x86\x31\xa7\x31\x87\x31\xa7\x31\x86\x31\xa7\x31\x87\x31\xa7\x31" // .1.1.1.1.1.1.1.1
    };

    Bitmap bogus(BitsPerPixel{16}, BitsPerPixel{16}, nullptr, 64, 25, ori_bmp_data, 3200);

    uint8_t const bpp = 16;

    bool const use_waiting_list = false;

    BmpCache bmp_cache( BmpCache::Recorder, BitsPerPixel{bpp}, 3, use_waiting_list
                       , BmpCache::CacheOption(120,  nbbytes(bpp) * 16 * 16, false)
                       , BmpCache::CacheOption(120,  nbbytes(bpp) * 32 * 32, false)
                       , BmpCache::CacheOption(2553, nbbytes(bpp) * 64 * 64, true)
                       , BmpCache::CacheOption()
                       , BmpCache::CacheOption()
                       , BmpCache::Verbose::none);

    GlyphCache gly_cache;

    StaticOutStream<65535> stream_orders;
    StaticOutStream<65535> stream_bitmaps;

    int const bitmap_cache_version = 2;
    bool const use_bitmap_comp = true;
    bool const use_compact_packets = true;
    size_t const max_data_block_size = 3400;
    bool const experimental_enable_serializer_data_block_size_limit = true;

    FakeSerializer serializer(stream_orders, stream_bitmaps, BitsPerPixel{bpp},
        bmp_cache, gly_cache, bitmap_cache_version, use_bitmap_comp,
        use_compact_packets, max_data_block_size, experimental_enable_serializer_data_block_size_limit, RDPSerializerVerbose::internal_buffer);

    serializer.draw(RDPMemBlt(0, Rect(300, 100, bogus.cx(), bogus.cy()), 0xCC, 0, 0, 0), Rect(0, 0, bogus.cx(), bogus.cy()), bogus);

    RED_CHECK_EQUAL(3236, stream_orders.get_offset());
}
