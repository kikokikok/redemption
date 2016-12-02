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
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestWidgetTooltip
#include "system/redemption_unit_tests.hpp"

#define LOGNULL

#include "core/font.hpp"
#include "mod/internal/widget2/tooltip.hpp"
#include "mod/internal/widget2/screen.hpp"
#include "mod/internal/widget2/label.hpp"
#include "check_sig.hpp"

#include "fake_draw.hpp"

BOOST_AUTO_TEST_CASE(TraceWidgetTooltip)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetTooltip is a tooltip widget at position 0,0 in it's parent context
    WidgetScreen parent(drawable.gd, 800, 600, font, nullptr, Theme{});

    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int border_color = BLACK;
    int16_t x = 10;
    int16_t y = 10;
    const char * tooltiptext = "testémq";

    WidgetTooltip wtooltip(drawable.gd, x, y, parent, notifier, tooltiptext, fg_color, bg_color, border_color, font);

    // ask to widget to redraw
    wtooltip.rdp_input_invalidate(Rect(0, 0, 100, 100));

    // drawable.save_to_png(OUTPUT_FILE_PATH "tooltip.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x48\xaa\x0c\x28\xb2\x58\x37\x02\x98\x48\xf0\xaf\xfa\xfb\x2a\x06\x6f\x8c\xae\x80"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }
}

inline
void rdp_input_mouse(int device_flags, int x, int y, Keymap2* keymap, WidgetScreen * parent, Widget2 * w, const char * text)
{
    parent->rdp_input_mouse(device_flags, x, y, keymap);
    if (device_flags == MOUSE_FLAG_MOVE) {
        Widget2 * wid = parent->widget_at_pos(x, y);
        if (wid == w) {
            parent->show_tooltip(w, text, x, y, Rect(0, 0, 0, 0));
        }
    }

}

BOOST_AUTO_TEST_CASE(TraceWidgetTooltipScreen)
{
    TestDraw drawable(800, 600);
    int x = 50;
    int y = 20;

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetTooltip is a tooltip widget at position 0,0 in it's parent context
    WidgetScreen parent(drawable.gd, 800, 600, font, nullptr, Theme{});

    WidgetLabel label(drawable.gd, x, y, parent, &parent, "TOOLTIPTEST",
                      true, 0, BLACK, WHITE, font);
    WidgetLabel label2(drawable.gd, x + 50, y + 90, parent, &parent, "TOOLTIPTESTMULTI",
                      true, 0, BLACK, WHITE, font);

    parent.add_widget(&label);
    parent.add_widget(&label2);

    parent.rdp_input_invalidate(Rect(0, 0, parent.cx(), parent.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "tooltipscreen1.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x2a\xe5\xb7\x02\xd4\xa4\x88\x01\x65\xd5\x93\x8d\x83\x8d\xd1\xe7\x6f\x10\x2a\x59"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

    rdp_input_mouse(MOUSE_FLAG_MOVE,
                    label.x() + 2, label.y() + 2,
                    nullptr, &parent, &label, "Test tooltip description");
    parent.rdp_input_invalidate(parent.get_rect());

    // drawable.save_to_png(OUTPUT_FILE_PATH "tooltipscreen2.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\x81\x11\xa7\xb0\xbc\x79\xd7\x22\x6b\x89\x20\xc8\xf2\x62\x13\x5f\x6e\x59\x45\xdf"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

    rdp_input_mouse(MOUSE_FLAG_MOVE,
                    label2.x() + 2, label2.y() + 2,
                    nullptr, &parent, &label2,
                    "Test tooltip<br>"
                    "description in<br>"
                    "multilines !");

    parent.rdp_input_invalidate(parent.get_rect());

    // drawable.save_to_png(OUTPUT_FILE_PATH "tooltipscreen3.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\xdd\xc9\x40\x79\x91\xbd\x3c\xe8\xf4\x14\x17\xe6\x2d\x09\x2d\xae\x23\x54\xb7\x17"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

    parent.tooltip->set_text("Test tooltip<br>"
                             "Text modification<br>"
                             "text has been changed !");
    parent.rdp_input_invalidate(parent.get_rect());

    // drawable.save_to_png(OUTPUT_FILE_PATH "tooltipscreen4.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\x6b\x0e\x8a\x3a\x5a\x37\x42\xaa\x36\x2d\xf5\x43\xb4\x50\x88\xbf\xcf\xee\xf8\x02"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

    parent.clear();
}
