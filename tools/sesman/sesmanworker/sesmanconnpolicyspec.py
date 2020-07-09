#!/usr/bin/python -O
# -*- coding: utf-8 -*-

# DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN

cp_spec = {
    'session': {
        u'globals:inactivity_timeout': (
            'inactivity_timeout', 0
        ),
    },
    'session_log': {
        u'session_log:keyboard_input_masking_level': (
            'keyboard_input_masking_level', 2
        ),
    },
    'rdp': {
        u'mod_rdp:disabled_orders': (
            'disabled_orders', u''
        ),
        u'mod_rdp:enable_nla': (
            'enable_nla', True
        ),
        u'mod_rdp:enable_kerberos': (
            'enable_kerberos', False
        ),
        u'mod_rdp:tls_min_level': (
            'tls_min_level', 0
        ),
        u'mod_rdp:tls_max_level': (
            'tls_max_level', 0
        ),
        u'mod_rdp:cipher_string': (
            'cipher_string', u'ALL'
        ),
        u'mod_rdp:show_common_cipher_list': (
            'show_common_cipher_list', False
        ),
        u'mod_rdp:allowed_dynamic_channels': (
            'allowed_dynamic_channels', u'*'
        ),
        u'mod_rdp:denied_dynamic_channels': (
            'denied_dynamic_channels', u''
        ),
        u'mod_rdp:server_redirection_support': (
            'server_redirection', False
        ),
        u'mod_rdp:load_balance_info': (
            'load_balance_info', u''
        ),
        u'mod_rdp:use_client_provided_alternate_shell': (
            'use_client_provided_alternate_shell', False
        ),
        u'mod_rdp:use_client_provided_remoteapp': (
            'use_client_provided_remoteapp', False
        ),
        u'mod_rdp:use_native_remoteapp_capability': (
            'use_native_remoteapp_capability', True
        ),
        u'mod_rdp:wabam_uses_translated_remoteapp': (
            'wabam_uses_translated_remoteapp', False
        ),
        u'mod_rdp:enable_remotefx': (
            'enable_remotefx', False
        ),
        u'mod_rdp:enable_restricted_admin_mode': (
            'enable_restricted_admin_mode', False
        ),
        u'mod_rdp:force_smartcard_authentication': (
            'force_smartcard_authentication', False
        ),
        u'mod_rdp:enable_ipv6': (
            'enable_ipv6', False
        ),
        u'mod_rdp:mode_console': (
            'mode_console', 0
        ),
        u'context:rail_disconnect_message_delay': (
            'remote_programs_disconnect_message_delay', 3000
        ),
        u'context:use_session_probe_to_launch_remote_program': (
            'use_session_probe_to_launch_remote_program', True
        ),
    },
    'session_probe': {
        u'mod_rdp:enable_session_probe': (
            'enable_session_probe', False
        ),
        u'mod_rdp:session_probe_use_smart_launcher': (
            'use_smart_launcher', True
        ),
        u'mod_rdp:session_probe_enable_launch_mask': (
            'enable_launch_mask', True
        ),
        u'enable_session_probe_launch_mask': (
            'enable_launch_mask', None
        ),  # Deprecated, for compatibility only.
        u'mod_rdp:session_probe_on_launch_failure': (
            'on_launch_failure', 1
        ),
        u'mod_rdp:session_probe_launch_timeout': (
            'launch_timeout', 40000
        ),
        u'mod_rdp:session_probe_launch_fallback_timeout': (
            'launch_fallback_timeout', 10000
        ),
        u'mod_rdp:session_probe_start_launch_timeout_timer_only_after_logon': (
            'start_launch_timeout_timer_only_after_logon', True
        ),
        u'mod_rdp:session_probe_keepalive_timeout': (
            'keepalive_timeout', 5000
        ),
        u'mod_rdp:session_probe_on_keepalive_timeout': (
            'on_keepalive_timeout', 1
        ),
        u'mod_rdp:session_probe_end_disconnected_session': (
            'end_disconnected_session', False
        ),
        u'mod_rdp:session_probe_enable_log': (
            'enable_log', False
        ),
        u'mod_rdp:session_probe_enable_log_rotation': (
            'enable_log_rotation', True
        ),
        u'mod_rdp:session_probe_disconnected_application_limit': (
            'disconnected_application_limit', 0
        ),
        u'mod_rdp:session_probe_disconnected_session_limit': (
            'disconnected_session_limit', 0
        ),
        u'mod_rdp:session_probe_idle_session_limit': (
            'idle_session_limit', 0
        ),
        u'mod_rdp:session_probe_clipboard_based_launcher_clipboard_initialization_delay': (
            'smart_launcher_clipboard_initialization_delay', 2000
        ),
        u'mod_rdp:session_probe_clipboard_based_launcher_start_delay': (
            'smart_launcher_start_delay', 0
        ),
        u'mod_rdp:session_probe_clipboard_based_launcher_long_delay': (
            'smart_launcher_long_delay', 500
        ),
        u'mod_rdp:session_probe_clipboard_based_launcher_short_delay': (
            'smart_launcher_short_delay', 50
        ),
        u'mod_rdp:session_probe_launcher_abort_delay': (
            'launcher_abort_delay', 2000
        ),
        u'mod_rdp:session_probe_enable_crash_dump': (
            'enable_crash_dump', False
        ),
        u'mod_rdp:session_probe_handle_usage_limit': (
            'handle_usage_limit', 0
        ),
        u'mod_rdp:session_probe_memory_usage_limit': (
            'memory_usage_limit', 0
        ),
        u'mod_rdp:session_probe_end_of_session_check_delay_time': (
            'end_of_session_check_delay_time', 0
        ),
        u'mod_rdp:session_probe_ignore_ui_less_processes_during_end_of_session_check': (
            'ignore_ui_less_processes_during_end_of_session_check', True
        ),
        u'mod_rdp:session_probe_update_disabled_features': (
            'update_disabled_features', True
        ),
        u'mod_rdp:session_probe_childless_window_as_unidentified_input_field': (
            'childless_window_as_unidentified_input_field', True
        ),
        u'mod_rdp:session_probe_disabled_features': (
            'disabled_features', 352
        ),
        u'mod_rdp:session_probe_bestsafe_integration': (
            'enable_bestsafe_interaction', False
        ),
        u'mod_rdp:session_probe_alternate_directory_environment_variable': (
            'alternate_directory_environment_variable', u''
        ),
        u'mod_rdp:session_probe_public_session': (
            'public_session', False
        ),
        u'mod_rdp:session_probe_on_account_manipulation': (
            'on_account_manipulation', 0
        ),
        u'context:session_probe_outbound_connection_monitoring_rules': (
            'outbound_connection_monitoring_rules', u''
        ),
        u'context:session_probe_process_monitoring_rules': (
            'process_monitoring_rules', u''
        ),
        u'context:session_probe_extra_system_processes': (
            'extra_system_processes', u''
        ),
        u'context:session_probe_windows_of_these_applications_as_unidentified_input_field': (
            'windows_of_these_applications_as_unidentified_input_field', u''
        ),
    },
    'server_cert': {
        u'mod_rdp:server_cert_store': (
            'server_cert_store', True
        ),
        u'mod_rdp:server_cert_check': (
            'server_cert_check', 1
        ),
        u'mod_rdp:server_access_allowed_message': (
            'server_access_allowed_message', 1
        ),
        u'mod_rdp:server_cert_create_message': (
            'server_cert_create_message', 1
        ),
        u'mod_rdp:server_cert_success_message': (
            'server_cert_success_message', 1
        ),
        u'mod_rdp:server_cert_failure_message': (
            'server_cert_failure_message', 1
        ),
    },
    'file_verification': {
        u'file_verification:enable_up': (
            'enable_up', False
        ),
        u'file_verification:enable_down': (
            'enable_down', False
        ),
        u'file_verification:clipboard_text_up': (
            'clipboard_text_up', False
        ),
        u'file_verification:clipboard_text_down': (
            'clipboard_text_down', False
        ),
        u'file_verification:block_invalid_file_up': (
            'block_invalid_file_up', False
        ),
        u'file_verification:block_invalid_file_down': (
            'block_invalid_file_down', False
        ),
        u'file_verification:log_if_accepted': (
            'log_if_accepted', True
        ),
        u'file_verification:max_file_size_rejected': (
            'max_file_size_rejected', 1024
        ),
    },
    'file_storage': {
        u'file_storage:store_file': (
            'store_file', 0
        ),
    },
    'video': {
        u'video:disable_keyboard_log': (
            'disable_keyboard_log', 1
        ),
    },
    'vnc': {
        u'mod_vnc:server_is_macos': (
            'server_is_macos', False
        ),
        u'mod_vnc:server_unix_alt': (
            'server_unix_alt', False
        ),
        u'mod_vnc:support_cursor_pseudo_encoding': (
            'support_cursor_pseudo_encoding', True
        ),
        u'mod_vnc:enable_ipv6': (
            'enable_ipv6', False
        ),
    },
}
