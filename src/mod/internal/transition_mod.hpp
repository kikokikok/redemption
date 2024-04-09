/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2019
 *   Author(s): Meng Tan
 *
 *   Show a waiting message in modules transitions,
 *   also allow to quit with escape key
 */


#pragma once

#include "mod/internal/rail_mod_base.hpp"
#include "utils/static_string.hpp"

class TransitionMod : public RailInternalModBase
{
public:
    TransitionMod(
        chars_view message,
        gdi::GraphicApi & drawable,
        uint16_t width, uint16_t height,
        Rect const widget_rect, ClientExecute & rail_client_execute, Font const& font,
        Theme const& theme
    );

    void rdp_input_scancode(KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap) override;

    void rdp_input_invalidate(Rect r) override;

    void acl_update(AclFieldMask const&/* acl_fields*/) override {}

private:
    static_string<127> ttmessage;
    gdi::GraphicApi & drawable;
    Font const& font;
    Rect widget_rect;
    BGRColor fgcolor;
    BGRColor bgcolor;
    BGRColor border_color;
};
