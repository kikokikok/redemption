#!/usr/bin/python -O
# -*- coding: iso-8859-1 -*-
#
# Copyright (c) 2021 WALLIX, SAS. All rights reserved.
#
# Licensed computer software. Property of WALLIX.
# Author(s) : Meng Tan <dev@wallix.com>


import re

from logger import Logger
from typing import Optional, Tuple, Dict


AccoutName = str
DomainName = str
DeviceName = str
AccountResult = Tuple[AccoutName, DomainName, Optional[DeviceName]]


def parse_param(param: str, forced_device: Optional[str] = None) -> Optional[AccountResult]:
    """
    Extract account representation

    string format is <account_name>@<domain_name>[@<device_name>]

    If @<device_name> is absent, <domain_name> is a global domain name
    Else <domain_name> is a local domain name of the forced_device

    if 'forced_device' is not None
    <device_name> should be empty or equal to forced_device
    Else <device_name> can be another device

    """
    parsed = param.rsplit("@", 2)
    if len(parsed) > 1:
        account_name = parsed[0]
        domain_name = parsed[1]
        if (len(parsed) == 3
            and parsed[2]
            and forced_device
            and (parsed[2] != forced_device)):
            return None
        device_name = (forced_device or parsed[2]) if len(parsed) == 3 else None
        return account_name, domain_name, device_name
    else:
        return None


def replace_token(param: str, replace_dict: Dict[str, str]) -> str:
    """
    token = match.group(0) # <..>
    field = match.group(1) # field
    """
    field_token = re.compile(r"\<(\w*)\>")
    matches = field_token.finditer(param)
    for res in matches:
        token = res.group(0)
        field = res.group(1)
        param = param.replace(token, replace_dict.get(field.lower(), ""))
    return param


def parse_account(param: Optional[str], replace_dict: Dict[str, str], force_device: bool = False) -> Optional[AccountResult]:
    if param is None:
        return None
    param = replace_token(param, replace_dict)
    device_name = replace_dict.get("device") if force_device else None
    return parse_param(param, device_name)


def resolve_scenario_account(enginei, param: str, force_device: bool = True):
    """
    Get password or login field from scenario account

    :param enginei: engine interface
        must implement:
        - get_target_login_info
        - get_username
        - get_account_infos_by_type
    :param param: str
        string representing scenario account:
        string format is account_name@domain_name[@device_name]
        each part can contain placeholder <attribute>
    :param force_device: bool
        force device_name part in param to match current device
        True by default

    :return: requested account infos of scenario account
             if param is not a scenario account, returns None
    :rtype: AccountInfos
    """
    session_infos = enginei.get_target_login_info().get_target_dict()
    session_infos["user"] = enginei.get_username()
    acc_tuple = parse_account(param, session_infos, force_device)
    if acc_tuple is None:
        return None
    acc_infos = enginei.get_account_infos_by_type(
        *acc_tuple,
        with_ssh_key=True,
        account_type="scenario"
    )
    if acc_infos is None:
        Logger().debug(
            f"Error: Unable to retrieve account info from '{param}'"
        )
    return acc_infos
