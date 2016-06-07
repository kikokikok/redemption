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
#define BOOST_TEST_MODULE TestWidgetFlatButton
#include "system/redemption_unit_tests.hpp"

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL

#include "core/font.hpp"
#include "mod/internal/widget2/flat_button.hpp"
#include "mod/internal/widget2/screen.hpp"
#include "check_sig.hpp"

#undef OUTPUT_FILE_PATH
#define OUTPUT_FILE_PATH "/tmp/"

#include "fake_draw.hpp"

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButton)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget at position 0,0 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = 0;
    int16_t y = 0;
    int xtext = 4;
    int ytext = 1;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test1", auto_resize, id,
                             fg_color, bg_color, fc_color, font, xtext, ytext);

    // ask to widget to redraw at it's current position
    wbutton.rdp_input_invalidate(Rect(0, 0, wbutton.cx(), wbutton.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x68\xe5\xb3\xd8\x73\x90\x97\x63\x7b\x69\x3f\xb3\x59\x98\xf5\xff\xe6\x1a\x39\x6f"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }
}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButton2)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 100x20 at position 10,100 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = 10;
    int16_t y = 100;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test2", auto_resize, id,
                             fg_color, bg_color, fc_color, font);

    // ask to widget to redraw at it's current position
    wbutton.rdp_input_invalidate(Rect(0 + wbutton.dx(),
                                      0 + wbutton.dy(),
                                      wbutton.cx(),
                                      wbutton.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button2.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x2a\x11\x27\x83\xfc\xca\xe3\x9d\x61\x10\x17\xd8\xbc\xae\x31\x27\x4f\x67\x6d\x71"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }
}


BOOST_AUTO_TEST_CASE(TraceWidgetFlatButton3)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 100x20 at position -10,500 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = -10;
    int16_t y = 500;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test3", auto_resize, id,
                             fg_color, bg_color, fc_color, font);

    // ask to widget to redraw at it's current position
    wbutton.rdp_input_invalidate(Rect(0 + wbutton.dx(),
                                      0 + wbutton.dy(),
                                      wbutton.cx(),
                                      wbutton.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button3.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\xd4\x20\x28\xee\x67\x81\xee\x29\x81\xa2\xe2\x2a\xe9\xdc\x3b\xb6\x96\x09\xd6\xd5"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButton4)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 100x20 at position 770,500 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = 770;
    int16_t y = 500;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test4", auto_resize, id,
                             fg_color, bg_color, fc_color, font);

    // ask to widget to redraw at it's current position
    wbutton.rdp_input_invalidate(Rect(0 + wbutton.dx(),
                                      0 + wbutton.dy(),
                                      wbutton.cx(),
                                      wbutton.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button4.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\xa0\x82\xc3\x05\x38\x74\xb0\xe0\xa6\xb7\x6e\x4e\xc7\xaf\x7c\x0c\xe7\x95\xad\x33"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButton5)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 100x20 at position -20,-7 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = -20;
    int16_t y = -7;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test5", auto_resize, id,
                             fg_color, bg_color, fc_color, font);

    // ask to widget to redraw at it's current position
    wbutton.rdp_input_invalidate(Rect(0 + wbutton.dx(),
                                      0 + wbutton.dy(),
                                      wbutton.cx(),
                                      wbutton.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button5.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\xdb\xab\xd3\x2c\x94\x64\xa7\xf0\xd8\xf5\xc6\xda\xc9\x29\x69\x72\x0f\xda\xcf\xe5"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButton6)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 100x20 at position 760,-7 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = 760;
    int16_t y = -7;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test6", auto_resize, id,
                             fg_color, bg_color, fc_color, font);

    // ask to widget to redraw at it's current position
    wbutton.rdp_input_invalidate(Rect(0 + wbutton.dx(),
                                      0 + wbutton.dy(),
                                      wbutton.cx(),
                                      wbutton.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button6.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x6c\x19\x06\x10\x85\xc1\x56\x63\xf3\x0a\xef\x1f\xed\xce\xf8\xa2\x36\x0a\x12\x78"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButtonClip)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 100x20 at position 760,-7 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int id = 0;
    int fc_color = WINBLUE;
    bool auto_resize = true;
    int16_t x = 760;
    int16_t y = -7;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test6", auto_resize, id,
                             fg_color, bg_color, fc_color, font);

    // ask to widget to redraw at position 780,-7 and of size 120x20. After clip the size is of 20x13
    wbutton.rdp_input_invalidate(Rect(20 + wbutton.dx(),
                                      0 + wbutton.dy(),
                                      wbutton.cx(),
                                      wbutton.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button7.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x85\x49\x79\x3f\xf1\x05\x4d\x53\x2d\x0c\x58\xe6\xf3\x6b\xb8\x7a\xb0\x21\xe5\x9d"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }

}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButtonClip2)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 100x20 at position 10,7 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = 0;
    int16_t y = 0;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test6", auto_resize, id,
                             fg_color, bg_color, fc_color, font);

    // ask to widget to redraw at position 30,12 and of size 30x10.
    wbutton.rdp_input_invalidate(Rect(20 + wbutton.dx(),
                                      5 + wbutton.dy(),
                                      30,
                                      10));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button8.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x84\x60\xe3\xf9\x31\xe5\x7e\x90\x4a\xb5\x18\xd9\xc9\x3a\xc5\x8f\xf3\x38\x30\xae"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }
}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButtonDownAndUp)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = 10;
    int16_t y = 10;
    int xtext = 4;
    int ytext = 1;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test6", auto_resize, id,
                             fg_color, bg_color, fc_color, font, xtext, ytext);

    wbutton.rdp_input_invalidate(wbutton.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "button9.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x4d\x48\x1f\x6f\xe8\x0e\x26\x61\x7c\xfb\xdd\x70\x77\x94\xd6\x70\x10\xce\xe0\xda"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }


    wbutton.rdp_input_mouse(MOUSE_FLAG_BUTTON1|MOUSE_FLAG_DOWN, 15, 15, nullptr);
    wbutton.rdp_input_invalidate(wbutton.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "button10.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\x69\x43\xb9\x96\x60\x66\xb9\xe6\x32\x1a\x66\xdb\xa4\xf6\x69\x99\x0c\x5b\xd7\x0a"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }


    wbutton.rdp_input_mouse(MOUSE_FLAG_BUTTON1, 15, 15, nullptr);
    wbutton.rdp_input_invalidate(wbutton.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "button11.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\x4d\x48\x1f\x6f\xe8\x0e\x26\x61\x7c\xfb\xdd\x70\x77\x94\xd6\x70\x10\xce\xe0\xda"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }
}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButtonEvent)
{
    TestDraw drawable(800, 600);

    struct WidgetReceiveEvent : public Widget2 {
        Widget2* sender = nullptr;
        NotifyApi::notify_event_t event = 0;

        WidgetReceiveEvent(TestDraw& drawable)
        : Widget2(drawable, Rect(), *this, nullptr)
        {}

        virtual void draw(const Rect&) override
        {}

        virtual void notify(Widget2* sender, NotifyApi::notify_event_t event) override
        {
            this->sender = sender;
            this->event = event;
        }
    } widget_for_receive_event(drawable);

    struct Notify : public NotifyApi {
        Widget2* sender = nullptr;
        notify_event_t event = 0;

        Notify() = default;
        virtual void notify(Widget2* sender, notify_event_t event) override
        {
            this->sender = sender;
            this->event = event;
        }
    } notifier;

    Widget2& parent = widget_for_receive_event;
    bool auto_resize = false;
    int16_t x = 0;
    int16_t y = 0;

    Font font;

    WidgetFlatButton wbutton(drawable, x, y, parent, &notifier, "", auto_resize, 0, WHITE,
                             DARK_BLUE_BIS, WINBLUE, font);

    wbutton.rdp_input_mouse(MOUSE_FLAG_BUTTON1|MOUSE_FLAG_DOWN, x, y, nullptr);
    BOOST_CHECK(widget_for_receive_event.sender == nullptr);
    BOOST_CHECK(widget_for_receive_event.event == 0);
    BOOST_CHECK(notifier.sender == nullptr);
    BOOST_CHECK(notifier.event == 0);
    wbutton.rdp_input_mouse(MOUSE_FLAG_BUTTON1|MOUSE_FLAG_DOWN, x, y, nullptr);
    BOOST_CHECK(widget_for_receive_event.sender == nullptr);
    BOOST_CHECK(widget_for_receive_event.event == 0);
    BOOST_CHECK(notifier.sender == nullptr);
    BOOST_CHECK(notifier.event == 0);
    wbutton.rdp_input_mouse(MOUSE_FLAG_BUTTON1, x, y, nullptr);
    BOOST_CHECK(widget_for_receive_event.sender == nullptr);
    BOOST_CHECK(widget_for_receive_event.event == 0);
    BOOST_CHECK(notifier.sender == &wbutton);
    BOOST_CHECK(notifier.event == NOTIFY_SUBMIT);
    notifier.sender = nullptr;
    notifier.event = 0;
    wbutton.rdp_input_mouse(MOUSE_FLAG_BUTTON1|MOUSE_FLAG_DOWN, x, y, nullptr);
    BOOST_CHECK(widget_for_receive_event.sender == nullptr);
    BOOST_CHECK(widget_for_receive_event.event == 0);
    BOOST_CHECK(notifier.sender == nullptr);
    BOOST_CHECK(notifier.event == 0);

    Keymap2 keymap;
    keymap.init_layout(0x040C);

    keymap.push_kevent(Keymap2::KEVENT_KEY);
    keymap.push_char('a');
    wbutton.rdp_input_scancode(0, 0, 0, 0, &keymap);
    BOOST_CHECK(widget_for_receive_event.sender == nullptr);
    BOOST_CHECK(widget_for_receive_event.event == 0);
    BOOST_CHECK(notifier.sender == nullptr);
    BOOST_CHECK(notifier.event == 0);

    keymap.push_kevent(Keymap2::KEVENT_KEY);
    keymap.push_char(' ');
    wbutton.rdp_input_scancode(0, 0, 0, 0, &keymap);
    BOOST_CHECK(widget_for_receive_event.sender == nullptr);
    BOOST_CHECK(widget_for_receive_event.event == 0);
    BOOST_CHECK(notifier.sender == &wbutton);
    BOOST_CHECK(notifier.event == NOTIFY_SUBMIT);
    notifier.sender = nullptr;
    notifier.event = 0;

    keymap.push_kevent(Keymap2::KEVENT_ENTER);
    wbutton.rdp_input_scancode(0, 0, 0, 0, &keymap);
    BOOST_CHECK(widget_for_receive_event.sender == nullptr);
    BOOST_CHECK(widget_for_receive_event.event == 0);
    BOOST_CHECK(notifier.sender == &wbutton);
    BOOST_CHECK(notifier.event == NOTIFY_SUBMIT);
    notifier.sender = nullptr;
    notifier.event = 0;
}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButtonAndComposite)
{
    TestDraw drawable(800, 600);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    // WidgetFlatButton is a button widget of size 256x125 at position 0,0 in it's parent context
    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;

    WidgetComposite wcomposite(drawable, Rect(0,0,800,600), parent, notifier);

    WidgetFlatButton wbutton1(drawable, 0,0, wcomposite, notifier,
                              "abababab", true, 0, YELLOW, BLACK, WINBLUE, font);
    WidgetFlatButton wbutton2(drawable, 0,100, wcomposite, notifier,
                              "ggghdgh", true, 0, WHITE, RED, WINBLUE, font);
    WidgetFlatButton wbutton3(drawable, 100,100, wcomposite, notifier,
                              "lldlslql", true, 0, BLUE, RED, WINBLUE, font);
    WidgetFlatButton wbutton4(drawable, 300,300, wcomposite, notifier,
                              "LLLLMLLM", true, 0, PINK, DARK_GREEN, WINBLUE, font);
    WidgetFlatButton wbutton5(drawable, 700,-10, wcomposite, notifier,
                              "dsdsdjdjs", true, 0, LIGHT_GREEN, DARK_BLUE, WINBLUE, font);
    WidgetFlatButton wbutton6(drawable, -10,550, wcomposite, notifier,
                              "xxwwp", true, 0, ANTHRACITE, PALE_GREEN, WINBLUE, font);

    wcomposite.add_widget(&wbutton1);
    wcomposite.add_widget(&wbutton2);
    wcomposite.add_widget(&wbutton3);
    wcomposite.add_widget(&wbutton4);
    wcomposite.add_widget(&wbutton5);
    wcomposite.add_widget(&wbutton6);

    // ask to widget to redraw at position 100,25 and of size 100x100.
    wcomposite.rdp_input_invalidate(Rect(100, 25, 100, 100));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button12.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\xea\x26\xfc\x6b\x6e\x30\x6d\x39\xa4\x06\xb2\xae\xe0\xde\x95\x20\x67\x8a\x44\xba"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }


    // ask to widget to redraw at it's current position
    wcomposite.rdp_input_invalidate(Rect(0, 0, wcomposite.cx(), wcomposite.cy()));

    // drawable.save_to_png(OUTPUT_FILE_PATH "button13.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\x56\x5c\x7e\x82\x85\xba\xdf\x0b\xf8\x63\x07\x97\x71\xba\x55\x91\x2f\xfc\x70\xe2"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }


    wcomposite.clear();
}

