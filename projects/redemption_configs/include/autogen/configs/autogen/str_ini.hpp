#include "config_variant.hpp"

#define CONFIG_PP_STRINGIZE_I(x) #x
#define CONFIG_PP_STRINGIZE(x) CONFIG_PP_STRINGIZE_I(x)


"## Config file for RDP proxy.\n\n\n"
"[globals]\n\n"

"# Support of Bitmap Cache.\n"
"#_advanced\n"
"#bitmap_cache = 1\n\n"

"#_advanced\n"
"#glyph_cache = 0\n\n"

"#_advanced\n"
"#port = 3389\n\n"

"#_advanced\n"
"#nomouse = 0\n\n"

"#_advanced\n"
"#notimestamp = 0\n\n"

"#_advanced\n"
"#encryptionLevel = low\n\n"

"#_advanced\n"
"#authfile = " REDEMPTION_CONFIG_AUTHFILE "\n\n"

"# Time out during RDP handshake stage.\n"
"# (is in second)\n"
"#handshake_timeout = 10\n\n"

"# No traffic auto disconnection.\n"
"# (is in second)\n"
"#session_timeout = 900\n\n"

"# No traffic auto disconnection.\n"
"# (is in second)\n"
"#_hidden\n"
"#inactivity_timeout = 0\n\n"

"# Keepalive.\n"
"# (is in second)\n"
"#_advanced\n"
"#keepalive_grace_delay = 30\n\n"

"# Specifies the time to spend on the login screen of proxy RDP before closing client window (0 to desactivate).\n"
"# (is in second)\n"
"#_advanced\n"
"#authentication_timeout = 120\n\n"

"# Specifies the time to spend on the close box of proxy RDP before closing client window (0 to desactivate).\n"
"# (is in second)\n"
"#_advanced\n"
"#close_timeout = 600\n\n"

"# Session record options.\n"
"#   0: No encryption (faster).\n"
"#   1: No encryption, with checksum.\n"
"#   2: Encryption enabled.\n"
"# When session records are encrypted, they can be read only by the WAB where they have been generated.\n"
"#_advanced\n"
"#trace_type = 1\n\n"

"#_advanced\n"
"#listen_address = 0.0.0.0\n\n"

"# Allow Transparent mode.\n"
"#_iptables\n"
"#enable_transparent_mode = 0\n\n"

"# Proxy certificate password.\n"
"#_advanced\n"
"#_password\n"
"#certificate_password = inquisition\n\n"

"#_advanced\n"
"#png_path = " << app_path(AppPath::Png) << "\n\n"

"#_advanced\n"
"#wrm_path = " << app_path(AppPath::Wrm) << "\n\n"

"#_advanced\n"
"#movie_path = \n\n"

"# Support of Bitmap Update.\n"
"#_advanced\n"
"#enable_bitmap_update = 1\n\n"

"# Show close screen.\n"
"#enable_close_box = 1\n\n"

"#_advanced\n"
"#enable_osd = 1\n\n"

"#_advanced\n"
"#enable_osd_display_remote_target = 1\n\n"

"#_advanced\n"
"#persistent_path = " << app_path(AppPath::Persistent) << "\n\n"

"#_hidden\n"
"#enable_wab_integration = " CONFIG_PP_STRINGIZE(REDEMPTION_CONFIG_ENABLE_WAB_INTEGRATION) "\n\n"

"#allow_using_multiple_monitors = 1\n\n"

"# Needed to refresh screen of Windows Server 2012.\n"
"#_advanced\n"
"#bogus_refresh_rect = 1\n\n"

"#_advanced\n"
"#codec_id = flv\n\n"

"#_advanced\n"
"#video_quality = high\n\n"

"#_advanced\n"
"#large_pointer_support = 1\n\n"

"#unicode_keyboard_event_support = 1\n\n"

"# (is in millisecond)\n"
"#_advanced\n"
"#mod_recv_timeout = 1000\n\n"

"#_advanced\n"
"#spark_view_specific_glyph_width = 0\n\n"

"#_advanced\n"
"#experimental_enable_serializer_data_block_size_limit = 0\n\n"

