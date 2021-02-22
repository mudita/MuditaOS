# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness import log


# @pytest.mark.skip("not working ;/")
@pytest.mark.rt1051
# @pytest.mark.usefixtures("phone_unlocked")
def test_send_AT(harness):
    log.info("LOL")
    ret = harness.connection.send_at("AT")
    log.info(ret)
    assert "OK" in ret
