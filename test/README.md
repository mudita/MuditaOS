# Test harness

The aim of this [test harness](https://en.wikipedia.org/wiki/Test_harness) is to enable tests automation and facilitate integration testing.

Mudita Pure test harness makes use of USB-CDC serial (on RT1051 target) or pseudo-tty (on Linux) to communicate
with `service-desktop` service in the operating system and get data from the internal database and device status.

Both `service-desktop` and functional tests are using [`pyTest`](https://github.com/pytest-dev/pytest/) -
 a testing framework written for Python. 
Convenient usage of pre-defined fixtures and `harness` API enables quick development of further tests.

## Test harness API
As a part of the test bundle, test harness is responsible for low-level communication with target or linux simulator.
As for now, it consists of the following methods that can be used during writing tests:

* `Harness(port_name)`
 
    constructor used to create `Harness` object and open a serial connection to the operating system
    
* `get_connection()`

   returns a `CDCSerial` object - wrapper for Python's `Serial`
   
* `get_window_name()`

   returns current application(window) name
   
* `with_phone_unlocked(func)`

    decorator allowing to call a function with an unlocked operating system
    
* `open_application(application: str)`

    opens application from main screen (unlocked phone with clock on top)
    
    possible valid arguments: `phone, contacts, messages, music, meditation, settings, tools, alarm, calendar`

* `send_text(text: str)`

    emulates sending text using numpad presses to the text window (eg. SMS)

* `send_number(number: str)`

    emulates sending numbers to the number window (eg. contact number)
    
* `endpoint_request(ep_name: str, met: str, body: dict) -> dict`

    method used to send endpoint request to the service-desktop API and gather requested data from the operating system

    * `ep_name` - valid options: `deviceInfo, update, filesystemUpload, backup, restore, factory, contacts, messages,
     calllog, events, developerMode`
    * `met` - HTTP methods: `get, post, put, del`
    * `body` - JSON payload

#### CDCSerial API
CDCSerial is `Serial` wrapper intended to encapsulate commands and data into communication protocol thus 
allowing to seamlessly transfer data between `Harness` and the operating system. It has the following user-applicable
methods:


* `send_key(key_code, key_type=Keytype.short_press, wait=10)`

    emulates sending one keypress as short press (by default) or as a long press (with `Keytype.long_press` argument)
    
    * `key_code` - value of character from `harness.interface.defs.key_codes` array, for numbers `0-9` just ASCII value
    * `wait` - timeout
    
* `send_at(at_command: str, wait=10)`

    send AT command to the Cellular module, returns AT response
    
    * `at_command` - string with AT command, without any terminator
    * `wait` - timeout
    
#### Example
```python
from harness.harness import Harness
from harness.interface.defs import key_codes

# try to init Harness object with automatic port detection
harness = Harness.from_detect()

# get current application name
current_window = harness.get_window_name()

#open messages when phone is unlocked
@harness.with_phone_unlocked
def do_after_unlock(connection):
    # open menu
    connection.send_key_code(key_codes["enter"])

    harness.open_application("messages")
    # send joystick down keypress
    connection.send_key_code(key_codes["down"])

    # open a thread
    connection.send_key_code(key_codes["enter"])
```

### pyTest running
To execute pyTest test cases make sure that you have installed the `pyTest` package and you have `python3`. It is 
recommended to execute those tests in a virtual environment (eg. in `test` dir):

```shell script
pip3 install virtualenv 
virtualenv -p python3 test_env
source test_env/bin/activate
pip3 install -r requirements.txt
```
and then execute the tests (located in `pytest` subdirectory and below):
```shell script
pytest ./pytest --port=/dev/ttyACM2 --timeout=20 
```
by default, call and message sending tests are enabled thus you have to provide additional parameters:
```shell script
--phone_number=123456789 --call_duration=30 --sms_text="sms text"
```
which gives full command like this:
```shell script
pytest ./pytest --port=/dev/ttyACM2 --timeout=20  --phone_number=123456789 --call_duration=30 --sms_text="sms text"
```
To run the tests with Linux simulator just pass `simulator` as the port parameter and use `-m` switch with marker negation:
```shell script
pytest ./pytest --port=simulator --timeout=20 -m "not rt1051"
```
`timeout` parameter sets port discovery timeout in seconds (if after this timeout port is not available, pyTest will exit).

After finished test session just call:
```shell script
deactivate
```
to exit the virtual environment.

### sample pyTest test
All tests in the default configuration of pyTest require test files starting with `test_` and test functions also 
starting with `test_`. To use fixture like `harness`, just pass it as an argument to the test function:
```python
test_first_sample_test.py

def test_sample_test(harness):
    body = {}
    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == 200
   
``` 
All tests written in this manner will be found by pyTest.
#### Notice: all tests that are applicable only to the target should be marked with a `@pytest.mark.rt1051` decorator
