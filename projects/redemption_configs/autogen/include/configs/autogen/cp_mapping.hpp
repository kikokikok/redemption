//
// DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN
//

R"xxxyyyzzz(# -*- coding: utf-8 -*-
#
# DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN
# 

cp_spec = {
'rdp': ({
    'session': {
        ('globals:inactivity_timeout', 'inactivity_timeout', 0),
    },
    'all_target_mod': {
        ('all_target_mod:tcp_user_timeout', 'tcp_user_timeout', 0),
    },
    'rdp': {
        ('mod_rdp:disabled_orders', 'disabled_orders', ''),
        ('mod_rdp:enable_nla', 'enable_nla', True),
        ('mod_rdp:enable_kerberos', 'enable_kerberos', False),
        ('mod_rdp:tls_min_level', 'tls_min_level', 0),
        ('mod_rdp:tls_max_level', 'tls_max_level', 0),
        ('mod_rdp:cipher_string', 'cipher_string', 'ALL'),
        ('mod_rdp:show_common_cipher_list', 'show_common_cipher_list', False),
        ('mod_rdp:allowed_dynamic_channels', 'allowed_dynamic_channels', '*'),
        ('mod_rdp:denied_dynamic_channels', 'denied_dynamic_channels', ''),
        ('mod_rdp:server_redirection_support', 'server_redirection', False),
        ('mod_rdp:load_balance_info', 'load_balance_info', ''),
        ('mod_rdp:use_client_provided_alternate_shell', 'use_client_provided_alternate_shell', False),
        ('mod_rdp:use_client_provided_remoteapp', 'use_client_provided_remoteapp', False),
        ('mod_rdp:use_native_remoteapp_capability', 'use_native_remoteapp_capability', True),
        ('mod_rdp:enable_rdpdr_data_analysis', 'enable_rdpdr_data_analysis', True),
        ('mod_rdp:wabam_uses_translated_remoteapp', 'wabam_uses_translated_remoteapp', False),
        ('mod_rdp:enable_remotefx', 'enable_remotefx', False),
        ('mod_rdp:enable_restricted_admin_mode', 'enable_restricted_admin_mode', False),
        ('mod_rdp:force_smartcard_authentication', 'force_smartcard_authentication', False),
        ('mod_rdp:enable_ipv6', 'enable_ipv6', True),
        ('mod_rdp:mode_console', 'mode_console', 'allow'),
        ('mod_rdp:auto_reconnection_on_losing_target_link', 'auto_reconnection_on_losing_target_link', False),
        ('mod_rdp:forward_client_build_number', 'forward_client_build_number', True),
        ('mod_rdp:bogus_monitor_layout_treatment', 'bogus_monitor_layout_treatment', False),
        ('mod_rdp:remote_programs_disconnect_message_delay', 'remote_programs_disconnect_message_delay', 3000),
        ('mod_rdp:use_session_probe_to_launch_remote_program', 'use_session_probe_to_launch_remote_program', True),
    },
    'session_probe': {
        ('session_probe:enable_session_probe', 'enable_session_probe', True),
        ('session_probe:use_smart_launcher', 'use_smart_launcher', True),
        ('session_probe:enable_launch_mask', 'enable_launch_mask', True),
        ('session_probe:on_launch_failure', 'on_launch_failure', 1),
        ('session_probe:launch_timeout', 'launch_timeout', 40000),
        ('session_probe:launch_fallback_timeout', 'launch_fallback_timeout', 40000),
        ('session_probe:start_launch_timeout_timer_only_after_logon', 'start_launch_timeout_timer_only_after_logon', True),
        ('session_probe:keepalive_timeout', 'keepalive_timeout', 5000),
        ('session_probe:on_keepalive_timeout', 'on_keepalive_timeout', 1),
        ('session_probe:end_disconnected_session', 'end_disconnected_session', False),
        ('session_probe:enable_log', 'enable_log', False),
        ('session_probe:enable_log_rotation', 'enable_log_rotation', False),
        ('session_probe:log_level', 'log_level', 5),
        ('session_probe:disconnected_application_limit', 'disconnected_application_limit', 0),
        ('session_probe:disconnected_session_limit', 'disconnected_session_limit', 0),
        ('session_probe:idle_session_limit', 'idle_session_limit', 0),
        ('session_probe:smart_launcher_clipboard_initialization_delay', 'smart_launcher_clipboard_initialization_delay', 2000),
        ('session_probe:smart_launcher_start_delay', 'smart_launcher_start_delay', 0),
        ('session_probe:smart_launcher_long_delay', 'smart_launcher_long_delay', 500),
        ('session_probe:smart_launcher_short_delay', 'smart_launcher_short_delay', 50),
        ('session_probe:launcher_abort_delay', 'launcher_abort_delay', 2000),
        ('session_probe:enable_crash_dump', 'enable_crash_dump', False),
        ('session_probe:handle_usage_limit', 'handle_usage_limit', 0),
        ('session_probe:memory_usage_limit', 'memory_usage_limit', 0),
        ('session_probe:cpu_usage_alarm_threshold', 'cpu_usage_alarm_threshold', 0),
        ('session_probe:cpu_usage_alarm_action', 'cpu_usage_alarm_action', 0),
        ('session_probe:end_of_session_check_delay_time', 'end_of_session_check_delay_time', 0),
        ('session_probe:ignore_ui_less_processes_during_end_of_session_check', 'ignore_ui_less_processes_during_end_of_session_check', True),
        ('session_probe:childless_window_as_unidentified_input_field', 'childless_window_as_unidentified_input_field', True),
        ('session_probe:update_disabled_features', 'update_disabled_features', True),
        ('session_probe:disabled_features', 'disabled_features', 352),
        ('session_probe:enable_bestsafe_interaction', 'enable_bestsafe_interaction', False),
        ('session_probe:on_account_manipulation', 'on_account_manipulation', 0),
        ('session_probe:alternate_directory_environment_variable', 'alternate_directory_environment_variable', ''),
        ('session_probe:public_session', 'public_session', False),
        ('session_probe:outbound_connection_monitoring_rules', 'outbound_connection_monitoring_rules', ''),
        ('session_probe:process_monitoring_rules', 'process_monitoring_rules', ''),
        ('session_probe:extra_system_processes', 'extra_system_processes', ''),
        ('session_probe:windows_of_these_applications_as_unidentified_input_field', 'windows_of_these_applications_as_unidentified_input_field', ''),
    },
    'server_cert': {
        ('server_cert:server_cert_store', 'server_cert_store', True),
        ('server_cert:server_cert_check', 'server_cert_check', 1),
        ('server_cert:server_access_allowed_message', 'server_access_allowed_message', 1),
        ('server_cert:server_cert_create_message', 'server_cert_create_message', 1),
        ('server_cert:server_cert_success_message', 'server_cert_success_message', 1),
        ('server_cert:server_cert_failure_message', 'server_cert_failure_message', 1),
    },
    'session_log': {
        ('session_log:keyboard_input_masking_level', 'keyboard_input_masking_level', 2),
    },
    'video': {
        ('capture:disable_keyboard_log', 'disable_keyboard_log', 1),
    },
    'file_verification': {
        ('file_verification:enable_up', 'enable_up', False),
        ('file_verification:enable_down', 'enable_down', False),
        ('file_verification:clipboard_text_up', 'clipboard_text_up', False),
        ('file_verification:clipboard_text_down', 'clipboard_text_down', False),
        ('file_verification:block_invalid_file_up', 'block_invalid_file_up', False),
        ('file_verification:block_invalid_file_down', 'block_invalid_file_down', False),
        ('file_verification:log_if_accepted', 'log_if_accepted', True),
        ('file_verification:max_file_size_rejected', 'max_file_size_rejected', 256),
    },
    'file_storage': {
        ('file_storage:store_file', 'store_file', 'never'),
    },
}, {
}),
'rdp-jumphost': ({
    'session': {
        ('globals:inactivity_timeout', 'inactivity_timeout', 0),
    },
    'rdp': {
        ('mod_rdp:disabled_orders', 'disabled_orders', ''),
        ('mod_rdp:tls_min_level', 'tls_min_level', 0),
        ('mod_rdp:tls_max_level', 'tls_max_level', 0),
        ('mod_rdp:cipher_string', 'cipher_string', 'ALL'),
        ('mod_rdp:show_common_cipher_list', 'show_common_cipher_list', False),
        ('mod_rdp:allowed_dynamic_channels', 'allowed_dynamic_channels', '*'),
        ('mod_rdp:denied_dynamic_channels', 'denied_dynamic_channels', ''),
        ('mod_rdp:enable_rdpdr_data_analysis', 'enable_rdpdr_data_analysis', True),
        ('mod_rdp:force_smartcard_authentication', 'force_smartcard_authentication', False),
        ('mod_rdp:enable_ipv6', 'enable_ipv6', True),
        ('mod_rdp:auto_reconnection_on_losing_target_link', 'auto_reconnection_on_losing_target_link', False),
        ('mod_rdp:forward_client_build_number', 'forward_client_build_number', True),
    },
    'server_cert': {
        ('server_cert:server_cert_store', 'server_cert_store', True),
        ('server_cert:server_cert_check', 'server_cert_check', 1),
        ('server_cert:server_access_allowed_message', 'server_access_allowed_message', 1),
        ('server_cert:server_cert_create_message', 'server_cert_create_message', 1),
        ('server_cert:server_cert_success_message', 'server_cert_success_message', 1),
        ('server_cert:server_cert_failure_message', 'server_cert_failure_message', 1),
    },
    'session_log': {
        ('session_log:keyboard_input_masking_level', 'keyboard_input_masking_level', 2),
    },
    'video': {
        ('capture:disable_keyboard_log', 'disable_keyboard_log', 1),
    },
    'file_verification': {
        ('file_verification:enable_up', 'enable_up', False),
        ('file_verification:enable_down', 'enable_down', False),
        ('file_verification:clipboard_text_up', 'clipboard_text_up', False),
        ('file_verification:clipboard_text_down', 'clipboard_text_down', False),
        ('file_verification:block_invalid_file_up', 'block_invalid_file_up', False),
        ('file_verification:block_invalid_file_down', 'block_invalid_file_down', False),
        ('file_verification:log_if_accepted', 'log_if_accepted', True),
        ('file_verification:max_file_size_rejected', 'max_file_size_rejected', 256),
    },
    'file_storage': {
        ('file_storage:store_file', 'store_file', 'never'),
    },
}, {
    'mod_rdp:enable_nla': False,
    'session_probe:enable_session_probe': False,
}),
'vnc': ({
    'session': {
        ('globals:inactivity_timeout', 'inactivity_timeout', 0),
    },
    'vnc': {
        ('mod_vnc:server_is_macos', 'server_is_macos', False),
        ('mod_vnc:server_unix_alt', 'server_unix_alt', False),
        ('mod_vnc:support_cursor_pseudo_encoding', 'support_cursor_pseudo_encoding', True),
        ('mod_vnc:enable_ipv6', 'enable_ipv6', True),
    },
}, {
}),
}
)xxxyyyzzz"
