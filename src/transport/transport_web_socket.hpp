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
   Copyright (C) Wallix 2012
   Author(s): Clement Moroldo

   Transport layer abstraction
*/


#ifndef TRANSPORT_WEB_SOCKET_HPP
#define TRANSPORT_WEB_SOCKET_HPP


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#include "transport/transport.hpp"
#include "mod/rdp/rdp.hpp"


// bjam -a test_transport_web_socket |& grep error || iceweasel file:///home/cmoroldo/Bureau/redemption/projects/browser_client_JS/sandbox/test_transport_web_socket.html

// source emsdk_portable/emsdk_env.sh

class FrontAPI;

class TransportWebSocket :  public Transport
{
    char     *  buffer;
    uint8_t     pduState;
    size_t      filledSize;
    size_t      pduSize;

    size_t      sentSize;
    mod_rdp  *  callback;
    FrontAPI *  drawable;

    enum : uint8_t {
        PDU_HEADER_FLAG = 0x03,
        PDU_HEADER_SIZE = 4
    };

    enum : uint8_t {
        PUD_HEADER_EMPTY,
        PUD_HEADER_OCT_1,
        PUD_HEADER_OCT_2,
        PUD_HEADER_OCT_3,
        PUD_HEADER_OCT_4,
    };



    void do_send(const char * const buffer, size_t len) override {
        EM_ASM_({ send_to_serveur(HEAPU8.subarray($0, $0 + $1 - 1), $1); }, buffer, len);
    }

    void do_recv(char ** pbuffer, size_t len) override {
        if (this->buffer !=  nullptr) {
            int lenMax(len);

            if (lenMax > 0) {

                //std::copy(std::begin(this->buffer), std::end(this->buffer+len), std::begin(*pbuffer));

                for (int i = 0; i < len; i++) {
                    (*pbuffer)[i] = this->buffer[i + this->sentSize];
                }
                this->sentSize += len;
                *pbuffer += lenMax;

            } else {
                EM_ASM_({ console.log('do_recv len='+$0); }, len);
            }
        } else {

        }
    }


public:
    TransportWebSocket(FrontAPI * draw)
      : Transport()
      , buffer(nullptr)
      , pduState(0)
      , filledSize(0)
      , pduSize(0)
      , sentSize(0)
      , callback(nullptr)
      , drawable(draw)
      {}

    void setMod(mod_rdp * mod) {
        this->callback = mod;
    }

    void setBufferValue(uint8_t octet) {

    //      Format of the TPKT packet header
    //
    //           -----------------------------
    //           |         0000 0011         | 1
    //           -----------------------------
    //           |         Reserved 2        | 2
    //           -----------------------------
    //           | Most significant octet    | 3
    //           |    of TPKT length         |
    //           -----------------------------
    //           | least significant octet   | 4
    //           |       of TPKT length      |
    //           -----------------------------
    //           :         TPDU              : 5-?
    //           - - - - - - - - - - - - - - -

        switch (this->pduState) {
            case PUD_HEADER_EMPTY: if (octet == PDU_HEADER_FLAG) {
                                        this->pduState = PUD_HEADER_OCT_1;
                                   }
                break;

            case PUD_HEADER_OCT_1: this->pduState = PUD_HEADER_OCT_2;
                break;

            case PUD_HEADER_OCT_2: this->sentSize = 0;
                                   this->pduSize += octet << 8;
                                   this->pduState = PUD_HEADER_OCT_3;
                break;

            case PUD_HEADER_OCT_3: this->pduSize += octet;
                                   if (this->callback !=  nullptr) {
                                        this->buffer = new char[this->pduSize];
                                   }
                                   this->buffer[0] = PDU_HEADER_FLAG;
                                   this->buffer[1] = 0x00;  // reserved for further study
                                   this->buffer[2] = uint8_t(this->pduSize >> 8);
                                   this->buffer[3] = uint8_t(this->pduSize);

                                   this->filledSize = PDU_HEADER_SIZE;
                                   this->pduState = PUD_HEADER_OCT_4;
                break;

            case PUD_HEADER_OCT_4: this->buffer[filledSize] = octet;
                                   this->filledSize++;
                                   if (this->filledSize == this->pduSize) {

                                       if (this->callback !=  nullptr) {
                                           while (this->sentSize < this->pduSize) {
                                                this->callback->draw_event(time_t(nullptr), *(this->drawable));
                                           }
                                           this->sentSize = 0;
                                           this->pduSize  = 0;
                                           delete (this->buffer -= this->sentSize);
                                           this->buffer = nullptr;

                                       }

                                       this->pduState = PUD_HEADER_EMPTY;
                                   }
                break;

            default:
                break;
        }
    }


};


#endif
