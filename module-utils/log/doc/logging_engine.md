# Logging engine

- [Logger](#Logger)
- [Dumping to a file](#Dumping-to-a-file)

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

## Dumping to a file

Logs from `Circular buffer` are dumped to a file named `MuditaOS.log` every 10 sec by `EventManager` timer.

Current max log file size is 50 MB (after reaching this size no more logs are dumped).

Logs can be accessed using `mount_user_lfs_partition.py` script from `tools` directory.
