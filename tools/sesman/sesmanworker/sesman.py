#!/usr/bin/python -O
# -*- coding: utf-8 -*-
##
# Copyright (c) 2017 WALLIX. All rights reserved.
# Licensed computer software. Property of WALLIX.
# Product Name: WALLIX Bastion v6.0
# Author(s): Olivier Hervieu, Christophe Grosjean, Raphael Zhou, Meng Tan
# Id: $Id$
# URL: $URL$
# Module description:  Sesman Worker
##
from __future__ import with_statement
from __future__ import absolute_import

import random
import os
import signal
import traceback
import json
import re
import sys
from logger import Logger

from struct import unpack_from, pack
from select import select
from time import time
from time import ctime
from time import mktime
from datetime import datetime
import socket
from socket import gethostname

from .sesmanconf import TR, SESMANCONF
from . import engine
from .sesmanconnpolicyspec import cp_spec
from .sesmanbacktoselector import (
    back_to_selector_default_reinit,
    back_to_selector_default_sent
)

from .engine import LOCAL_TRACE_PATH_RDP
from .engine import APPROVAL_ACCEPTED, APPROVAL_REJECTED, \
    APPROVAL_PENDING, APPROVAL_NONE
from .engine import APPREQ_REQUIRED, APPREQ_OPTIONAL
from .engine import PASSWORD_VAULT, PASSWORD_INTERACTIVE, PASSWORD_MAPPING
from .engine import TargetContext

import syslog

from .logtime import logtimer
from .logtime import logtime_function_pause

# Python 2.7 compatibility layer
if sys.version_info[0] < 3:
    def bytes(data, encoding="utf-8"):
        return data.encode(encoding)

    text_type = unicode
    binary_type = str

    def to_syslog(data):
        return to_utf8(data)

    def bin2hex(bstr):
        return bstr.encode('hex')
else:
    text_type = str
    binary_type = (bytes, bytearray)
    unicode = None

    def to_syslog(data):
        return to_unicode(data)

    def bin2hex(bstr):
        return bstr.hex()


def to_utf8(string):
    if isinstance(string, text_type):
        return bytes(string, "utf-8")
    return string


def to_unicode(string):
    if isinstance(string, binary_type):
        return string.decode("utf-8")
    return string


def print_exception_caught(func):
    def method_wrapper(self, *args, **kwargs):
        try:
            return func(self, *args, **kwargs)
        except Exception:
            Logger().info(traceback.format_exc())
            raise
    return method_wrapper


class RdpProxyLog(object):
    def __init__(self):
        syslog.openlog('rdpproxy')
        self._context = '[rdpproxy] '

    def update_context(self, psid, user):
        self._context = '[rdpproxy] psid="%s" user="%s"' % (psid, user)

    def log(self, type, **kwargs):
        target = kwargs.pop('target', None)
        arg_list = list(kwargs.items())
        if target:
            arg_list[:0] = [('target', target)]
        arg_list[:0] = [('type', type)]
        args = ' '.join(('%s="%s"' % (k, self.escape_bs_dq(v)))
                        for (k, v) in arg_list if v)
        line = to_utf8(' '.join((self._context, args)))
        syslog.syslog(syslog.LOG_INFO, to_syslog(line))

    @staticmethod
    def escape_bs_dq(string):
        if type(string) in (str, unicode):
            return string.replace('\\', '\\\\').replace('"', '\\"')
        return string


MAGICASK = (u'UNLIKELYVALUEMAGICASPICONSTANTS'
            u'3141592926ISUSEDTONOTIFYTHEVALUEMUSTBEASKED')

KEEPALIVE_INTERVAL = 30
KEEPALIVE_GRACEDELAY = 30
KEEPALIVE_TIMEOUT = KEEPALIVE_INTERVAL + KEEPALIVE_GRACEDELAY

WORKFLOW_POLL_INTERVAL = 5


def mundane(value):
    if value == MAGICASK:
        return u'Unknown'
    return value


def rvalue(value):
    if value == MAGICASK:
        return u''
    return value


DEBUG = False


def truncat_string(item, maxsize=20):
    return (item[:maxsize] + '..') if len(item) > maxsize else item


def parse_auth(username):
    """
    Extract actual username and target if provided
    from authentication identity

    string format is <secondaryuser>@<target>:<service>:<group>:<primaryuser>
    always return primaryuser and either secondary target or None

    Note: primary user can be a path instead when this function
    is called to parse scp or sftp arguments.

    Because of compatibility issues with some ssh command line tools
    '+' can be used instead of ':'

    fields can be missing (typically service and group if there is
    no ambiguity)

    """
    user_dev_service_group, sep, primary = username.rpartition(':')
    if not sep:
        user_dev_service_group, sep, primary = username.rpartition('+')
    if sep:
        user_dev_service, sep, group = user_dev_service_group.rpartition(sep)
        if not sep:
            # service and group not provided
            user_at_dev, service, group = user_dev_service_group, '', ''
        else:
            user_at_dev, sep, service = user_dev_service.rpartition(sep)
            if not sep:
                # group not provided
                user_at_dev, service, group = user_dev_service, group, ''
        user, sep, dev = user_at_dev.rpartition('@')
        if sep:
            return primary, (user, dev, service, group)
    return username, None


# PM Function
def parse_param(param, current_device=None):
    """
    Extract account representation

    string format is <account_name>@<domain_name>[@<device_name>]

    If @<device_name> is absent, <domain_name> is a global domain name
    Else <domain_name> is a local domain name of the current_device

    if 'current_device' is not None
    <device_name> should be empty or equal to current_device
    Else <device_name> can be another device

    """
    parsed = param.rsplit("@", 2)
    if len(parsed) > 1:
        account_name = parsed[0]
        domain_name = parsed[1]
        if (len(parsed) == 3
            and parsed[2]
            and current_device
            and (parsed[2] != current_device)):
            return None
        device_name = (current_device or parsed[2]) if len(parsed) == 3 \
            else None
        return account_name, domain_name, device_name
    else:
        return None


# PM Function
def pm_request(engine, request):
    Logger().debug("pm_request: '%s'" % request)
    if request and request[0] == '/':
        request = request[1:]
    reql = request.split('/', 2)
    if len(reql) < 3:
        Logger().debug("pm_request: invalid request")
        return {
            'response_code': 202,
            'response_message': "Invalid request format",
        }
    req_name, req_cmd, req_param = reql
    if req_name != u'targetpasswords':
        Logger().debug("pm_request: invalid request name")
        return {
            'response_code': 203,
            'response_message': "Invalid request name",
        }
    res_param = parse_param(req_param)
    if res_param is None:
        Logger().debug("pm_request: invalid params")
        return {
            'response_code': 204,
            'response_message': "Invalid request parameters",
        }
    acc_n, domain_n, dev_n = res_param
    if req_cmd == u'checkout':
        res = engine.get_account_infos_by_type(
            account_name=acc_n,
            domain_name=domain_n,
            device_name=dev_n,
            account_type='pm'
        )
        if res is not None:
            res['response_code'] = 0
        else:
            res = {
                'response_code': 301,
                'response_message': "Account not found",
            }
        return res
    if req_cmd == u'checkin':
        engine.release_account_by_type(
            account_name=acc_n,
            domain_name=domain_n,
            device_name=dev_n,
            account_type='pm'
        )
        return {
            'response_code': 0,
        }
    return {
        'response_code': 205,
        'response_message': "Unknown request command",
    }


class AuthentifierSocketClosed(Exception):
    pass


class BastionSignal(Exception):
    def __init__(self, message="BastionSignal"):
        super(BastionSignal, self).__init__(message)


class RTManager(object):
    __slots__ = ("sesman", "time_limit", "last_start")

    def __init__(self, sesman, time_limit):
        self.sesman = sesman
        self.time_limit = time_limit
        self.last_start = 0

    def reset(self):
        self.last_start = 0

    def start(self, current_time):
        Logger().debug("Start RT Manager at %s" % current_time)
        self.last_start = current_time
        self.send_rtdisplay(1)

    def stop(self):
        self.last_start = 0
        self.send_rtdisplay(0)

    def check(self, current_time):
        if (self.last_start
            and (current_time > self.last_start + self.time_limit)):
            Logger().debug("Check RT Manager at %s STOP" % current_time)
            # stop rt_display
            self.stop()
            return False
        return True

    def send_rtdisplay(self, rt_display):
        if self.sesman.shared.get("rt_display") != rt_display:
            Logger().debug("sending rt_display=%s" % rt_display)
            self.sesman.send_data({"rt_display": rt_display})