"#_advanced\n"
"#experimental_support_resize_session_during_recording = 0\n\n"

"[session_log]\n\n"

"#enable_session_log = 1\n\n"

"#_advanced\n"
"#log_path = \n\n"

"#   0: keyboard input are not masked\n"
"#   1: only passwords are masked\n"
"#   2: passwords and unidentified texts are masked\n"
"#   3: keyboard input are fully masked\n"
"#_advanced\n"
"#keyboard_input_masking_level = 2\n\n"

"#_advanced\n"
"#hide_non_printable_kbd_input = 0\n\n"

"[client]\n\n"

"# cs-CZ, da-DK, de-DE, el-GR, en-US, es-ES, fi-FI.finnish, fr-FR, is-IS, it-IT, nl-NL, nb-NO, pl-PL.programmers, pt-BR.abnt, ro-RO, ru-RU, hr-HR, sk-SK, sv-SE, tr-TR.q, uk-UA, sl-SI, et-EE, lv-LV, lt-LT.ibm, mk-MK, fo-FO, mt-MT.47, se-NO, kk-KZ, ky-KG, tt-RU, mn-MN, cy-GB, lb-LU, mi-NZ, de-CH, en-GB, es-MX, fr-BE.fr, nl-BE, pt-PT, sr-La, se-SE, uz-Cy, iu-La, fr-CA, sr-Cy, en-CA.fr, fr-CH, bs-Cy, bg-BG.latin, cs-CZ.qwerty, en-IE.irish, de-DE.ibm, el-GR.220, es-ES.variation, hu-HU, en-US.dvorak, it-IT.142, pl-PL, pt-BR.abnt2, ru-RU.typewriter, sk-SK.qwerty, tr-TR.f, lv-LV.qwerty, lt-LT, mt-MT.48, se-NO.ext_norway, fr-BE, se-SE, en-CA.multilingual, en-IE, cs-CZ.programmers, el-GR.319, en-US.international, se-SE.ext_finland_sweden, bg-BG, el-GR.220_latin, en-US.dvorak_left, el-GR.319_latin, en-US.dvorak_right, el-GR.latin, el-GR.polytonic\n"
"#_advanced\n"
"#keyboard_layout_proposals = en-US, fr-FR, de-DE, ru-RU\n\n"

"# If true, ignore password provided by RDP client, user need do login manually.\n"
"#_advanced\n"
"#ignore_logon_password = 0\n\n"

"# Enable font smoothing (0x80).\n"
"#_advanced\n"
"#_hex\n"
"#performance_flags_default = 128\n\n"

"# Disable theme (0x8).\n"
"# Disable mouse cursor shadows (0x20).\n"
"#_advanced\n"
"#_hex\n"
"#performance_flags_force_present = 40\n\n"

"#_advanced\n"
"#_hex\n"
"#performance_flags_force_not_present = 0\n\n"

"# If enabled, avoid automatically font smoothing in recorded session.\n"
"#_advanced\n"
"#auto_adjust_performance_flags = 1\n\n"

"# Fallback to RDP Legacy Encryption if client does not support TLS.\n"
"#tls_fallback_legacy = 0\n\n"

"#tls_support = 1\n\n"

"# Needed to connect with jrdp, based on bogus X224 layer code.\n"
"#_advanced\n"
"#bogus_neg_request = 0\n\n"

"# Needed to connect with Remmina 0.8.3 and freerdp 0.9.4, based on bogus MCS layer code.\n"
"#_advanced\n"
"#bogus_user_id = 1\n\n"

"# If enabled, ignore CTRL+ALT+DEL and CTRL+SHIFT+ESCAPE (or the equivalents) keyboard sequences.\n"
"#_advanced\n"
"#disable_tsk_switch_shortcuts = 0\n\n"

"# Specifies the highest compression package support available on the front side\n"
"#   0: The RDP bulk compression is disabled\n"
"#   1: RDP 4.0 bulk compression\n"
"#   2: RDP 5.0 bulk compression\n"
"#   3: RDP 6.0 bulk compression\n"
"#   4: RDP 6.1 bulk compression\n"
"#_advanced\n"
"#rdp_compression = 4\n\n"

