# Test harness

The aim of this [test harness](https://en.wikipedia.org/wiki/Test_harness) is to enable tests automation and facilitate integration testing.

Mudita Pure test harness makes use of USB-CDC serial (on RT1051 target) or pseudo-tty (on Linux) to communicate
with `service-desktop` service in the operating system and get data from the internal database and device status.

Both `service-desktop` and functional tests are using [`pyTest`](https://github.com/pytest-dev/pytest/) -
 a testing framework written for Python. 
Convenient usage of pre-defined fixtures and `harness` API enables quick development of further tests.

# Please note that
* active development of test harness is kept here: [MuditaOSHarness](https://github.com/mudita/QAMuditaHarness)
* active tests development is kept here: [QAMuditaOS](https://github.com/mudita/QAMuditaHarness)

## Test harness API
As a part of the test bundle, test harness is responsible for low-level communication with target or linux simulator.
As for now, it consists of the following methods that can be used during writing tests:

* `Harness(port_name)`
 
    constructor used to create `Harness` object and open a serial connection to the operating system
    
* `get_connection()`

   returns a `CDCSerial` object - wrapper for Python's `Serial`
   
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
allowing to seamlessly transfer data between `Harness` and the operating system. It can automatically detect a connected phone, so you don't have to pass the port name anymore.
It has the following user-applicable
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
current_window = harness.get_application_name()


# open messages when phone is unlocked
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
python3 -m venv test_venv
source test_env/bin/activate
pip3 install -r requirements.txt
```
and then execute the tests (located in `pytest` subdirectory and below):
```shell script
pytest ./pytest --timeout=20 
```
by default, call and message sending tests are enabled thus you have to provide additional parameters:
```shell script
--phone_number=123456789 --call_duration=30 --sms_text="sms text"
```
which gives full command like this:
```shell script
pytest ./pytest  --timeout=20  --phone_number=123456789 --call_duration=30 --sms_text="sms text"
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

## FAQ:

### My computer doesn't see harness and there is no session -  just timeout, and an error

#### Check if Mudita Pure is properly connected

Check the device, it should be either `ls /dev/tty*` or `cat /dev/ttyUSBx`.
If there is no device:
- try connecting and disconnecting the phone with the USB port while verifying with `sudo journalctl -f` if there are any new logs
- check if your USB cable is right (and works)
- check if your USB port on the computer works fine - please check with a direct USB port without any hubs

Check device origin:
- `udevadm info -a -n /dev/ttyACM1 | less`
- `lsusb -vvv | less` -> and then write `/Mudita`

#### Add udev rule if you do not have access to device without sudo

`cat /dev/ttyACMx` or `cat /dev/ttyUSBx`

If there is no dialout group on your computer?
`sudo groupadd dialout`

If your user isn't in the dialout group?
`sudo usermod -a -G dialout $USER`

Add new udev rule and please check the `idVendor` in `idProduct` (how-to is available: `Check if Mudita Pure is properly connected`, lsusb or udevadm commands)
`ATTRS{idVendor}=="1fc9", ATTRS{idProduct}=="0094", MODE="666", GROUP="dialout"`

Reload the udev rules - **this is super important**!
`sudo udevadm control --reload-rules && sudo udevadm trigger`

### AT commands
Fixing AT commands in harness is planned

### Can I test a subset of tests?
That's the purpose of harness. We can either:
- use tests markers
- run a single test via file, test name - both: read the manual

### What are the folders?

```
├── test
│   ├── firmware_update_test      : Folder with release package update utility
│   ├── harness                   : All harness implementation
│   │   └── interface             : Serial link PHY implementation
│   └── pytest                    : Folder with ALL harness tests to run
```

### How to print all logs
Pass to pytest execution `-s`
Please use the `logger` module instead of prints:
`from harness import log`

### How does the endpoit work?

Please read the dedicated [endpoints documentation](https://appnroll.atlassian.net/wiki/spaces/MFP/pages/656637953/Protocol+description)

### How to add harness API

Please use existing API, do not modify existing endpoints behavior if it's not needed for Mudita Center
If you need to expand the existing API - please contact solution architects first.

### How do I add a test?

- See the docs and copy example
- Add it to pytest folder ( see `What are the folders` ) 
- Name it properly (if you do not know how - please read this readme once again :) )

### How do I verify harness issues

- Fail will have exit code != 0
- Fail will be displayed in red
- If a test skips - it's displayed in yellow, please double-check why - if it's not needed please remove it, if needed - inform the rest of the team.
- Double-check that you have properly loaded software - that it's not lacking any assets and that you have proper code
- Contact QA to add Issue

### Do we plan to run Harware in the loop tests on CI

- Yes, there is such plan. It's executed step by step. [HIL docs](https://appnroll.atlassian.net/wiki/spaces/MFP/pages/794296490/Project+Hardware+in+the+loop+integration+testing)

### Why do we use virtual env? why no pyenv? 

We started with nothing, added virtual env as something - pyenv would be a natural extension. If you know how to integrate it, then please do - but please remember to add the docs.

### Do we want to emulate states in harness

For now we are emulating key-press and checking the windows and database indirectly. We are gathering requirements and improvements - please contact someone from the core dev team at Mudita.

### What's next?

If you managed to read this all, you are ready to use harness! Enjoy! You are welcome :)
