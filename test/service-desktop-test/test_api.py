# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


import json
import random


class Test:
    def __init__(self, serial, payload, expected_result):
        self.result = ""
        self.serial = serial
        self.message = build_message(payload)
        self.expected_result = expected_result

    def execute(self):
        self.serial.write(self.message.encode())
        header = self.serial.read(10).decode()
        payload_length = int(header[1:])
        result = self.serial.read(payload_length).decode()
        # print("result full:" + result)
        self.result = json.loads(result)
        return self.result == self.expected_result, self.result["body"]


def build_message(json_data):
    json_dump = json.dumps(json_data)
    return "#%09d%s" % (len(json_dump), json_dump)


def prepare_message(endpoint, method, status, msg_body, result_body={}):
    uuid = random.randint(1, 32768)
    msg = {
        "endpoint": endpoint,
        "method": method,
        "uuid": uuid,
        "body": msg_body
    }
    msg_result = {
        "endpoint": endpoint,
        "status": status,
        "uuid": str(uuid),
        "body": result_body
    }
    return msg, msg_result
