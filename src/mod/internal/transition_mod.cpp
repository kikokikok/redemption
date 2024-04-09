/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2019
 *   Author(s): Meng Tan
 */

#include "mod/internal/transition_mod.hpp"
#include "gdi/text_metrics.hpp"
#include "gdi/draw_utils.hpp"
#include "core/font.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"

TransitionMod::TransitionMod(
    chars_view message,
    gdi::GraphicApi & drawable,
    uint16_t width, uint16_t height,
    Rect const widget_rect, ClientExecute & rail_client_execute, Font const& font,
    Theme const& theme
)
    : RailInternalModBase(drawable, width, height, rail_client_execute, font, theme, nullptr)
    , ttmessage(truncated_bounded_array_view(message))
    , drawable(drawable)
    , font(font)
    , widget_rect(widget_rect)
    , fgcolor(theme.tooltip.fgcolor)
    , bgcolor(theme.tooltip.bgcolor)
    , border_color(theme.tooltip.border_color)
{
    this->set_mod_signal(BACK_EVENT_NONE);
    this->rdp_input_invalidate(widget_rect);
}

void TransitionMod::rdp_input_invalidate(Rect r)
{
    Rect const clip = r.intersect(widget_rect);

    if (!clip.isempty()) {
        int padding = 20;
        int width = gdi::TextMetrics(font, ttmessage.c_str()).width + padding * 2;
        int height = this->font.max_height() + padding * 2;
        int x = widget_rect.x + (widget_rect.cx - width) / 2;
        int y = widget_rect.y + (widget_rect.cy - height) / 2;

        auto color_ctx = gdi::ColorCtx::depth24();
        encode_color24 encode;

        Rect area(x, y, width, height);

        drawable.draw(
            RDPOpaqueRect(area, encode(bgcolor)),
            clip, gdi::ColorCtx::depth24()
        );
        gdi::server_draw_text(
            drawable, font,
            x + padding, y + padding,
            ttmessage.c_str(), encode(fgcolor), encode(bgcolor),
            color_ctx, clip
        );
        gdi_draw_border(drawable, encode(border_color), area, 1, clip, color_ctx);
    }
}

void TransitionMod::rdp_input_scancode(
    KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap)
{
    RailModBase::check_alt_f4(keymap);

    if (pressed_scancode(flags, scancode) == Scancode::Esc) {
        this->set_mod_signal(BACK_EVENT_STOP);
    }
    else {
        this->screen.rdp_input_scancode(flags, scancode, event_time, keymap);
    }
}
