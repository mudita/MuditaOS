# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness import log


@pytest.mark.xfail
@pytest.mark.usb_cdc_echo
@pytest.mark.skip("not working ;/")
def test_echo_enable(harness, message_to_echo="Testing that echo mode on USB-CDC on Mudita Pure works!"):
    harness.connection_echo_mode_on()

    if not harness.is_echo_mode:
        assert False, "Failed to enable Echo mode"

    # Send the message
    harness.connection.serial.write(message_to_echo.encode())

    # Read back the message
    result = harness.connection.serial.read(int(len(message_to_echo))).decode()

    harness.connection_echo_mode_off()

    assert message_to_echo == result, "Echo mode test failed"
