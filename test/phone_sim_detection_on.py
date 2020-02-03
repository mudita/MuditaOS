from harness.uart import conn, log
# it would be much better to wait for ok in script, but resend doesn't work now...
# due to Service Cellular arch

conn.gsm("at+qsimdet=1,0\r")
conn.sleep(10)
conn.gsm("AT+QSIMSTAT=1\r")
conn.sleep(10)
conn.gsm("at&w\r")
conn.sleep(10)
