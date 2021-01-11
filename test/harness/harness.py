# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
from enum import Enum

from harness import utils, log
from harness.interface import CDCSerial as serial
from harness.interface.defs import key_codes, endpoint, method
from harness.utils import send_keystoke, application_keypath, send_char
from harness.interface.error import TestError, Error
import random


class Harness:
    connection = None
    is_phone_unlocked = False
    is_echo_mode = False
    port_name = ''

    def __init__(self, port):
        self.port_name = port
        self.connection = serial.CDCSerial(port)

    @classmethod
    def from_detect(cls):
        '''
        Try to instantiate from first detected device.
        Do not use this method if you need >1 unique devices.
        '''
        found = serial.CDCSerial.find_Pures()
        if found:
            port = found[0]
            return cls(port)
        else:
            raise TestError(Error.PORT_NOT_FOUND)

    def get_connection(self):
        return self.connection

    def get_window_name(self):
        return self.connection.get_window_name()

    def unlock_phone(self):
        if self.connection.is_phone_locked():
            self.connection.send_key_code(key_codes["enter"])
            self.connection.send_key_code(key_codes["#"])
            self.connection.send_key_code(3)
            self.connection.send_key_code(3)
            self.connection.send_key_code(3)
            self.connection.send_key_code(3)
            log.info("Phone unlocked")
        else:
            log.info("Phone already unlocked")
        self.is_phone_unlocked = True

    def with_phone_unlocked(self, func):
        if not self.is_phone_unlocked:
            self.unlock_phone()

        func(self.connection)

    def connection_echo_mode_on(self):
        if self.connection.enable_echo_mode():
            self.is_echo_mode = True

    def connection_echo_mode_off(self):
        if self.connection.disable_echo_mode():
            self.is_echo_mode = False

    def open_application(self, app):
        send_keystoke(application_keypath[app], self.connection)

    def send_text(self, text: str):
        for letter in text:
            send_char(letter, self.connection)

    def send_number(self, number: str):
        utils.send_number(number, self.connection)

    def endpoint_request(self, ep_name: str, met: str, body: dict) -> dict:
        ret = self.connection.write({
            "endpoint": endpoint[ep_name],
            "method": method[met],
            "uuid": random.randint(1, 32000),
            "body": body
        })
        return ret

