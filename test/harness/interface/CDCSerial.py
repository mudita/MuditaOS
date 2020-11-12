# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time

import serial
import json
import random
import logging

from harness.interface.defs import endpoint, method, status

log = logging.getLogger(__name__)


class CDCSerial:
    def __init__(self, port_name):
        self.body = ""
        try:
            self.serial = serial.Serial(port_name, baudrate=115200, timeout=10)
            self.serial.flushInput()
            log.info("port opened!")
        except (FileNotFoundError, serial.serialutil.SerialException) as err:
            log.error("uart {} not found".format(port_name))
            exit(1)

    def __del__(self):
        self.serial.close()

    def __wrap_message(self, body):
        msg = {
            "endpoint": endpoint["developerMode"],
            "method": method["put"],
            "uuid": 0,
            "body": body
        }
        return msg

    def get_serial(self):
        return self.serial

    def __build_message(self, json_data):
        json_dump = json.dumps(json_data)
        return "#%09d%s" % (len(json_dump), json_dump)

    def write(self, msg, timeout=10):
        message = self.__build_message(msg)
        self.serial.write(message.encode())

        header = self.serial.read(timeout).decode()
        payload_length = int(header[1:])
        result = self.serial.read(payload_length).decode()
        return json.loads(result)

    def send_key(self, key_code, wait=10):
        body = {
            "keyPressed": key_code
        }

        ret = self.write(self.__wrap_message(body), wait)
        return ret

    def send_at(self, at_command, wait=10):
        body = {
            "AT": at_command + "\r"
        }

        ret = self.write(self.__wrap_message(body), wait)
        return ret["body"]["ATResponse"]

    def get_window(self):
        body = {
            "focus": True
        }

        ret = self.write(self.__wrap_message(body))
        return ret["body"]["focus"]

    def is_phone_locked(self):
        body = {
            "isLocked": True
        }

        ret = self.write(self.__wrap_message(body))
        return ret["body"]["isLocked"]
