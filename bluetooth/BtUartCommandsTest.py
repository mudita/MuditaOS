import serial
from FileReader import FileReader
import time

port = '/dev/ttyACM1'
baud = 115200


if __name__ == "__main__":
    '''
    This code reads bluetooth transmission from file
    then sends it on UART and checks responses
    '''
    serial_port = serial.Serial(port, baud, timeout=1)

    todo = FileReader()
    todo.open();

    while True:
        [out, send] = todo.from_file()
        if send == '' or send == []:
            print("done");
            break

        # Send HCI command to serial port
        serial_port.write(send)

        # get and verify response
        [out, rec_i] = todo.from_file()
        for a in rec_i:
            val = b''
            while val == b'':
                val = serial_port.read(1)
                if len(val) == 0:
                    continue
                else:
                    val = val[0]
            if val != a :
                print('bad response {} vs {}'.format(a, val))
        print("done: {}".format(send))
