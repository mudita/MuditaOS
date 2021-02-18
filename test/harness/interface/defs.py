# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
from enum import Enum

endpoint = {
    "deviceInfo": 1,
    "update": 2,
    "filesystemUpload": 3,
    "backup": 4,
    "restore": 5,
    "factory": 6,
    "contacts": 7,
    "messages": 8,
    "calllog": 9,
    "events": 10,
    "developerMode": 11,
    "bluetooth":12
}

method = {
    "get": 1,
    "post": 2,
    "put": 3,
    "del": 4
}

status = {
    "OK": 200,
    "Accepted": 202,
    "SeeOther": 303,
    "BadRequest": 400,
    "Forbidden": 403,
    "NotFound": 404,
    "NotAcceptable": 406,
    "InternalServerError": 500,
}

key_codes = {
    "left": ord('a'),
    "right": ord('d'),
    "up": ord('w'),
    "down": ord('s'),
    "enter": ord('\n'),
    "fnLeft": 11,
    "fnRight": 12,
    "volUp": 13,
    "volDown": 14,
    "torch": 15,
    "sliderUp": 16,
    "sliderMid": 18,
    "sliderDown": 17,
    "#": ord('#'),
    "*": ord('*'),
}


class SMSType(Enum):
    DRAFT = 0x01
    FAILED = 0x02
    INBOX = 0x04
    OUTBOX = 0x08
    QUEUED = 0x10
    INPUT = 0x12
    UNKNOWN = 0xFF
