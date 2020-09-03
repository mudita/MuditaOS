# Mudita PurePhone changelog

## [Current release]

### Added

* `[music player]` Added Empty window

### Changed



### Fixed



### Other



## [0.38.1 2020-09-18]

### Added

* `[calendar]` Add/Edit/Remove events capability.
* `[calendar]` Add recurring events.
* `[desktop]` Add info on how to unlock the phone.
* `[settings]` Add PIN change option in settings' main window.

### Changed

* `[calendar]` Marking days which have events in Calendar Main Window.
* `[calendar]` Display events in "Day" and "All Events" windows.
* `[gui]` Fallback font is used before displaying unsupported glyph.
* `[phonebook]` Replace vCard encoding with plain text when exporting contact data.
* `[phonebook]` Make SAVE button appear when at least one field is filled up.

### Fixed

* `[gui]` Fixed displaying notifications on an unlocked screen.
* `[audio]` `[music player]` Fixed meta tags parsing in audio files.

### Other

* `[doc]` Updated development documentation.
* `[db]` Treat temporary contacts as a group.

## [0.37.1 2020-09-11]

### Added

* `[audio]` Add storing and updating audio settings in database.

### Changed

* `[listview]` List returns to previously focused element on CRUD notifications (messages and phonebook).
* `[messages]` Use different thread for each of contact's number.
* `[phonebook]` Sort list of contacts by first name if there is no second name.
* `[phonebook]` Unnamed contacts at the end of contacts list.
* `[phonebook]` Change name of alternative/other number to second number.

### Fixed

* `[desktop]` Fixed endpoint handling.
* `[database]` Fixed boot error caused by new settings table.
* `[phonebook]` Fix speed dial number conflict resolution.

### Other

* `[desktop]` Added functional tests.

## [0.36.1 2020-09-04]

### Added

* `[fonts]` Added phone emoji support.
* `[phonebook]` ICE contacts window.

### Changed

* `[messages]` Improved sms input.

### Fixed

* `[messages]` Fixed crashing in the new message window.
* `[home screen]``[notifications]` Fixed notifications widget when phone is locked.
* `[text]`	Handling new line when entering text.

### Other

* `[build]` Allow excluding applications from the build to decrease debug firmware size.
* `[build]` Allow choosing which applications are optimized to decrease debug firmware size.
* `[doc]` Procedure on flashing a phone without ecoboot using D1 flash loader.
* `[database]` Message handling refactor.
* `[database]` Add new settings table in database.


## [0.35.1 2020-08-28]

### Added

* `[phonebook]` Forwarding VCard via SMS.
* `[phonebook]` Show block icon next to blocked contact name on contact list.
* `[phonebook]` Contact details in bottom bar on call/send.
* `[music player]` Added support for playing mp3 and flac files.

### Changed

* `[desktop]` Automatic confirmation of unlock pin if correct.

### Fixed

* `[audio]` Fixed playback of mp3 files.
* `[gui]` Fixed label padding handling.
* `[settings]` Fixed settings info.

## [0.34.2 - 2020-08-21]

### Changed

* `[settings]` Rearranged settings window.

### Fixed

* `[audio]` Fixed audio buffers handling to eliminate sending uninitialized data and making loud noise on start of a call.
* `[phonebook]` Fix duplicated contact detection on contact update.

## [0.34.1 - 2020-08-19]

### Added

* `[calendar]` Added new/edit event window.
* `[gui]` Added bootloader version information.

### Changed

* `[phonebook]` Show/Hide flags widget in contact details depending if any flags are set.
* `[phonebook]` Update icons in contact add/edit window.

### Fixed

* `[music player]` Fixed songs list building and song name font size.

### Other

* `[fonts]` Added special characters for german, spanish, french, polish languages in font files.
* `[gui]` Added check box widget.

## [0.33.1 - 2020-08-13]

### Added

* `[torch]` Toggle torch colours for certification purposes (redish, blueish, off).
* `[calendar]` database structure
* `[music player]` Added simple volume indicator.

### Changed

* `[boot]` Switched configuration file format from INI to JSON.
* `[phonebook]` Auto releasing speed dial number of blocked contact.
* `[phonebook]` Redesigned contact details window.

### Fixed

