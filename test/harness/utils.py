# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time

from harness.interface.defs import key_codes

# assuming that the harness is actually in the menu
application_keypath = {
    "phone": [
        "enter"
    ],
    "contacts": [
        "right",
        "enter"
    ],
    "messages": [
        "right",
        "right",
        "enter"
    ],
    "music": [
        "down",
        "enter"
    ],
    "meditation": [
        "down",
        "right",
        "enter"
    ],
    "settings": [
        "down",
        "right",
        "right",
        "enter"
    ],
    "tools": [
        "up",
        "enter"
    ],
    "alarm": [
        "up",
        "right",
        "enter"
    ],
    "calendar": [
        "up",
        "left",
        "left",
        "enter"
    ]
}


def send_keystoke(keypath, connection):
    for i in keypath:
        print(i)
        connection.send_key(key_codes[i])
        time.sleep(0.3)