class Sesman():

    def __init__(self, conn, addr):
        """
        =======================================================================
        __INIT__
        =======================================================================
        """
        try:
            confwab = engine.read_config_file(
                modulename='sesman',
                confdir='/var/wab/etc/sesman/config'
            )
            seswabconfig = confwab.get(u'sesman', {})
            SESMANCONF.conf[u'sesman'].update(seswabconfig)
            # Logger().info(" WABCONFIG SESMANCONF = '%s'" % seswabconfig)
        except Exception:
            Logger().info("Failed to load Sesman WabConfig")
        # Logger().info(" SESMANCONF = '%s'" % SESMANCONF[u'sesman'])
        if SESMANCONF[u'sesman'].get(u'debug', False):
            global DEBUG
            DEBUG = True

        self.rdplog = RdpProxyLog()

        self.cn = u'Unknown'

        self.proxy_conx = conn
        self.addr = addr
        self.full_path = None  # path + basename (without extension)
        self.record_filebase = None
        self.full_log_path = None

        legacy_auth = SESMANCONF[u'sesman'].get(
            u'legacy_wabengine_auth', False
        )
        self.engine = engine.Engine(legacy_auth)

        self.effective_login = None

        # shared should be read from sesman but never written
        # except when sending
        self.shared = {}
        self._changed_keys = []

        self._full_user_device_account = u'Unknown'
        self.target_service_name = None
        self.target_group = None
        self.target_context = TargetContext()

        self.shared[u'module'] = u'login'
        self.shared[u'selector_group_filter'] = u''
        self.shared[u'selector_device_filter'] = u''
        self.shared[u'selector_proto_filter'] = u''
        self.shared[u'selector'] = u'False'
        self.shared[u'selector_current_page'] = u'1'
        self.shared[u'selector_lines_per_page'] = u'0'
        self.shared[u'real_target_device'] = MAGICASK
        self.shared[u'reporting'] = u''

        self._trace_type = self.engine.get_trace_type()
        self._selector_banner = self.engine.get_selector_banner()
        self.language = None
        self.pid = os.getpid()

        self.shared[u'target_login'] = MAGICASK
        self.shared[u'target_device'] = MAGICASK
        self.shared[u'target_host'] = MAGICASK
        self.shared[u'login'] = MAGICASK
        self.shared[u'ip_client'] = MAGICASK
        self.shared[u'target_protocol'] = MAGICASK
        self.shared[u'keyboard_layout'] = MAGICASK

        self.shared[u'auth_channel_answer'] = u''
        self.shared[u'auth_channel_target'] = u''

        self.shared[u'recording_started'] = u'False'

        self.shared[u'auth_notify'] = u''

        self.shared[u'login_language'] = MAGICASK

        self.internal_target = False
        self.check_session_parameters = False

        # Passthrough context
        self.passthrough_mode = SESMANCONF[u'sesman'].get(
            'auth_mode_passthrough', False
        )
        self.default_login = (
            SESMANCONF[u'sesman'].get('default_login', '').strip()
            or None
        )
        self.passthrough_target_login = None
        self.allow_back_selector = SESMANCONF[u'sesman'].get(
            'allow_back_to_selector', True
        )
        self.back_selector = False
        self.hide_approval_back_selector = False
        self.target_app_rights = {}

        self.login_message = (u"Warning! Unauthorized access to this system "
                              u"is forbidden and will be prosecuted by law.")

        self.shared[u'session_probe_launch_error_message'] = u''

        self.rtmanager = RTManager(
            self,
            self.engine.wabengine_conf.get("session_4eyes_timer", 60)
        )

    def reset_session_var(self):
        """
        Proxy Session Vars reset
        """
        self._full_user_device_account = u'Unknown'
        self.target_service_name = None
        self.target_group = None
        self.internal_target = False
        self.passthrough_target_login = None
        self.target_context = TargetContext()

    def reset_target_session_vars(self):
        """
        Target Session Disconnection
        """
        self._full_user_device_account = u'Unknown'
        self.target_service_name = None
        self.target_group = None
        self.internal_target = False
        self.target_app_rights = {}
        # Should set context values back to default
        for key, value in back_to_selector_default_reinit.items():
            self.shared[key] = value
        back_to_selector_default_sent[u"module"] = u'transitory'
        self.send_data(back_to_selector_default_sent)
        self.engine.reset_proxy_rights()
        self.rtmanager.reset()

    def load_login_message(self, language):
        try:
            self.login_message = self.engine.get_warning_message(language)
        except Exception:
            pass

    def set_language_from_keylayout(self):
        self.language = SESMANCONF.language
        french_layouts = [0x0000040C,  # French (France)
                          0x00000C0C,  # French (Canada) Canadian French
                                       #     (Legacy)
                          0x0000080C,  # French (Belgium)
                          0x0001080C,  # French (Belgium) Belgian (Comma)
                          0x0000100C]  # French (Switzerland)
        keylayout = 0
        if self.shared.get(u'keyboard_layout') != MAGICASK:
            try:
                keylayout = int(self.shared.get(u'keyboard_layout'))
            except Exception:
                pass
        if keylayout in french_layouts:
            self.language = 'fr'

        login_language = (self.shared.get(u'login_language').lower()
                          if (self.shared.get(u'login_language') != MAGICASK
                              and self.shared.get(u'login_language') != 'Auto')
                          else self.language)

        self.load_login_message(login_language)

    # TODO: is may be possible to delay sending data until the next
    #       input through receive_data
    def send_data(self, data):
        """ NB : Strings sent to the ReDemPtion proxy MUST be UTF-8 encoded
        * Packet format:
        uint16                   request_count
        request[request_count]   requests

        * request format:
        request := request_update | request_ask

        * request_update format:
        uint8                    "!"
        uint8                    key_size
        byte[key_size]           key
        uint32                   value_size
        byte[value_size]         value

        * request_ask format:
        uint8                    "?"
        uint8                    key_size
        byte[key_size]           key

        key and value are utf8 encoded
        uint are big-endian byte ordered
        """

        if DEBUG:
            Logger().info(u'=> send_data (update) = %s' % data.keys())

        # if current language changed, send translations
        if self.language != SESMANCONF.language:
            if not self.language:
                self.set_language_from_keylayout()
            SESMANCONF.language = self.language

            data[u'language'] = SESMANCONF.language
            # if self.shared.get(u'password') == MAGICASK:
            #     data[u'password'] = u''

        self.shared.update(data)

        def _toval(key, value):
            key = to_utf8(key)
            value = to_utf8(str(value))
            key_len = len(key)
            value_len = len(value)
            return (
                True, key,
                pack(f'>1sB{key_len}sL{value_len}s',
                     b'!', key_len, key, value_len, value)
            )

        def _toask(key):
            key = to_utf8(key)
            key_len = len(key)
            return (
                False, key,
                pack(f'>1sB{key_len}s', b'?', key_len, key)
            )

        _list = [(_toval(key, value) if value != MAGICASK else _toask(key))
                 for key, value in data.items()]
        _list.sort()

        if DEBUG:
            Logger().info(u'send_data (on the wire) length = %s' % len(_list))

        _r_data = b''.join(t[2] for t in _list)
        self.proxy_conx.sendall(pack('>H', len(_list)))
        self.proxy_conx.sendall(_r_data)

    def wait_read_proxy_conx(self):
        self.proxy_conx.setblocking(False)
        r = []
        while True:
            r, w, x = select([self.proxy_conx], [], [], KEEPALIVE_INTERVAL)
            self.engine.keepalive(timeout=KEEPALIVE_TIMEOUT)
            if self.proxy_conx in r:
                break
        self.proxy_conx.setblocking(True)
        return r

    @logtime_function_pause
    def receive_data(self, expected_list=None, blocking_call=True):
        """ NB : Strings coming from the ReDemPtion proxy are UTF-8 encoded
        * Packet format:
        uint16                   request_count
        request[request_count]   requests

        * request format:
        request := request_update | request_ask

        * request_update format:
        uint8                    "!"
        uint8                    key_size
        byte[key_size]           key
        uint32                   value_size
        byte[value_size]         value

        * request_ask format:
        uint8                    "?"
        uint8                    key_size
        byte[key_size]           key

        key and value are utf8 encoded
        uint are big-endian byte ordered
        """

        if blocking_call:
            _ = self.wait_read_proxy_conx()

        self._changed_keys = []

        def read_sck():
            try:
                d = self.proxy_conx.recv(65536)
                if len(d):
                    if DEBUG:
                        Logger().debug(d)
                    return d

            except Exception:
                # Logger().info("%s <<<%s>>>" % (
                #     u"Failed to read data from rdpproxy authentifier socket",
                #     traceback.format_exc(e))
                # )
                raise AuthentifierSocketClosed()

            if DEBUG:
                Logger().info("received_buffer (empty packet)")
            raise AuthentifierSocketClosed()

        class Buffer:
            def __init__(self):
                self._data = read_sck()
                self._offset = 0

            def reserve_data(self, n):
                while len(self._data) - self._offset < n:
                    if DEBUG:
                        Logger().info("received_buffer (big packet) "
                                      "old = %d / %d ; required = %d"
                                      % (self._offset, len(self._data), n))
                    self._data = self._data[self._offset:] + read_sck()
                    self._offset = 0

            def extract_string(self, n):
                self.reserve_data(n)
                _name = self._data[self._offset:self._offset + n]
                _name = _name.decode('utf8')
                self._offset += n
                return _name

            def unpack(self, fmt, n):
                self.reserve_data(n)
                r = unpack_from(fmt, self._data, self._offset)
                self._offset += n
                return r

            def is_empty(self):
                return len(self._data) == self._offset

        _buffer = Buffer()
        _data = {}

        while True:
            _nfield, = _buffer.unpack('>H', 2)

            if DEBUG:
                Logger().info("received_buffer (nfield) = %d" % (_nfield,))

            for _ in range(0, _nfield):
                _type, _n = _buffer.unpack("BB", 2)
                _key = _buffer.extract_string(_n)

                if DEBUG:
                    Logger().info("received_buffer (key)   = %s%s"
                                  % ('?' if _type == 0x3f else '!', _key,))

                if _type == 0x3f:  # b'?'
                    _data[_key] = MAGICASK
                else:
                    _n, = _buffer.unpack('>L', 4)
                    _data[_key] = _buffer.extract_string(_n)

                    if DEBUG:
                        if "password" in _key.lower():
                            Logger().info("received_buffer (value) = *****")
                        else:
                            Logger().info(
                                f"received_buffer (value) = {_data[_key]}"
                            )

            if _buffer.is_empty():
                break

            if DEBUG:
                Logger().info("received_buffer (several packet)")

        if not (expected_list and isinstance(expected_list, list)):
            expected_list = []
        for key in expected_list:
            if (key in _data and _data[key] != self.shared.get(key)):
                self._changed_keys.append(key)
        self.shared.update(_data)

        return True, u''

    def parse_username(self, wab_login, target_login, target_device,
                       target_service, target_group):
        effective_login = None
        wab_login, target_tuple = parse_auth(wab_login)
        if target_tuple is not None:
            (target_login, target_device,
             target_service, target_group) = target_tuple
        if self.passthrough_mode:
            Logger().info(u'Passthrough mode is enabled')
            if self.passthrough_target_login is None:
                self.passthrough_target_login = wab_login
            if not self.passthrough_target_login:
                Logger().info(
                    u"Passthrough mode is enabled, "
                    u"but login information is not provided"
                )
            if self.default_login:
                effective_login = self.passthrough_target_login
                wab_login = self.default_login
            Logger().info(u'ip_target="%s" real_target_device="%s"' % (
                self.shared.get(u'ip_target'), self.shared.get(
                    u'real_target_device')))
        return (True, "", wab_login, target_login, target_device,
                target_service, target_group, effective_login)

    def interactive_ask_x509_connection(self):
        """
        Send a message to the proxy to prompt the user to validate x509
        in his browser.
        Wait until the user clicks Ok in Proxy prompt or until timeout
        """
        _status = False
        data_to_send = ({
            u'message': TR(u'valid_authorisation'),
            u'password': u'x509',
            u'module': u'confirm',
            u'display_message': MAGICASK,
        })

        self.send_data(data_to_send)

        # Wait for the user to click Ok in proxy

        while self.shared.get(u'display_message') == MAGICASK:
            Logger().info(u'wait user grant or reject connection')
            _status, _error = self.receive_data()
            if not _status:
                break

            Logger().info(u'Data received')
            if self.shared.get(u'display_message').lower() != u'true':
                _status = False

        return _status

    def interactive_display_message(self, data_to_send):
        """ NB : Strings sent to the ReDemPtion proxy MUST be UTF-8 encoded """
        # TODO: we should not have to care about target login or device
        #       to display messages we should be able to send messages before
        #       or after defining target seamlessly
        data_to_send.update({u'module': u'confirm'})

        self.send_data(data_to_send)
        _status, _error = self.receive_data()

        if self.shared.get(u'display_message') != u'True':
            _status, _error = False, TR(u'not_display_message')

        return _status, _error

    def interactive_accept_message(self, data_to_send):
        data_to_send.update({u'module': u'valid'})
        self.send_data(data_to_send)

        _status, _error = self.receive_data()
        if self.shared.get(u'accept_message') != u'True':
            _status, _error = False, TR(u'not_accept_message')

        return _status, _error

    def check_deconnection_time(self, selected_target):
        Logger().info(u"Checking timeframe")
        _status, _error = True, ""
        timeclose = None
        infinite_connection = False
        deconnection_time = self.engine.get_deconnection_time(selected_target)
        if not deconnection_time:
            Logger().error("No timeframe available, "
                           "Timeframe has not been checked !")
            _status = False
        if (deconnection_time == u"-"
            or deconnection_time[0:4] >= u"2034"):
            deconnection_time = u"2034-12-31 23:59:59"
            infinite_connection = True

        now = datetime.strftime(datetime.now(), "%Y-%m-%d %H:%M:%S")
        if _status and not infinite_connection and now < deconnection_time:
            # deconnection time to epoch
            tt = datetime.strptime(
                deconnection_time, "%Y-%m-%d %H:%M:%S"
            ).timetuple()
            timeclose = int(mktime(tt))
            _status, _error = self.interactive_display_message(
                {u'message': TR(u'session_closed_at %s') % deconnection_time}
            )
        return timeclose, _status, _error

    def interactive_target(self, data_to_send):
        data_to_send.update({u'module': u'interactive_target'})
        self.send_data(data_to_send)
        _status, _error = self.receive_data()
        if self.shared.get(u'display_message') != u'True':
            _status, _error = False, TR(u'Connection closed by client')
        return _status, _error

    def complete_target_info(self, kv, allow_interactive_password,
                             allow_interactive_login):
        """
        This procedure show interactive screen to enter target host,
        target login and target password if needed:
        * Host is asked if host information is a subnet
        * Login is asked if it is a interactive login
        * password is asked if it is missing and it is allowed to ask
              for interactive password
        """
        keylist = [u'target_password', u'target_login', u'target_host']
        extkv = dict((x, kv.get(x)) for x in keylist if kv.get(x) is not None)
        tries = 3
        _status, _error = None, None
        while (tries > 0) and (_status is None):
            tries -= 1
            interactive_data = {}
            if (not extkv[u'target_password']
                and allow_interactive_password):
                interactive_data[u'target_password'] = MAGICASK
            if not extkv.get(u'target_login') and allow_interactive_login:
                interactive_data[u'target_login'] = MAGICASK
                if allow_interactive_password:
                    interactive_data[u'target_password'] = MAGICASK
            target_subnet = None
            if '/' in extkv.get(u'target_host'):  # target_host is a subnet
                target_subnet = extkv.get(u'target_host')
                interactive_data[u'target_host'] = MAGICASK
                if _error:
                    host_note = TR("error %s") % _error
                else:
                    host_note = TR(u"in_subnet %s") % target_subnet
                interactive_data[u'target_device'] = host_note
            if interactive_data:
                Logger().info(u"Interactive Target Info asking")
                if not target_subnet:
                    interactive_data[u'target_host'] = \
                        extkv.get(u'target_host')
                    interactive_data[u'target_device'] = (
                        kv.get(u'target_device') if self.target_context.host
                        else self.shared.get(u'target_device')
                    )
                if not interactive_data.get(u'target_password'):
                    interactive_data[u'target_password'] = ''
                if not interactive_data.get(u'target_login'):
                    interactive_data[u'target_login'] = \
                        extkv.get(u'target_login')
                _status, _error = self.interactive_target(interactive_data)
                if _status:
                    if interactive_data.get(u'target_password') == MAGICASK:
                        extkv[u'target_password'] = \
                            self.shared.get(u'target_password')
                    if interactive_data.get(u'target_login') == MAGICASK:
                        extkv[u'target_login'] = \
                            self.shared.get(u'target_login')
                    if interactive_data.get(u'target_host') == MAGICASK:
                        if self.check_hostname_in_subnet(
                                self.shared.get(u'target_host'),
                                target_subnet
                        ):
                            extkv[u'target_host'] = \
                                self.shared.get(u'target_host')
                            extkv[u'target_device'] = \
                                self.shared.get(u'target_host')
                        else:
                            extkv[u'target_host'] = target_subnet
                            _status = None
                            _error = TR("no_match_subnet %s %s") % (
                                truncat_string(
                                    self.shared.get(u'target_host')
                                ),
                                target_subnet
                            )
            else:
                _status, _error = True, "OK"
        return extkv, _status, _error

    def interactive_close(self, target, message):
        data_to_send = {
            u'error_message': message,
            u'trans_ok': u'OK',
            u'module': u'close',
            u'target_device': target,
            u'target_login': self.shared.get(u'target_login'),
            u'target_password': u'Default',
        }

        # If we send close we should expect authentifier socket will be
        # closed by the other end
        # No need to return some warning message if that happen
        self.send_data(data_to_send)
        _status, _error = self.receive_data()

        return _status, _error

    def authentify(self):
        """ Authentify the user through password engine and then retreive his rights
             The user preferred language will be set as the language to use in
             interactive messages
        """
        _status, _error = self.receive_data()
        if not _status:
            return False, _error

        if self.shared.get(u'login') == MAGICASK:
            return None, TR(u"Empty user, try again")

        (_status, _error,
         wab_login, target_login, target_device,
         self.target_service_name, self.target_group,
         self.effective_login) = self.parse_username(
            self.shared.get(u'login'),
            self.shared.get(u'target_login'),
            self.shared.get(u'target_device'),
            self.target_service_name,
            self.target_group)
        if not _status:
            return None, TR(u"Invalid user, try again")

        self.rdplog.update_context(self.shared.get(u'psid'), wab_login)
        Logger().info(u"Continue with authentication (%s) -> %s" % (
            self.shared.get(u'login'), wab_login)
        )

        method = "Password"

        try:
            target_info = None
            if (target_login
                and target_device
                and not target_login == MAGICASK
                and not target_device == MAGICASK):
                if (self.target_service_name
                    and not self.target_service_name == MAGICASK):
                    target_info = u"%s@%s:%s" % (target_login, target_device,
                                                 self.target_service_name)
                else:
                    target_info = u"%s@%s" % (target_login, target_device)
                try:
                    target_info = target_info.encode('utf8')
                except Exception:
                    target_info = None

            # Check if we are using OTP
            # before trying any authentification method
            is_otp = wab_login.startswith('_OTP_')

            # Check if X509 Authentication is active
            if (not is_otp
                and self.engine.is_x509_connected(
                    wab_login,
                    self.shared.get(u'ip_client'),
                    u"RDP",
                    target_info,
                    self.shared.get(u'ip_target'))):
                method = "X509"
                self.rdplog.log("AUTHENTICATION_TRY", method=method)
                # Prompt the user in proxy window
                # Wait for confirmation from GUI (or timeout)
                if not ((self.engine.is_x509_validated()
                         or self.interactive_ask_x509_connection())
                        and self.engine.x509_authenticate(
                            self.shared.get(u'ip_client'),
                            self.shared.get(u'ip_target'))):
                    self.rdplog.log("AUTHENTICATION_FAILURE", method=method)

                    return False, TR(u"x509 browser authentication not "
                                     u"validated by user")
            elif self.passthrough_mode:
                # Passthrough Authentification
                method = "Passthrough"
                self.rdplog.log("AUTHENTICATION_TRY", method=method)
                if not self.engine.passthrough_authenticate(
                        wab_login,
                        self.shared.get(u'ip_client'),
                        self.shared.get(u'ip_target')):
                    self.rdplog.log("AUTHENTICATION_FAILURE", method=method)
                    emsg = TR(u"passthrough_auth_failed_wab %s") % wab_login
                    return False, emsg
            else:
                # PASSWORD based Authentication
                is_magic_password = self.shared.get(u'password') == MAGICASK
                method = ((is_otp and "OTP")
                          or (self.engine.get_challenge() and "Challenge")
                          or "Password")
                self.rdplog.log("AUTHENTICATION_TRY", method=method)
                if ((is_magic_password and not is_otp)  # one-time pwd
                    or not self.engine.password_authenticate(
                        wab_login,
                        self.shared.get(u'ip_client'),
                        rvalue(self.shared.get(u'password')),
                        self.shared.get(u'ip_target'))):
                    if is_magic_password:
                        self.engine.reset_challenge()
                    self.rdplog.log("AUTHENTICATION_FAILURE", method=method)
                    return None, TR(u"auth_failed_wab %s") % wab_login

            # At this point, User is authentified.
            if wab_login.startswith('_OTP_'):
                method = "OTP"
                real_wab_login = self.engine.get_username()
                self.rdplog.update_context(self.shared.get(u'psid'),
                                           real_wab_login)
                if re.search('WALLIX Access Manager',
                             self.engine.get_otp_client(), re.IGNORECASE):
                    self.send_data({
                        u'module': u'transitory',
                        u'is_wabam': u'True'
                    })
                self.shared[u'login'] = self.shared.get(u'login').replace(
                    wab_login, real_wab_login
                )

            self.language = self.engine.get_language()
            self.load_login_message(self.language)
            self._load_selector_banner()

            self.rdplog.log("AUTHENTICATION_SUCCESS", method=method)
            Logger().info(u'lang=%s' % self.language)

        except Exception:
            if DEBUG:
                import traceback
                Logger().info("<<<%s>>>" % traceback.format_exc())
            _status, _error = None, TR(u'auth_failed_wab %s') % wab_login
            self.rdplog.log("AUTHENTICATION_FAILURE", method=method)

        return _status, _error

    def get_service(self):
        """ Send service pages to proxy until the selected service is returned.
        """

        Logger().info(u"get_service")
        self.back_selector = False
        self.hide_approval_back_selector = True
        (_status, _error,
         wab_login, target_login, target_device,
         self.target_service_name, self.target_group,
         self.effective_login) = self.parse_username(
            self.shared.get(u'login'),
            self.shared.get(u'target_login'),
            self.shared.get(u'target_device'),
            self.target_service_name,
            self.target_group)

        if not _status:
            Logger().info(u"Invalid user %s, try again" %
                          self.shared.get(u'login'))
            return None, TR(u"Invalid user, try again")
        _status, _error = None, TR(u"No error")

        (target_device,
         self.target_context) = self.engine.resolve_target_host(
             rvalue(target_device), rvalue(target_login),
             self.target_service_name, self.target_group,
             rvalue(self.shared.get(u'real_target_device')),
             self.target_context,
             self.passthrough_mode, [u'RDP', u'VNC'])

        while _status is None:
            if (target_device and target_device != MAGICASK
                and (target_login or self.passthrough_mode)
                and target_login != MAGICASK):
                # Target is provided at login
                self._full_user_device_account = u"%s@%s:%s" % (
                    target_login, target_device, wab_login
                )
                data_to_send = {
                    u'login': wab_login,
                    u'target_login': target_login,
                    u'target_device': target_device,
                    u'module': u'transitory',
                    u'target_service': self.target_service_name,
                }
                if not self.internal_target:
                    self.internal_target = (
                        True if self.target_service_name == u'INTERNAL'
                        else False
                    )
                self.send_data(data_to_send)
                _status = True
            elif self.shared.get(u'selector') == MAGICASK:
                # filters ("Group" and "Account/Device") entered by user
                # in selector are applied to raw services list
                self.engine.get_proxy_rights(
                    [u'RDP', u'VNC'],
                    target_context=self.target_context
                )
                selector_filters_case_sensitive = SESMANCONF[u'sesman'].get(
                    'selector_filters_case_sensitive', False
                )
                services, item_filtered = self.engine.get_targets_list(
                    group_filter=self.shared.get(u'selector_group_filter'),
                    device_filter=self.shared.get(u'selector_device_filter'),
                    protocol_filter=self.shared.get(u'selector_proto_filter'),
                    case_sensitive=selector_filters_case_sensitive
                )
                if (len(services) > 1) or item_filtered:
                    try:
                        _current_page = \
                            int(self.shared.get(u'selector_current_page')) - 1
                        _lines_per_page = \
                            int(self.shared.get(u'selector_lines_per_page'))

                        if not _lines_per_page:
                            target_login = u""
                            target_device = u""
                            proto_dest = u""

                            data_to_send = {
                                u'login': wab_login,
                                u'target_login': target_login,
                                u'target_device': target_device,
                                u'proto_dest': proto_dest,
                                # , u'selector'                : u"True"
                                u'ip_client': self.shared.get(u'ip_client'),
                                u'selector_number_of_pages': u"0",
                                # No lines sent, reset filters
                                u'selector_group_filter': u"",
                                u'selector_device_filter': u"",
                                u'selector_proto_filter': u"",
                                u'module': u'selector',
                            }

                        else:
                            _number_of_pages = (
                                1 + (len(services) - 1) // _lines_per_page
                            )
                            if _current_page >= _number_of_pages:
                                _current_page = _number_of_pages - 1
                            if _current_page < 0:
                                _current_page = 0
                            _start_of_page = _current_page * _lines_per_page
                            _end_of_page = _start_of_page + _lines_per_page

                            services = sorted(
                                services,
                                key=lambda x: x[1]
                            )[_start_of_page:_end_of_page]

                            all_target_login = [s[0] for s in services]
                            all_target_device = [s[1] for s in services]
                            all_proto_dest = [s[2] for s in services]

                            target_login = u"\x01".join(all_target_login)
                            target_device = u"\x01".join(all_target_device)
                            proto_dest = u"\x01".join(all_proto_dest)

                            data_to_send = {
                                u'login': wab_login,
                                u'target_login': target_login,
                                u'target_device': target_device,
                                u'proto_dest': proto_dest,
                                u'ip_client': self.shared.get(u'ip_client'),
                                u'selector_number_of_pages': (
                                    "%s" %
                                    max(_number_of_pages, _current_page + 1)
                                ),
                                u'selector_current_page': (
                                    "%s" % (_current_page + 1)
                                ),
                                u'selector_group_filter':
                                self.shared.get(u'selector_group_filter'),
                                u'selector_device_filter':
                                self.shared.get(u'selector_device_filter'),
                                u'selector_proto_filter':
                                self.shared.get(u'selector_proto_filter'),
                                u'opt_message': u'',
                                u'module': u'selector',
                            }

                        self.send_data(data_to_send)

                        _status, _error = self.receive_data()

                        if self.shared.get(u'login') == MAGICASK:
                            self.language = self.engine.get_language()
                            self.load_login_message(self.language)
                            self.send_data({
                                u'login': MAGICASK,
                                u'selector_lines_per_page': u'0',
                                u'login_message': self.login_message,
                                u'language': self.language,
                                u'module': u'login',
                            })
                            Logger().info(u"Logout")
                            return None, u"Logout"

                        target_login = MAGICASK
                        target_device = MAGICASK
                        # proto_dest = MAGICASK
                        (_status, _error,
                         wab_login, target_login, target_device,
                         self.target_service_name, self.target_group,
                         self.effective_login) = self.parse_username(
                            self.shared.get(u'login'), target_login,
                             target_device, self.target_service_name,
                             self.target_group)
                        if not _status:
                            Logger().info(u"Invalid user %s, try again" %
                                          self.shared.get(u'login'))
                            return None, TR(u"Invalid user, try again")

                        _status = None  # One more loop
                    except Exception:
                        _emsg = u"Unexpected error in selector pagination"
                        if DEBUG:
                            import traceback
                            Logger().info(
                                u"%s %s" %
                                (_emsg, traceback.format_exc())
                            )

                        return False, _emsg
                    if self.allow_back_selector:
                        self.back_selector = True
                    self.hide_approval_back_selector = False
                elif len(services) == 1:
                    Logger().info(u"service len = 1 %s" % str(services))
                    s = services[0]
                    data_to_send = {}
                    data_to_send[u'login'] = wab_login
                    data_to_send[u'module'] = u'transitory'
                    # service_login (s[1]) format:
                    # target_login@device_name:service_name
                    # target_login can contains '@'
                    # device_name and service_name can not contain ':', nor '@'

                    # target_split = [**target_login, device_name:service_name]
                    target_split = s[1].split('@')
                    target_login = '@'.join(target_split[:-1])
                    # device_service_split = [ device_name, service_name ]
                    device_service_split = target_split[-1].split(':')
                    device_name = device_service_split[0]
                    service_name = device_service_split[-1]

                    data_to_send[u'target_login'] = target_login
                    data_to_send[u'target_device'] = device_name
                    data_to_send[u'target_service'] = service_name
                    self._full_user_device_account = u"%s@%s:%s" % (
                        target_login,
                        device_name,
                        wab_login
                    )
                    if not self.internal_target:
                        self.internal_target = (True if s[2] == u'INTERNAL'
                                                else False)
                    self.send_data(data_to_send)
                    self.target_service_name = service_name
                    self.target_group = s[0]
                    # Logger().info("Only one target : service name %s" %
                    #               self.target_service_name)
                    _status = True
                else:
                    _status, _error = False, TR(u"Target unreachable")

            else:
                self.send_data({
                    u'login': MAGICASK,
                    u'login_message': self.login_message,
                    u'module': 'login',
                })
                return None, u"Logout"

        return _status, _error
    # END METHOD - GET_SERVICE

    def check_password_expiration_date(self):
        _status, _error = True, u''
        try:
            notify, days = self.engine.password_expiration_date()
            if notify:
                if days == 0:
                    message = TR(u'Your Bastion password will expire soon. '
                                 u'Please change it.')
                else:
                    message = TR(u'Your Bastion password will expire in '
                                 u'%s days. Please change it.') % days
                _status, _error = self.interactive_display_message({
                    u'message': message
                })
        except Exception:
            if DEBUG:
                import traceback
                Logger().info("<<<<%s>>>>" % traceback.format_exc())
        return _status, _error

    def create_record_path_directory(self, rec_path):
        try:
            os.stat(rec_path)
        except OSError:
            try:
                os.mkdir(rec_path)
            except Exception:
                Logger().info(u"Failed creating recording path (%s)" %
                              rec_path)
                self.send_data({u'rejected': TR(u'error_getting_record_path')})
                return False, TR(u'error_getting_record_path %s') % rec_path
        return True, u''

    def generate_record_filebase(self, session_id, user, account, start_time):
        """
        Naming convention :
        {session_id},{username}@{userip},
        {account}@{devicename},
        YYYYMMDD-HHMMSS,{wabhostname},{uid}

        NB :  backslashes are replaced by pipes for IE compatibility
        """
        random.seed(self.pid)
        fname = (u"%(session_id)s,%(username)s@%(source_ip)s,"
                 u"%(account)s@%(device)s,"
                 u"%(timestamp)s,%(host)s,%(random)s")

        basename = fname % {
            'session_id': session_id,
            'username': user,
            'source_ip': self.shared.get(u'ip_client'),
            'account': account,
            'device': self.shared.get(u'target_device'),
            'timestamp': start_time.strftime("%Y%m%d-%H%M%S"),
            'host': gethostname(),
            'random': random.randint(1000, 9999)
        }
        basename = re.sub(r'[^-A-Za-z0-9_@,.]', "", basename)
        return basename

    def get_trace_keys(self):
        derivator = self.record_filebase + u".mwrm"
        encryption_key = self.engine.get_trace_encryption_key(derivator, False)
        formated_encryption_key = bin2hex(encryption_key)
        sign_key = self.engine.get_trace_sign_key()
        formated_sign_key = bin2hex(sign_key)
        return formated_encryption_key, formated_sign_key

    def interactive_ask_recording(self, user):
        message = (u"Warning! Your remote session may be recorded and"
                   u"kept in electronic format.")
        try:
            message = self.engine.get_banner(self.language)
            _status, _error = self.interactive_accept_message(
                {u'message': message}
            )
        except Exception:
            if DEBUG:
                import traceback
                Logger().debug("%s" % traceback.format_exc())
            _status, _error = False, TR(u"Connection closed by client")
        Logger().info(u"Recording agreement of %s to %s@%s : %s" %
                      (user,
                       self.shared.get(u'target_login'),
                       self.shared.get(u'target_device'),
                       ["NO", "YES"][_status]))
        if _status is False:
            reason = u"Session recording rejected by user"
            self.engine.set_session_status(result=False, diag=reason)
        return _status, _error

    def load_video_recording(self, rec_path, user):
        Logger().info(u"Checking video")

        _status, _error = True, u''
        data_to_send = {
            u'is_rec': u'False',
            u'trace_type': u"0",
            u'module': u'transitory',
        }

        data_to_send[u'is_rec'] = True
        if self._trace_type == "localfile":
            data_to_send[u"trace_type"] = u'0'
        elif self._trace_type == "cryptofile":
            data_to_send[u"trace_type"] = u'2'
        else:   # localfile_hashed
            data_to_send[u"trace_type"] = u'1'

        self.full_path = os.path.join(rec_path, self.record_filebase)

        self.send_data(data_to_send)

        return _status, _error

    def load_session_log_redirection(self, rec_path):
        Logger().info(u"Checking session log redirection")

        data_to_send = {
            u'module': u'transitory'
        }

        self.full_log_path = os.path.join(
            rec_path,
            self.record_filebase + u'.log'
        )

        Logger().info(u"Session log will be redirected to %s" %
                      self.full_log_path)
        self.send_data(data_to_send)

        return True, u''

    def select_target(self):
        """
        FIND TARGET

        The purpose of the snippet below is electing the first right that match
        the login AND device AND service that have been passed in the
        connection string.
        If service is blank take the first right that match login AND device
        (may happen with a command line or a mstsc '.rdp' file connections ;
        never happens if the selector is used).
        NB : service names are supposed to be in alphabetical ascending order.
        """
        selected_target = None
        target_device = self.shared.get(u'target_device')
        target_login = self.shared.get(u'target_login')
        target_service = (
            self.target_service_name if self.target_service_name != u'INTERNAL'
            else u'RDP'
        )
        target_group = self.target_group

        Logger().info("selected target ==> %s %s %s" % (
            target_login, target_device, target_service)
        )
        selected_target = self.engine.get_selected_target(target_login,
                                                          target_device,
                                                          target_service,
                                                          target_group,
                                                          self.target_context)
        if not selected_target:
            _target = u"%s@%s:%s (%s)" % (
                target_login, target_device, target_service, target_group)
            _error_log = TR(u"Target %s not found in user rights") % _target
            _status, _error = False, _error_log
            self.rdplog.log("TARGET_ERROR",
                            target=target_login,
                            reason="Target not found in user rights")
            Logger().info("%s" % _error)
            return None, _status, _error
        return selected_target, True, ""

    def check_target(self, selected_target):
        """ Checking selected target validity
        """
        ticket = None
        status = None
        info_message = None
        got_signal = False
        while True:
            Logger().info(u"Begin check_target ticket = %s..." % ticket)
            previous_status = status
            previous_info_message = info_message
            status, infos = self.engine.check_target(selected_target,
                                                     self.pid,
                                                     ticket)
            ticket = None
            info_message = infos.get('message')
            refresh_page = (got_signal
                            or (status != previous_status)
                            or (previous_info_message != info_message)
                            or (status == APPROVAL_NONE))
            Logger().info(u"End check_target ... refresh : %s" % refresh_page)
