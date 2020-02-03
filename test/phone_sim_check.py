#!/bin/python
from harness.uart import conn, log

conn.gsm("AT+CPIN?\r");
print(conn.read(10))
