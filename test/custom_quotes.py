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
port_timeout_medium_s = 2
port_timeout_long_s = 5
port_baudrate = 115200

default_rx_data_size = 1024
chunk_rx_data_size = 1024 * 200

sync_filename = 'sync.tar'
sync_path = '/user/temp/' + sync_filename

def get_new_uuid() -> int:
    return random.randint(1, 10000)

def send_data(port_path: str, payload_str: str, expected_response: int) -> bool:
    payload_len_str = str(len(payload_str)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + payload_str

    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_short_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(2048)
        resp_json = json.loads(response[10:])
        status = resp_json['status']
        if status == expected_response:
            print(f'Request success')
            return True
        print(f'Request failed, status {status}')
        return False
    
def get_request_response(port_path: str, request: str, expected_size: int) -> str:
    payload_len_str = str(len(request)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + request

    response = ''
    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_medium_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(expected_size)
    return response
    
def get_str_data(port_path: str, payload_str: str, expected_responses: int, body_str: str) -> str:
    payload_len_str = str(len(payload_str)).rjust(payload_size_len, '0')
    request_str = payload_marker + payload_len_str + payload_str

    with serial.Serial(port_path, port_baudrate, timeout=port_timeout_short_s) as port:
        port.write(request_str.encode('ascii'))
        response = port.read(2048)
        resp_json = json.loads(response[10:])
        status = resp_json['status']

        for expected_status in expected_responses:
            if status == expected_status:
                print(f'Request success')
                body = resp_json['body']
                return body[body_str]
            
        print(f'Request failed, status {status}')
        return ""

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
    return send_data(port_path, payload_str, 200)

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
    return send_data(port_path, payload_str, 204)

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
    return send_data(port_path, payload_str, 200)

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
    return send_data(port_path, payload_str, 200)
    
def get_settings(port_path: str, settings: str) -> str:
    uuid = get_new_uuid()
    payload = {
        'endpoint': endpoint_types['Quotes'],
        'method': http_methods['GET'],
        'uuid': uuid,
        'body': {
            'settings': settings
        }
    }
    payload_str = json.dumps(payload)
    return get_str_data(port_path, payload_str, [200], settings)

def start_sync(port_path: str) -> bool:
    uuid = get_new_uuid()
    payload = {
        'endpoint': endpoint_types['Backup'],
        'method': http_methods['POST'],
        'uuid': uuid,
        'body': {
            "category" : "sync"
        }
    }
    payload_str = json.dumps(payload)
    return send_data(port_path, payload_str, 202)

def wait_for_sync_end(port_path: str, timeout: int) -> bool:
    uuid = get_new_uuid()
    payload = {
        'endpoint': endpoint_types['Backup'],
        'method': http_methods['GET'],
        'uuid': uuid,
        'body': {
            "category" : "sync"
        }
    }
    payload_str = json.dumps(payload)

    start_timestamp = time.time()
    while time.time() < start_timestamp + timeout:
        status = get_str_data(port_path, payload_str, [204, 303], 'state')
        print("sync status: " + status)
        if status == "finished":
            return True
        time.sleep(1)

    return False

def download_file(port_path: str, path: str) -> bool:
    uuid = get_new_uuid()
    chunk_size = 0
    file_size = 0

    payload = {
        'endpoint': endpoint_types['FilesystemUpload'],
        'method': http_methods['GET'],
        'uuid': uuid,
        'body': {
            'fileName': path
        }
    }
    payload_str = json.dumps(payload)

    requestID = {
        'endpoint': endpoint_types['FilesystemUpload'],
        'method': http_methods['GET'],
        'uuid': uuid,
        'body': {
            'rxID': 0,
            'chunkNo': 1
        }
    }

    response = get_request_response(port_path, payload_str, default_rx_data_size)
    if not response:
        print("Request GET file " + path + " info failed!")
        return False
    
    resp_json = json.loads(response[10:])
    status = resp_json['status']
    if status == 200:
        requestID['body']['rxID'] = resp_json['body']['rxID']
        chunk_size = resp_json['body']['chunkSize']
        file_size = resp_json['body']['fileSize']
    else:
        print(f'Request failed, status: {status}')
        return False

    chunks = int((file_size / chunk_size) + 1)
    print(f'File size: {file_size} chunk size: {chunk_size} chunks: {chunks}')

    filename = "temp/" + sync_filename
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    progress_bar = tqdm(desc=f'Downloading \'{path}\'', total=file_size, unit='bytes', unit_scale=True, unit_divisor=1024)
    with open(filename, 'wb') as file:
        for i in range(1, chunks + 1):
            requestID_str = json.dumps(requestID)
            response = get_request_response(port_path, requestID_str, chunk_rx_data_size)
            if not response:
                print("Request GET chunkNo: " + requestID['body']['chunkNo'] + " failed!")
                return False

            resp_json = json.loads(response[10:])
            status = resp_json['status']
            if status == 200:
                data = base64.b64decode(resp_json['body']['data'])
                file.write(data)
                requestID['body']['chunkNo'] = i + 1
                progress_bar.update(len(data))
            else:
                print(f'Request failed, status {status}')
                progress_bar.close()
                return False

    progress_bar.close()
    return True

def delete_sync(port_path: str, path: str) -> bool:
    uuid = get_new_uuid()
    payload = {
        'endpoint': endpoint_types['FilesystemUpload'],
        'method': http_methods['DELETE'],
        'uuid': uuid,
        'body': {
            "removeFile" : path
        }
    }
    payload_str = json.dumps(payload)
    return send_data(port_path, payload_str, 204)

def get_quotes(port_path: str) -> bool:
    if start_sync(port_path) == False:
        print("Error! Synchronization has not been started.")
        return False
    print("Synchronization has started...")

    if wait_for_sync_end(port_path, 20) == False:
        print("Error! Synchronization fail.")
        return False
    print("Synchronization completed successfully.")

    if download_file(port_path, sync_path) == False:
        print("Error! Sync file download fail.")
        return False
    print("Sync file download completed.")

    if delete_sync(port_path, sync_path) == False:
        print("Error! Failed to delete sync file.")
        return False
    print("Sync file deletion completed successfully.")

    return True

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
                        help='quotes display interval [\'x\' minutes or \'AtMidnight\']')
    parser.add_argument('-s', '--synchro',
                        metavar='settings_to_get',
                        help='get quotes file or settings [\'quotes\' or \'group\' or \'interval\']')

    args = parser.parse_args()
    if not args.port:
        print('Invalid usage: please specify device port')
        print('Run with -h to see help')
        return
    if not args.add and not args.delete and not args.edit and not args.group and not args.interval and not args.synchro:
        print('Invalid usage: please specify add, delete, edit, synchro, group or interval argument')
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
    elif args.synchro:
        if args.synchro == "quotes":
            print("downloading quotes file")
            status = get_quotes(args.port)
        elif args.synchro == "group":
            group = get_settings(args.port, args.synchro)
            print("quotes group: " + group)
        elif args.synchro == "interval":
            interval = get_settings(args.port, args.synchro)
            print("quotes interval: " + interval)
        else:
            print('Invalid usage: please choose one option: \'quotes\' or \'group\' or \'interval\'')
            print('Run with -h to see help')

if __name__ == '__main__':
    main()
