# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness import log


@pytest.mark.rt1051
def test_send_AT(harness):
    ret = harness.connection.send_at("AT", 1000)
    log.info(ret)
    assert "OK" in ret
