/*
SPDX-FileCopyrightText: 2024 Wallix Proxies Team
SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "utils/trkey.hpp"


class ErrorMessageCtx
{
public:
    ErrorMessageCtx() = default;
    ErrorMessageCtx(ErrorMessageCtx const&) = delete;
    ErrorMessageCtx& operator=(ErrorMessageCtx const&) = delete;

    void set_msg(TrKey k)
    {
        trkey = k;
        translated_msg.clear();
    }

    void set_msg(zstring_view msg)
    {
        translated_msg = msg;
        trkey.translation = {};
    }

    void set_msg(std::string&& msg)
    {
        translated_msg = std::move(msg);
        trkey.translation = {};
    }

    void clear()
    {
        trkey.translation = {};
        translated_msg.clear();
    }

    bool is_translated() const
    {
        return trkey.translation.empty();
    }

    zstring_view get_msg() const
    {
        if (!trkey.translation.empty()) {
            return trkey.translation;
        }
        return translated_msg;
    }

private:
    TrKey trkey;
    std::string translated_msg;
};
