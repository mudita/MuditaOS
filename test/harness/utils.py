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
    "A": "2",
    "B": "22",
    "C": "222",
    "D": "3",
    "E": "33",
    "F": "333",
    "G": "4",
    "H": "44",
    "I": "444",
    "J": "5",
    "K": "55",
    "L": "555",
    "M": "6",
    "N": "66",
    "O": "666",
    "P": "7",
    "Q": "77",
    "R": "777",
    "S": "7777",
    "T": "8",
    "U": "88",
    "V": "888",
    "W": "9",
    "X": "99",
    "Y": "999",
    "Z": "9999",
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
    for key in keypath:
        connection.send_key_code(key_codes[key])
        time.sleep(0.3)


last_char = '\0'


def send_char(char: str, connection):
    global last_char
    key_type = Keytype.short_press
    if char.isdigit():
        key_type = Keytype.long_press
        if last_char is char:
            print("repeated key!")
            connection.send_key_code(key_codes["right"])
        connection.send_key_code(int(char), key_type)
        connection.send_key_code(key_codes["right"])
        last_char = char

    else:
        if last_char is keymap[char][0]:
            print("repeated key!")
            connection.send_key_code(key_codes["right"], key_type)
        for key in keymap[char]:
            connection.send_key_code(int(key), key_type)
        last_char = keymap[char][0]


def send_number(number: str, connection):
    if number.isnumeric():
        for digit in number:
            connection.send_key_code(int(digit))
            time.sleep(0.3)

