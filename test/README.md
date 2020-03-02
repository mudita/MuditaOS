dumb test harness py
====================

If something is stupid but it works, it's not stupid

Simple utility to run key presses, AT commands & wait for response form phone scriptable in python.
Great for repetitive tasks as i.e. phone unlock, or UI tests with multiple key presses.

# running in console

run:
```
python3
from harness.common import conn

# for key press: 0 - 9, second parameter = wait time after key press, here set to '1'
conn.key(0, 1)
# ...
conn.key(9, 1)

# send AT command - '\r' is important - othervise modem will ignore command
conn.gsm("AT?\r") 

# wait for some data
data = conn.read(4)
```

# how:

Frame:
```
<STX>
{
    Type : harness::Enum::Events
    Data : [ <element for selected data type>, ]
}
<ETX>
```

* Frames start with Frame enum
* Fata is per enum
* All data between `<STX>` abd `<ETX>` is Json
* Binary data should be encoded (base64)
* Partial frames can be added easily

I considered protobuf, tlv and msgpack, protobuf ain't very easy when you want to have Any types inside, msgpack don't have schema.
Json is super easy - json it is

## In app:

* bsp - UART3 added (can be USB, IP whatever really)
    * STX signals application that old data have to be ignored
    * ETX signals app to parse data
* EventManager
    * Informs on start of Eventmanager
    * Informs about application focus change
    * Reads emits fake key releases
* `test/*.py` - test code for handling phone
    * phone_start.py - wait till phone sends its on
    * phone_unlock.py - unlock phone and show desktop
    * switch_app.py - switch to sms app and back waiting for info on application focus change
    * walk presses on `arrow keys` for ~30 min
