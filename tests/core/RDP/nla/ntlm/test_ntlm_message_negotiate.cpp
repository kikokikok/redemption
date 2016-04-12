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
    Author(s): Christophe Grosjean, Raphael Zhou, Meng Tan
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestNtlmMessageNegotiate
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL

#include "core/RDP/nla/credssp.hpp"
#include "core/RDP/nla/ntlm/ntlm_message_negotiate.hpp"

#include "check_sig.hpp"

BOOST_AUTO_TEST_CASE(TestNegotiate)
{
    // ===== NTLMSSP_NEGOTIATE =====
    uint8_t packet[] = {
        0x30, 0x37, 0xa0, 0x03, 0x02, 0x01, 0x02, 0xa1,
        0x30, 0x30, 0x2e, 0x30, 0x2c, 0xa0, 0x2a, 0x04,
        0x28, 0x4e, 0x54, 0x4c, 0x4d, 0x53, 0x53, 0x50,
        0x00, 0x01, 0x00, 0x00, 0x00, 0xb7, 0x82, 0x08,
        0xe2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x05, 0x01, 0x28, 0x0a, 0x00, 0x00, 0x00,
        0x0f
    };

    StaticOutStream<65536> s;

    s.out_copy_bytes(packet, sizeof(packet));

    uint8_t sig[20];
    get_sig(s, sig, sizeof(sig));

    InStream in_s(s.get_data(), s.get_offset());
    TSRequest ts_req(in_s);

    BOOST_CHECK_EQUAL(ts_req.version, 2);

    BOOST_CHECK_EQUAL(ts_req.negoTokens.size(), 0x28);
    BOOST_CHECK_EQUAL(ts_req.authInfo.size(), 0);
    BOOST_CHECK_EQUAL(ts_req.pubKeyAuth.size(), 0);

    StaticOutStream<65536> to_send;

    BOOST_CHECK_EQUAL(to_send.get_offset(), 0);
    ts_req.emit(to_send);

    BOOST_CHECK_EQUAL(to_send.get_offset(), 0x37 + 2);

    char message[1024];
    if (!check_sig(to_send, message, reinterpret_cast<const char *>(sig))){
        BOOST_CHECK_MESSAGE(false, message);
    }


    NTLMNegotiateMessage NegoMsg;

    InStream nego(ts_req.negoTokens.get_data(), ts_req.negoTokens.size());
    NegoMsg.recv(nego);


    BOOST_CHECK_EQUAL(NegoMsg.negoFlags.flags, 0xe20882b7);
    // NegoMsg.negoFlags.print();

    BOOST_CHECK_EQUAL(NegoMsg.DomainName.len, 0);
    BOOST_CHECK_EQUAL(NegoMsg.DomainName.bufferOffset, 0);
    // hexdump_c(NegoMsg.DomainName.Buffer.get_data(),
    //           NegoMsg.DomainName.Buffer.size());
    // uint8_t domainname_match[] = "";
    // BOOST_CHECK_EQUAL(memcmp(domainname_match,
    //                          NegoMsg.DomainName.Buffer.get_data(),
    //                          NegoMsg.DomainName.len),
    //                   0);

    BOOST_CHECK_EQUAL(NegoMsg.Workstation.len, 0);
    BOOST_CHECK_EQUAL(NegoMsg.Workstation.bufferOffset, 0);
    // uint8_t workstation_match[] = "";
    // BOOST_CHECK_EQUAL(memcmp(workstation_match,
    //                          NegoMsg.Workstation.Buffer.get_data(),
    //                          NegoMsg.Workstation.len),
    //                   0);
    // hexdump_c(NegoMsg.DomainName.Buffer.get_data(),
    //           NegoMsg.Workstation.Buffer.size());

    NegoMsg.version.print();

    // // hexdump_c(to_send.get_data(), to_send.size());
    // BStream outMsg;
    // NegoMsg.emit(outMsg);
    // hexdump_c(outMsg.get_data(), outMsg.size());


}



