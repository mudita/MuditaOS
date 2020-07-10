# Mudita PurePhone changelog

## Current version

### Added
* `[sms]` Added option to mark conversation as read/unread 

### Changed

### Fixed
* `[phonebook]` Fix wrong text label for back action.

### Other

## [0.28.1] - 2020-07-10

### Added
* `[phonebook]` Add icon flags for showing contact special groups.
* `[gui]` Add digit input on long-press in Text.
* `[gui]` Add selector for newline in special character selection.

### Changed
 * `[antenna]` Improved reception and decreased SAR by adding antenna control service.
 * `[sms]` Options have contact name in bold.
 * `[phonebook]` Improved contact searching.
 * `[phonebook]` New implementation of Add/Edit Contact window.
 * `[sms][home screen]` Distinguish between unread and unseen message notifications.

### Fixed
 * `[sms]` Return to conversations view after deleting the last message in the thread.
 * `[sms]` Delete old testing empty threads (conversations) on opening.

### Other
* `[backup-restore]` Add backup tar archive handling (packing, unpacking). 
 
## [0.27.1] - 2020-07-03

### Fixed
* `[sms]` Do not show "no message" prompt where there is an SMS.
* `[gui]` Fixed with new text: movement, addition and line breaking

### Other
* `[build]` CI improvements.
* `[doc]` GUI documentation update on window switching.
* `[gui]` Improved widget resizing support.
* `[gui]` Add layout margins support.

## [0.26.2] - 2020-06-26

### Added

* `[fota]` Added FOTA user interface.

### Changed

* `[desktop][notifications]` Improved missed call home screen notifications.
* `[desktop][notifications]` Improved incoming message home screen notifications.

### Fixed

* `[listview]` Fixed top axis alignment not working properly on lists with scrolls.
* `[sms]` Do not jump to bottom and delete entered text on new SMS or deleted SMS.
* `[sms]` Fixed crash when receiving SMS prior to entering any thread.

### Other
* `[database]` Added database backup feature.
* `[doc]` Added release process documentation.
* `[listview]` Added support for deletable/non deletable items. 
* `[phonebook]` Added widget for New/Edit Contact window.
* `[test]` Improved support for Catch2 unit testing.

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