"# Specifies the maximum color resolution (color depth) for client session:\n"
"#   8: 8-bit\n"
"#   15: 15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)\n"
"#   16: 16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)\n"
"#   24: 24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)\n"
"#_advanced\n"
"#max_color_depth = 24\n\n"

"# Persistent Disk Bitmap Cache on the front side.\n"
"#_advanced\n"
"#persistent_disk_bitmap_cache = 1\n\n"

"# Support of Cache Waiting List (this value is ignored if Persistent Disk Bitmap Cache is disabled).\n"
"#_advanced\n"
"#cache_waiting_list = 0\n\n"

"# If enabled, the contents of Persistent Bitmap Caches are stored on disk.\n"
"#_advanced\n"
"#persist_bitmap_cache_on_disk = 0\n\n"

"# Support of Bitmap Compression.\n"
"#_advanced\n"
"#bitmap_compression = 1\n\n"

"# Enables support of Clent Fast-Path Input Event PDUs.\n"
"#_advanced\n"
"#fast_path = 1\n\n"

"#enable_suppress_output = 1\n\n"

"# [Not configured]: Compatible with more RDP clients (less secure)\n"
"# HIGH:!ADH:!3DES: Compatible only with MS Windows 7 client or more recent (moderately secure)HIGH:!ADH:!3DES:!SHA: Compatible only with MS Server Windows 2008 R2 client or more recent (more secure)\n"
"#ssl_cipher_list = HIGH:!ADH:!3DES:!SHA\n\n"

"#show_target_user_in_f12_message = 0\n\n"

"#enable_new_pointer_update = 0\n\n"

"#bogus_ios_glyph_support_level = 1\n\n"

"#_advanced\n"
"#transform_glyph_to_bitmap = 0\n\n"

"#   0: disabled\n"
"#   1: pause key only\n"
"#   2: all input events\n"
"#bogus_number_of_fastpath_input_event = 1\n\n"

"# (is in millisecond)\n"
"#_advanced\n"
"#recv_timeout = 1000\n\n"

"[mod_rdp]\n\n"

"# Specifies the highest compression package support available on the front side\n"
"#   0: The RDP bulk compression is disabled\n"
"#   1: RDP 4.0 bulk compression\n"
"#   2: RDP 5.0 bulk compression\n"
"#   3: RDP 6.0 bulk compression\n"
"#   4: RDP 6.1 bulk compression\n"
"#_advanced\n"
"#rdp_compression = 4\n\n"

"#_advanced\n"
"#disconnect_on_logon_user_change = 0\n\n"

"# (is in second)\n"
"#_advanced\n"
"#open_session_timeout = 0\n\n"

"# Enables support of additional drawing orders:\n"
"#   15: MultiDstBlt\n"
"#   16: MultiPatBlt\n"
"#   17: MultiScrBlt\n"
"#   18: MultiOpaqueRect\n"
"#   22: Polyline\n"
"#_advanced\n"
"#extra_orders = 15,16,17,18,22\n\n"

"# NLA authentication in secondary target.\n"
"#enable_nla = 1\n\n"

"# If enabled, NLA authentication will try Kerberos before NTLM.\n"
"# (if enable_nla is disabled, this value is ignored).\n"
"#enable_kerberos = 0\n\n"

"# Persistent Disk Bitmap Cache on the mod side.\n"
"#_advanced\n"
"#persistent_disk_bitmap_cache = 1\n\n"

"# Support of Cache Waiting List (this value is ignored if Persistent Disk Bitmap Cache is disabled).\n"
"#_advanced\n"
"#cache_waiting_list = 1\n\n"

"# If enabled, the contents of Persistent Bitmap Caches are stored on disk.\n"
"#_advanced\n"
"#persist_bitmap_cache_on_disk = 0\n\n"

"# Enables channels names (example: channel1,channel2,etc). Character * only, activate all with low priority.\n"
"#_advanced\n"
"#allow_channels = *\n\n"

