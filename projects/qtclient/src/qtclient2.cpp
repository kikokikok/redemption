/*
SPDX-FileCopyrightText: 2023 Wallix Proxies Team

SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "acl/license_api.hpp"
#include "client/common/new_mod_rdp.hpp"
#include "configs/config.hpp"
#include "core/channel_list.hpp"
#include "core/channels_authorizations.hpp"
#include "core/client_info.hpp"
#include "core/events.hpp"
#include "core/font.hpp"
#include "core/front_api.hpp"
#include "gdi/osd_api.hpp"
#include "mod/rdp/mod_rdp_factory.hpp"
#include "mod/rdp/rdp_params.hpp"
#include "system/scoped_ssl_init.hpp"
#include "utils/genrandom.hpp"
#include "utils/redirection_info.hpp"
#include "utils/set_exception_handler_pretty_message.hpp"
#include "utils/sugar/unique_fd.hpp"
#include "utils/theme.hpp"

#include "qtclient/socket.hpp"
#include "qtclient/event_manager.hpp"
#include "qtclient/profile/profile.hpp"
#include "qtclient/profile/cli_parse_profile.hpp"
#include "qtclient/widget/screen_widget.hpp"
#include "headlessclient/headless_configuration.hpp"

#include <QtWidgets/QApplication>


struct QtFront final : FrontAPI
{
    QtFront(qtclient::ScreenWidget& screen)
    : screen(screen)
    {}

    bool can_be_start_capture(SessionLogApi& session_log) override
    {
        (void)session_log;
        return false;
    }

    void must_flush_capture() override
    {
    }

    bool must_be_stop_capture() override
    {
        return false;
    }

    bool is_capture_in_progress() const override
    {
        return false;
    }

    ResizeResult server_resize(ScreenInfo screen_server) override
    {
        screen.resize(screen_server.width, screen_server.height);
        return ResizeResult::instant_done;
    }

    const CHANNELS::ChannelDefArray& get_channel_list() const override
    {
        return cl;
    }

    void send_to_channel(
        CHANNELS::ChannelDef const& channel_def, bytes_view chunk_data,
        std::size_t total_data_len, uint32_t flags) override
    {
        (void)channel_def;
        (void)chunk_data;
        (void)total_data_len;
        (void)flags;
    }

    void update_pointer_position(uint16_t x, uint16_t y) override
    {
        (void)x;
        (void)y;
    }

    void session_update(MonotonicTimePoint now, LogId id, KVLogList kv_list) override
    {
        (void)now;
        (void)id;
        (void)kv_list;
    }

    void possible_active_window_change() override
    {}

private:
    CHANNELS::ChannelDefArray cl;
    qtclient::ScreenWidget& screen;
};


int main(int argc, char** argv)
{
    set_exception_handler_pretty_message();

    qtclient::Profile profile;
    switch (qtclient::cli_parse_profile(argc, argv, profile))
    {
        case qtclient::CliResult::Ok: break;
        case qtclient::CliResult::Exit: return 0;
        case qtclient::CliResult::Error: return 1;
    }

    LOG(LOG_DEBUG, "%s %d", profile.target_address, profile.target_port);

    qtclient::SocketIO socket(
        "Client"_sck_name,
        invalid_fd(),
        profile.target_address,
        profile.target_port,
        std::chrono::seconds(5),
        std::chrono::seconds(5),
        std::chrono::seconds(5),
        SocketTransport::Verbose());

    if (!socket.connect()) {
        return 1;
    }

    ScopedSslInit scoped_init;

    Font glyph;
    Inifile ini;
    Theme theme;
    UdevRandom rnd;
    gdi::NullOsd osd;
    qtclient::EventManager event_manager;
    NullSessionLog session_log;
    RedirectionInfo redir_info;
    ModRdpFactory mod_rdp_factory;
    NullLicenseStore license_store;
    TLSClientParams tls_client_params {};
    std::array<uint8_t, 28> server_auto_reconnect_packet_ref;
    std::string close_box_extra_message_ref;
    const ChannelsAuthorizations channels_authorizations("*"_zv, ""_zv);
    tls_client_params.tls_min_level = profile.tls_min_level;
    tls_client_params.tls_max_level = profile.tls_max_level;
    tls_client_params.cipher_string = profile.cipher_string;
    tls_client_params.show_common_cipher_list = true;

    ClientInfo client_info;
    headless_init_client_info(client_info);
    client_info.screen_info = profile.screen_info;
    client_info.keylayout = profile.key_layout;

    ModRDPParams mod_rdp_params(
        profile.user_name.c_str(),
        profile.user_password.c_str(),
        profile.target_address.c_str(),
        "0.0.0.0",
        kbdtypes::KeyLocks(),
        glyph,
        theme,
        server_auto_reconnect_packet_ref,
        close_box_extra_message_ref,
        {},
        profile.rdp_verbose
    );

    mod_rdp_params.device_id          = "device_id";
    mod_rdp_params.enable_tls         = profile.enable_tls;
    mod_rdp_params.enable_nla         = profile.enable_nla;
    mod_rdp_params.enable_fastpath    = true;
    mod_rdp_params.enable_new_pointer = true;
    mod_rdp_params.enable_glyph_cache = true;
    mod_rdp_params.enable_remotefx    = false;
    mod_rdp_params.file_system_params.enable_rdpdr_data_analysis = false;

    QApplication app(argc, argv);

    auto* screen = new qtclient::ScreenWidget();
    QtFront front(*screen);

    auto mod = new_mod_rdp(
        socket, *screen, osd, event_manager.event_container,
        session_log, front, client_info, redir_info, rnd,
        channels_authorizations, mod_rdp_params, tls_client_params,
        license_store, ini, nullptr, mod_rdp_factory
    );

    screen->set_rdp_input(*mod);
    screen->show();

    event_manager.exception_notifier = [&](Error& err) {
        LOG(LOG_ERR, "%s", err.errmsg());
        app.quit();
    };
    event_manager.start();

    return app.exec();
}
