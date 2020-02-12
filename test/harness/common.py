#!/bin/python3
# common elements for super simple test harness

import serial
import json
import time

import logging
log = logging.getLogger(__name__)

CMD_KEY = 0
CMD_GSM = 3
CMD_GPIO = 4
STX = b'\x02'
CTX = b'\x03'


class Serial:

    def __init__(self, uart='/dev/ttyACM0'):
        log.debug('start harness on {}'.format(uart))
        self.ser = None
        try:
            self.ser = serial.Serial(uart, baudrate=115200)
        except (FileNotFoundError, serial.serialutil.SerialException) as err:
            log.error("uart {} not found".format(uart))
            exit(1)

        import signal
        import sys

        def signal_handler(sig, frame):
                log.info('You pressed Ctrl+C!')
                self.ser.close()
                sys.exit(0)
        signal.signal(signal.SIGINT, signal_handler)

    def __del__(self):
        if self.ser:
            self.ser.close()
        log.info("test end")

    def get(self):
        return self.ser

    def sleep(self, sec: float):
        time.sleep(sec)

    def write(self, code: int, val, t=1):
        '''
        writes data on serial in frame needed for harness
        '''
        size = self.ser.write(STX)
        data = {
            "t": code,
            "d": val
        }
        size += self.ser.write(json.dumps(data).encode())
        log.info("-> write: {}".format(json.dumps(data)))
        size += self.ser.write(CTX)
        if size != 2 + len(json.dumps(data).encode()):
            log.error("-> error write bad size! {}".format(size))
        else:
            log.info("-> success")

    def read(self, val) -> str:
        buff = bytearray()
        start = time.time()
        while 1:
            ch = self.ser.read()
            if ch == STX:
                buff = bytearray()
            elif ch == CTX:
                return buff.decode()
            else:
                buff += ch
        return ""

    def key(self, val, wait=0.5):
        '''
        write key on keyboard
        param wait - delay after key press
        '''
        self.write(CMD_KEY, [val])
        if wait > 0:
            self.sleep(wait)

    def gsm(self, val):
        '''
        write GSM command i.e. `AT?`
        '''
        self.write(CMD_GSM, val)

    def gpio(self, val):
        '''
        GPIO phone request
        '''
        self.write(CMD_GPIO, val)
