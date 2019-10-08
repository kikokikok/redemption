//
// DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN
//

#pragma once

#include "utils/sugar/array_view.hpp"

enum authid_t : unsigned;

constexpr authid_t MAX_AUTHID = authid_t(174);

constexpr array_view_const_char const authstr[] = {
    "capture_chunk"_av,
    "login"_av,
    "ip_client"_av,
    "ip_target"_av,
    "target_device"_av,
    "device_id"_av,
    "primary_user_id"_av,
    "target_login"_av,
    "target_application"_av,
    "target_application_account"_av,
    "target_application_password"_av,
    "inactivity_timeout"_av,
    "trace_type"_av,
    "is_rec"_av,
    "rec_path"_av,
    "unicode_keyboard_event_support"_av,
    "mod_recv_timeout"_av,
    "session_log_path"_av,
    "keyboard_input_masking_level"_av,
    "keyboard_layout"_av,
    "disable_tsk_switch_shortcuts"_av,
    "enable_nla"_av,
    "enable_kerberos"_av,
    "server_redirection"_av,
    "load_balance_info"_av,
    "rdp_bogus_sc_net_size"_av,
    "proxy_managed_drives"_av,
    "ignore_auth_channel"_av,
    "alternate_shell"_av,
    "shell_arguments"_av,
    "shell_working_directory"_av,
    "use_client_provided_alternate_shell"_av,
    "use_client_provided_remoteapp"_av,
    "use_native_remoteapp_capability"_av,
    "session_probe"_av,
    "session_probe_use_smart_launcher"_av,
    "session_probe_enable_launch_mask"_av,
    "session_probe_on_launch_failure"_av,
    "session_probe_launch_timeout"_av,
    "session_probe_launch_fallback_timeout"_av,
    "session_probe_start_launch_timeout_timer_only_after_logon"_av,
    "session_probe_keepalive_timeout"_av,
    "session_probe_on_keepalive_timeout"_av,
    "session_probe_end_disconnected_session"_av,
    "session_probe_enable_log"_av,
    "session_probe_enable_log_rotation"_av,
    "session_probe_disconnected_application_limit"_av,
    "session_probe_disconnected_session_limit"_av,
    "session_probe_idle_session_limit"_av,
    "session_probe_smart_launcher_clipboard_initialization_delay"_av,
    "session_probe_smart_launcher_start_delay"_av,
    "session_probe_smart_launcher_long_delay"_av,
    "session_probe_smart_launcher_short_delay"_av,
    "session_probe_launcher_abort_delay"_av,
    "session_probe_enable_crash_dump"_av,
    "session_probe_handle_usage_limit"_av,
    "session_probe_memory_usage_limit"_av,
    "session_probe_ignore_ui_less_processes_during_end_of_session_check"_av,
    "session_probe_childless_window_as_unidentified_input_field"_av,
    "session_probe_disabled_features"_av,
    "session_probe_public_session"_av,
    "server_cert_store"_av,
    "server_cert_check"_av,
    "server_access_allowed_message"_av,
    "server_cert_create_message"_av,
    "server_cert_success_message"_av,
    "server_cert_failure_message"_av,
    "enable_rdpdr_data_analysis"_av,
    "wabam_uses_translated_remoteapp"_av,
    "enable_server_cert_external_validation"_av,
    "server_cert"_av,
    "server_cert_response"_av,
    "enable_remotefx"_av,
    "clipboard_up"_av,
    "clipboard_down"_av,
    "vnc_server_clipboard_encoding_type"_av,
    "vnc_bogus_clipboard_infinite_loop"_av,
    "server_is_apple"_av,
    "server_unix_alt"_av,
    "enable_up"_av,
    "enable_down"_av,
    "log_if_accepted"_av,
    "clipboard_text_data"_av,
    "clipboard_text_data"_av,
    "replay_on_loop"_av,
    "disable_keyboard_log"_av,
    "rt_display"_av,
    "encryption_key"_av,
    "sign_key"_av,
    "allow_resize_hosted_desktop"_av,
    "language"_av,
    "password_en"_av,
    "password_fr"_av,
    "psid"_av,
    "bpp"_av,
    "height"_av,
    "width"_av,
    "selector"_av,
    "selector_current_page"_av,
    "selector_device_filter"_av,
    "selector_group_filter"_av,
    "selector_proto_filter"_av,
    "selector_lines_per_page"_av,
    "selector_number_of_pages"_av,
    "target_password"_av,
    "target_host"_av,
    "target_str"_av,
    "target_service"_av,
    "target_port"_av,
    "proto_dest"_av,
    "password"_av,
    "reporting"_av,
    "auth_channel_answer"_av,
    "auth_channel_target"_av,
    "message"_av,
    "accept_message"_av,
    "display_message"_av,
    "rejected"_av,
    "authenticated"_av,
    "keepalive"_av,
    "session_id"_av,
    "timeclose"_av,
    "mode_console"_av,
    "real_target_device"_av,
    "authentication_challenge"_av,
    "ticket"_av,
    "comment"_av,
    "duration"_av,
    "duration_max"_av,
    "waitinforeturn"_av,
    "showform"_av,
    "formflag"_av,
    "module"_av,
    "forcemodule"_av,
    "proxy_opt"_av,
    "pattern_kill"_av,
    "pattern_notify"_av,
    "opt_message"_av,
    "login_message"_av,
    "session_probe_outbound_connection_monitoring_rules"_av,
    "session_probe_process_monitoring_rules"_av,
    "session_probe_extra_system_processes"_av,
    "session_probe_windows_of_these_applications_as_unidentified_input_field"_av,
    "disconnect_reason"_av,
    "disconnect_reason_ack"_av,
    "recording_started"_av,
    "rt_ready"_av,
    "auth_command"_av,
    "auth_notify"_av,
    "auth_notify_rail_exec_flags"_av,
    "auth_notify_rail_exec_exe_or_file"_av,
    "auth_command_rail_exec_exec_result"_av,
    "auth_command_rail_exec_flags"_av,
    "auth_command_rail_exec_original_exe_or_file"_av,
    "auth_command_rail_exec_exe_or_file"_av,
    "auth_command_rail_exec_working_dir"_av,
    "auth_command_rail_exec_arguments"_av,
    "auth_command_rail_exec_account"_av,
    "auth_command_rail_exec_password"_av,
    "rail_disconnect_message_delay"_av,
    "use_session_probe_to_launch_remote_program"_av,
    "session_probe_launch_error_message"_av,
    "is_wabam"_av,
    "pm_response"_av,
    "pm_request"_av,
    "native_session_id"_av,
    "rd_shadow_available"_av,
    "rd_shadow_userdata"_av,
    "rd_shadow_type"_av,
    "rd_shadow_invitation_error_code"_av,
    "rd_shadow_invitation_error_message"_av,
    "rd_shadow_invitation_id"_av,
    "rd_shadow_invitation_addr"_av,
    "rd_shadow_invitation_port"_av,
};
