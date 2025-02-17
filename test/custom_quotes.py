import base64
import os.path
import serial
import random
import json
import zlib
import argparse
import time
from tqdm import tqdm

http_methods = {
    'GET': 1,
    'POST': 2,
    'PUT': 3,
    'DELETE': 4
}

endpoint_types = {
    'Invalid': 0,
    'DeviceInfo': 1,
    'Update': 2,
    'FilesystemUpload': 3,
    'Backup': 4,
    'Restore': 5,
    'Factory': 6,
    'Contacts': 7,
    'Messages': 8,
    'Calllog': 9,
    'CalendarEventsPlaceholder': 10,
    'DeveloperMode': 11,
    'Bluetooth': 12,
    'UsbSecurity': 13,
    'Outbox': 14,
    'Reboot': 15,
    'TimeSync': 16,
    'Quotes': 17
}

payload_marker = '#'
payload_size_len = 9
port_timeout_short_s = 1
port_timeout_long_s = 5
port_baudrate = 115200


def get_new_uuid() -> int:
    return random.randint(1, 10000)

def add_quote(port_path: str, quote_str: str, author_str: str) -> bool:
    uuid = get_new_uuid()

    payload = {
        'endpoint': endpoint_types['Quotes'],
        'method': http_methods['POST'],
        'uuid': uuid,
        'body': {
            'quote': quote_str,
            'author': author_str
        }
    }

    payload_str = json.dumps(payload)
    payload_len_str = str(len(payload_str)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + payload_str

    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_short_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(2048)
        resp_json = json.loads(response[10:])
        status = resp_json['status']
        if status == 200:
            print(f'Request success {resp_json["body"]}')
            return True
        print(f'Request failed, status {status}')
        return False
    
def edit_quote(port_path: str, quote_id: int,  quote_str: str, author_str: str) -> bool:
    uuid = get_new_uuid()

    payload = {
        'endpoint': endpoint_types['Quotes'],
        'method': http_methods['PUT'],
        'uuid': uuid,
        'body': {
            'quoteID': quote_id,
            'quote': quote_str,
            'author': author_str
        }
    }

    payload_str = json.dumps(payload)
    payload_len_str = str(len(payload_str)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + payload_str

    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_short_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(2048)
        resp_json = json.loads(response[10:])
        status = resp_json['status']
        if status == 200:
            print(f'Request success')
            return True
        print(f'Request failed, status {status}')
        return False

def delete_quote(port_path: str, quote_id: int) -> bool:
    uuid = get_new_uuid()

    payload = {
        'endpoint': endpoint_types['Quotes'],
        'method': http_methods['DELETE'],
        'uuid': uuid,
        'body': {
            'quoteID': quote_id
        }
    }

    payload_str = json.dumps(payload)
    payload_len_str = str(len(payload_str)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + payload_str

    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_short_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(2048)
        resp_json = json.loads(response[10:])
        status = resp_json['status']
        if status == 204:
            print(f'Request success')
            return True
        print(f'Request failed, status {status}')
        return False

def change_group(port_path: str, quote_group: str) -> bool:
    uuid = get_new_uuid()

    payload = {
        'endpoint': endpoint_types['Quotes'],
        'method': http_methods['PUT'],
        'uuid': uuid,
        'body': {
            'group': quote_group
        }
    }

    payload_str = json.dumps(payload)
    payload_len_str = str(len(payload_str)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + payload_str

    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_short_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(2048)
        resp_json = json.loads(response[10:])
        status = resp_json['status']
        if status == 200:
            print(f'Request success')
            return True
        print(f'Request failed, status {status}')
        return False

def change_interval(port_path: str, quote_interval: str) -> bool:
    uuid = get_new_uuid()

    payload = {
        'endpoint': endpoint_types['Quotes'],
        'method': http_methods['PUT'],
        'uuid': uuid,
        'body': {
            'interval': quote_interval
        }
    }

    payload_str = json.dumps(payload)
    payload_len_str = str(len(payload_str)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + payload_str

    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_short_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(2048)
        resp_json = json.loads(response[10:])
        status = resp_json['status']
        if status == 200:
            print(f'Request success')
            return True
        print(f'Request failed, status {status}')
        return False

def main():
    parser = argparse.ArgumentParser(
        prog='custom_quotes',
        description='Script used to test functionality of custom quotes endpoint in Harmony project'
    )
    parser.add_argument('-p', '--port',
                        metavar='path_to_com_port',
                        help='path to COM port of the device (e.g. /dev/ttyACM0)')
    parser.add_argument('-a', '--add',
                        action='store_true',
                        help='add new custom quote')
    parser.add_argument('-d', '--delete',
                        metavar='quote_id_to_delete',
                        type=int,
                        help='delete custom quote')
    parser.add_argument('-e', '--edit',
                        metavar='quote_id_to_edit',
                        type=int,
                        help='edit custom quote')
    parser.add_argument('-q', '--quote',
                        metavar='string_with_quote',
                        help='string with new quote')
    parser.add_argument('-t', '--author',
                        metavar='string_with_author',
                        help='string with author')
    parser.add_argument('-g', '--group',
                        metavar='string_with_group_type',
                        help='string with type of group of displayed quotes [Predefined or Custom]')
    parser.add_argument('-i', '--interval',
                        metavar='quotes_display_interval',
                        help='quotes display interval [\'x\' minutes or \'at midnight\']')

    args = parser.parse_args()
    if not args.port:
        print('Invalid usage: please specify device port')
        print('Run with -h to see help')
        return
    if not args.add and not args.delete and not args.edit and not args.group and not args.interval:
        print('Invalid usage: please specify add, delete, edit, group or interval argument')
        print('Run with -h to see help')
        return
    if args.add:
        if not args.quote:
            print('Invalid usage: please add quote')
            print('Run with -h to see help')
            return
        else:
            print("adding new quotes: " + args.quote)
            status = add_quote(args.port, args.quote, args.author)
    elif args.delete:
        print("deleting quotes nr: " + str(args.delete))
        status = delete_quote(args.port, args.delete)
    elif args.edit:
        if not args.quote:
            print('Invalid usage: please add quote')
            print('Run with -h to see help')
            return
        else:
            print("editing quotes nr: " + str(args.edit))
            status = edit_quote(args.port, args.edit, args.quote, args.author)
    elif args.group:
        print("quotes group: " + args.group)
        status = change_group(args.port, args.group)
    elif args.interval:
        print("quotes interval: " + args.interval)
        status = change_interval(args.port, args.interval)

if __name__ == '__main__':
    main()
