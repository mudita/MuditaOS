# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness.interface.defs import key_codes


def get_calllog_count(harness):
    body = {"count": True}
    return harness.endpoint_request("calllog", "get", body)["body"]["count"]


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_in_desktop")
@pytest.mark.usefixtures("phone_unlocked")
def test_call(harness, call_duration):
    count_before = get_calllog_count(harness)

    # enter menu
    harness.connection.send_key_code(key_codes["enter"])
    harness.open_application("calllog")
    if harness.connection.get_application_name() != "ApplicationCallLog":
        time.sleep(2)
        assert harness.connection.get_application_name() == "ApplicationCallLog"

    # call
    harness.connection.send_key_code(key_codes["fnLeft"])
    time.sleep(call_duration)
    # hang up
    harness.connection.send_key_code(key_codes["fnRight"])
    count_after = get_calllog_count(harness)

    for _ in range(3):
        harness.connection.send_key_code(key_codes["fnRight"])
        time.sleep(1)

    assert count_before + 1 == count_after
    time.sleep(2)  # needed to restore after call
