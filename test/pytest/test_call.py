# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from module_apps.call_utils import get_calllog_count, select_call_button

@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_ends_test_in_desktop")
@pytest.mark.usefixtures("phone_unlocked")
def test_call(harness, phone_number, call_duration):
    count_before = get_calllog_count(harness)
    # enter number
    harness.send_number(str(phone_number))
    # call
    select_call_button(harness, call_duration)
    count_after = get_calllog_count(harness)

    assert count_before + 1 == count_after
    time.sleep(2)  # needed to restore after call
