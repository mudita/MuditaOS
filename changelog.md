# Mudita PurePhone changelog

## Current Release

### Added

* `[fota]` UI for FOTA
* `[doc]` Added release process documentation.
* `[phonebook]` Added widget with label and text line for New/Edit Contact window.
* `[listview]` Added item delete or not support.
* `[phonebook]` Added widget with input box, description label and icon for New/Edit Contact window.
* `[bottombar]` Added selective temporary mode.

### Changed

* `[desktop]` Missed call home screen notifications
* `[desktop]` Incoming message home screen notifications

### Fixed

* `[listview]` Fixed top axis alignment not working properly on lists with scrolls.

### Other
* `[database]` Add database backup feature with sqlite's VACUUM command.
* `[UT]` added UT for calllog and notifications.db
* `[notifications]` added notifications db

## [0.26.1] - 2020-06-22

### Added
* `[notifications]` Added bottom bar action indicators.

### Changed
* `[phonebook][sms]` Improved contact matching by phone number.
* `[sms]` Contact Add/Contact details option.

### Fixed
* `[development]` Fixed colour accuracy in Linux simulator. 
* `[phonebook]` Fixed phonebook contact select from new SMS window.
* `[listview]` Fixed double click on last ListView element switching two pages.
* `[boxlayout]` Performance improvements to BoxLayout - hence ListView.
* `[sms]` Removed redundant select in new sms window 

### Other
* `[vfs]` Technical improvements and enhancements.
* `[doc]` FOTA implementation and testing report which is required by Verizon certification process.

## [0.25.1] - 2020-06-16

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
