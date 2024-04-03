/*
SPDX-FileCopyrightText: 2024 Wallix Proxies Team
SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "test_only/test_framework/redemption_unit_tests.hpp"

#include "utils/error_message_ctx.hpp"
#include "utils/trkeys.hpp"

RED_AUTO_TEST_CASE(TestErrMsgCtx)
{
    ErrorMessageCtx ctx;
    RED_CHECK(ctx.get_msg() == "");
    RED_CHECK(ctx.is_translated());

    ctx.set_msg(trkeys::err_transport_tls_certificate_changed);
    RED_CHECK(ctx.get_msg() == trkeys::err_transport_tls_certificate_changed.translation);
    RED_CHECK(!ctx.is_translated());

    ctx.set_msg("bla bla"_zv);
    RED_CHECK(ctx.get_msg() == "bla bla");
    RED_CHECK(ctx.is_translated());

    ctx.clear();
    RED_CHECK(ctx.get_msg() == "");
    RED_CHECK(ctx.is_translated());

    ctx.set_msg("bla bla"_zv);
    RED_CHECK(ctx.get_msg() == "bla bla");
    RED_CHECK(ctx.is_translated());

    ctx.set_msg(trkeys::err_transport_tls_certificate_changed);
    RED_CHECK(ctx.get_msg() == trkeys::err_transport_tls_certificate_changed.translation);
    RED_CHECK(!ctx.is_translated());
}
