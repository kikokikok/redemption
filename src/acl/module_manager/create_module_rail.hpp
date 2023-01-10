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
Copyright (C) Wallix 2020
Author(s): Proxy Team
*/

#pragma once

class RailModuleHostMod;
class Inifile;
class EventContainer;
class ClientInfo;
class ClientExecute;
class Font;
class Theme;

namespace gdi
{
    class GraphicApi;
}

RailModuleHostMod* create_mod_rail(
    Inifile& ini,
    EventContainer& events,
    gdi::GraphicApi & drawable,
    ClientInfo const& client_info,
    ClientExecute& rail_client_execute,
    Font const& font,
    Theme const& theme,
    bool can_resize_hosted_desktop);
