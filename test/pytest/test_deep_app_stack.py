# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness import log
from harness.interface.defs import Application


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
def test_call(harness):
    stack = harness.get_application_stack()
    assert stack == [Application.Desktop]

    harness.start_application(Application.Calendar)
    time.sleep(1)
    harness.start_application(Application.AlarmClock)
    time.sleep(1)
    harness.start_application(Application.Messages)
    time.sleep(1)
    harness.start_application(Application.Phonebook)
    time.sleep(1)
    harness.start_application(Application.Meditation)
    time.sleep(1)

    stack = harness.get_application_stack()
    assert stack == [
            Application.Meditation,
            Application.Phonebook,
            Application.Messages,
            Application.AlarmClock,
            Application.Calendar,
            Application.Desktop
            ]

    harness.switch_back_application()
    time.sleep(1)
    harness.switch_back_application()
    time.sleep(1)
    harness.switch_back_application()
    time.sleep(1)
    harness.switch_back_application()
    time.sleep(1)
    harness.switch_back_application()
    time.sleep(1)

    stack = harness.get_application_stack()
    assert stack == [Application.Desktop]