"# Disable channels names (example: channel1,channel2,etc). Character * only, deactivate all with low priority.\n"
"#_advanced\n"
"#deny_channels = \n\n"

"# Enables support of Client/Server Fast-Path Input/Update PDUs.\n"
"# Fast-Path is required for Windows Server 2012 (or more recent)!\n"
"#_advanced\n"
"#fast_path = 1\n\n"

"# Enables Server Redirection Support.\n"
"#server_redirection_support = 0\n\n"

"# Needed to connect with VirtualBox, based on bogus TS_UD_SC_NET data block.\n"
"#_advanced\n"
"#bogus_sc_net_size = 1\n\n"

"# Needed to get the old behavior of cursor rendering.\n"
"#   0: More compatible with MSTSC.\n"
"#   1: The old behavior of cursor rendering.\n"
"#   2: Depend on platform RDP client.\n"
"#_advanced\n"
"#bogus_linux_cursor = 2\n\n"

"#_advanced\n"
"#proxy_managed_drives = \n\n"

"#_hidden\n"
"#ignore_auth_channel = 0\n\n"

"# Authentication channel used by Auto IT scripts. May be '*' to use default name. Keep empty to disable virtual channel.\n"
"#auth_channel = *\n\n"

"# Authentication channel used by other scripts. No default name. Keep empty to disable virtual channel.\n"
"#checkout_channel = \n\n"

"#_hidden\n"
"#alternate_shell = \n\n"

"#_hidden\n"
"#shell_arguments = \n\n"

"#_hidden\n"
"#shell_working_directory = \n\n"

"#_advanced\n"
"#use_client_provided_alternate_shell = 0\n\n"

"#_advanced\n"
"#use_client_provided_remoteapp = 0\n\n"

"#_advanced\n"
"#use_native_remoteapp_capability = 1\n\n"

"#_hidden\n"
"#enable_session_probe = 0\n\n"

"# Minimum supported server : Windows Server 2008.\n"
"# Clipboard redirection should be remain enabled on Terminal Server.\n"
"#_hidden\n"
"#session_probe_use_smart_launcher = 1\n\n"

"#_hidden\n"
"#session_probe_enable_launch_mask = 1\n\n"

"# Behavior on failure to launch Session Probe.\n"
"#   0: ignore failure and continue.\n"
"#   1: disconnect user.\n"
"#   2: reconnect without Session Probe.\n"
"#_hidden\n"
"#session_probe_on_launch_failure = 2\n\n"

"# This parameter is used if session_probe_on_launch_failure is 1 (disconnect user).\n"
"# 0 to disable timeout.\n"
"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_launch_timeout = 20000\n\n"

"# This parameter is used if session_probe_on_launch_failure is 0 (ignore failure and continue) or 2 (reconnect without Session Probe).\n"
"# 0 to disable timeout.\n"
"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_launch_fallback_timeout = 7000\n\n"

"# Minimum supported server : Windows Server 2008.\n"
"#_hidden\n"
"#session_probe_start_launch_timeout_timer_only_after_logon = 1\n\n"

"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_keepalive_timeout = 5000\n\n"

"#   0: ignore and continue\n"
"#   1: disconnect user\n"
"#   2: freeze connection and wait\n"
"#_hidden\n"
"#session_probe_on_keepalive_timeout = 1\n\n"

"# End automatically a disconnected session\n"
"#_hidden\n"
"#session_probe_end_disconnected_session = 0\n\n"

"#_advanced\n"
"#session_probe_customize_executable_name = 0\n\n"

"#_hidden\n"
"#session_probe_enable_log = 0\n\n"

"#_hidden\n"
"#session_probe_enable_log_rotation = 1\n\n"

"# This policy setting allows you to configure a time limit for disconnected application sessions.\n"
"# 0 to disable timeout.\n"
"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_disconnected_application_limit = 0\n\n"

"# This policy setting allows you to configure a time limit for disconnected Terminal Services sessions.\n"
"# 0 to disable timeout.\n"
"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_disconnected_session_limit = 0\n\n"