* `[phonebook]` Fix favourites contacts display order.
* `[phonebook]` Fix (un)blocking option name and improve handling of contact blocking window.

## [0.32.1 - 2020-08-07]

### Added

* `[calendar]` Added event details window.
* `[calendar]` Added all events list window.
* `[phonebook]` Added alternative number handling.
* `[music player]` Added simple music player implementation.
* `[audio]` Added headset handling (insertion / removal).
* `[torch]` Added initial torch functionality.

### Changed

* `[phonebook]` Phonebook search no window - new version of dialog box.
* `[phonebook]` Switch to contact list instead of contact details after saving contact.
* `[phonebook]` Require filling in at least one field while adding/editing contact.
* `[phonebook]` Replace static icon with number inside circle in speed dial duplicate window.
* `[phonebook]` Add final icons to Add/Edit Contact window.
* `[sms]` Do not delete sms if thread is malformed (not found what expected).
* `[listview]` Improved data refresh time.

### Fixed
* `[phonebook][db]` Fixed reading contact without a number from database.
* `[listview]` Fixed wrong data refresh.
* `[gui]` Fix crash when rendering font glyph.
* `[sms]` When opening a thread show it from the newest message.

### Other

* `[gui]` Optimization on BoxLayout elements resize.
* `[desktop]` Added service-desktop <-> DB interface.
* `[desktop]` Added contact handling from service-desktop.
* `[utilities]` Fixed copying UTF8 by character.

## [0.31.1] - 2020-07-31

### Added

* `[calendar]` Added events options and event delete windows.
* `[sms][audio]` Added incoming text message ringtone.
* `[usb]` Added ability to expose the cellular modem onto USB-C connector (USB re-routing).

### Fixed

* `[sms]` Fix selecting contact for a new message after searching in phonebook.

### Other

* `[desktop]` Reimplemented JSON message parser.
* `[audio]` Error codes unification.
* `[build]` Optimize third party libraries in debug configuration to lower the size footprint for the RT1051 platform.
* `[text]` Fixed text size and underline support.
* `[listview]` List view data handling fixes.
* `[gui]` Added generic ImageBox widget.

## [0.30.1] - 2020-07-24

### Added

* `[calendar]` Added no-events window in general purpose windows.
* `[calendar]` Added day window with list of events.
* `[calendar]` main screen UI
* `[calendar]` main screen navigation
* `[audio]` added incoming call ringtone
* `[phonebook]` general support for groups in DB
* `[phonebook]` proper displaying of favorites, ice, blocked flags
* `[sms]` thread summaries cover all SMS states (incoming, failed etc.)

### Changed

* `[text]` set the cursor at the end of text by default

### Fixed

* `[sms]` new message properly adds templates and resizes
* `[sms]` fixed focus in new message window after message forward option
* `[audio]` added audio workers synchronization mutexes
* `[phonebook]` Fixed bottom bar in main phonebook window
* `[info]` fixed displaying of version
* `[boxlayout]` min-max resize on alignment fix.
* `[sms]` fixed logic around re-sending messages
* `[sms]` conversation view summary refreshes instantly

### Other

* `[gui]` Moved Icon item into general widgets folder.
* `[backup-restore]` Add basic handling of backup and restore through JSon parser.
* `[factory-reset]` Add basic handling of factory reset through JSon parser.

## [0.29.1] - 2020-07-17

### Added

* `[sms]` Added option to mark conversation as read/unread

### Changed

### Fixed

* `[phonebook]` Fix wrong text label for back action.
* `[sms]` Concatenated sms sending fixed.
* `[sms]` Messages window margins fix.
* `[sms]` Fix for sms forward
* `[sms]` Fix for missing send label in new message window
* `[sms]` Fix for handling temporary contacts
* `[sms]` Fix for clipboard usage
* `[call]` Fix for adding new contact from call app
* `[text]` Text Vertical Alignment and Text Cursor starting position fix.

### Other

* `[Alignment]` Refactored Alignment and added support to BoxLayout.

## [0.28.1] - 2020-07-10

### Added

* `[phonebook]` Add icon flags for showing contact special groups.
* `[gui]` Add digit input on long-press in Text.
* `[gui]` Add selector for newline in special character selection.
* `[gui]` Added Color to text

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
* `[factory-reset]` Add factory reset operation for restoring entire phone back to the factory state.

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
