# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time
import pytest

from test_call import get_calllog_count, test_call as call
from test_call_back import test_call as call_back


@pytest.fixture(scope='function')
def two_phones_available(harnesses):
    assert len(harnesses) == 2

@pytest.mark.rt1051
@pytest.mark.usefixtures("phones_unlocked")
@pytest.mark.usefixtures("two_phones_available")
def test_call_number(harnesses, phone_number, call_duration):
    role_calling = harnesses[1]
    role_receiving = harnesses[0]

    reciving_calllog_count = get_calllog_count(role_receiving)

    call(role_calling, phone_number, call_duration)  # asserts calling calllog count

    time.sleep(1)  # let the db transactions settle…
    assert (get_calllog_count(role_receiving) == reciving_calllog_count + 1)


@pytest.mark.rt1051
@pytest.mark.usefixtures("phones_unlocked")
@pytest.mark.usefixtures("two_phones_available")
def test_call_back(harnesses, phone_number, call_duration):
    role_calling = harnesses[0]
    role_receiving = harnesses[1]

    reciving_calllog_count = get_calllog_count(role_receiving)

    call_back(role_calling, call_duration)

    time.sleep(1)  # let the db transactions settle…
    assert (get_calllog_count(role_receiving) == reciving_calllog_count + 1)