"# This parameter allows you to specify the maximum amount of time that an active Terminal Services session can be idle (without user input) before it is automatically locked by Session Probe.\n"
"# 0 to disable timeout.\n"
"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_idle_session_limit = 0\n\n"

"#_hidden\n"
"#session_probe_exe_or_file = ||CMD\n\n"

"#_hidden\n"
"#session_probe_arguments = " REDEMPTION_CONFIG_SESSION_PROBE_ARGUMENTS "\n\n"

"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_clipboard_based_launcher_clipboard_initialization_delay = 2000\n\n"

"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_clipboard_based_launcher_start_delay = 2000\n\n"

"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_clipboard_based_launcher_long_delay = 500\n\n"

"# (is in millisecond)\n"
"#_hidden\n"
"#session_probe_clipboard_based_launcher_short_delay = 50\n\n"

"#_advanced\n"
"#session_probe_allow_multiple_handshake = 0\n\n"

"#_advanced\n"
"#session_probe_enable_crash_dump = 0\n\n"

"#_advanced\n"
"#session_probe_handle_usage_limit = 0\n\n"

"#_advanced\n"
"#session_probe_memory_usage_limit = 0\n\n"

"# Keep known server certificates on WAB\n"
"#_hidden\n"
"#server_cert_store = 1\n\n"

"# Behavior of certificates check.\n"
"#   0: fails if certificates doesn't match or miss.\n"
"#   1: fails if certificate doesn't match, succeed if no known certificate.\n"
"#   2: succeed if certificates exists (not checked), fails if missing.\n"
"#   3: always succeed.\n"
"# System errors like FS access rights issues or certificate decode are always check errors leading to connection rejection.\n"
"#_hidden\n"
"#server_cert_check = 1\n\n"

"# Warn if check allow connexion to server.\n"
"#   0: nobody\n"
"#   1: message sent to syslog\n"
"#   2: User notified (through proxy interface)\n"
"#   4: admin notified (wab notification)\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#_hidden\n"
"#server_access_allowed_message = 1\n\n"

"# Warn that new server certificate file was created.\n"
"#   0: nobody\n"
"#   1: message sent to syslog\n"
"#   2: User notified (through proxy interface)\n"
"#   4: admin notified (wab notification)\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#_hidden\n"
"#server_cert_create_message = 1\n\n"

"# Warn that server certificate file was successfully checked.\n"
"#   0: nobody\n"
"#   1: message sent to syslog\n"
"#   2: User notified (through proxy interface)\n"
"#   4: admin notified (wab notification)\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#_hidden\n"
"#server_cert_success_message = 1\n\n"

"# Warn that server certificate file checking failed.\n"
"#   0: nobody\n"
"#   1: message sent to syslog\n"
"#   2: User notified (through proxy interface)\n"
"#   4: admin notified (wab notification)\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#_hidden\n"
"#server_cert_failure_message = 1\n\n"

"# Warn that server certificate check raised some internal error.\n"
"#   0: nobody\n"
"#   1: message sent to syslog\n"
"#   2: User notified (through proxy interface)\n"
"#   4: admin notified (wab notification)\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#_hidden\n"
"#server_cert_error_message = 1\n\n"

"# Do not transmit client machine name or RDP server.\n"
"#hide_client_name = 0\n\n"

"#_advanced\n"
"#clean_up_32_bpp_cursor = 0\n\n"

"#bogus_ios_rdpdr_virtual_channel = 1\n\n"

"#_advanced\n"
"#enable_rdpdr_data_analysis = 1\n\n"

"# Delay before automatically bypass Windows's Legal Notice screen in RemoteApp mode.\n"
"# Set to 0 to disable this feature.\n"
"# (is in millisecond)\n"
"#_advanced\n"
"#remoteapp_bypass_legal_notice_delay = 0\n\n"

"# Time limit to automatically bypass Windows's Legal Notice screen in RemoteApp mode.\n"
"# Set to 0 to disable this feature.\n"
"# (is in millisecond)\n"
"#_advanced\n"
"#remoteapp_bypass_legal_notice_timeout = 20000\n\n"

"#_advanced\n"
"#experimental_fix_input_event_sync = 1\n\n"

