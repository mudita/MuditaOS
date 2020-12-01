# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest


@pytest.mark.skip("not working ;/")
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_send_AT(harness):
    ret = harness.connection.send_at("AT")
    print(ret)
    assert "OK" in ret
