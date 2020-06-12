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
Copyright (C) Wallix 2010-2019
Author(s): Jonathan Poelen
*/

#pragma once

#include "configs/attributes/spec.hpp"
#include "configs/generators/utils/spec_writer.hpp"
#include "configs/generators/utils/write_template.hpp"
#include "configs/generators/python_spec.hpp"
#include "configs/enumeration.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <cerrno>
#include <cstring>


namespace cfg_generators {

namespace sesman_default_map {

using namespace cfg_attributes;

namespace python
{
    template<class T, class R = void>
    constexpr bool is_integral_type_v =
        std::is_base_of<types::integer_base, T>::value
     || std::is_integral<T>::value;

    template<class T, class R = void>
    using enable_if_integral_type = std::enable_if_t<is_integral_type_v<T>, R>;

    template<class T>
    void write_type(std::ostream& out, type_<std::string>, T const& x)
    {
        out << "u'" << io_quoted2{x} << "'";
    }

    template<class T, unsigned n>
    void write_type(std::ostream& out, type_<types::fixed_string<n>>, T const& x)
    {
        if constexpr (std::is_same_v<types::fixed_string<n>, T>) {
            out << "u''";
        }
        else {
            out << "u'" << io_quoted2{x} << "'";
        }
    }

    template<class Int, class T>
    enable_if_integral_type<Int>
    write_type(std::ostream& out, type_<types::list<Int>>, T const& x)
    {
        out << "u'" << io_quoted2{x} << "'";
    }

    inline void write_type(std::ostream& out, type_<bool>, bool x)
    {
        out << (x ? "True" : "False");
    }

    template<class T, class X>
    void write_type(std::ostream& out, type_<T>, X const& x)
    {
        if constexpr (std::is_enum_v<T>) {
            out << +std::underlying_type_t<T>(x);
        }
        else if constexpr (is_integral_type_v<T>) {
            out << +python_spec_writer::impl::stringize_integral(x);
        }
        else {
            static_assert(!sizeof(T), "missing implementation");
        }
    }

    template<class T1, class Ratio1, class T2, class Ratio2>
    void write_type(std::ostream& out, type_<std::chrono::duration<T1, Ratio1>>, std::chrono::duration<T2, Ratio2> const& d)
    {
        out << +std::chrono::duration_cast<std::chrono::duration<T1, Ratio1>>(d).count();
    }

    template<class T1, class Ratio1, class T>
    void write_type(std::ostream& out, type_<std::chrono::duration<T1, Ratio1>>, T const& i)
    {
        out << +python_spec_writer::impl::stringize_integral(i);
    }

    template<class T1, long min, long max, class T>
    void write_type(std::ostream& out, type_<types::range<T1, min, max>>, T const& x)
    {
        write_type(out, type_<T>{}, x);
    }
}

class SesmanDefaultMapBase
{
    std::string filename;
    std::ofstream out;
    std::vector<std::string> values_sent;
    std::vector<std::string> values_reinit;
    std::ostringstream buf;

public:
    using attribute_name_type = sesman::name;

    SesmanDefaultMapBase(std::string filename)
    : filename(std::move(filename))
    {}

    template<class Pack>
    void evaluate_member(Names const& /*names*/, std::string const&, Pack const& infos, type_enumerations&)
    {
        if constexpr (is_convertible_v<Pack, decltype(sesman::constants::is_target_ctx)>)
        {
            auto sesman_type = get_type<sesman::type_>(infos);
            this->buf << "    u'" << get_name<sesman::name>(infos) << "': ";
            python::write_type(this->buf, sesman_type, get_default(sesman_type, infos));
            this->buf << ",\n";
            if (sesman::constants::proxy_to_sesman == value_or<sesman_io_t>(infos,
                sesman_io_t{sesman::internal::io::proxy_to_sesman}).value)
            {
                this->values_reinit.emplace_back(this->buf.str());
            }
            else
            {
                this->values_sent.emplace_back(this->buf.str());
            }
            this->buf.str("");
        }
    }

    void do_start_section(Names const& /*names*/, std::string const & /*section_name*/)
    {}

    void do_stop_section(Names const& /*names*/, std::string const & /*section_name*/)
    {}

    void do_init()
    {
        out.open(filename);
        out <<
            "#!/usr/bin/python -O\n"
            "# -*- coding: utf-8 -*-\n\n"
            "# DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN\n\n"
        ;
    }

    int do_finish()
    {
        std::sort(this->values_sent.begin(), this->values_sent.end());
        std::sort(this->values_reinit.begin(), this->values_reinit.end());
        auto write = [&](char const* name, std::vector<std::string> const& values){
            out << name << " = {\n";
            for (auto&& x : values)
            {
                out << x;
            }
            out << "}\n";
        };
        write("back_to_selector_default_sent", this->values_sent);
        write("back_to_selector_default_reinit", this->values_reinit);
        out.flush();
        int errnum = 0;
        if (!out)
        {
            errnum = errno;
            std::cerr << "SesmanDefaultMapBase: " << filename << ": " << strerror(errnum) << "\n";
            return errnum ? errnum : 1;
        }
        return errnum;
    }
};

}

}
