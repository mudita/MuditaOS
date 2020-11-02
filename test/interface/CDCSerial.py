# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time

import serial
import json
import random
import logging
from interface.defs import *

log = logging.getLogger(__name__)


class CDCSerial:
    def __init__(self, port_name='/dev/ttyACM2'):
        try:
            self.serial = serial.Serial(port_name, baudrate=115200, timeout=10)
            self.serial.flushInput()
            log.info("port opened!")
        except (FileNotFoundError, serial.serialutil.SerialException) as err:
            log.error("uart {} not found".format(port_name))
            exit(1)

    def __del__(self):
        self.serial.close()

    def get_serial(self):
        return self.serial

    def __build_message(self, json_data):
        json_dump = json.dumps(json_data)
        return "#%09d%s" % (len(json_dump), json_dump)

    def write(self, msg):
        message = self.__build_message(msg)
        self.serial.write(message.encode())

        header = self.serial.read(10).decode()
        payload_length = int(header[1:])
        result = self.serial.read(payload_length).decode()
        return json.loads(result)

    def send_key(self, key_code, wait=0):
        uuid = random.randint(1, 32768)

        msg_body = {
            "keyPressed": key_code
        }

        msg = {
            "endpoint": endpoint["developerMode"],
            "method": method["put"],
            "uuid": uuid,
            "body": msg_body
        }

        msg_result = {
            "endpoint": endpoint["developerMode"],
            "status": status["OK"],
            "uuid": str(uuid),
            "body": None
        }
        ret = self.write(msg)
        if wait > 0:
            time.sleep(wait)

        return ret == msg_result


