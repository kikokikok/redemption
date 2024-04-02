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
  Author(s): Wallix Team
*/

#include "utils/local_err_msg.hpp"
#include "utils/translation.hpp"
#include "core/error.hpp"

zstring_view local_err_msg(Error const& error, Language lang, bool with_id) noexcept
{
    trkeys::TrKey const* trkey;

    REDEMPTION_DIAGNOSTIC_PUSH()
    REDEMPTION_DIAGNOSTIC_GCC_IGNORE("-Wswitch-enum")
    switch (error.id) {
    case ERR_SESSION_UNKNOWN_BACKEND:
        trkey = &trkeys::err_session_unknown_backend;
        break;

    case ERR_NLA_AUTHENTICATION_FAILED:
        trkey = &trkeys::err_nla_authentication_failed;
        break;

    case ERR_TRANSPORT_TLS_CERTIFICATE_CHANGED:
        trkey = &trkeys::err_transport_tls_certificate_changed;
        break;

    case ERR_TRANSPORT_TLS_CERTIFICATE_MISSED:
        trkey = &trkeys::err_transport_tls_certificate_missed;
        break;

    case ERR_TRANSPORT_TLS_CERTIFICATE_CORRUPTED:
        trkey = &trkeys::err_transport_tls_certificate_corrupted;
        break;

    case ERR_TRANSPORT_TLS_CERTIFICATE_INACCESSIBLE:
        trkey = &trkeys::err_transport_tls_certificate_inaccessible;
        break;

    case ERR_VNC_CONNECTION_ERROR:
        trkey = &trkeys::err_vnc_connection_error;
        break;

    case ERR_RDP_UNSUPPORTED_MONITOR_LAYOUT:
        trkey = &trkeys::err_rdp_unsupported_monitor_layout;
        break;

    case ERR_RDP_NEGOTIATION:
        trkey = &trkeys::err_rdp_negotiation;
        break;

    case ERR_LIC:
        trkey = &trkeys::err_lic;
        break;

    case ERR_RAIL_CLIENT_EXECUTE:
        trkey = &trkeys::err_rail_client_execute;
        break;

    case ERR_RAIL_STARTING_PROGRAM:
        trkey = &trkeys::err_rail_starting_program;
        break;

    case ERR_RAIL_UNAUTHORIZED_PROGRAM:
        trkey = &trkeys::err_rail_unauthorized_program;
        break;

    case ERR_RDP_OPEN_SESSION_TIMEOUT:
        trkey = &trkeys::err_rdp_open_session_timeout;
        break;

    case ERR_RDP_SERVER_REDIR:
        trkey = &trkeys::err_rdp_server_redir;
        break;

    case ERR_SESSION_PROBE_LAUNCH:
        trkey = &trkeys::err_session_probe_launch;
        break;

    case ERR_SESSION_PROBE_ASBL_FSVC_UNAVAILABLE:
        trkey = &trkeys::err_session_probe_asbl_fsvc_unavailable;
        break;

    case ERR_SESSION_PROBE_ASBL_MAYBE_SOMETHING_BLOCKS:
        trkey = &trkeys::err_session_probe_asbl_maybe_something_blocks;
        break;

    case ERR_SESSION_PROBE_ASBL_UNKNOWN_REASON:
        trkey = &trkeys::err_session_probe_asbl_unknown_reason;
        break;

    case ERR_SESSION_PROBE_CBBL_FSVC_UNAVAILABLE:
        trkey = &trkeys::err_session_probe_cbbl_fsvc_unavailable;
        break;

    case ERR_SESSION_PROBE_CBBL_CBVC_UNAVAILABLE:
        trkey = &trkeys::err_session_probe_cbbl_cbvc_unavailable;
        break;

    case ERR_SESSION_PROBE_CBBL_DRIVE_NOT_READY_YET:
        trkey = &trkeys::err_session_probe_cbbl_drive_not_ready_yet;
        break;

    case ERR_SESSION_PROBE_CBBL_MAYBE_SOMETHING_BLOCKS:
        trkey = &trkeys::err_session_probe_cbbl_maybe_something_blocks;
        break;

    case ERR_SESSION_PROBE_CBBL_LAUNCH_CYCLE_INTERRUPTED:
        trkey = &trkeys::err_session_probe_cbbl_launch_cycle_interrupted;
        break;

    case ERR_SESSION_PROBE_CBBL_UNKNOWN_REASON_REFER_TO_SYSLOG:
        trkey = &trkeys::err_session_probe_cbbl_unknown_reason_refer_to_syslog;
        break;

    case ERR_SESSION_PROBE_RP_LAUNCH_REFER_TO_SYSLOG:
        trkey = &trkeys::err_session_probe_rp_launch_refer_to_syslog;
        break;

    case ERR_SESSION_CLOSE_ENDDATE_REACHED:
        trkey = &trkeys::session_out_time;
        break;

    case ERR_MCS_APPID_IS_MCS_DPUM:
        trkey = &trkeys::end_connection;
        break;

    case ERR_SESSION_CLOSE_ACL_KEEPALIVE_MISSED:
        trkey = &trkeys::miss_keepalive;
        break;

    case ERR_SESSION_CLOSE_USER_INACTIVITY:
        trkey = &trkeys::close_inactivity;
        break;

    default:
        return error.errmsg(with_id);
    }
    REDEMPTION_DIAGNOSTIC_POP()

    return TR(*trkey, lang);
}
