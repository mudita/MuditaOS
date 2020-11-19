# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

endpoint = {
    "deviceInfo": 1,
    "update": 2,
    "backup": 3,
    "restore": 4,
    "factory": 5,
    "contacts": 6,
    "messages": 7,
    "calllog": 8,
    "developerMode": 9
}

method = {
    "get": 1,
    "post": 2,
    "put": 3,
    "del": 4
}

status = {
    "OK": 200,
    "BadRequest": 400,
    "InternalServerError": 500
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
    "*": ord('*')

}
