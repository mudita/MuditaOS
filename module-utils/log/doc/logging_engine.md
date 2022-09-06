# Logging engine

- [Logger](#Logger)
- [Dumping to a file](#Dumping-to-a-file)
- [System Logs](#System-logs)

## Logger

Logger class is used for sending `log messages` coming from LOG macros:

- `LOG_PRINTF`
- `LOG_TRACE`
- `LOG_DEBUG`
- `LOG_INFO`
- `LOG_WARN`
- `LOG_ERROR`
- `LOG_FATAL`
- `LOG_CUSTOM`

to a proper device (`SEGGER_RTT`, `console output`, `SYSTEMVIEW`)
and at the same time to put them to a `circular buffer`.

`Circular buffer` has a limited size which sometimes results in losing some logs.
In such a case, proper `lost message info` is added to `msg` received from the buffer.

However, it should not happen because the logger has a worker and 2 logger buffers. When
the buffer is full the logger switch buffer and sends message to the worker to dump logs.

## Dumping to a file

Logs from `Circular buffer` are dumped to a file named `MuditaOS.log` when:
- `Circular buffer` is full
- every 15 minutes from last dump
- download diagnostic from the phone
- system shutdown

Current max log file size is 15MB. After reaching this size the `Rotator` save file and add
extension at the end of file extension eg. `MuditaOS.log.1`. Then create the new file.

Logs can be accessed using `mount_user_lfs_partition.py` script from `tools` directory.
Additionally, `test/get_os_log.py` script allows getting a log file from a running phone.

## System logs

There are a series of useful system logging capabilities defined in:
`module-utils/log/api/log/debug.hpp`
