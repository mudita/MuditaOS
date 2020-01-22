dumb test harness py
====================

If something is stupid but it works, it's not stupid

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
* Partial frames can be added easilly

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
