This python code below can be used to test bt passthrough / commands
--------------------------------------------------------------------

FileReader - reads commands from file, needed file format is:
```
in:  0x1 0x3 0xC 0x0
out: 0x4 0xE 0x4 0x1 0x3 0xC 0x0
in:  0x1 0x1 0x10 0x0
o
```

`in: `  - command to send to bt chip
`out: ` - reply which should happen

BtUartCommandsTest - loads commands with FileReader and sends & checks them one by one

to_header.py - loads commands from FileReader and creates header to be loaded on phone