// // ===== PUBKEYAUTH =====
// uint8_t packet4[] = {
//     0x30, 0x82, 0x01, 0x2b, 0xa0, 0x03, 0x02, 0x01,
//     0x02, 0xa3, 0x82, 0x01, 0x22, 0x04, 0x82, 0x01,
//     0x1e, 0x01, 0x00, 0x00, 0x00, 0xc9, 0x88, 0xfc,
//     0xf1, 0x11, 0x68, 0x2c, 0x72, 0x00, 0x00, 0x00,
//     0x00, 0xc7, 0x51, 0xf4, 0x71, 0xd3, 0x9f, 0xb6,
//     0x50, 0xbe, 0xa8, 0xf6, 0x20, 0x77, 0xa1, 0xfc,
//     0xdd, 0x8e, 0x02, 0xf0, 0xa4, 0x6b, 0xba, 0x3f,
//     0x9d, 0x65, 0x9d, 0xab, 0x4a, 0x95, 0xc9, 0xb4,
//     0x38, 0x03, 0x87, 0x04, 0xb1, 0xfe, 0x42, 0xec,
//     0xfa, 0xfc, 0xaa, 0x85, 0xf1, 0x31, 0x2d, 0x26,
//     0xcf, 0x63, 0xfd, 0x62, 0x36, 0xcf, 0x56, 0xc3,
//     0xfb, 0xf6, 0x36, 0x9b, 0xe5, 0xb2, 0xe7, 0xce,
//     0xcb, 0xe1, 0x82, 0xb2, 0x89, 0xff, 0xdd, 0x87,
//     0x5e, 0xd3, 0xd8, 0xff, 0x2e, 0x16, 0x35, 0xad,
//     0xdb, 0xda, 0xc9, 0xc5, 0x81, 0xad, 0x48, 0xf1,
//     0x8b, 0x76, 0x3d, 0x74, 0x34, 0xdf, 0x80, 0x6b,
//     0xf3, 0x68, 0x6d, 0xf6, 0xec, 0x5f, 0xbe, 0xea,
//     0xb7, 0x6c, 0xea, 0xe4, 0xeb, 0xe9, 0x17, 0xf9,
//     0x4e, 0x0d, 0x79, 0xd5, 0x82, 0xdd, 0xb7, 0xdc,
//     0xcd, 0xfc, 0xbb, 0xf1, 0x0b, 0x9b, 0xe9, 0x18,
//     0xe7, 0xb3, 0xb3, 0x8b, 0x40, 0x82, 0xa0, 0x9d,
//     0x58, 0x73, 0xda, 0x54, 0xa2, 0x2b, 0xd2, 0xb6,
//     0x41, 0x60, 0x8a, 0x64, 0xf2, 0xa2, 0x59, 0x64,
//     0xcf, 0x27, 0x1a, 0xe6, 0xb5, 0x1a, 0x0e, 0x0e,
//     0xe1, 0x14, 0xef, 0x26, 0x68, 0xeb, 0xc8, 0x49,
//     0xe2, 0x66, 0xbb, 0x11, 0x71, 0x49, 0xad, 0x7e,
//     0xae, 0xde, 0xa8, 0x78, 0xfd, 0x64, 0x51, 0xd8,
//     0x18, 0x01, 0x11, 0xc0, 0x8d, 0x3b, 0xec, 0x40,
//     0x2b, 0x1f, 0xc5, 0xa4, 0x45, 0x1e, 0x07, 0xae,
//     0x5a, 0xd8, 0x1c, 0xab, 0xdf, 0x89, 0x96, 0xdc,
//     0xdc, 0x29, 0xd8, 0x30, 0xdb, 0xbf, 0x48, 0x2a,
//     0x42, 0x27, 0xc2, 0x50, 0xac, 0xf9, 0x02, 0xd1,
//     0x20, 0x12, 0xdd, 0x50, 0x22, 0x09, 0x44, 0xac,
//     0xe0, 0x22, 0x1f, 0x66, 0x64, 0xec, 0xfa, 0x2b,
//     0xb8, 0xcd, 0x43, 0x3a, 0xce, 0x40, 0x74, 0xe1,
//     0x34, 0x81, 0xe3, 0x94, 0x47, 0x6f, 0x49, 0x01,
//     0xf8, 0xb5, 0xfc, 0xd0, 0x75, 0x80, 0xc6, 0x35,
//     0xac, 0xc0, 0xfd, 0x1b, 0xb5, 0xa2, 0xd3
// };

