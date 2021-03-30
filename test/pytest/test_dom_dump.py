# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness import log

def test_get_dom(harness):
    body = {"ui": True, "getWindow": True}
    result = harness.endpoint_request("developerMode", "get", body)
    #log.info("data {}".format(result))
    assert result['body']
    assert 'Window' in result['body']['dom']