BOOST_AUTO_TEST_CASE(TraceWidgetFlatButtonFocus)
{
    TestDraw drawable(70, 40);

    Font font(FIXTURES_PATH "/dejavu-sans-10.fv1");

    WidgetScreen parent(drawable, 800, 600, font);
    NotifyApi * notifier = nullptr;
    int fg_color = RED;
    int bg_color = YELLOW;
    int fc_color = WINBLUE;
    int id = 0;
    bool auto_resize = true;
    int16_t x = 10;
    int16_t y = 10;
    int xtext = 4;
    int ytext = 1;

    WidgetFlatButton wbutton(drawable, x, y, parent, notifier, "test7", auto_resize, id,
                             fg_color, bg_color, fc_color, font, xtext, ytext);

    wbutton.rdp_input_invalidate(wbutton.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "button14.png");

    char message[1024];
    if (!check_sig(drawable.gd.impl(), message,
        "\x49\x06\x79\x76\xc9\x88\xca\xd0\x0c\xca\x82\x27\xe0\xe4\xe4\xd9\x8b\xf0\xb1\x39"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }


    wbutton.focus(Widget2::focus_reason_tabkey);

    wbutton.rdp_input_invalidate(wbutton.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "button15.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\xca\xc9\xc3\xa6\xd9\xa2\x8b\xd8\xce\x1f\x8d\x18\xf2\x94\xda\x44\x3a\xfe\x41\x78"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }



    wbutton.blur();

    wbutton.rdp_input_invalidate(wbutton.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "button16.png");

    if (!check_sig(drawable.gd.impl(), message,
        "\x49\x06\x79\x76\xc9\x88\xca\xd0\x0c\xca\x82\x27\xe0\xe4\xe4\xd9\x8b\xf0\xb1\x39"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }


    wbutton.focus(Widget2::focus_reason_tabkey);

    wbutton.rdp_input_invalidate(wbutton.rect);

    // drawable.save_to_png(OUTPUT_FILE_PATH "button17.png");


    if (!check_sig(drawable.gd.impl(), message,
    "\xca\xc9\xc3\xa6\xd9\xa2\x8b\xd8\xce\x1f\x8d\x18\xf2\x94\xda\x44\x3a\xfe\x41\x78"
    )){
        BOOST_CHECK_MESSAGE(false, message);
    }
}
