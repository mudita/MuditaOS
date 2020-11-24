# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
from enum import Enum

from harness import utils, log
from harness.interface import CDCSerial as serial
from harness.interface.defs import key_codes, endpoint, method
from harness.utils import send_keystoke, application_keypath, send_char
import random


class Harness:
    connection = None
    is_phone_unlocked = False

    def __init__(self, port='/dev/ttyACM2'):
        self.connection = serial.CDCSerial(port)

    def get_connection(self):
        return self.connection

    def unlock_phone(self):
        if self.connection.is_phone_locked():
            self.connection.send_key(key_codes["enter"])
            self.connection.send_key(key_codes["#"])
            self.connection.send_key(3)
            self.connection.send_key(3)
            self.connection.send_key(3)
            self.connection.send_key(3)
            log.info("Phone unlocked")
        else:
            log.info("Phone already unlocked")

    def with_phone_unlocked(self, func):
        if not self.is_phone_unlocked:
            self.unlock_phone()
            self.is_phone_unlocked = True

        func(self.connection)

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
        return ret["body"]
