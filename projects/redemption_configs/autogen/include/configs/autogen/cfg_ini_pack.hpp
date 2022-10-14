// DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN
// 

#pragma once

#include "configs/autogen/variables_configuration_fwd.hpp"
#include "utils/sugar/zstring_view.hpp"

namespace configs::cfg_ini_infos {
using IniPack = Pack<
cfg::globals::glyph_cache,
cfg::globals::port,
cfg::globals::encryptionLevel,
cfg::globals::authfile,
cfg::globals::handshake_timeout,
cfg::globals::base_inactivity_timeout,
cfg::globals::inactivity_timeout,
cfg::globals::keepalive_grace_delay,
cfg::globals::authentication_timeout,
cfg::globals::close_timeout,
cfg::globals::trace_type,
cfg::globals::listen_address,
cfg::globals::enable_transparent_mode,
cfg::globals::certificate_password,
cfg::globals::enable_bitmap_update,
cfg::globals::enable_close_box,
cfg::globals::enable_osd,
cfg::globals::enable_osd_display_remote_target,
cfg::globals::enable_wab_integration,
cfg::globals::allow_using_multiple_monitors,
cfg::globals::allow_scale_factor,
cfg::globals::bogus_refresh_rect,
cfg::globals::large_pointer_support,
cfg::globals::new_pointer_update_support,
cfg::globals::unicode_keyboard_event_support,
cfg::globals::mod_recv_timeout,
cfg::globals::experimental_enable_serializer_data_block_size_limit,
cfg::globals::experimental_support_resize_session_during_recording,
cfg::globals::support_connection_redirection_during_recording,
cfg::globals::rdp_keepalive_connection_interval,
cfg::globals::enable_ipv6,
cfg::client::keyboard_layout_proposals,
cfg::client::ignore_logon_password,
cfg::client::performance_flags_default,
cfg::client::performance_flags_force_present,
cfg::client::performance_flags_force_not_present,
cfg::client::auto_adjust_performance_flags,
cfg::client::tls_fallback_legacy,
cfg::client::tls_support,
cfg::client::tls_min_level,
cfg::client::tls_max_level,
cfg::client::show_common_cipher_list,
cfg::client::enable_nla,
cfg::client::bogus_neg_request,
cfg::client::bogus_user_id,
cfg::client::disable_tsk_switch_shortcuts,
cfg::client::rdp_compression,
cfg::client::max_color_depth,
cfg::client::persistent_disk_bitmap_cache,
cfg::client::cache_waiting_list,
cfg::client::persist_bitmap_cache_on_disk,
cfg::client::bitmap_compression,
cfg::client::fast_path,
cfg::client::enable_suppress_output,
cfg::client::ssl_cipher_list,
cfg::client::show_target_user_in_f12_message,
cfg::client::bogus_ios_glyph_support_level,
cfg::client::transform_glyph_to_bitmap,
cfg::client::bogus_number_of_fastpath_input_event,
cfg::client::recv_timeout,
cfg::client::enable_osd_4_eyes,
cfg::client::enable_remotefx,
cfg::client::disabled_orders,
cfg::all_target_mod::connection_establishment_timeout,
cfg::all_target_mod::tcp_user_timeout,
cfg::remote_program::allow_resize_hosted_desktop,
cfg::mod_rdp::rdp_compression,
cfg::mod_rdp::disconnect_on_logon_user_change,
cfg::mod_rdp::open_session_timeout,
cfg::mod_rdp::disabled_orders,
cfg::mod_rdp::enable_nla,
cfg::mod_rdp::enable_kerberos,
cfg::mod_rdp::persistent_disk_bitmap_cache,
cfg::mod_rdp::cache_waiting_list,
cfg::mod_rdp::persist_bitmap_cache_on_disk,
cfg::mod_rdp::allow_channels,
cfg::mod_rdp::deny_channels,
cfg::mod_rdp::fast_path,
cfg::mod_rdp::server_redirection_support,
cfg::mod_rdp::client_address_sent,
cfg::mod_rdp::bogus_sc_net_size,
cfg::mod_rdp::proxy_managed_drives,
cfg::mod_rdp::ignore_auth_channel,
cfg::mod_rdp::auth_channel,
cfg::mod_rdp::checkout_channel,
cfg::mod_rdp::alternate_shell,
cfg::mod_rdp::shell_arguments,
cfg::mod_rdp::shell_working_directory,
cfg::mod_rdp::use_client_provided_alternate_shell,
cfg::mod_rdp::use_client_provided_remoteapp,
cfg::mod_rdp::use_native_remoteapp_capability,
cfg::mod_rdp::application_driver_exe_or_file,
cfg::mod_rdp::application_driver_script_argument,
cfg::mod_rdp::application_driver_chrome_dt_script,
cfg::mod_rdp::application_driver_chrome_uia_script,
cfg::mod_rdp::application_driver_ie_script,
cfg::mod_rdp::hide_client_name,
cfg::mod_rdp::bogus_ios_rdpdr_virtual_channel,
cfg::mod_rdp::enable_rdpdr_data_analysis,
cfg::mod_rdp::remoteapp_bypass_legal_notice_delay,
cfg::mod_rdp::remoteapp_bypass_legal_notice_timeout,
cfg::mod_rdp::log_only_relevant_clipboard_activities,
cfg::mod_rdp::experimental_fix_too_long_cookie,
cfg::mod_rdp::split_domain,
cfg::mod_rdp::wabam_uses_translated_remoteapp,
cfg::mod_rdp::session_shadowing_support,
cfg::mod_rdp::use_license_store,
cfg::mod_rdp::enable_remotefx,
cfg::mod_rdp::accept_monitor_layout_change_if_capture_is_not_started,
cfg::mod_rdp::enable_restricted_admin_mode,
cfg::mod_rdp::force_smartcard_authentication,
cfg::mod_rdp::enable_ipv6,
cfg::mod_rdp::auto_reconnection_on_losing_target_link,
cfg::mod_rdp::forward_client_build_number,
cfg::mod_rdp::bogus_monitor_layout_treatment,
cfg::session_probe::enable_session_probe,
cfg::session_probe::exe_or_file,
cfg::session_probe::arguments,
cfg::session_probe::use_smart_launcher,
cfg::session_probe::enable_launch_mask,
cfg::session_probe::on_launch_failure,
cfg::session_probe::launch_timeout,
cfg::session_probe::launch_fallback_timeout,
cfg::session_probe::start_launch_timeout_timer_only_after_logon,
cfg::session_probe::keepalive_timeout,
cfg::session_probe::on_keepalive_timeout,
cfg::session_probe::end_disconnected_session,
cfg::session_probe::enable_autodeployed_appdriver_affinity,
cfg::session_probe::enable_log,
cfg::session_probe::enable_log_rotation,
cfg::session_probe::log_level,
cfg::session_probe::disconnected_application_limit,
cfg::session_probe::disconnected_session_limit,
cfg::session_probe::idle_session_limit,
cfg::session_probe::smart_launcher_clipboard_initialization_delay,
cfg::session_probe::smart_launcher_start_delay,
cfg::session_probe::smart_launcher_long_delay,
cfg::session_probe::smart_launcher_short_delay,
cfg::session_probe::smart_launcher_enable_wabam_affinity,
cfg::session_probe::launcher_abort_delay,
cfg::session_probe::enable_crash_dump,
cfg::session_probe::handle_usage_limit,
cfg::session_probe::memory_usage_limit,
cfg::session_probe::cpu_usage_alarm_threshold,
cfg::session_probe::cpu_usage_alarm_action,
cfg::session_probe::end_of_session_check_delay_time,
cfg::session_probe::ignore_ui_less_processes_during_end_of_session_check,
cfg::session_probe::childless_window_as_unidentified_input_field,
cfg::session_probe::update_disabled_features,
cfg::session_probe::disabled_features,
cfg::session_probe::enable_bestsafe_interaction,
cfg::session_probe::on_account_manipulation,
cfg::session_probe::alternate_directory_environment_variable,
cfg::session_probe::public_session,
cfg::session_probe::customize_executable_name,
cfg::session_probe::allow_multiple_handshake,
cfg::session_probe::at_end_of_session_freeze_connection_and_wait,
cfg::session_probe::enable_cleaner,
cfg::session_probe::clipboard_based_launcher_reset_keyboard_status,
cfg::server_cert::server_cert_store,
cfg::server_cert::server_cert_check,
cfg::server_cert::server_access_allowed_message,
cfg::server_cert::server_cert_create_message,
cfg::server_cert::server_cert_success_message,
cfg::server_cert::server_cert_failure_message,
cfg::server_cert::error_message,
cfg::mod_vnc::clipboard_up,
cfg::mod_vnc::clipboard_down,
cfg::mod_vnc::encodings,
cfg::mod_vnc::server_clipboard_encoding_type,
cfg::mod_vnc::bogus_clipboard_infinite_loop,
cfg::mod_vnc::server_is_macos,
cfg::mod_vnc::server_unix_alt,
cfg::mod_vnc::support_cursor_pseudo_encoding,
cfg::mod_vnc::enable_ipv6,
cfg::session_log::enable_session_log,
cfg::session_log::enable_arcsight_log,
cfg::session_log::keyboard_input_masking_level,
cfg::session_log::hide_non_printable_kbd_input,
cfg::ocr::version,
cfg::ocr::locale,
cfg::ocr::interval,
cfg::ocr::on_title_bar_only,
cfg::ocr::max_unrecog_char_rate,
cfg::video::capture_flags,
cfg::video::png_interval,
cfg::video::break_interval,
cfg::video::png_limit,
cfg::video::hash_path,
cfg::video::record_tmp_path,
cfg::video::record_path,
cfg::video::disable_keyboard_log,
cfg::video::disable_clipboard_log,
cfg::video::disable_file_system_log,
cfg::video::wrm_color_depth_selection_strategy,
cfg::video::wrm_compression_algorithm,
cfg::video::bogus_vlc_frame_rate,
cfg::video::codec_id,
cfg::video::framerate,
cfg::video::ffmpeg_options,
cfg::video::notimestamp,
cfg::video::smart_video_cropping,
cfg::video::play_video_with_corrupted_bitmap,
cfg::video::allow_rt_without_recording,
cfg::video::file_permissions,
cfg::audit::use_redis,
cfg::audit::redis_timeout,
cfg::file_verification::socket_path,
cfg::file_verification::enable_up,
cfg::file_verification::enable_down,
cfg::file_verification::clipboard_text_up,
cfg::file_verification::clipboard_text_down,
cfg::file_verification::block_invalid_file_up,
cfg::file_verification::block_invalid_file_down,
cfg::file_verification::block_invalid_clipboard_text_up,
cfg::file_verification::block_invalid_clipboard_text_down,
cfg::file_verification::log_if_accepted,
cfg::file_verification::max_file_size_rejected,
cfg::file_verification::tmpdir,
cfg::file_storage::store_file,
cfg::crypto::encryption_key,
cfg::crypto::sign_key,
cfg::websocket::enable_websocket,
cfg::websocket::use_tls,
cfg::websocket::listen_address,
cfg::internal_mod::enable_target_field,
cfg::mod_replay::replay_path,
cfg::mod_replay::on_end_of_data,
cfg::mod_replay::replay_on_loop,
cfg::translation::language,
cfg::translation::login_language,
cfg::theme::enable_theme,
cfg::theme::logo_path,
cfg::theme::bgcolor,
cfg::theme::fgcolor,
cfg::theme::separator_color,
cfg::theme::focus_color,
cfg::theme::error_color,
cfg::theme::edit_bgcolor,
cfg::theme::edit_fgcolor,
cfg::theme::edit_focus_color,
cfg::theme::tooltip_bgcolor,
cfg::theme::tooltip_fgcolor,
cfg::theme::tooltip_border_color,
cfg::theme::selector_line1_bgcolor,
cfg::theme::selector_line1_fgcolor,
cfg::theme::selector_line2_bgcolor,
cfg::theme::selector_line2_fgcolor,
cfg::theme::selector_selected_bgcolor,
cfg::theme::selector_selected_fgcolor,
cfg::theme::selector_focus_bgcolor,
cfg::theme::selector_focus_fgcolor,
cfg::theme::selector_label_bgcolor,
cfg::theme::selector_label_fgcolor,
cfg::debug::fake_target_ip,
cfg::debug::capture,
cfg::debug::auth,
cfg::debug::session,
cfg::debug::front,
cfg::debug::mod_rdp,
cfg::debug::mod_vnc,
cfg::debug::mod_internal,
cfg::debug::sck_mod,
cfg::debug::sck_front,
cfg::debug::password,
cfg::debug::compression,
cfg::debug::cache,
cfg::debug::ocr,
cfg::debug::ffmpeg,
cfg::debug::config,
cfg::debug::mod_rdp_use_failure_simulation_socket_transport,
cfg::debug::probe_client_addresses
>;

struct SectionAndName { zstring_view section; zstring_view name; };
constexpr SectionAndName const ini_names[] = {
{"globals"_zv, "glyph_cache"_zv},
{"globals"_zv, "port"_zv},
{"globals"_zv, "encryptionLevel"_zv},
{"globals"_zv, "authfile"_zv},
{"globals"_zv, "handshake_timeout"_zv},
{"globals"_zv, "base_inactivity_timeout"_zv},
{"globals"_zv, "inactivity_timeout"_zv},
{"globals"_zv, "keepalive_grace_delay"_zv},
{"globals"_zv, "authentication_timeout"_zv},
{"globals"_zv, "close_timeout"_zv},
{"globals"_zv, "trace_type"_zv},
{"globals"_zv, "listen_address"_zv},
{"globals"_zv, "enable_transparent_mode"_zv},
{"globals"_zv, "certificate_password"_zv},
{"globals"_zv, "enable_bitmap_update"_zv},
{"globals"_zv, "enable_close_box"_zv},
{"globals"_zv, "enable_osd"_zv},
{"globals"_zv, "enable_osd_display_remote_target"_zv},
{"globals"_zv, "enable_wab_integration"_zv},
{"globals"_zv, "allow_using_multiple_monitors"_zv},
{"globals"_zv, "allow_scale_factor"_zv},
{"globals"_zv, "bogus_refresh_rect"_zv},
{"globals"_zv, "large_pointer_support"_zv},
{"globals"_zv, "new_pointer_update_support"_zv},
{"globals"_zv, "unicode_keyboard_event_support"_zv},
{"globals"_zv, "mod_recv_timeout"_zv},
{"globals"_zv, "experimental_enable_serializer_data_block_size_limit"_zv},
{"globals"_zv, "experimental_support_resize_session_during_recording"_zv},
{"globals"_zv, "support_connection_redirection_during_recording"_zv},
{"globals"_zv, "rdp_keepalive_connection_interval"_zv},
{"globals"_zv, "enable_ipv6"_zv},
{"client"_zv, "keyboard_layout_proposals"_zv},
{"client"_zv, "ignore_logon_password"_zv},
{"client"_zv, "performance_flags_default"_zv},
{"client"_zv, "performance_flags_force_present"_zv},
{"client"_zv, "performance_flags_force_not_present"_zv},
{"client"_zv, "auto_adjust_performance_flags"_zv},
{"client"_zv, "tls_fallback_legacy"_zv},
{"client"_zv, "tls_support"_zv},
{"client"_zv, "tls_min_level"_zv},
{"client"_zv, "tls_max_level"_zv},
{"client"_zv, "show_common_cipher_list"_zv},
{"client"_zv, "enable_nla"_zv},
{"client"_zv, "bogus_neg_request"_zv},
{"client"_zv, "bogus_user_id"_zv},
{"client"_zv, "disable_tsk_switch_shortcuts"_zv},
{"client"_zv, "rdp_compression"_zv},
{"client"_zv, "max_color_depth"_zv},
{"client"_zv, "persistent_disk_bitmap_cache"_zv},
{"client"_zv, "cache_waiting_list"_zv},
{"client"_zv, "persist_bitmap_cache_on_disk"_zv},
{"client"_zv, "bitmap_compression"_zv},
{"client"_zv, "fast_path"_zv},
{"client"_zv, "enable_suppress_output"_zv},
{"client"_zv, "ssl_cipher_list"_zv},
{"client"_zv, "show_target_user_in_f12_message"_zv},
{"client"_zv, "bogus_ios_glyph_support_level"_zv},
{"client"_zv, "transform_glyph_to_bitmap"_zv},
{"client"_zv, "bogus_number_of_fastpath_input_event"_zv},
{"client"_zv, "recv_timeout"_zv},
{"client"_zv, "enable_osd_4_eyes"_zv},
{"client"_zv, "enable_remotefx"_zv},
{"client"_zv, "disabled_orders"_zv},
{"all_target_mod"_zv, "connection_establishment_timeout"_zv},
{"all_target_mod"_zv, "tcp_user_timeout"_zv},
{"remote_program"_zv, "allow_resize_hosted_desktop"_zv},
{"mod_rdp"_zv, "rdp_compression"_zv},
{"mod_rdp"_zv, "disconnect_on_logon_user_change"_zv},
{"mod_rdp"_zv, "open_session_timeout"_zv},
{"mod_rdp"_zv, "disabled_orders"_zv},
{"mod_rdp"_zv, "enable_nla"_zv},
{"mod_rdp"_zv, "enable_kerberos"_zv},
{"mod_rdp"_zv, "persistent_disk_bitmap_cache"_zv},
{"mod_rdp"_zv, "cache_waiting_list"_zv},
{"mod_rdp"_zv, "persist_bitmap_cache_on_disk"_zv},
{"mod_rdp"_zv, "allow_channels"_zv},
{"mod_rdp"_zv, "deny_channels"_zv},
{"mod_rdp"_zv, "fast_path"_zv},
{"mod_rdp"_zv, "server_redirection_support"_zv},
{"mod_rdp"_zv, "client_address_sent"_zv},
{"mod_rdp"_zv, "bogus_sc_net_size"_zv},
{"mod_rdp"_zv, "proxy_managed_drives"_zv},
{"mod_rdp"_zv, "ignore_auth_channel"_zv},
{"mod_rdp"_zv, "auth_channel"_zv},
{"mod_rdp"_zv, "checkout_channel"_zv},
{"mod_rdp"_zv, "alternate_shell"_zv},
{"mod_rdp"_zv, "shell_arguments"_zv},
{"mod_rdp"_zv, "shell_working_directory"_zv},
{"mod_rdp"_zv, "use_client_provided_alternate_shell"_zv},
{"mod_rdp"_zv, "use_client_provided_remoteapp"_zv},
{"mod_rdp"_zv, "use_native_remoteapp_capability"_zv},
{"mod_rdp"_zv, "application_driver_exe_or_file"_zv},
{"mod_rdp"_zv, "application_driver_script_argument"_zv},
{"mod_rdp"_zv, "application_driver_chrome_dt_script"_zv},
{"mod_rdp"_zv, "application_driver_chrome_uia_script"_zv},
{"mod_rdp"_zv, "application_driver_ie_script"_zv},
{"mod_rdp"_zv, "hide_client_name"_zv},
{"mod_rdp"_zv, "bogus_ios_rdpdr_virtual_channel"_zv},
{"mod_rdp"_zv, "enable_rdpdr_data_analysis"_zv},
{"mod_rdp"_zv, "remoteapp_bypass_legal_notice_delay"_zv},
{"mod_rdp"_zv, "remoteapp_bypass_legal_notice_timeout"_zv},
{"mod_rdp"_zv, "log_only_relevant_clipboard_activities"_zv},
{"mod_rdp"_zv, "experimental_fix_too_long_cookie"_zv},
{"mod_rdp"_zv, "split_domain"_zv},
{"mod_rdp"_zv, "wabam_uses_translated_remoteapp"_zv},
{"mod_rdp"_zv, "session_shadowing_support"_zv},
{"mod_rdp"_zv, "use_license_store"_zv},
{"mod_rdp"_zv, "enable_remotefx"_zv},
{"mod_rdp"_zv, "accept_monitor_layout_change_if_capture_is_not_started"_zv},
{"mod_rdp"_zv, "enable_restricted_admin_mode"_zv},
{"mod_rdp"_zv, "force_smartcard_authentication"_zv},
{"mod_rdp"_zv, "enable_ipv6"_zv},
{"mod_rdp"_zv, "auto_reconnection_on_losing_target_link"_zv},
{"mod_rdp"_zv, "forward_client_build_number"_zv},
{"mod_rdp"_zv, "bogus_monitor_layout_treatment"_zv},
{"session_probe"_zv, "enable_session_probe"_zv},
{"session_probe"_zv, "exe_or_file"_zv},
{"session_probe"_zv, "arguments"_zv},
{"session_probe"_zv, "use_smart_launcher"_zv},
{"session_probe"_zv, "enable_launch_mask"_zv},
{"session_probe"_zv, "on_launch_failure"_zv},
{"session_probe"_zv, "launch_timeout"_zv},
{"session_probe"_zv, "launch_fallback_timeout"_zv},
{"session_probe"_zv, "start_launch_timeout_timer_only_after_logon"_zv},
{"session_probe"_zv, "keepalive_timeout"_zv},
{"session_probe"_zv, "on_keepalive_timeout"_zv},
{"session_probe"_zv, "end_disconnected_session"_zv},
{"session_probe"_zv, "enable_autodeployed_appdriver_affinity"_zv},
{"session_probe"_zv, "enable_log"_zv},
{"session_probe"_zv, "enable_log_rotation"_zv},
{"session_probe"_zv, "log_level"_zv},
{"session_probe"_zv, "disconnected_application_limit"_zv},
{"session_probe"_zv, "disconnected_session_limit"_zv},
{"session_probe"_zv, "idle_session_limit"_zv},
{"session_probe"_zv, "smart_launcher_clipboard_initialization_delay"_zv},
{"session_probe"_zv, "smart_launcher_start_delay"_zv},
{"session_probe"_zv, "smart_launcher_long_delay"_zv},
{"session_probe"_zv, "smart_launcher_short_delay"_zv},
{"session_probe"_zv, "smart_launcher_enable_wabam_affinity"_zv},
{"session_probe"_zv, "launcher_abort_delay"_zv},
{"session_probe"_zv, "enable_crash_dump"_zv},
{"session_probe"_zv, "handle_usage_limit"_zv},
{"session_probe"_zv, "memory_usage_limit"_zv},
{"session_probe"_zv, "cpu_usage_alarm_threshold"_zv},
{"session_probe"_zv, "cpu_usage_alarm_action"_zv},
{"session_probe"_zv, "end_of_session_check_delay_time"_zv},
{"session_probe"_zv, "ignore_ui_less_processes_during_end_of_session_check"_zv},
{"session_probe"_zv, "childless_window_as_unidentified_input_field"_zv},
{"session_probe"_zv, "update_disabled_features"_zv},
{"session_probe"_zv, "disabled_features"_zv},
{"session_probe"_zv, "enable_bestsafe_interaction"_zv},
{"session_probe"_zv, "on_account_manipulation"_zv},
{"session_probe"_zv, "alternate_directory_environment_variable"_zv},
{"session_probe"_zv, "public_session"_zv},
{"session_probe"_zv, "customize_executable_name"_zv},
{"session_probe"_zv, "allow_multiple_handshake"_zv},
{"session_probe"_zv, "at_end_of_session_freeze_connection_and_wait"_zv},
{"session_probe"_zv, "enable_cleaner"_zv},
{"session_probe"_zv, "clipboard_based_launcher_reset_keyboard_status"_zv},
{"server_cert"_zv, "server_cert_store"_zv},
{"server_cert"_zv, "server_cert_check"_zv},
{"server_cert"_zv, "server_access_allowed_message"_zv},
{"server_cert"_zv, "server_cert_create_message"_zv},
{"server_cert"_zv, "server_cert_success_message"_zv},
{"server_cert"_zv, "server_cert_failure_message"_zv},
{"server_cert"_zv, "error_message"_zv},
{"mod_vnc"_zv, "clipboard_up"_zv},
{"mod_vnc"_zv, "clipboard_down"_zv},
{"mod_vnc"_zv, "encodings"_zv},
{"mod_vnc"_zv, "server_clipboard_encoding_type"_zv},
{"mod_vnc"_zv, "bogus_clipboard_infinite_loop"_zv},
{"mod_vnc"_zv, "server_is_macos"_zv},
{"mod_vnc"_zv, "server_unix_alt"_zv},
{"mod_vnc"_zv, "support_cursor_pseudo_encoding"_zv},
{"mod_vnc"_zv, "enable_ipv6"_zv},
{"session_log"_zv, "enable_session_log"_zv},
{"session_log"_zv, "enable_arcsight_log"_zv},
{"session_log"_zv, "keyboard_input_masking_level"_zv},
{"session_log"_zv, "hide_non_printable_kbd_input"_zv},
{"ocr"_zv, "version"_zv},
{"ocr"_zv, "locale"_zv},
{"ocr"_zv, "interval"_zv},
{"ocr"_zv, "on_title_bar_only"_zv},
{"ocr"_zv, "max_unrecog_char_rate"_zv},
{"video"_zv, "capture_flags"_zv},
{"video"_zv, "png_interval"_zv},
{"video"_zv, "break_interval"_zv},
{"video"_zv, "png_limit"_zv},
{"video"_zv, "hash_path"_zv},
{"video"_zv, "record_tmp_path"_zv},
{"video"_zv, "record_path"_zv},
{"video"_zv, "disable_keyboard_log"_zv},
{"video"_zv, "disable_clipboard_log"_zv},
{"video"_zv, "disable_file_system_log"_zv},
{"video"_zv, "wrm_color_depth_selection_strategy"_zv},
{"video"_zv, "wrm_compression_algorithm"_zv},
{"video"_zv, "bogus_vlc_frame_rate"_zv},
{"video"_zv, "codec_id"_zv},
{"video"_zv, "framerate"_zv},
{"video"_zv, "ffmpeg_options"_zv},
{"video"_zv, "notimestamp"_zv},
{"video"_zv, "smart_video_cropping"_zv},
{"video"_zv, "play_video_with_corrupted_bitmap"_zv},
{"video"_zv, "allow_rt_without_recording"_zv},
{"video"_zv, "file_permissions"_zv},
{"audit"_zv, "use_redis"_zv},
{"audit"_zv, "redis_timeout"_zv},
{"file_verification"_zv, "socket_path"_zv},
{"file_verification"_zv, "enable_up"_zv},
{"file_verification"_zv, "enable_down"_zv},
{"file_verification"_zv, "clipboard_text_up"_zv},
{"file_verification"_zv, "clipboard_text_down"_zv},
{"file_verification"_zv, "block_invalid_file_up"_zv},
{"file_verification"_zv, "block_invalid_file_down"_zv},
{"file_verification"_zv, "block_invalid_clipboard_text_up"_zv},
{"file_verification"_zv, "block_invalid_clipboard_text_down"_zv},
{"file_verification"_zv, "log_if_accepted"_zv},
{"file_verification"_zv, "max_file_size_rejected"_zv},
{"file_verification"_zv, "tmpdir"_zv},
{"file_storage"_zv, "store_file"_zv},
{"crypto"_zv, "encryption_key"_zv},
{"crypto"_zv, "sign_key"_zv},
{"websocket"_zv, "enable_websocket"_zv},
{"websocket"_zv, "use_tls"_zv},
{"websocket"_zv, "listen_address"_zv},
{"internal_mod"_zv, "enable_target_field"_zv},
{"mod_replay"_zv, "replay_path"_zv},
{"mod_replay"_zv, "on_end_of_data"_zv},
{"mod_replay"_zv, "replay_on_loop"_zv},
{"translation"_zv, "language"_zv},
{"translation"_zv, "login_language"_zv},
{"theme"_zv, "enable_theme"_zv},
{"theme"_zv, "logo"_zv},
{"theme"_zv, "bgcolor"_zv},
{"theme"_zv, "fgcolor"_zv},
{"theme"_zv, "separator_color"_zv},
{"theme"_zv, "focus_color"_zv},
{"theme"_zv, "error_color"_zv},
{"theme"_zv, "edit_bgcolor"_zv},
{"theme"_zv, "edit_fgcolor"_zv},
{"theme"_zv, "edit_focus_color"_zv},
{"theme"_zv, "tooltip_bgcolor"_zv},
{"theme"_zv, "tooltip_fgcolor"_zv},
{"theme"_zv, "tooltip_border_color"_zv},
{"theme"_zv, "selector_line1_bgcolor"_zv},
{"theme"_zv, "selector_line1_fgcolor"_zv},
{"theme"_zv, "selector_line2_bgcolor"_zv},
{"theme"_zv, "selector_line2_fgcolor"_zv},
{"theme"_zv, "selector_selected_bgcolor"_zv},
{"theme"_zv, "selector_selected_fgcolor"_zv},
{"theme"_zv, "selector_focus_bgcolor"_zv},
{"theme"_zv, "selector_focus_fgcolor"_zv},
{"theme"_zv, "selector_label_bgcolor"_zv},
{"theme"_zv, "selector_label_fgcolor"_zv},
{"debug"_zv, "fake_target_ip"_zv},
{"debug"_zv, "capture"_zv},
{"debug"_zv, "auth"_zv},
{"debug"_zv, "session"_zv},
{"debug"_zv, "front"_zv},
{"debug"_zv, "mod_rdp"_zv},
{"debug"_zv, "mod_vnc"_zv},
{"debug"_zv, "mod_internal"_zv},
{"debug"_zv, "sck_mod"_zv},
{"debug"_zv, "sck_front"_zv},
{"debug"_zv, "password"_zv},
{"debug"_zv, "compression"_zv},
{"debug"_zv, "cache"_zv},
{"debug"_zv, "ocr"_zv},
{"debug"_zv, "ffmpeg"_zv},
{"debug"_zv, "config"_zv},
{"debug"_zv, "mod_rdp_use_failure_simulation_socket_transport"_zv},
{"debug"_zv, "probe_client_addresses"_zv},
};
} // namespace configs::cfg_ini_infos
