# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status, key_codes
from harness.interface.CDCSerial import Keytype
from harness import log
import time
import random


def change_auto_lock_timer(harness, value: str):
    body = {"changeAutoLockTimeout": value}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

@pytest.fixture(scope='function')
def phone_ends_with_default_auto_lock(harness):
    yield
    timeout = str(180)
    log.info("Setting back default timeout to {}ms".format(timeout))
    change_auto_lock_timer(harness, timeout)

app_navigation_to_name_mapping = {
    "messages": "ApplicationMessages",
    "calllog": "ApplicationCallLog",
    "contacts": "ApplicationPhonebook",
    "music": "ApplicationMusicPlayer",
    #    "meditation": "ApplicationMeditation",
    #    "settings": "ApplicationSettings",
    "tools": "ApplicationDesktop",  # other MenuPage
    "alarm": "ApplicationAlarmClock",
    "calendar": "ApplicationCalendar"
}


def enter_application(harness, nav_name, app_name):
    harness.connection.send_key_code(key_codes["enter"])
    harness.open_application(nav_name)
    if harness.get_application_name() != app_name:
        time.sleep(1)
        assert harness.get_application_name() == app_name


def open_random_application(harness):
    navigation, name = random.choice(list(app_navigation_to_name_mapping.items()))
    print(navigation)
    print(name)

    # enter_application(harness, navigation, name)
    enter_application(harness, "calendar", "ApplicationCalendar")
    return "ApplicationCalendar"
    # return name


def contains_value(body: dict, attr_name, attr_value):
    if attr_name in body:
        return body[attr_name] == attr_value


def contains_value_recursively(body: dict, attr_name, attr_value):
    if len(body) == 1:
        print(next(iter(body)))
        return contains_value_recursively(next(iter(body.values())), attr_name, attr_value)

    if contains_value(body, attr_name, attr_value):
        return True

    if 'Children' in body:
        for child in body['Children']:
            value_found = contains_value_recursively(child, attr_name, attr_value)
            if value_found:
                return True

    return False


def get_dom(harness):
    body = {"ui": True, "getWindow": True}
    result = harness.endpoint_request("developerMode", "get", body)
    # log.info("data {}".format(find_item_depth_first(result['body']['dom'], 'StatusBar')))
    assert result['body']
    assert 'Window' in result['body']['dom']
    return result

@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_ends_test_in_desktop")
@pytest.mark.usefixtures("phone_in_desktop")
@pytest.mark.usefixtures("phone_ends_with_default_auto_lock")
@pytest.mark.usefixtures("phone_unlocked")
def test_auto_lock(harness):
    # change timer lock value
    change_auto_lock_timer(harness, str(5))
    assert harness.get_application_name() == "ApplicationDesktop"

    time.sleep(6)
    assert harness.connection.is_phone_locked() is True
    harness.unlock_phone()
    app = open_random_application(harness)
    # we should go back to desktop an be locked after 6 seconds
    time.sleep(6)
    assert harness.connection.is_phone_locked() is True
    harness.unlock_phone()
    # we should go back to previously chosen application
    assert harness.get_application_name() == app


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_ends_test_in_desktop")
@pytest.mark.usefixtures("phone_ends_with_default_auto_lock")
@pytest.mark.usefixtures("phone_unlocked")
def test_no_auto_lock_for_meditation_app(harness):
    # change timer lock value
    change_auto_lock_timer(harness, str(5))
    assert harness.get_application_name() == "ApplicationDesktop"

    time.sleep(6)
    assert harness.connection.is_phone_locked() is True
    harness.unlock_phone()
    enter_application(harness, "meditation", "ApplicationMeditation")
    # we should go back to desktop an be locked after 4 seconds
    time.sleep(6)
    assert harness.connection.is_phone_locked() is False

    # we should go back to previously chosen application
    assert harness.get_application_name() == "ApplicationMeditation"

    # go back to main screen
    harness.connection.send_key_code(key_codes["fnRight"], Keytype.long_press)
