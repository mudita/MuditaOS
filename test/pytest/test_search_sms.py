# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness.interface.defs import key_codes


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_search_sms(harness, sms_text, phone_number):
    body = {"category": "message", "messageBody": sms_text, "phoneNumber": str(phone_number)}
    messages = harness.endpoint_request("messages", "get", body)["body"]
    assert len(messages) != 0
