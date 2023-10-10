MuditaOS Logging Guide
======================

This document describes how and when we should use logs and different levels of logging.
The main goal is to keep our log files as simple as possible, with only the most important data.
Following these rules should make log files shorter, easier to read and give a better understanding of what is happening
in the software.

### Logging principles

- Do not add any information to the log that is already included in the log prefix, such as: parent service name, file,
  line number or function name.
- Do not add additional tags to the log, e.g. \[DB\], \[Desktop\], etc.
- Use magic_enum or other functions to log an enum name instead of logging an int value, unless the underlying value is
  important in the particular case.
- Start logs with a capital letter.
- Logs should be statements that are easy to read and understand out of context.
- Before adding a new log line, think about whether it is important, or whether it might be useful for debugging.
- Try to keep the log message to a single line.

### Log level description

- INFO — This is the default level.
  It should be used to inform about various actions and states that are taking place in software.
  It shows the current state and helps to navigate to understand what the user has done.
  It should be used to log information important from the software point of view.

- DEBUG — This level should be used to log anything that may be important for debugging or locating the error after it
  has occurred.
  Note that our official release builds also have this level available.

- WARNING — This level should be used to log anything that could potentially become an error.

- ERROR — This level should be used to log all errors.

- FATAL — This level should only be used to log fatal errors that could prevent the system from working properly.
  Normally, logging at this level means the end of the program.

- SENSITIVE — This level should be used to log sensitive data.
  This level is behind an additional flag, and we can control whether we want to add these logs to the package or not.
  By default, this level is enabled.

- TRACE — Generally not used in MuditaOS.

- PRINTF — This level is used to log raw information from the system.
  It does not include a log header.
  It should be used rarely and only when really necessary.

### Reminder

If you find a log in the code that does not follow the rules, try to adapt it.
This will help keep our logs in good order and prevent them from rotting.

#### Development and Debugging

When developing new features or debugging, feel free to use log levels without restriction. Also log all data
you may need to find solutions faster.
Before creating a PR, remember to check the logs and make sure they comply with our logging rules. 