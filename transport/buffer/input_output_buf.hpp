/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Raphael Zhou, Jonathan Poelen, Meng Tan
 */

#ifndef REDEMPTION_TRANSPORT_BUFFER_INPUT_OUTPUT_BUF_HPP
#define REDEMPTION_TRANSPORT_BUFFER_INPUT_OUTPUT_BUF_HPP

#include "params.hpp"
#include <unistd.h>

namespace transbuf
{

    struct open_close_base
    {
        template<class Buf>
        static int init(Buf & buf) /*noexcept*/
        { return buf.init(); }

        template<class Buf>
        static int close(Buf & buf) /*noexcept*/
        { return buf.close(); }

        template<class Buf>
        static bool is_open(const Buf & buf) /*noexcept*/
        { return buf.is_open(); }
    };


    template<class Reader, class OpenClosePolicy = open_close_base>
    class input_buf
    : private OpenClosePolicy
    {
        Reader buf;

    public:
        input_buf()
        {}

        template<class T>
        input_buf(const T & params)
        : OpenClosePolicy(params)
        {}

        template<class BufParams, class OtherParams>
        input_buf(const two_params<BufParams, OtherParams> & params)
        : OpenClosePolicy(params.other_params)
        , buf(params.buf_params)
        {}

        template<class OtherParams>
        input_buf(const two_params<no_param_t, OtherParams> & params)
        : OpenClosePolicy(params.other_params)
        {}

        template<class BufParams>
        input_buf(const two_params<BufParams, no_param_t> & params)
        : buf(params.buf_params)
        {}

        ~input_buf()
        {
            if (this->is_open()) {
                this->close();
            }
        }

        ssize_t read(void * data, size_t len) /*noexcept*/
        {
            if (!this->is_open()) {
                const int res = OpenClosePolicy::init(this->buf);
                if (res < 0) {
                    return res;
                }
            }
            return this->buf.read(data, len);
        }

        int close() /*noexcept*/
        { return OpenClosePolicy::close(this->buf); }

        bool is_open() const /*noexcept*/
        { return OpenClosePolicy::is_open(this->buf); }

        off_t seek(off_t offset, int whence) /*noexcept*/
        {
            if (!this->is_open()) {
                const int res = OpenClosePolicy::init(this->buf);
                if (res < 0) {
                    return res;
                }
            }
            return this->buf.seek(offset, whence);
        }

        OpenClosePolicy & policy() /*noexcept*/
        { return *this; }

        const OpenClosePolicy & policy() const /*noexcept*/
        { return *this; }
    };

    template<class Writer, class OpenClosePolicy = open_close_base>
    class output_buf
    : private OpenClosePolicy
    {
        Writer buf;

    public:
        output_buf()
        {}

        template<class T>
        output_buf(const T & params)
        : OpenClosePolicy(params)
        {}

        template<class BufParams, class OtherParams>
        output_buf(const two_params<BufParams, OtherParams> & params)
        : OpenClosePolicy(params.other_params)
        , buf(params.buf_params)
        {}

        template<class OtherParams>
        output_buf(const two_params<no_param_t, OtherParams> & params)
        : OpenClosePolicy(params.other_params)
        {}

        template<class BufParams>
        output_buf(const two_params<BufParams, no_param_t> & params)
        : buf(params.buf_params)
        {}

        ~output_buf()
        {
            if (this->is_open()) {
                this->close();
            }
        }

        ssize_t write(const void * data, size_t len) /*noexcept*/
        {
            if (!this->is_open()) {
                const int res = OpenClosePolicy::init(this->buf);
                if (res < 0) {
                    return res;
                }
            }
            return this->buf.write(data, len);
        }

        bool is_open() const /*noexcept*/
        { return OpenClosePolicy::is_open(this->buf); }

        off_t seek(off_t offset, int whence) /*noexcept*/
        {
            if (!this->is_open()) {
                const int res = OpenClosePolicy::init(this->buf);
                if (res < 0) {
                    return res;
                }
            }
            return this->buf.seek(offset, whence);
        }

        int close() /*noexcept*/
        { return OpenClosePolicy::close(this->buf); }

        OpenClosePolicy & policy() /*noexcept*/
        { return *this; }

        const OpenClosePolicy & policy() const /*noexcept*/
        { return *this; }
    };


    template<class Reader, class OpenClosePolicy = open_close_base>
    class reopen_input
    : public input_buf<Reader, OpenClosePolicy>
    {
        typedef input_buf<Reader, OpenClosePolicy> base_buf;

    public:
        reopen_input()
        {}

        template<class T>
        reopen_input(const T & params)
        : base_buf(params)
        {}

        ssize_t read(void * data, size_t len) /*noexcept*/
        {
            ssize_t res = this->base_buf::read(data, len);
            if (res < 0) {
                return res;
            }
            if (size_t(res) != len) {
                ssize_t res2 = res;
                do {
                    if (const ssize_t err = this->close()) {
                        return res;
                    }
                    reinterpret_cast<char*&>(data) += res2;
                    len -= res2;
                    res2 = this->base_buf::read(data, len);
                    if (res2 < 0) {
                        return res;
                    }
                    res += res2;
                } while (size_t(res2) != len);
            }
            return res;
        }
    };

}

#endif
