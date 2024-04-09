/*
SPDX-FileCopyrightText: 2024 Wallix Proxies Team
SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gdi/draw_utils.hpp"
#include "utils/sugar/numerics/safe_conversions.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"


void gdi_draw_border(
    gdi::GraphicApi& drawable, RDPColor color,
    int16_t x, int16_t y, uint16_t cx, uint16_t cy,
    uint16_t border_width, Rect clip, gdi::ColorCtx color_ctx)
{
    auto draw = [&](Rect rect){
        drawable.draw(RDPOpaqueRect(rect, color), clip, color_ctx);
    };

    // top
    draw(Rect(x, y, cx - border_width, border_width));
    // left
    draw(Rect(x, checked_int(y + border_width), border_width, cy - border_width));
    // right
    draw(Rect(checked_int(x + cx - border_width), y, border_width, cy));
    // bottom
    draw(Rect(x, checked_int(y + cy - border_width), cx - border_width, border_width));
}
