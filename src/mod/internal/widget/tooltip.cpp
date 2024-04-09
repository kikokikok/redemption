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
 *   Copyright (C) Wallix 2010-2012
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen
 *              Meng Tan
 */

#include "mod/internal/widget/tooltip.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"
#include "gdi/draw_utils.hpp"

WidgetTooltip::WidgetTooltip(
    gdi::GraphicApi & drawable, const char * text, unsigned max_width,
    Color fgcolor, Color bgcolor, Color border_color,
    Font const & font
)
    : Widget(drawable, Focusable::No)
    , w_border(10)
    , h_border(10)
    , desc(drawable, text, max_width, fgcolor, bgcolor, font, 0, 0)
    , border_color(border_color)
{
}

WidgetTooltip::~WidgetTooltip() = default;

Dimension WidgetTooltip::get_optimal_dim() const
{
    Dimension dim = this->desc.get_optimal_dim();

    dim.w += 2 * this->w_border;
    dim.h += 2 * this->h_border;

    return dim;
}

void WidgetTooltip::set_text(const char * text)
{
    this->desc.set_text(text, this->cx());
}

void WidgetTooltip::set_text(const char * text, unsigned max_width)
{
    this->desc.set_text(text, max_width);
    Dimension dim = this->desc.get_optimal_dim();
    this->desc.set_wh(dim);

    this->set_wh(this->desc.cx() + 2 * w_border,
                 this->desc.cy() + 2 * h_border);
}

void WidgetTooltip::rdp_input_invalidate(Rect clip)
{
    Rect rect_intersect = clip.intersect(this->get_rect());

    if (!rect_intersect.isempty()) {
        this->drawable.draw(
            RDPOpaqueRect(this->get_rect(), this->desc.get_bg_color()),
            rect_intersect, gdi::ColorCtx::depth24());
        this->desc.rdp_input_invalidate(rect_intersect);

        gdi_draw_border(
            drawable, this->border_color, this->get_rect(), 1,
            rect_intersect, gdi::ColorCtx::depth24()
        );
    }
}

void WidgetTooltip::set_xy(int16_t x, int16_t y)
{
    Widget::set_xy(x, y);
    this->desc.set_xy(x + w_border, y + h_border);
}

void WidgetTooltip::set_wh(uint16_t w, uint16_t h)
{
    Widget::set_wh(w, h);
    this->desc.set_wh(w -  2 * w_border, h - 2 * h_border);
}
