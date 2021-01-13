# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time

import pytest

import sys
import os.path

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)))

from harness import log
from harness.harness import Harness
from harness.interface.error import TestError, Error
from harness.interface.CDCSerial import CDCSerial as serial

simulator_port = 'simulator'


def pytest_addoption(parser):
    parser.addoption("--port", type=str, action="store", required=False)
    parser.addoption("--timeout", type=int, action="store", default=15)
    parser.addoption("--phone_number", type=int, action="store")
    parser.addoption("--call_duration", type=int, action="store", default=30)
    parser.addoption("--sms_text", type=str, action="store", default='')


@pytest.fixture(scope='session')
def phone_number(request):
    phone_number = request.config.option.phone_number
    assert phone_number
    return phone_number


@pytest.fixture(scope='session')
def call_duration(request):
    call_duration = request.config.option.call_duration
    assert call_duration
    return call_duration


@pytest.fixture(scope='session')
def sms_text(request):
    sms_text = request.config.option.sms_text
    assert sms_text != ''
    return sms_text


@pytest.fixture(scope='session')
def harness(request):
    '''
    Try to init one Pure phone with serial port path or automatically
    '''
    port_name = request.config.option.port

    RETRY_EVERY_SECONDS = 0.7
    retries = request.config.option.timeout / RETRY_EVERY_SECONDS

    if port_name is None:
        log.warning("no port provided! trying automatic detection")
        try:
            harness = Harness.from_detect()
        except TestError as e:
            if e.get_error_code() == Error.PORT_NOT_FOUND:
                pytest.exit("couldn't find any viable port. exiting")
            else:
                raise(e)
    else:
        assert '/dev' in port_name or simulator_port in port_name

        if simulator_port in port_name:
            while retries > 0:
                try:
                    file = open("/tmp/purephone_pts_name", "r")
                    break
                except FileNotFoundError as err:
                    time.sleep(RETRY_EVERY_SECONDS)
                    retries -= 1
                    log.info("waiting for simulator port...")
            else:
                raise TestError(Error.PORT_FILE_NOT_FOUND)

            port_name = file.readline()
            if port_name.isascii():
                log.debug("found {} entry!".format(port_name))
            else:
                pytest.exit("not a valid sim pts entry!")

        harness = Harness(port_name)
    return harness


@pytest.fixture(scope='session')
def harnesses():
    '''
    Automatically init at least two Pure phones
    '''
    found_pures = serial.find_Pures()
    harnesses = [Harness(pure) for pure in found_pures]
    if not len(harnesses) >= 2:
        pytest.skip("At least two phones are needed for this test")
    assert len(harnesses) >= 2
    return harnesses

@pytest.fixture(scope='session')
def phone_unlocked(harness):
    harness.unlock_phone()
    assert harness.is_phone_unlocked

@pytest.fixture(scope='session')
def phone_unlocked(harnesses):
    for harness in harnesses:
        harness.unlock_phone()
        assert harness.is_phone_unlocked

def pytest_configure(config):
    config.addinivalue_line("markers",
                            "service_desktop_test: mark test if it's related to service-desktop API")
    config.addinivalue_line("markers",
                            "rt1051: mark test if it's target only (eg. calls, messages)")
    config.addinivalue_line("markers",
                            "usb_cdc_echo: mark test if it's intended for usb-cdc echo mode")