#            if refresh_page:
#                self.send_data({u'forcemodule': True})
            if refresh_page:
                self.send_data({u'module': u'transitory'})

            if status == APPROVAL_ACCEPTED:
                return True, ""
            if refresh_page:
                self.interactive_display_waitinfo(status, infos)
            got_signal = False
            r = []
            try:
                Logger().info(u"Start Select ...")
                logtimer.pause()
                if status == APPROVAL_PENDING:
                    # waiting for status update
                    r, w, x = select([self.proxy_conx], [], [],
                                     WORKFLOW_POLL_INTERVAL)
                else:
                    r = self.wait_read_proxy_conx()
                logtimer.resume()
            except BastionSignal as e:
                Logger().info("Got Signal %s" % e)
                got_signal = True
            except Exception as e:
                logtimer.resume()
                if DEBUG:
                    Logger().info("exception: '%s'" % e)
                    import traceback
                    Logger().info("<<<<%s>>>>" % traceback.format_exc())
                raise
            if self.proxy_conx in r:
                _status, _error = self.receive_data(blocking_call=False)
                if self.shared.get(u'waitinforeturn') == "backselector":
                    # received back to selector
                    self.send_data({
                        u'module': u'selector', u'target_login': '',
                        u'target_device': ''
                    })
                    return None, ""
                if self.shared.get(u'waitinforeturn') == "exit":
                    # received exit
                    self.send_data({u'module': u'close'})
                    return False, ""
                if self.shared.get(u'waitinforeturn') == "confirm":
                    # should parse the ticket info
                    desc = self.shared.get(u'comment')
                    ticketno = self.shared.get(u'ticket')
                    duration = self.parse_duration(
                        self.shared.get(u'duration')
                    )
                    ticket = {u"description": desc if desc else None,
                              u"ticket": ticketno if ticketno else None,
                              u"duration": duration}
        return False, ""

    def parse_duration(self, duration):
        if duration:
            try:
                mpat = re.compile(r"(\d+)m")
                hpat = re.compile(r"(\d+)h")
                hres = hpat.search(duration)
                mres = mpat.search(duration)
                duration = 0
                if mres:
                    duration += 60 * int(mres.group(1))
                if hres:
                    duration += 60 * 60 * int(hres.group(1))
                if duration == 0:
                    duration = 3600
            except Exception:
                duration = 3600
        else:
            duration = 3600
        return duration

    @staticmethod
    def _get_tf_flags(ticketfields):
        flag = 0
        field = ticketfields.get("description")
        if field is not None:
            flag += 0x01
            if field == APPREQ_REQUIRED:
                flag += 0x02
        field = ticketfields.get("ticket")
        if field is not None:
            flag += 0x04
            if field == APPREQ_REQUIRED:
                flag += 0x08
        field = ticketfields.get("duration")
        if field is not None:
            flag += 0x10
            if field == APPREQ_REQUIRED:
                flag += 0x20
        return flag

    # 'request_fields':{
    #     '<name>': {
    #         'label': str, # translated field to be displayed by the clients
    #         'type': 'str'|'int'|'datetime'|'duration',
    #         'mandatory': bool,
    #         'min': int,
    #         'max': int
    # }, ...}

    MAP_FIELD_FLAG = {
        'description': 0x01,
        'ticket': 0x04,
        'duration': 0x10
    }

    @staticmethod
    def _get_rf_flags(request_fields):
        flag = 0
        if not request_fields:
            return flag
        for key, value in request_fields.items():
            bitset = Sesman.MAP_FIELD_FLAG.get(key)
            if bitset is not None:
                flag += bitset
                flag += (bitset << 1) if value.get('mandatory') else 0
        return flag

    @staticmethod
    def _get_rf_duration_max(request_fields):
        return request_fields.get('duration', {}).get('max', 0)

    def interactive_display_waitinfo(self, status, infos):
        show_message = infos.get('message') or ''
        target = infos.get('target')
        if target:
            show_message = "%s: %s\n%s" % (
                TR(u"selected_target"), target, show_message
            )
        tosend = {
            u'module': u'waitinfo',
            u'message': show_message,
            u'display_message': MAGICASK,
            u'waitinforeturn': MAGICASK
        }
        flag = 0
        duration_max = infos.get("duration_max") or 0
        ticketfields = infos.get("ticket_fields")
        if ticketfields:
            flag = self._get_tf_flags(ticketfields)
        request_fields = infos.get('request_fields')
        if request_fields:
            flag = self._get_rf_flags(request_fields)
            # duration_max is in minutes
            duration_max = self._get_rf_duration_max(request_fields) // 60
        if self.hide_approval_back_selector:
            flag |= 0x10000
        if status == APPROVAL_NONE:
            tosend["showform"] = True
            tosend["duration_max"] = duration_max
        else:
            tosend["showform"] = False
        tosend["formflag"] = flag
        self.send_data(tosend)

    def start(self):
        _status, tries = None, 5
        while _status is None and tries > 0:
            self.reset_session_var()

            # AUTHENTIFY
            # [ LOGIN ]
            logtimer.start("PRIMARY_AUTH")
            _status, _error = self.authentify()

            if _status is None and self.engine.get_challenge():
                challenge = self.engine.get_challenge()
                # submit challenge:
                message = challenge.fields[0] if challenge.fields else ""
                echo = challenge.echos[0] if challenge.echos else False
                if not message:
                    message = challenge.message
                elif challenge.challenge_type == "MFA":
                    message = "%s:" % message
                    if challenge.message:
                        message = "%s\n%s" % (challenge.message, message)
                data_to_send = {
                    u'authentication_challenge': echo,
                    u'message': message,
                    u'module': u'challenge'
                }
                self.send_data(data_to_send)
                continue

            tries = tries - 1
            if _status is None and tries > 0:
                Logger().info(
                    u"Bastion user '%s' authentication "
                    u"from %s failed [%u tries remains]" %
                    (mundane(self.shared.get(u'login')),
                     mundane(self.shared.get(u'ip_client')),
                     tries)
                )

                (current_status, current_error,
                 current_wab_login, current_target_login,
                 current_target_device, self.target_service_name,
                 self.target_group,
                 self.effective_login) = self.parse_username(
                     self.shared.get(u'login'),
                     self.shared.get(u'target_login'),
                     self.shared.get(u'target_device'),
                     self.target_service_name,
                     self.target_group)

                if self.language != SESMANCONF.language:
                    if not self.language:
                        self.set_language_from_keylayout()
                    SESMANCONF.language = self.language

                data_to_send = {
                    u'login': (
                        self.shared.get(u'login')
                        if not current_wab_login.startswith('_OTP_')
                        else MAGICASK
                    ),
                    u'password': MAGICASK,
                    u'module': u'login',
                    u'login_message': self.login_message,
                    u'language': SESMANCONF.language,
                    u'opt_message': (
                        TR(u'authentication_failed')
                        if self.shared.get(u'password') != MAGICASK
                        else u''
                    )
                }
                self.send_data(data_to_send)
                continue

            if _status:
                tries = 5
                Logger().info(
                    u"Bastion user '%s' authentication succeeded" %
                    mundane(self.shared.get(u'login'))
                )
                if not self.engine.check_license():
                    _status, _error = False, "License 'sm' not available"
                    break
                # Warn password will expire soon for user
                _status, _error = self.check_password_expiration_date()

                # Get services for identified user
                _status = None
                while _status is None:
                    # [ SELECTOR ]
                    logtimer.start("FETCH_RIGHTS")
                    _status, _error = self.get_service()
                    Logger().info("get_service end :%s" % _status)
                    if not _status:
                        # logout or error in selector
                        self.engine.reset_proxy_rights()
                        logtimer.stop("FETCH_RIGHTS")
                        break
                    logtimer.start("CHECKOUT_TARGET")
                    selected_target, _status, _error = self.select_target()
                    Logger().info("select_target end :%s" % _status)
                    if not _status:
                        # target not available
                        self.engine.reset_proxy_rights()
                        logtimer.stop("CHECKOUT_TARGET")
                        break
                    # [ WAIT INFO ]
                    _status, _error = self.check_target(selected_target)
                    Logger().info("check_target end :%s" % _status)
                    if not _status:
                        logtimer.stop("CHECKOUT_TARGET")
                        if _status is None:
                            continue
                        self.engine.reset_proxy_rights()
                        break
                    # [ CONNECT TO TARGET ]
                    try:
                        _status, _error = self.connect_to_target(
                            selected_target
                        )
                    except Exception as e:
                        Logger().info(
                            "Unexpected Error on target connection "
                            "(%s)" % e
                        )
                        _status, _error = False, "End of Session"
                        logtimer.stop()
                        self.engine.release_all_target()
                        self.engine.stop_session()
                    self.reset_target_session_vars()
                self.engine.proxy_session_logout()
                self.rdplog.log("LOGOUT")
                self.engine.close_client()

        if tries <= 0:
            Logger().info(u"Too many login failures")
            _status = False

        if _status:
            Logger().info(
                u"Asking service %s@%s" % (
                    self.shared.get(u'target_login'),
                    self.shared.get(u'target_device')
                )
            )

    # END METHOD - START

    def connect_to_target(self, selected_target):
        """
        #####################
        ### START_SESSION ###
        #####################
        """
        extra_info = self.engine.get_target_extra_info()
        _status, _error = True, u''

        Logger().info(u"Fetching protocol")
        kv = {}
        if _status:
            target_login_info = self.engine.get_target_login_info(
                selected_target
            )
            proto_info = self.engine.get_target_protocols(selected_target)
            kv[u'proto_dest'] = proto_info.protocol
            kv[u'target_str'] = target_login_info.get_target_str()

        if _status:
            kv['password'] = 'pass'

            # id to recognize primary user for session probe
            kv['primary_user_id'] = self.engine.get_username()

            # register signal
            signal.signal(signal.SIGUSR1, self.kill_handler)
            signal.signal(signal.SIGUSR2, self.check_handler)

            Logger().info(u"Starting Session, effective login='%s'" %
                          self.effective_login)

            user = self.engine.get_username()
            uname = self.effective_login or target_login_info.account_login

            # Add connection to the observer
            session_id, start_time, error_msg = self.engine.start_session(
                selected_target,
                self.pid,
                self.effective_login
            )
            if session_id is None:
                _status, _error = False, \
                    TR(u"start_session_failed") + " (" + error_msg + ")"


        if _status:
            record_warning = SESMANCONF[u'sesman'].get('record_warning', True)
            if record_warning and extra_info.is_recorded:
                _status, _error = self.interactive_ask_recording(
                    user
                )

        if _status:
            timeclose, _status, _error = self.check_deconnection_time(
                selected_target
            )
            if timeclose is not None:
                kv['timeclose'] = timeclose

        if _status:
            # add "Year-Month-Day" subdirectory to record path
            date_path = start_time.strftime("%Y-%m-%d")
            rec_path = os.path.join(LOCAL_TRACE_PATH_RDP, date_path)
            self.record_filebase = self.generate_record_filebase(
                session_id,
                user,
                uname,
                start_time
            )
            kv['record_filebase'] = self.record_filebase
            kv['record_subdirectory'] = date_path
            # TODO kv[u'record_path'] = LOCAL_TRACE_PATH_RDP
            Logger().info(u"Session will be recorded in %s" %
                          self.record_filebase)
            try:
                _status, _error = self.create_record_path_directory(
                    rec_path
                )
                if _status and extra_info.is_recorded:
                    _status, _error = self.load_video_recording(
                        rec_path, user
                    )
                if _status:
                    _status, _error = self.load_session_log_redirection(
                        rec_path
                    )
                if _status:
                    encryption_key, sign_key = self.get_trace_keys()
                    kv['encryption_key'] = encryption_key
                    kv['sign_key'] = sign_key
            except Exception:
                import traceback
                Logger().debug("%s" % traceback.format_exc())
                _status, _error = False, TR(u"Connection closed by client")

        if not _status:
            self.send_data({u'rejected': _error})

        if _status:
            kv[u'session_id'] = session_id
            trace_written = False  # reminder to write_trace later
            pattern_kill, pattern_notify = self.engine.get_restrictions(
                selected_target, "RDP"
            )
            if pattern_kill:
                self.send_data({u'module': u'transitory',
                                u'pattern_kill': pattern_kill})
            if pattern_notify:
                self.send_data({u'module': u'transitory',
                                u'pattern_notify': pattern_notify})

        if _status:
            module = kv.get(u'proto_dest')
            if module not in [u'RDP', u'VNC', u'INTERNAL']:
                module = u'RDP'
            if self.internal_target:
                module = u'INTERNAL'
            kv[u'module'] = module
            # proto = u'RDP' if  kv.get(u'proto_dest') != u'VNC' else u'VNC'

        self.shared[u'recording_started'] = 'False'

        self.reporting_reason = None
        self.reporting_target = None
        self.reporting_message = None

        try_next = False
        close_box = False

        if _status:
            for physical_target in self.engine.get_effective_target(
                    selected_target
            ):
                try_next = False
                close_box = False
                kv[u'recording_started'] = "False"

                cstatus, infos = self.engine.check_target(physical_target,
                                                          self.pid,
                                                          None)
                if cstatus != APPROVAL_ACCEPTED:
                    Logger().info("Jump server unavailable (%s)"
                                  % infos.get('message'))
                    _status = False
                    continue

                physical_info = self.engine.get_physical_target_info(
                    physical_target
                )
                if not _status:
                    physical_target = None
                    break

                physical_proto_info = self.engine.get_target_protocols(
                    physical_target
                )
                application = self.engine.get_application(selected_target)
                conn_opts = self.engine.get_target_conn_options(
                    physical_target
                )
                if (physical_proto_info.protocol == u'RDP'
                    or physical_proto_info.protocol == u'VNC'):
                    if physical_proto_info.protocol == u'RDP':
                        kv[u'proxy_opt'] = ",".join(
                            physical_proto_info.subprotocols
                        )

                    kv.update(self.fetch_connectionpolicy(conn_opts))

                if 'rdp' in conn_opts:
                    rdp_opts = conn_opts.get('rdp', {})
                    krb_armoring_account = rdp_opts.get('krb_armoring_account')
                    krb_armoring_realm = rdp_opts.get('krb_armoring_realm')
                    krb_armoring_fallback_user = rdp_opts.get('krb_armoring_fallback_user', '')
                    krb_armoring_fallback_password = rdp_opts.get('krb_armoring_fallback_password', '')

                    if krb_armoring_account:
                        effective_krb_armoring_user = self.engine.get_scenario_account_field(
                            u'login', krb_armoring_account, default=krb_armoring_fallback_user)
                        effective_krb_armoring_password = self.engine.get_scenario_account_field(
                            u'password', krb_armoring_account, default=krb_armoring_fallback_password)
                    else:
                        effective_krb_armoring_user = krb_armoring_fallback_user
                        effective_krb_armoring_password = krb_armoring_fallback_password

                    if krb_armoring_realm:
                        effective_krb_armoring_user += '@' + krb_armoring_realm

                    kv[u'effective_krb_armoring_user'] = effective_krb_armoring_user
                    kv[u'effective_krb_armoring_password'] = effective_krb_armoring_password

                kv[u'disable_tsk_switch_shortcuts'] = u'no'
                if application:
                    app_params = self.engine.get_app_params(
                        selected_target,
                        physical_target
                    )
                    if not app_params:
                        continue
                    kv[u'alternate_shell'] = app_params.program
                    if app_params.params is not None:
                        kv[u'shell_arguments'] = app_params.params
                    kv[u'shell_working_directory'] = app_params.workingdir

                    kv[u'target_application'] = "%s@%s" % \
                        (target_login_info.account_name,
                         target_login_info.target_name)
                    if app_params.params is not None:
                        if u'${USER}' in app_params.params:
                            kv[u'target_application_account'] = \
                                target_login_info.account_login or \
                                self.target_context.login or ""
                        if u'${PASSWORD}' in app_params.params:
                            kv[u'target_application_password'] = (
                                self.engine.get_target_password(
                                    selected_target
                                )
                                or self.engine.get_primary_password(
                                    selected_target
                                )
                                or ''
                            )
                    self.target_app_rights[kv[u'target_application']] = (
                        selected_target, app_params
                    )

                    # kv[u'target_application'] = selected_target.service_login
                    kv[u'disable_tsk_switch_shortcuts'] = u'yes'
                self.cn = target_login_info.target_name

                if self.target_context.host:
                    kv[u'target_host'] = self.target_context.host
                    kv[u'target_device'] = self.target_context.showname()
                else:
                    kv[u'target_host'] = physical_info.device_host

                kv[u'target_login'] = physical_info.account_login
                if (not kv.get(u'target_login')
                    and self.target_context.login
                    and not application):
                    # on application,
                    # login in target_context is the login of application
                    kv[u'target_login'] = self.target_context.login

                kv[u'target_port'] = physical_info.service_port
                kv[u'device_id'] = physical_info.device_id

                try:
                    auth_policy_methods = self.engine.get_target_auth_methods(
                        physical_target)
                    Logger().info("auth_mode_passthrough=%s" %
                                  self.passthrough_mode)

                    target_password = ''
                    if self.passthrough_mode:
                        kv[u'target_login'] = self.passthrough_target_login
                        if self.shared.get(u'password') == MAGICASK:
                            target_password = u''
                        else:
                            target_password = self.shared.get(u'password')
                        # Logger().info(
                        #     "auth_mode_passthrough target_password=%s" %
                        #     target_password
                        # )
                        kv[u'password'] = u'password'
                    elif PASSWORD_VAULT in auth_policy_methods:
                        target_passwords = self.engine.get_target_passwords(
                            physical_target
                        )
                        target_password = u'\x01'.join(target_passwords)

                    if (not target_password
                        and PASSWORD_MAPPING in auth_policy_methods):
                        target_password = (
                            self.engine.get_primary_password(physical_target)
                            or ''
                        )

                    force_sc_auth = conn_opts.get(
                        u'rdp', {}
                    ).get(
                        u'force_smartcard_authentication', False
                    )
                    Logger().info("force_smartcard_authentication = %s" %
                                  force_sc_auth)
                    allow_interactive_password = (
                        (PASSWORD_INTERACTIVE in auth_policy_methods
                         or self.passthrough_mode)
                        and not force_sc_auth
                    )
                    allow_interactive_login = not force_sc_auth

                    kv[u'target_password'] = target_password
                    is_interactive_login = (
                        not bool(kv.get('target_login'))
                        and not force_sc_auth
                    )
                    extra_kv, _status, _error = self.complete_target_info(
                        kv, allow_interactive_password,
                        allow_interactive_login)
                    kv.update(extra_kv)

                    if self.target_context.host:
                        self._physical_target_host = self.target_context.host
                    elif ('/' in physical_info.device_host
                          and extra_kv.get(u'target_host') != MAGICASK):
                        self._physical_target_host = \
                            extra_kv.get(u'target_host')
                    else:
                        self._physical_target_host = physical_info.device_host

                    Logger().info(
                        u"Send critic notification "
                        u"(every attempt to connect to some physical node)"
                    )
                    if extra_info.is_critical:
                        Logger().info("CRITICAL CONNECTION")
                        import socket
                        self.engine.NotifyConnectionToCriticalEquipment(
                            (u'APP' if application else proto_info.protocol),
                            self.shared.get(u'login'),
                            socket.getfqdn(self.shared.get(u'ip_client')),
                            self.shared.get(u'ip_client'),
                            self.shared.get(u'target_login'),
                            self.shared.get(u'target_device'),
                            self._physical_target_host,
                            ctime(),
                            None
                        )

                    update_args = {
                        "is_application": bool(application),
                        "target_host": self._physical_target_host,
                        "session_log_path": self.full_log_path
                    }
                    if is_interactive_login:
                        update_args["effective_login"] = kv.get('target_login')
                    if self.shared.get('width'):
                        update_args["video_width"] = \
                            int(self.shared.get('width'))
                    if self.shared.get('height'):
                        update_args["video_height"] = \
                            int(self.shared.get('height'))

                    self.engine.update_session_target(physical_target,
                                                      **update_args)

                    if not _status:
                        Logger().info(
                            u"(%s):%s:REJECTED : User message: \"%s\"" % (
                                mundane(self.shared.get(u'ip_client')),
                                mundane(self.shared.get(u'login')),
                                _error
                            )
                        )

                        kv = {
                            u"login": u"",
                            u'password': u"",
                            u'target_login': u"",
                            u'target_password': u"",
                            u'target_device': u"",
                            u'target_host': u"",
                            u'rejected': _error,
                        }

                    try_next = False
                    logtimer.stop("CHECKOUT_TARGET")
                    try:
                        ###########
                        # SEND KV #
                        ###########
                        self.send_data(kv)

                        Logger().info(
                            u"Added connection to active Bastion services"
                        )

                        # Looping on keepalived socket
                        while True:
                            r = []
                            got_signal = False
                            try:
                                r, w, x = select([self.proxy_conx], [], [],
                                                 KEEPALIVE_TIMEOUT)
                            except BastionSignal as e:
                                Logger().info("Got Signal %s" % e)
                                got_signal = True
                            except Exception as e:
                                if DEBUG:
                                    Logger().info("exception: '%s'" % e)
                                    import traceback
                                    Logger().info("<<<<%s>>>>" %
                                                  traceback.format_exc())
                                raise
                            self.engine.keepalive(timeout=KEEPALIVE_TIMEOUT)
                            current_time = time()
                            if self.check_session_parameters:
                                self.update_session_parameters(current_time)
                                self.check_session_parameters = False
                            self.rtmanager.check(current_time)
                            if self.proxy_conx in r:
                                _status, _error = self.receive_data(
                                    Sesman.EXPECTING_KEYS,
                                    blocking_call=False
                                )

                                if self._changed_keys:
                                    self.update_session_data(
                                        self._changed_keys
                                    )

                                self.process_target_connection_time()

                                if self.shared.get(u'auth_notify'):
                                    self.handle_auth_notify(physical_target)
                                    self.shared[u'auth_notify'] = u''

                                if (not trace_written
                                    and (self.shared.get('recording_started')
                                         == 'True')):
                                    _status, _error = \
                                        self.handle_recording_started()
                                    trace_written = True

                                if self.shared.get("pm_request"):
                                    self._manage_pm()

                                self.handle_shadowing()

                                if self.shared.get(u'reporting'):
                                    report_status = self.handle_reporting()
                                    if not report_status:
                                        try_next = True
                                        break

                                if self.shared.get(u'disconnect_reason_ack'):
                                    break

                                if self.shared.get(u'auth_channel_target'):
                                    self.handle_auth_channel_target(
                                        selected_target
                                    )
                                    self.shared[u'auth_channel_target'] = u''
                                if self.shared.get(u'module') == u"close":
                                    break
                                if self.shared.get(u'keepalive') == MAGICASK:
                                    self.send_data({u'keepalive': u'True'})
                            # r can be empty
                            else:
                                # (if self.proxy_conx in r)
                                if not self.internal_target and not got_signal:
                                    Logger().info(u'Missing Keepalive')
                                    Logger().error(u'break connection')
                                    break
                        if self.shared.get(u'module') == u"close":
                            close_box = True
                        Logger().debug(u"End Of Keep Alive")

                    except AuthentifierSocketClosed:
                        if DEBUG:
                            import traceback
                            Logger().info(
                                u"RDP/VNC connection terminated by client"
                            )
                            Logger().info("<<<%s>>>" % traceback.format_exc())
                    except Exception:
                        if DEBUG:
                            import traceback
                            Logger().info(
                                u"RDP/VNC connection terminated by client"
                            )
                            Logger().info("<<<%s>>>" % traceback.format_exc())
                    if not try_next:
                        break
                finally:
                    self.engine.release_target(physical_target)

        self.engine.release_all_target()
        Logger().info(u"Stop session ...")
        # Notify WabEngine to stop connection if it has been
        # launched successfully
        self.engine.stop_session(title=u"End session")

        Logger().info(u"Stop session done.")
        if self.shared.get(u"module") == u"close":
            if close_box and self.back_selector:
                try:
                    self.send_data({u'module': u'close_back',
                                    u'selector': u'False'})
                    while True:
                        _status, _error = self.receive_data()
                        if (_status
                            and self.shared.get(u'selector') == MAGICASK):
                            return None, "Go back to selector"
                except Exception:
                    _status, _error = False, "End of Session"
            else:
                self.send_data({u'module': u'close'})
        # Error
        if try_next:
            _status, _error = self.interactive_close(self.reporting_target,
                                                     self.reporting_message)

        try:
            Logger().info(u"Close connection ...")

            self.proxy_conx.close()

            Logger().info(u"Close connection done.")
        except IOError:
            if DEBUG:
                Logger().info(u"Close connection: Exception")
                Logger().info("<<<<%s>>>>" % traceback.format_exc())
        return False, "End of Session"

    def handle_reporting(self):
        _reporting = self.shared.get(u'reporting')
        _reporting_reason, _, _remains = \
            _reporting.partition(':')
        _reporting_target, _, _reporting_message = \
            _remains.partition(':')
        self.shared[u'reporting'] = u''

        Logger().info(u"Reporting: reason=\"%s\" "
                      "target=\"%s\" message=\"%s\"" %
                      (_reporting_reason,
                       _reporting_target,
                       _reporting_message))

        self.process_report(_reporting_reason,
                            _reporting_target,
                            _reporting_message)

        if _reporting_reason == u'CONNECTION_FAILED':
            self.reporting_reason = _reporting_reason
            self.reporting_target = _reporting_target
            self.reporting_message = _reporting_message

            release_reason = u'Connection failed'
            self.engine.set_session_status(
                result=False, diag=release_reason)
            return False
        elif _reporting_reason == u'FINDPATTERN_KILL':
            Logger().info(
                u"RDP connection terminated. Reason: Kill pattern detected"
            )
            release_reason = u'Kill pattern detected'
            self.engine.set_session_status(
                result=False, diag=release_reason)
            self.send_data({u'disconnect_reason': TR(u"pattern_kill")})
        elif _reporting_reason == u'SERVER_REDIRECTION':
            (redir_login, _, redir_host) = \
                _reporting_message.rpartition('@')
            update_args = {}
            if redir_host:
                update_args["target_host"] = redir_host
            if redir_login:
                update_args["target_account"] = redir_login
            self.engine.update_session(**update_args)
        elif _reporting_reason == u'SESSION_EXCEPTION':
            Logger().info(
                u"RDP connection terminated. Reason: Session exception"
            )
            release_reason = u'Session exception: ' + _reporting_message
            self.engine.set_session_status(
                diag=release_reason)
        elif _reporting_reason == u'SESSION_EXCEPTION_NO_RECORD':
            Logger().info(
                u"RDP connection terminated. Reason: Session exception "
                u"(no record)"
            )
            release_reason = u'Session exception: ' + _reporting_message
            self.engine.set_session_status(
                result=False, diag=release_reason)
        elif _reporting_reason == u'SESSION_PROBE_LAUNCH_FAILED':
            Logger().info(
                u"RDP connection terminated. Reason: Session Probe "
                u"launch failed"
            )
            release_reason = u'Interrupt: Session Probe launch failed'
            self.engine.set_session_status(
                result=False, diag=release_reason)
            if self.shared.get(u'session_probe_launch_error_message'):
                self.send_data({
                    u'disconnect_reason':
                    self.shared.get(u'session_probe_launch_error_message')
                })
                self.shared[u'session_probe_launch_error_message'] = u''
            else:
                self.send_data({
                    u'disconnect_reason': TR(u"session_probe_launch_failed")
                })
        elif _reporting_reason == u'SESSION_PROBE_KEEPALIVE_MISSED':
            Logger().info(
                u'RDP connection terminated. Reason: Session Probe '
                u'keepalive missed'
            )
            release_reason = u'Interrupt: Session Probe keepalive missed'
            self.engine.set_session_status(
                result=False, diag=release_reason)
            self.send_data({
                u'disconnect_reason': TR(u"session_probe_keepalive_missed")
            })
        elif (_reporting_reason
              == u'SESSION_PROBE_OUTBOUND_CONNECTION_BLOCKING_FAILED'):
            Logger().info(
                u'RDP connection terminated. Reason: Session Probe failed '
                u'to block outbound connection'
            )
            release_reason = (
                u'Interrupt: Session Probe failed to block outbound connection'
            )
            self.engine.set_session_status(
                result=False, diag=release_reason)
            self.send_data({
                u'disconnect_reason':
                TR(u"session_probe_outbound_connection_blocking_failed")
            })
        elif _reporting_reason == u'SESSION_PROBE_PROCESS_BLOCKING_FAILED':
            Logger().info(
                u'RDP connection terminated. Reason: Session Probe failed '
                u'to block process'
            )
            release_reason = (
                u'Interrupt: Session Probe failed to block process'
            )
            self.engine.set_session_status(
                result=False, diag=release_reason
            )
            self.send_data({
                u'disconnect_reason':
                TR(u"session_probe_process_blocking_failed")
            })
        elif (_reporting_reason
              == u'SESSION_PROBE_RUN_STARTUP_APPLICATION_FAILED'):
            Logger().info(
                u'RDP connection terminated. Reason: Session Probe failed '
                u'to run startup application'
            )
            release_reason = (
                u'Interrupt: Session Probe failed to run startup application'
            )
            self.engine.set_session_status(
                result=False, diag=release_reason
            )
            self.send_data({
                u'disconnect_reason':
                TR(u"session_probe_failed_to_run_startup_application")
            })
        elif _reporting_reason == u'SESSION_PROBE_RECONNECTION':
            Logger().info(
                u'RDP connection terminated. Reason: Session Probe '
                u'reconnection without disconnection'
            )
            release_reason = (
                u'Interrupt: Session Probe reconnection without disconnection'
            )
            self.engine.set_session_status(
                result=False, diag=release_reason
            )
            self.send_data({
                u'disconnect_reason': TR(u"session_probe_reconnection")
            })
        elif _reporting_reason == u'SESSION_EVENT':
            (event_level, event_id, event_details) = \
                _reporting_message.split(" : ", 2)
            update_args = {}
            update_args["event_level"] = event_level
            update_args["event_id"] = event_id
            update_args["event_level"] = event_level
            self.engine.update_session(**update_args)

            if u'FATAL' == update_args["event_level"]:
                Logger().info(
                    u'RDP connection terminated. Reason: Application '
                    u'fatal error'
                )
                release_reason = (
                    u'Interrupt: Application fatal error'
                )
                self.engine.set_session_status(
                    result=False, diag=release_reason
                )
                self.send_data({
                    u'disconnect_reason': TR(u"application_fatal_error")
                })
        return True

    def process_target_connection_time(self):
        if self.shared.get(u"target_connection_time"):
            try:
                tct = int(self.shared.get(u"target_connection_time", 0))
                fct = int(self.shared.get(u"front_connection_time", 0))
                logtimer.add_step_time("TARGET_CONN", tct / 1000.0)
                logtimer.add_step_time("PRIMARY_CONN", fct / 1000.0)
                metrics = logtimer.report_metrics()
                self.rdplog.log("TIME_METRICS",
                                session_id=self.shared.get(u'session_id'),
                                **metrics)
            except Exception:
                pass
            self.shared[u"target_connection_time"] = None

    def process_report(self, reason, target, message):
        if reason == u'CLOSE_SESSION_SUCCESSFUL':
            pass
        elif reason == u'CONNECTION_FAILED':
            self.engine.NotifySecondaryConnectionFailed(
                self.shared.get(u'login'),
                self.shared.get(u'ip_client'),
                self.shared.get(u'target_login'),
                self._physical_target_host)
        elif reason == u'CONNECTION_SUCCESSFUL':
            pass
        elif reason == u'OPEN_SESSION_FAILED':
            self.engine.NotifySecondaryConnectionFailed(
                self.shared.get(u'login'),
                self.shared.get(u'ip_client'),
                self.shared.get(u'target_login'),
                self._physical_target_host)
        elif reason == u'OPEN_SESSION_SUCCESSFUL':
            pass
        elif reason == u'FILESYSTEM_FULL':
            data = message.split(u'|')
            used = data[0]
            filesystem = data[1]

            self.engine.NotifyFilesystemIsFullOrUsedAtXPercent(filesystem,
                                                               used)
        elif reason == u'SESSION_EXCEPTION':
            pass
        elif reason == u'SESSION_EXCEPTION_NO_RECORD':
            pass
        elif reason == u'SESSION_PROBE_LAUNCH_FAILED':
            pass
        elif reason == u'SESSION_PROBE_KEEPALIVE_MISSED':
            pass
        elif reason == u'SESSION_PROBE_OUTBOUND_CONNECTION_BLOCKING_FAILED':
            pass
        elif reason == u'SESSION_PROBE_PROCESS_BLOCKING_FAILED':
            pass
        elif reason == u'SESSION_PROBE_RUN_STARTUP_APPLICATION_FAILED':
            pass
        elif reason == u'SERVER_REDIRECTION':
            (nlogin, _, nhost) = message.rpartition('@')
            Logger().info("Server Redirection: login='%s', host='%s'" %
                          (nlogin, nhost))
        elif (reason == u'FINDPATTERN_KILL'
              or reason == u'FINDPATTERN_NOTIFY'):
            pattern = message.split(u'|')
            regexp = pattern[0]
            string = pattern[1]
            # Logger().info(
            #     u"regexp=\"%s\" string=\"%s\" user_login=\"%s\" "
            #     u"user=\"%s\" host=\"%s\"" % (
            #         regexp, string, self.shared.get(u'login'),
            #         self.shared.get(u'target_login'),
            #         self.shared.get(u'target_device')
            #     )
            # )
            self.engine.NotifyFindPatternInRDPFlow(
                regexp, string, self.shared.get(u'login'),
                self.shared.get(u'target_login'),
                self.shared.get(u'target_device'),
                self.cn, self.target_service_name
            )
        elif (reason == u'FINDCONNECTION_DENY'
              or reason == u'FINDCONNECTION_NOTIFY'):
            pattern = message.split(u'|')
            notify_params = {
                'rule': pattern[0],
                'deny': (reason == u'FINDCONNECTION_DENY'),
                'app_name': pattern[1],
                'app_cmd_line': pattern[2],
                'dst_addr': pattern[3],
                'dst_port': pattern[4],
                'user_login': self.shared.get(u'login'),
                'user': self.shared.get(u'target_login'),
                'host': self.shared.get(u'target_device'),
                'cn': self.cn,
                'service': self.target_service_name
            }
            self.engine.notify_find_connection_rdp(**notify_params)
        elif (reason == u'FINDPROCESS_DENY'
              or reason == u'FINDPROCESS_NOTIFY'):
            pattern = message.split(u'|')
            notify_params = {
                'regex': pattern[0],
                'deny': (reason == u'FINDPROCESS_DENY'),
                'app_name': pattern[1],
                'app_cmd_line': pattern[2],
                'user_login': self.shared.get(u'login'),
                'user': self.shared.get(u'target_login'),
                'host': self.shared.get(u'target_device'),
                'cn': self.cn,
                'service': self.target_service_name
            }
            self.engine.notify_find_process_rdp(**notify_params)
        elif reason == u'SESSION_EVENT':
            pass
        else:
            Logger().info(
                u"Unexpected reporting reason: "
                "\"%s\" \"%s\" \"%s\"" % (reason, target, message))

    def handle_auth_channel_target(self, selected_target):
        Logger().info(
            u"Auth channel target=\"%s\"" %
            self.shared.get(u'auth_channel_target')
        )
        if self.shared.get(u'auth_channel_target').startswith(
                u'GetWabSessionParameters'
        ):
            app_target = selected_target
            _prefix, _sep, _val = self.shared.get(
                u'auth_channel_target'
            ).partition(':')
            if _sep:
                app_right_params = self.target_app_rights.get(_val)
                if app_right_params is not None:
                    app_target, _app_param = app_right_params
            app_info = self.engine.get_target_login_info(app_target)
            account_login = app_info.account_login
            application_password = \
                self.engine.get_target_password(app_target) \
                or self.engine.get_primary_password(app_target) \
                or ''
            _message = {
                'user': account_login,
                'password': application_password
            }

            # Logger().info(
            #     u"GetWabSessionParameters (response):" %
            #     json.dumps(_message)
            # )
            self.send_data({u'auth_channel_answer': json.dumps(_message)})
            Logger().info(
                u"Sending of auth channel "
                u"answer ok "
                "(GetWabSessionParameters)"
            )

    def handle_shadowing(self):
        if self.shared.get("rd_shadow_available") == 'True':
            update_args = {}
            update_args["shadow_allow"] = True
            self.engine.update_session(**update_args)
            self.shared["rd_shadow_available"] = 'False'

        if self.shared.get("rd_shadow_invitation_error_code"):
            shadow_token = {
                "shadow_id":
                self.shared.get("rd_shadow_invitation_id"),
                "shadow_ip":
                self.shared.get("rd_shadow_invitation_addr"),
                "shadow_port":
                self.shared.get("rd_shadow_invitation_port"),
            }
            self.engine.shadow_response(
                errcode=self.shared.get("rd_shadow_invitation_error_code"),
                errmsg=self.shared.get("rd_shadow_invitation_error_message"),
                token=shadow_token,
                userdata=self.shared.get("rd_shadow_userdata")
            )
            self.shared["rd_shadow_available"] = 'False'

            self.shared["rd_shadow_userdata"] = None
            self.shared["rd_shadow_invitation_error_code"] = 0
            self.shared["rd_shadow_invitation_error_message"] = None
            self.shared["rd_shadow_invitation_id"] = None
            self.shared["rd_shadow_invitation_addr"] = None
            self.shared["rd_shadow_invitation_port"] = None

    def handle_auth_notify(self, physical_target):
        if self.shared.get(u'auth_notify') == u'rail_exec':
            Logger().info(
                u"rail_exec flags=\"%s\" exe_of_file=\"%s\"" %
                (self.shared.get(u'auth_notify_rail_exec_flags'),
                 self.shared.get(u'auth_notify_rail_exec_exe_or_file'))
            )
            auth_command_kv = self.check_application(
                physical_target,
                self.shared.get(u'auth_notify_rail_exec_flags'),
                self.shared.get(u'auth_notify_rail_exec_exe_or_file')
            )
            self.send_data(auth_command_kv)

            self.shared[u'auth_notify_rail_exec_flags'] = u''
            self.shared[u'auth_notify_rail_exec_exe_or_file'] = u''

    def handle_recording_started(self):
        # write mwrm path to rdptrc (allow real time display)
        Logger().info(u"Call write trace")
        _status, _error = self.engine.write_trace(self.full_path)
        if not _status:
            _error = TR("Trace writer failed for %s") % self.full_path
            Logger().info(u"Failed accessing recording path (%s)" %
                          self.full_path)
            self.send_data({u'rejected': TR(u'error_getting_record_path')})
        return _status, _error

    KEYMAPPING = {
        # exchange key : (acl key, type)
        'height': ('video_height', 'int'),
        'width': ('video_width', 'int'),
        'rt_ready': ('rt', 'bool'),
        'fdx_path': ('fdx_path', 'str'),
        'smartcard_login': ('effective_login', 'str'),
        'native_session_id': ('native_session_id', 'str'),
    }
    EXPECTING_KEYS = list(KEYMAPPING.keys())

    @staticmethod
    def convert_value(value, cotype):
        if not isinstance(value, (str, text_type)):
            return value
        if cotype == 'int':
            return int(value)
        if cotype == 'bool':
            return (value.lower() == 'true')
        return value

    def update_session_data(self, changed_keys):
        data_to_update = {
            acl_key: Sesman.convert_value(val, cotype)
            for (acl_key, cotype), val in (
                (Sesman.KEYMAPPING.get(key), self.shared.get(key))
                for key in changed_keys if (
                    Sesman.KEYMAPPING.get(key) is not None
                    and self.shared.get(key) is not None
                )
            )
        }
        self.engine.update_session(**data_to_update)

    def fetch_connectionpolicy(self, conn_opts):
        connectionpolicy_kv = {}
        # Logger().info(u"%s" % conn_opts)
        for (section, matches) in cp_spec.items():
            section_values = conn_opts.get(section)
            if section_values is not None:
                for (config_key, (cp_key, cp_value)) in matches.items():
                    value = section_values.get(cp_key)
                    if value is not None:
                        connectionpolicy_kv[config_key] = value
                    elif cp_value is not None:
                        connectionpolicy_kv[config_key] = cp_value

        return connectionpolicy_kv

    def kill_handler(self, signum, frame):
        # Logger().info("KILL_HANDLER = %s" % signum)
        if signum == signal.SIGUSR1:
            self.kill()
            raise BastionSignal()

    def check_handler(self, signum, frame):
        # Logger().info("CHECK_HANDLER = %s" % signum)
        if signum == signal.SIGUSR2:
            self.check_session_parameters = True
            raise BastionSignal()

    def kill(self):
        try:
            Logger().info(u"Closing a RDP/VNC connection")
            self.proxy_conx.close()
        except Exception:
            pass

    def check_hostname_in_subnet(self, host, subnet):
        try:
            host_ip = socket.getaddrinfo(host, None)[0][4][0]
            Logger().info("Resolve DNS Hostname %s -> %s" % (host,
                                                             host_ip))
        except Exception:
            return False
        return engine.is_device_in_subnet(host_ip, subnet)

    def update_session_parameters(self, current_time):
        params = self.engine.read_session_parameters()
        res = params.get("rt_display")
        Logger().debug("rt_display=%s" % res)
        if res:
            self.rtmanager.start(current_time)

        res = params.get("shadow_type")
        Logger().debug("shadow_type=%s" % res)
        if res:
            userdata = params.get("shadow_userdata")
            Logger().debug("sending rd_shadow_type=%s" % res)
            self.send_data({
                u'rd_shadow_type': res,
                u'rd_shadow_userdata': userdata
            })

    def parse_app(self, value):
        acc_name, sep, app_name = value.rpartition('@')
        if acc_name:
            acc, sep, dom = acc_name.rpartition('@')
            if sep:
                return acc, dom, app_name
        return acc_name, '', app_name

    def check_application(self, effective_target, flags, exe_or_file):
        kv = {
            u'auth_command_rail_exec_flags': flags,
            u'auth_command_rail_exec_original_exe_or_file': exe_or_file,
            u'auth_command_rail_exec_exec_result': '3',
            # RAIL_EXEC_E_NOT_IN_ALLOWLIST
            u'auth_command': 'rail_exec'
        }
        app_right_params = self.target_app_rights.get(exe_or_file)
        if app_right_params is not None:
            app_right, app_params = app_right_params
            kv = self._complete_app_infos(kv, app_right, app_params)
            return kv
        acc_name, dom_name, app_name = self.parse_app(exe_or_file)
        if not app_name or not acc_name:
            Logger().debug("check_application: Parsing failed")
            return kv
        app_rights = self.engine.get_proxy_user_rights(['RDP'], app_name)
        Logger().debug("check_application: app rights len = %s" %
                       len(app_rights))
        app_rights = self.engine.filter_app_rights(
            app_rights, acc_name, dom_name, app_name
        )
        app_params = None
        app_right = None
        Logger().debug("check_application: after filter app rights len = %s" %
                       len(app_rights))
        for ar in app_rights:
            if not self.engine.check_effective_target(ar, effective_target):
                Logger().debug("check_application: jump server not compatible")
                continue
            _status, _infos = self.engine.check_target(ar, self.pid, None)
            if _status != APPROVAL_ACCEPTED:
                Logger().debug("check_application: approval not accepted")
                continue
            _deconnection_time = _infos.get('deconnection_time')
            if (_deconnection_time != u"-"
                and _deconnection_time[0:4] < u"2034"):
                _tt = datetime.strptime(_deconnection_time,
                                        "%Y-%m-%d %H:%M:%S").timetuple()
                _timeclose = int(mktime(_tt))
                if _timeclose != self.shared.get('timeclose'):
                    Logger().debug("check_application: timeclose different")
                    self.engine.release_target(ar)
                    continue
            app_params = self.engine.get_app_params(ar, effective_target)
            if app_params is None:
                self.engine.release_target(ar)
                Logger().debug("check_application: Get app params failed")
                continue
            app_right = ar
            break
        if app_params is None:
            return kv
        self.target_app_rights[exe_or_file] = (app_right, app_params)
        kv = self._complete_app_infos(kv, app_right, app_params)
        return kv

    def _complete_app_infos(self, kv, app_right, app_params):
        app_login_info = self.engine.get_target_login_info(app_right)

        kv[u'auth_command_rail_exec_exe_or_file'] = app_params.program
        kv[u'auth_command_rail_exec_arguments'] = app_params.params or ''
        kv[u'auth_command_rail_exec_working_dir'] = app_params.workingdir
        kv[u'auth_command_rail_exec_exec_result'] = '0'  # RAIL_EXEC_S_OK
        kv[u'auth_command_rail_exec_account'] = ''
        kv[u'auth_command_rail_exec_password'] = ''
        if app_params.params is not None:
            if u'${USER}' in app_params.params:
                kv[u'auth_command_rail_exec_account'] = \
                    app_login_info.account_login
            if u'${PASSWORD}' in app_params.params:
                kv[u'auth_command_rail_exec_password'] = \
                    self.engine.get_target_password(app_right) \
                    or self.engine.get_primary_password(app_right) \
                    or ''
        return kv

    def _manage_pm(self):
        response = pm_request(self.engine, self.shared.get("pm_request"))
        self.shared["pm_request"] = u""
        self.send_data({'pm_response': json.dumps(response)})

    def _load_selector_banner(self):
        if not self._selector_banner:
            return

        banner_message = self._selector_banner.get("message", "")
        banner_type = self._selector_banner.get("type", "")
        banner_enable = self._selector_banner.get("enable", False)

        if banner_enable is True and banner_message:
            if banner_type == "alert":
                banner_type = 2
            elif banner_type == "warn":
                banner_type = 1
            else: # banner_type == "info"
                banner_type = 0

            data_to_send = {
                u"banner_message" : banner_message,
                u"banner_type" : banner_type
            }

            self.send_data(data_to_send)


# END CLASS - Sesman


# This little main permets to run the Sesman Server Alone for one connection
# if __name__ == u'__main__':
#    sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#    sck.bind(('', 3350))
#    sck.listen(100)
#    connection, address = sck.accept()

#    Sesman(connection, address)

# EOF