"[metrics]\n\n"

"#_advanced\n"
"#enable_rdp_metrics = 0\n\n"

"#_advanced\n"
"#enable_vnc_metrics = 0\n\n"

"#_hidden\n"
"#log_dir_path = " << app_path(AppPath::Metrics) << "\n\n"

"# (is in second)\n"
"#_advanced\n"
"#log_interval = 5\n\n"

"# (is in hour)\n"
"#_advanced\n"
"#log_file_turnover_interval = 24\n\n"

"# signature key to digest log metrics header info\n"
"#_advanced\n"
"#sign_key = \n\n"

"[mod_vnc]\n\n"

"# Enable or disable the clipboard from client (client to server).\n"
"#clipboard_up = 0\n\n"

"# Enable or disable the clipboard from server (server to client).\n"
"#clipboard_down = 0\n\n"

"# Sets the encoding types in which pixel data can be sent by the VNC server:\n"
"#   0: Raw\n"
"#   1: CopyRect\n"
"#   2: RRE\n"
"#   16: ZRLE\n"
"#   -239 (0xFFFFFF11): Cursor pseudo-encoding\n"
"#_advanced\n"
"#encodings = \n\n"

"#_advanced\n"
"#allow_authentification_retries = 0\n\n"

"# VNC server clipboard data encoding type.\n"
"#_advanced\n"
"#server_clipboard_encoding_type = latin1\n\n"

"#   0: delayed\n"
"#   1: duplicated\n"
"#   2: continued\n"
"#_advanced\n"
"#bogus_clipboard_infinite_loop = 0\n\n"

"#_hidden\n"
"#server_is_apple = 0\n\n"

"[mod_replay]\n\n"

"# 0 - Wait for Escape, 1 - End session\n"
"#_hidden\n"
"#on_end_of_data = 0\n\n"

"# 0 - replay once, 1 - loop replay\n"
"#_hidden\n"
"#replay_on_loop = 0\n\n"

"[ocr]\n\n"

"#   1: v1\n"
"#   2: v2\n"
"#version = 2\n\n"

"#locale = latin\n\n"

"# (is in 1/100 second)\n"
"#_advanced\n"
"#interval = 100\n\n"

"#_advanced\n"
"#on_title_bar_only = 1\n\n"

"# Expressed in percentage,\n"
"#   0   - all of characters need be recognized\n"
"#   100 - accept all results\n"
"#_advanced\n"
"#max_unrecog_char_rate = 40\n\n"

"[video]\n\n"

"#_advanced\n"
"#capture_groupid = 33\n\n"

"# Specifies the type of data to be captured:\n"
"#   0: none\n"
"#   1: png\n"
"#   2: wrm\n"
"#   4: video\n"
"#   8: ocr\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#_advanced\n"
"#capture_flags = 11\n\n"

"# Frame interval.\n"
"# (is in 1/10 second)\n"
"#_advanced\n"
"#png_interval = 10\n\n"

"# Frame interval.\n"
"# (is in 1/100 second)\n"
"#_advanced\n"
"#frame_interval = 40\n\n"

"# Time between 2 wrm movies.\n"
"# (is in second)\n"
"#_advanced\n"
"#break_interval = 600\n\n"

"# Number of png captures to keep.\n"
"#_advanced\n"
"#png_limit = 5\n\n"

"#_advanced\n"
"#replay_path = /tmp/\n\n"

"#_advanced\n"
"#hash_path = " << app_path(AppPath::Hash) << "\n\n"

"#_advanced\n"
"#record_tmp_path = " << app_path(AppPath::RecordTmp) << "\n\n"

"#_advanced\n"
"#record_path = " << app_path(AppPath::Record) << "\n\n"

"# Disable keyboard log:\n"
"#   0: none\n"
"#   1: disable keyboard log in syslog\n"
"#   2: disable keyboard log in recorded sessions\n"
"#   4: disable keyboard log in recorded meta\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#disable_keyboard_log = 1\n\n"

