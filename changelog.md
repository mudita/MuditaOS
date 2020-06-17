# Mudita PurePhone changelog

## [0.24.1] - 2020-06-17

### Added
* `[vfs]` fprintf wrapper

### Fixed
* `[vfs]` return() parenthesis removed
* `[sbini]` moved to c++
* `[microtar]` moved to c++

### Removed
* `[vfs]` obsolete methods for absolute paths

## [0.24.1] - 2020-06-16

### Added
* `[phonebook]` Add duplicated contact and speed dial numbers handling.
* `[phonebook]` Add confirmation windows after deleting/blocking contact. 
* `[sms]` Add thread message status indicators: read/unread and last thread message (Not sent, You, Draft).

### Fixed
* `[desktop]` Fix plus sign input on a 0 key longpress.
* `[sms]` Fix thread view refresh on a new message.
* `[sms]` Fix message reordering on page change.
* `[sms]` Remove empty screen view at the top of a thread view.

### Other
* `[test]` Improved modem testing with modem shutdown detection.
* `[build]` Add support for auto-releases.
