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
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen
 */

#if !defined(REDEMPTION_MOD_INTERNAL_WIDGET2_MSGBOX_HPP)
#define REDEMPTION_MOD_INTERNAL_WIDGET2_MSGBOX_HPP

#include "window.hpp"
#include "widget2_multiline.hpp"

class MessageBox : public Window
{
public:
    WidgetMultiLine msg;
    WidgetButton ok;

    MessageBox(DrawApi* drawable, int16_t x, int16_t y, Widget2 * parent,
               NotifyApi* notifier, const char * caption, const char * text,
               int group_id = 0, const char * ok_text = "Ok",
               int fgcolor = BLACK, int bgcolor = GREY)
    : Window(drawable, Rect(x,y,1,1), parent, notifier, caption, bgcolor, group_id)
    , msg(drawable, 0, 0, this, NULL, text, true, -10, fgcolor, bgcolor, 10, 2)
    , ok(drawable, 0,0, this, this, "Ok", true, -11, fgcolor, bgcolor, 6, 2, NOTIFY_CANCEL)
    {
        this->child_list.push_back(&this->msg);
        this->child_list.push_back(&this->ok);

        this->set_window_cx(std::max<int>(this->msg.cx(), this->ok.cx() + 20));
        this->msg.rect.x += (this->cx() - this->msg.cx()) / 2;

        this->set_window_cy(this->titlebar.cy() + this->msg.cy() + this->ok.cy() + 10);
        this->msg.rect.y += this->titlebar.cy() + 5;
        this->ok.set_button_x(this->dx() + this->cx() - this->ok.cx() - 10);
        this->ok.set_button_y(this->dy() + this->cy() - this->ok.cy() - 5);
    }

    virtual ~MessageBox()
    {}

    virtual void rdp_input_scancode(long int param1, long int param2, long int param3, long int param4, Keymap2* keymap)
    {
        if (keymap->nb_kevent_available() > 0){
            switch (keymap->top_kevent()){
                case Keymap2::KEVENT_ENTER:
                case Keymap2::KEVENT_ESC:
                    keymap->get_kevent();
                    this->send_notify(NOTIFY_CANCEL);
                    break;
                default:
                    Window::rdp_input_scancode(param1, param2, param3, param4, keymap);
                    break;
            }
        }
    }
};

#endif