// LOG(LOG_INFO, "=================================\n");
// s.init(sizeof(packet4));
// s.out_copy_bytes(packet4, sizeof(packet4));
// s.mark_end();
// s.rewind();
// get_sig(s, sig);

// TSRequest ts_req4(s);

// BOOST_CHECK_EQUAL(ts_req4.version, 2);

// BOOST_CHECK_EQUAL(ts_req4.negoTokens.size(), 0);
// BOOST_CHECK_EQUAL(ts_req4.authInfo.size(), 0);
// BOOST_CHECK_EQUAL(ts_req4.pubKeyAuth.size(), 0x11e);

// BStream to_send4;

// BOOST_CHECK_EQUAL(to_send4.size(), 0);
// ts_req4.emit(to_send4);

// BOOST_CHECK_EQUAL(to_send4.size(), 0x12b + 4);

// if (!check_sig(to_send4, message, (const char *)sig)){
//     BOOST_CHECK_MESSAGE(false, message);
// }

// // hexdump_c(to_send4.get_data(), to_send4.size());


// // ===== AUTHINFO =====
// uint8_t packet5[] = {
//     0x30, 0x5a, 0xa0, 0x03, 0x02, 0x01, 0x02, 0xa2,
//     0x53, 0x04, 0x51, 0x01, 0x00, 0x00, 0x00, 0xb3,
//     0x2c, 0x3b, 0xa1, 0x36, 0xf6, 0x55, 0x71, 0x01,
//     0x00, 0x00, 0x00, 0xa8, 0x85, 0x7d, 0x11, 0xef,
//     0x92, 0xa0, 0xd6, 0xff, 0xee, 0xa1, 0xae, 0x6d,
//     0xc5, 0x2e, 0x4e, 0x65, 0x50, 0x28, 0x93, 0x75,
//     0x30, 0xe1, 0xc3, 0x37, 0xeb, 0xac, 0x1f, 0xdd,
//     0xf3, 0xe0, 0x92, 0xf6, 0x21, 0xbc, 0x8f, 0xa8,
//     0xd4, 0xe0, 0x5a, 0xa6, 0xff, 0xda, 0x09, 0x50,
//     0x24, 0x0d, 0x8f, 0x8f, 0xf4, 0x92, 0xfe, 0x49,
//     0x2a, 0x13, 0x52, 0xa6, 0x52, 0x75, 0x50, 0x8d,
//     0x3e, 0xe9, 0x6b, 0x57
// };
// LOG(LOG_INFO, "=================================\n");
// s.init(sizeof(packet5));
// s.out_copy_bytes(packet5, sizeof(packet5));
// s.mark_end();
// s.rewind();
// get_sig(s, sig);

// TSRequest ts_req5(s);

// BOOST_CHECK_EQUAL(ts_req5.version, 2);

// BOOST_CHECK_EQUAL(ts_req5.negoTokens.size(), 0);
// BOOST_CHECK_EQUAL(ts_req5.authInfo.size(), 0x51);
// BOOST_CHECK_EQUAL(ts_req5.pubKeyAuth.size(), 0);

// BStream to_send5;

// BOOST_CHECK_EQUAL(to_send5.size(), 0);
// ts_req5.emit(to_send5);

// BOOST_CHECK_EQUAL(to_send5.size(), 0x5a + 2);

// if (!check_sig(to_send5, message, (const char *)sig)){
//     BOOST_CHECK_MESSAGE(false, message);
// }

// // hexdump_c(to_send5.get_data(), to_send5.size());