"# Disable clipboard log:\n"
"#   0: none\n"
"#   1: disable clipboard log in syslog\n"
"#   2: disable clipboard log in recorded sessions\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#disable_clipboard_log = 1\n\n"

"# Disable (redirected) file system log:\n"
"#   0: none\n"
"#   1: disable (redirected) file system log in syslog\n"
"#   2: disable (redirected) file system log in recorded sessions\n"
"# (note: values can be added (everyone: 1+2+4=7, mute: 0))\n"
"#disable_file_system_log = 1\n\n"

"#_hidden\n"
"#rt_display = 0\n\n"

"# The method by which the proxy RDP establishes criteria on which to chosse a color depth for native video capture:\n"
"#   0: 24-bit\n"
"#   1: 16-bit\n"
"#_advanced\n"
"#wrm_color_depth_selection_strategy = 1\n\n"

"# The compression method of native video capture:\n"
"#   0: no compression\n"
"#   1: gzip\n"
"#   2: snappy\n"
"#_advanced\n"
"#wrm_compression_algorithm = 1\n\n"

"# Needed to play a video with ffplay or VLC.\n"
"# Note: Useless with mpv and mplayer.\n"
"#_advanced\n"
"#bogus_vlc_frame_rate = 1\n\n"

"# Bitrate for low quality.\n"
"#_advanced\n"
"#l_bitrate = 10000\n\n"

"# Framerate for low quality.\n"
"#_advanced\n"
"#l_framerate = 5\n\n"

"# Height for low quality.\n"
"#_advanced\n"
"#l_height = 480\n\n"

"# Width for low quality.\n"
"#_advanced\n"
"#l_width = 640\n\n"

"# Qscale (parameter given to ffmpeg) for low quality.\n"
"#_advanced\n"
"#l_qscale = 28\n\n"

"# Bitrate for medium quality.\n"
"#_advanced\n"
"#m_bitrate = 20000\n\n"

"# Framerate for medium quality.\n"
"#_advanced\n"
"#m_framerate = 5\n\n"

"# Height for medium quality.\n"
"#_advanced\n"
"#m_height = 768\n\n"

"# Width for medium quality.\n"
"#_advanced\n"
"#m_width = 1024\n\n"

"# Qscale (parameter given to ffmpeg) for medium quality.\n"
"#_advanced\n"
"#m_qscale = 14\n\n"

"# Bitrate for high quality.\n"
"#_advanced\n"
"#h_bitrate = 30000\n\n"

"# Framerate for high quality.\n"
"#_advanced\n"
"#h_framerate = 5\n\n"

"# Height for high quality.\n"
"#_advanced\n"
"#h_height = 2048\n\n"

"# Width for high quality.\n"
"#_advanced\n"
"#h_width = 2048\n\n"

"# Qscale (parameter given to ffmpeg) for high quality.\n"
"#_advanced\n"
"#h_qscale = 7\n\n"

"[crypto]\n\n"

"#_hidden\n"
"#encryption_key = 000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F\n\n"

"#_hidden\n"
"#sign_key = 000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F\n\n"

"[debug]\n\n"

"#_advanced\n"
"#_hex\n"
"#x224 = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#mcs = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#sec = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#rdp = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#primary_orders = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#secondary_orders = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#bitmap_update = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#bitmap = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#capture = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#auth = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#session = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#front = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#mod_rdp = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#mod_vnc = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#mod_internal = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#mod_xup = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#widget = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#input = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#password = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#compression = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#cache = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#performance = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#pass_dialog_box = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#ocr = 0\n\n"

"#_advanced\n"
"#_hex\n"
"#ffmpeg = 0\n\n"

"#_advanced\n"
"#config = 1\n\n"

"[remote_program]\n\n"

"#allow_resize_hosted_desktop = 1\n\n"

"[translation]\n\n"

"#_advanced\n"
"#language = en\n\n"

"#_advanced\n"
"#password_en = \n\n"

"#_advanced\n"
"#password_fr = \n\n"

"[internal_mod]\n\n"

"#_advanced\n"
"#load_theme = \n\n"

#undef CONFIG_PP_STRINGIZE_I
#undef CONFIG_PP_STRINGIZE
