/*
SPDX-FileCopyrightText: 2024 Wallix Proxies Team
SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "utils/sugar/zstring_view.hpp"

struct TrKey
{
    zstring_view translation;
};

template<class T> struct TrKeyFmt
{
    zstring_view translation;
};
