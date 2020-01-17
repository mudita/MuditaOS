#!/bin/python3
# common elements for super simple test harness

import serial
import json
import time



def read_cmd(ser):
    buff = bytearray()
    while 1:
        ch = ser.read()
        if ch == b'\x02':
            buff = bytearray()
        elif ch == b'\x03':
            return str(buff)
        else:
            buff += ch

def writeKey(ser, val, t):
    ser.write(b'\x02')
    ser.write('{{"t":0,"d":[{}]}}'.format(val).encode())
    ser.write(b'\x03')
    time.sleep(t)

def start():
    ser = serial.Serial('/dev/ttyACM0', baudrate=115200)
    print(ser.name)
    return ser

def registerCTRLP(ser):
    import signal
    import sys
    def signal_handler(sig, frame):
            print('You pressed Ctrl+C!')
            ser.close()
            sys.exit(0)
    signal.signal(signal.SIGINT, signal_handler)
