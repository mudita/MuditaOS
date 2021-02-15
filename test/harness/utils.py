# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

special_chars_keymap = {
    U'.': "",
    U',': "d",
    U'\'': "dd",
    U'?': "ddd",
    U'!': "dddd",
    U'"': "ddddd",
    U'-': "dddddd",
    U'(': "s",
    U')': "sd",
    U'@': "sdd",
    U'/': "sddd",
    U':': "sdddd",
    U'_': "sddddd",
    U';': "sdddddd",
    U'␤': "ss",
    U'+': "ssd",
    U'&': "ssdd",
    U'%': "ssddd",
    U'*': "ssdddd",
    U'<': "ssddddd",
    U'>': "ssdddddd",
    U'=': "sss",
    U'£': "sssd",
    U'€': "sssdd",
    U'$': "sssddd",
    U'[': "sssdddd",
    U']': "sssddddd",
    U'{': "sssdddddd",
    U'}': "ssss",
    U'\'': "ssssd",
    U'^': "ssssdd",
    U'~': "ssssddd",
    U'`': "ssssdddd",
    U'į': "ssssddddd",
    U'§': "ssssdddddd",
    U'…': "sssss",
    U'#': "sssssd",
    U'|': "sssssdd",
    U'÷': "sssssddd",
    U'·': "sssssdddd",
    U'°': "sssssddddd",
    U'¿': "sssssdddddd",
    U'¡': "ssssss",
    U'ą': "ssssssd",
    U'à': "ssssssdd",
    U'á': "ssssssddd",
    U'ä': "ssssssdddd",
    U'â': "ssssssddddd",
    U'ć': "ssssssdddddd",
    U'ç': "sssssss",
    U'ę': "sssssssd",
    U'é': "sssssssdd",
    U'è': "sssssssddd",
    U'ê': "sssssssdddd",
    U'ë': "sssssssddddd",
    U'î': "sssssssdddddd",
    U'ï': "ssssssss",
    U'í': "ssssssssd",
    U'ł': "ssssssssdd",
    U'ń': "ssssssssddd",
    U'ñ': "ssssssssdddd",
    U'ó': "ssssssssddddd",
    U'ô': "ssssssssdddddd",
    U'ö': "sssssssss",
    U'ś': "sssssssssd",
    U'û': "sssssssssdd",
    U'ú': "sssssssssddd",
    U'ù': "sssssssssdddd",
    U'ü': "sssssssssddddd",
    U'ÿ': "sssssssssdddddd",
    U'ż': "ssssssssss",
    U'ź': "ssssssssssd",
    U'ß': "ssssssssssdd"
}

emojis_keymap = {
    U'😁': "",
    U'😂': "d",
    U'😃': "dd",
    U'😄': "ddd",
    U'😅': "dddd",
    U'😆': "ddddd",
    U'😉': "dddddd",
    U'😊': "s",
    U'😋': "sd",
    U'😌': "sdd",
    U'😍': "sddd",
    U'😏': "sdddd",
    U'😒': "sddddd",
    U'😓': "sdddddd",
    U'😔': "ss",
    U'😖': "ssd",
    U'😘': "ssdd",
    U'😚': "ssddd",
    U'😜': "ssdddd",
    U'😝': "ssddddd",
    U'😼': "ssdddddd"
}

def send_keystoke(keypath, connection):
    for key in keypath:
        connection.send_key_code(key_codes[key])
        time.sleep(0.3)


last_char = '\0'


def clear_last_char():
    global last_char
    last_char = '\0'


def send_char(char: str, connection):
    global last_char
    key_type = Keytype.short_press
    if char.isascii():
        if char.isdigit():
            key_type = Keytype.long_press
            if last_char is char:
                print("repeated key!")
                connection.send_key_code(key_codes["right"])
            connection.send_key_code(int(char), key_type)
            connection.send_key_code(key_codes["right"])
            last_char = char
        elif char.islower():
            tmp = char.upper()
            # toggle to lowercase mode
            connection.send_key_code(key_codes["*"], key_type)
            if last_char is keymap[tmp][0]:
                print("repeated key!")
                connection.send_key_code(key_codes["right"], key_type)
            for key in keymap[tmp]:
                connection.send_key_code(int(key), key_type)
            last_char = keymap[tmp][0]
            # toggle to uppercase mode
            connection.send_key_code(key_codes["*"], key_type)
            connection.send_key_code(key_codes["*"], key_type)
        else:
            if last_char is keymap[char][0]:
                print("repeated key!")
                connection.send_key_code(key_codes["right"], key_type)
            for key in keymap[char]:
                connection.send_key_code(int(key), key_type)
            last_char = keymap[char][0]
    else:
        connection.send_key_code(key_codes["*"], Keytype.long_press)
        if char in special_chars_keymap:
            for key in special_chars_keymap[char]:
                connection.send_key_code(ord(key), key_type)
        elif char in emojis_keymap:
            connection.send_key_code(key_codes["fnLeft"], Keytype.short_press)
            for key in emojis_keymap[char]:
                connection.send_key_code(ord(key), key_type)
        else:
            print("Not supported char {} !!!".format(char))
        connection.send_key_code(key_codes["enter"], key_type)


def send_number(number: str, connection):
    if number.isnumeric():
        for digit in number:
            connection.send_key_code(int(digit))
            time.sleep(0.3)


### timeout from https://stackoverflow.com/a/601168/5752094

from contextlib import contextmanager

import signal


class Timeout(Exception):
    '''
    usage:
        try:
            with Timeout.limit(10):
                long_function_call()
        except Timeout as e:
            print("Timed out!")
    '''

    @classmethod
    @contextmanager
    def limit(cls, seconds: int):
        assert seconds >= 1, "Timeout must be at least 1 second !"
        def signal_handler(signum, frame):
            raise Timeout("Timed out!")

        signal.signal(signal.SIGALRM, signal_handler)
        signal.alarm(seconds)
        try:
            yield
        finally:
            signal.alarm(0)
