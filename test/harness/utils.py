# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time
from enum import Enum

from harness.interface.CDCSerial import Keytype
from harness.interface.defs import key_codes


# assuming that the harness is actually in the menu
application_keypath = {
    "calllog": [
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

keymap = {
    "a": "2",
    "b": "22",
    "c": "222",
    "d": "3",
    "e": "33",
    "f": "333",
    "g": "4",
    "h": "44",
    "i": "444",
    "j": "5",
    "k": "55",
    "l": "555",
    "m": "6",
    "n": "66",
    "o": "666",
    "p": "7",
    "q": "77",
    "r": "777",
    "s": "7777",
    "t": "8",
    "u": "88",
    "v": "888",
    "w": "9",
    "x": "99",
    "y": "999",
    "z": "9999",
    " ": "0",
    ".": "1",
    ",": "11",
    "_": "111",
    ":": "1111",
    ";": "11111",
    ")": "111111",
    "(": "1111111",
    "?": "11111111",
    "!": "111111111",
    "#": "1111111111",
    "/": "11111111111",
    "*": "111111111111",
    "+": "1111111111111",
    "del": key_codes["#"],
    "caps": key_codes["*"],
}


def send_keystoke(keypath, connection):
    for i in keypath:
        connection.send_key(key_codes[i])
        time.sleep(0.3)


last_char = '\0'


def send_char(char: str, connection):
    global last_char
    key_type = Keytype.short_press
    if char.isdigit():
        key_type = Keytype.long_press
        if last_char is char:
            print("repeated key!")
            connection.send_key(key_codes["right"])
        connection.send_key(int(char), key_type)
        connection.send_key(key_codes["right"])
        last_char = char

    else:
        if last_char is keymap[char][0]:
            print("repeated key!")
            connection.send_key(key_codes["right"], key_type)
        for key in keymap[char]:
            connection.send_key(int(key), key_type)
        last_char = keymap[char][0]


def send_number(number: str, connection):
    if number.isnumeric():
        for digit in number:
            connection.send_key(int(digit))
            time.sleep(0.3)

