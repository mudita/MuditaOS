# MuditaOS changelog

## [0.61.1 2020-03-26]

### Added

* Add "About Your Pure" main window.
* Add `pureflash` rapid image flashing utility.
* Add a phone mode status bar widget.
* Add access to data backups over MTP.
* Add auto disconnect of a Bluetooth device on timeout to save energy.
* Add Bluetooth audio playback using the A2DP profile.
* Add documentation of the logging subsystem.
* Add documentation of the Pure simulator.
* Add dumping logs to file.
* Add linear backlight controller.
* Add phone mode popups.
* Add popup volume control.
* Add Swedish localization.

### Changed

* Add improvements to the test harness.
* Add several enhancements to the Quotes application.
* Block CPU frequency dropping when a USB cable is connected to improve USB communication stability.
* Improve memory usage.
* Improve power management when Bluetooth is active.
* Refactor of system timers.
* Remove a workaround to support old battery packs.
* Replace test data in Polish with the English version.
* Update torch controlling.

### Fixed

* Fix broken update process.
* Fix building simulator using Release configuration.
* Fix communication stability with the cellular modem by tweaking command timeouts.
* Fix flushing database contents to the non-volatile memory.
* Fix image building by updating its build dependencies.
* Fix image checksum validation.
* Fix issues with incoming messages.
* Fix newline handling in messages' thumbnails.
* Fix occasional crashes when handling MTP transfers.
* Fix persistent entry in the call log database.
* Fix transmission errors on communication with the Bluetooth modem.
* Fix wording for English localization.

## [0.60.1 2020-03-19]

### Added

* Add ability to discover disconnection of connected Bluetooth device.
* Add ability to pre-setup paired USB devices.
* Add battery charging UI
* Add dialog windows for the onboarding application.
* Add DMA transfers handling to the cellular modem communication.
* Add documentation for Bluetooth automatic tests.
* Add option to update windows' contents more effectively.
* Add phone flashing guide.
* Add selective Bluetooth connection based on a device class.
* Add status window on Bluetooth connection fail.
* Add technical documentation on Bluetooth pairing process implementation.
* Add the category for the quote selection window.
* Add USB passthrough to tethering.
* Add window to edit quotes.
* Add windows for phone modes.
* Introduce daily builds.

### Changed

* Change quotes storing from JSON files to the database.
* Improve communication stability with the cellular modem by fixing the configuration of the hardware flow control pins.
* Improve layout of the status bar.
* Improve power saving when using the cellular modem.
* Improve stability by tweaking applications' and services' stack size.
* Improve the design of the power-off window.
* Improve the number of logs produced by cellular modem handling.
* Improved design of the quote edit window.
* Optimize displaying of a call log by retrieving data in chunks.
* Reduce the number of logs produced by services and settings.
* Unify settings' menu text labels in terms of capital and small letters.
* Update quickstart guide with security-related information.

### Fixed

* Fix database performance issues related to the filesystem interface.
* Fix displaying Bluetooth configuration data window after changing phone visibility.
* Fix issues with e-ink refresh by prohibiting CPU frequency change during the process.
* Fix issues with the update process related to filesystem usage.
* Fix memory leak in the phonebook application.
* Fix missing version file in an update package issue.
* Fix modem start issue associated with an invalid battery level check.
* Fix parsing issues with first cellular command reply after a hot start.
* Fix refreshing Bluetooth devices list after disconnect.
* Fix removing buttons on Bluetooth devices list window's bottom bar after removing last Bluetooth device.
* Fix several broken functional tests.
* Fix text formatting in retry windows.
* Fix UI issues caused by consuming too much data by spin boxes.

## [0.59.1 2020-03-09]

### Added

* Add a separate set of audio settings for each mode.
* Add Bluetooth pairing.
* Add cache for settings reducing average settings processing time.
* Add clearing the phone screen when the phone is off.
* Add EULA window for the onboarding process.
* Add Handling a simultaneous A2DP/HSP Bluetooth connection.
* Add missing Swedish glyphs.
* Add pagination for message threads.
* Add post-update window with update status information.
* Add reusable "Retry action" dialog window.
* Add SIM PIN2 handling.
* Add system watchdog preventing random freezes.
* Add the ability to send a message using the contact information.
* Add time selection window.
* Add vibration handling.
* Add window for entering Bluetooth pairing code.
* Add windows to visualize the progress of the phone update process.

### Changed

* Improve Bluetooth communication stability.
* Improve displaying information on the top bar.
* Improve handling of URC, increasing communication stability with the modem.
* Improve quickstart procedure.
* Improve user actions handling by processing them in order. 
* Improve volume buttons logic to be more intuitive for the user.
* Increase stability by changing filesystem to littleFS.
* Introduce an improved and unified way to close the system.
* Remove volume indicator from the music player to be replaced with popup volume indicator in the future.
* Restore phone auto-locking on timeout.
* Secure USB endpoints with screen passcode.
* Several improvements to low power mode.

### Fixed

* Fix call application not showing up when there is no caller information available.
* Fix crashes in logging subsystem related to long messages.
* Fix database startup process.
* Fix improper contacts being displayed in the phonebook window.
* Fix minor UX issues in messages' user interface.
* Fix multimedia playback priorities regarding alarm and meditation sounds.
* Fix quotes windows.
* Fix several minor issues with system stability.
* Fix several random issues related to system heap shortage.
* Fix small Bluetooth icon.
* Fix status icons indicating charging status.
* Fix switching to loudspeaker not working on rare occasions.
* Fix timeout timer, increasing update process stability.
* Fix URC handling.
* Fix USD settings menu navigation.

### Other

* Add an option to use minimal phone number data, reducing firmware size.
* Add map parser and `puregdb` tools to the main repo.
* Add support for `newlib` time syscalls.
* Disable OS optimization for development build configurations.
* Enable differential static analysis with `clang-tidy-diff`.
* Fix Linux build with GSM enabled.
* Several tests added.
* Update GCC to version 10.

## [0.58.1 2020-02-19]

### Added

* Add charger temperature cutoff.
* Add onboarding application.
* Add signal strength bar indicator.

### Changed

* Change the date format separator from "/" to ".".
* Improve update process stability by introducing a special phone mode for updating.
* Increase visibility time of the "Call ended" window.
* Store battery critical level in the settings.

### Fixed

* Fix crashing on text removal.
* Fix displaying the "call waiting" and "call barring" MMI responses.
* Fix displaying white color during Color Test.
* Fix focus reset on GUI refresh in the "Time and Date" window.
* Fix handling of private numbers.
* Fix invalid parsing of messages containing emojis.
* Fix no call log entry when aborting a call.
* Fix problems with adding characters in the special input window.
* Fix problems with the inability to send long messages by setting a message size limit in the user interface to 7*67 characters.
* Fix problems with setting message recipient when creating a new message.
* Fix timeouts on MMI operations.
* Fix turning off Bluetooth.

### Other

* Add documentation for the Low Power mode.
* Add support for handling phone modes.
* Add tests harness for cellular hot/cold start.
* Improve charger state detection.
* Improve parsing of CLCC commands.
* Several improvements to the low power mode support.
* Update `clang-tidy` rules.
* Update documentation for Bluetooth support. 
* Update i18n documentation related to font rendering.
* Update test harness for Bluetooth.

## [0.57.1 2020-02-11]

### Added

* Add color test window to the settings app.
* Add support for an empty line in the rich text control.
* Add support for VoLTE on/off.
* Add USB charger detection.
* Add windows for custom responses to MMIs.

### Changed

* Add several stability improvements to the Bluetooth support.
* Change a reaction to a fatal error - restart the system instead of hanging.
* Change call handling to allow emergency and ICE numbers only.
* Change list of supported files type for MTP to MP3, WAV, and FLAC.
* Improve display features of the list view control.
* Improve power management by adding low power support to the drivers.
* Improve power management by powering off the e-ink display on timeout.
* Remove audio echo during voice call by tweaking echo cancellation parameters.

### Fixed

* Fix broken mute on a voice call option.
* Fix crashes when parsing a rich text.
* Fix freezes due to invalid handling of long messages during communication with the modem.
* Fix invalid warning about messages not being sent.
* Fix options distances in SMS and Settings options.

### Other

* Add preliminary support for the T6 platform.
* Add several harness tests.
* Add synchronization between services.
* Allow formatting hook scripts to ignore preconfigured paths.
* Fix excessive logging in the USB-CDC
* Fix memory leaks in the PIN lock window.
* Fix several issues with unit testing.
* Refactor code responsible for low-level drawing.
* Restore logs coloring for the RT1051 based platforms.
* Switch CI build to ninja to improve performance.
* Update documentation of test harness.

## [0.56.1 2020-02-03]

### Added

* Add the ability to change APNs.
* Add SMS Center validity verification.
* Add the "Wrong emergency number called" window.
* Add user interface to control keypad backlight.

### Changed

* Improve audio quality during voice calls by tweaking microphone settings.
* Improve network time synchronization with periodic updates.
* Reduce audio lag during voice calls, significantly reducing echo as a result.
* Tweak calendar's user interface.
* Tweak battery charger parameters.

### Fixed

* Fix lock screen not showing after reset.
* Fix lack of system clock refreshing.
* Fix lack of audio during voice calls.
* Fix SOS button showing on the PIN screen.

### Other

* Add an ability to interrupt the OS update process.
* Add harness test for Bluetooth on/off.
* Add harness test for the phone on/off.
* Add harness test for SIM card change.
* Add parser for the "call waiting" MMI response.
* Fix missing build dependency for the image file.
* Fix logs mocking in unit tests.
* Fix unit tests not always running on `check` operation.

## [0.55.1 2020-01-28]

### Added

* Add brownout information window.
* Add language select settings window.
* Add emergency call window.

### Changed

* Add several improvements to the USB handling.
* Improve e-ink display speed.

### Fixed

* Fix crash on USB deinitialization.
* Fix USB Full Speed.
* Fix selecting contact for edit by phone number from speed dial window.
* Fix VoLTE settings editing.
* Fix sim indicator displaying on the top bar.
* Fix returning from ICE contacts window.

### Other

* Improvements to the harness tests.
* Improve downloading assets for selected releases.
* Add backend for multi-day events in the calendar application.
* Add call forwarding and MMI status parsers.

## [0.54.3 2020-01-22]

### Added

* Add options to create, edit and remove quotes.
* Add main system settings window.
* Add APN settings window.

### Changed

* Improve e-ink responsiveness.
* Change time format on home screen and status bar.
* Add application-wide top bar manager.
* Improved screen light control.

### Fixed

* Fix changing passcode windows.
* Fix the possibility to write illogical operations in the calculator.
* Fix the possibility to write multiple zeroes in the calculator when it is not needed.
* Change the display format of a calculation result to scientific notation when it does not fit the screen.
* Fix freezes and resets due to invalid assertions.
* Fix grid layout navigation.
* Fix Linux simulator crashes.

### Other

* Add build target to generate a package with version information.
* Update licensing information.
* Add option to change VID/PID via build system.
* Add several filesystem improvements.
* Update development workflow documentation and scripts.
* Add integration with `clang-tidy`.
* Add assets uploading to Github on release.

## [0.54.2 2020-01-19]

### Added

* Add passcode changing.

### Fixed

* Fix the persistent answer and reject labels in the call window.
* Fix the wrong call duration issue in both Call and Calls apps.
* Fix call ignoring.

### Other

* Enable modem's debug output.
* Enable timers enhanced debug.
* Fix licensing information.

## [0.54.1 2020-01-18]

### Fixed

* Fix settings broken by the accidentally removed script.

## [0.53.2 2020-01-15]

### Added

* Add hardware in the loop tests.
* Add APN settings window.
* Add New/Edit APN window
* Add APN options window

### Changed

* Improve clock scaling and power management.
* Load input language from a file.
* Improve English messages displayed to the user when managing contacts list and messages.
* Change gain settings in the audio path to reduce echo.

### Fixed

* Fix crash when displaying system information window in settings.
* Fix not audible outgoing voice during calls.
* Fix crashing on multiple headphones connect-disconnect events.
* Fix keyboard input mode in the search windows.
* Fix missing notification dot when receiving a new message.

### Other

* Add GUI service documentation.
* Fix query-callback synchronization mechanism.
* Remove the requirement to start a commit message with the "Change" verb.

## [0.53.1 2020-01-11]

### Added

* Add battery brownout detection.
* Add several improvements to power saving.
* Add APN settings.

### Changed

* Change battery icons.

### Fixed

* Fix duplicated actions due to an error in events generation.

### Other

* Several changes related to the change of the filesystem.
* Switch to JSON format for localization files.
* Documentation fixes.

## [0.52.1 2020-12-23]

### Added

* Add battery critical level check.
* Add Bluetooth settings storing.
* Add custom repeat window for the alarm application.
* Add GUI for custom MMI messages.
* Add CPU frequency shift mechanism to save power.

### Fixed

* Fix auto-unlocking screen on idle.
* Fix missing texts in the desktop application.
* Fix occasional cases of no audio during an incoming call.

### Changed

* Torch light control button press time recognition.

### Other

* Several changes improving audio code stability.
* Several fixes to unit and harness tests.
* Changes related to VFS cleanup.
* Updated GUI documentation regarding DOM.

## [0.51.1 2020-12-18]

### Added

* Add screen light control.
* Add SIM switch setting.

### Changed

* Several database stability improvements.
* Change audio data path to solve synchronization issues.

### Fixed

* Fix empty list behavior.
* Fix input and display language settings.

### Removed

* Remove flawed log dumping mechanism.

### Other

* Add Ecoboot download target.
* Add several minor documentation fixes.
* Add commit format checker to the CI workflow.
* Add several VFS related fixes.

## [0.50.1 2020-12-14]

### Added

* Add window for MMI/USSD internal messages.
* Add windows for the alarm application.
* Add "nightshift" settings window.
* Add Bluetooth settings.

### Fixed

* Fix default SIM setting.
* Fix internal microphone echo and noise problem.
* Fix several problems with USSD.

### Other

* Add ICS parser for the calendar application.
* Clock configuration changes.
* Add power measurement features to the system.

## [0.49.1 2020-12-04]

### Added

* Add MMI/USSD communication window.
* Add Notes application.
* Add notes search engine.
* Add info window for incoming MMI messages.
* Add vertical text scrolling.
* Add text cursor starting position handling.
* Add logs dumping to a file on the RT1051 platform.

### Changed

* Change note characters limit to 4'000.

### Fixed

* Fix interval chimes choice not scaling with meditation time.
* Fix too many time options in the meditation application.
* Fix default counter settings.
* Fix displaying the special characters in a note title.


### Other

* Rewritten tests and updated test documentation.
* Split clock domains on the RT1051 platform.

## [0.48.1 2020-11-27]

### Added

* `[cellular]` Add basic GUI flow for SIM cards.
* `[text]` Add vertical text navigation without scrolling.

### Fixed

* `[audio]` Fix playback of mono sound when using headphones.
* `[audio]` Fix builtin microphone handling.

### Changed

* `[settings]` Load language setting from the settings file.
* `[cellular]` Improve MMI call forwarding support.

### Other

* `[test]` Add unit tests for time display.
* `[test]` Add unit tests for file indexer to database integration.
* `[build]` Fix emulator build error for the release target

## [0.47.2 2020-11-23]

### Added

* `[gui]` Added support for variable numbers in RichText
* `[gui][desktop]` Added SIM PIN basic flow implementation
* `[cellular]` Added CLIR, CLIP, COLP, call waiting, call barring MMIs support

### Fixed

* `[call]` Fixed issues with incoming calls.
* `[messages]` Fix changing the content of options windows when forwarding a message.
* `[messages]` Fixed saving a draft message for a phone number.
* `[messages]` Fixed usage of special characters while writing a new message.

### Changed

* `[messages]` Changed fonts of message snippet and its prefix.

## [0.47.1 2020-11-20]

### Added

* `[audio]` Added support for a headset microphone.
* `[cellular]` Added SIM PIN/PUK handling.
* `[cellular]` Added change PIN functionality.
* `[cellular]` Added a possibility to remove SIM PIN check on startup (SIM unlocking).
* `[usb]` First implementation of USB's CDC and MTP protocols for communication with Mudita Center.
* `[cellular]` Support for MMI Call Forwarding call.

### Changed

* `[text]` Text input boundaries support added (max signs or max widget area).

### Fixed

* `[audio]` Fixed handling of headset microphone LDO DCDC.
* `[desktop]` Absent call button on the desktop with notifications screen fix.
* `[desktop]` Fixed system freeze when pressing the "Back" button while focused on the desktop screen.

### Other

* `[test]` Added message sending harness test.
* `[gui][desktop]` SIM PIN flow charts added.

## [0.46.1 2020-11-13]

## Added

* `[cellular]` Added call request type recognition (URC).
* `[calendar]` Added a new field for event's date when the user adds/edits an event.
* `[file indexer db]` Added messages for File Indexer database.
* `[settings]` Added Torch window (front-end only).
* `[audio]` Added support for Bluetooth audio profiles
* `[filesystem]` Added support for standard file IO library.
* `[messages]` Added fetching text messages at phone startup.

## Changed

* `[calendar]` Changed default option to 'all day' event.
* `[bus]` Refactored message handling.
* `[appmgr]` Translating messages to actions introduced. 

### Fixed

* `[messages]` Fixed clearing text on options/recipient choice in the new message window.
* `[Text]` Fixed newline handling.
* `[appmgr]` Fixed applications closing.
* `[audio]` Fixed crash on attempt to open a non-existent file

## [0.45.1 2020-11-06]

### Added

* `[cellular]` Added PIN setting.
* `[bluetooth]` Added HSP playback and recording.
* `[cellular]` Added support for CMS and CME errors.
* `[music player]` Added new samples of music files.

### Fixed

* `[cellular]` Fixed SIM handling.
* `[GUI][messages]` Fixed not showing a phone number of temporary contact being deleted.
* `[GUI]` Fixed filling a label with color.
* `[calculator]` Fix freezing on the '=' keypress.
* `[messages][cellular]` Fixed handling messages when the recipient field does not contain a phone number.
* `[meditation]` Fixed crash on gong sound.
* `[vfs]` Improve data integrity by reducing filesystem operation on databases.

### Other

* `[testing]` Moved test harness into service-desktop.
* `[system]` Application manager actions introduced.

## [0.44.1 2020-10-30]

### Added

* `[settings][bluetooth]` Add "Phone name" window.
* `[cellular]` Add support for modem reset.
* `[cellular]` Obtain time zone through network.
* `[audio]` Add state notifications.
* `[antenna]` Enable antenna service.
* `[audio]` Added Mudita audio assets.
* `[gui]` Add dark mode switch.

### Fixed

* `[desktop][messages]` Fixed notifications display and navigation
* `[cellular]` Fixed 32 bit UCS2 codes handling.
* `[call]` Fixed incorrect start of call duration timer.
* `[GUI]` Minor refresh race fixed.
* `[calendar]` Fixed crash on event addition.

### Other

* `[doc]` Application manager documentation added.
* `[audio]` Improve synchronization during calls.
* `[system]` Application manager refactoring and improvements.

## [0.43.1 2020-10-23]

### Added

* `[meditation]` Added gong sound on meditation intervals.
* `[meditation]` Added basic meditation functionalities and settings.
* `[cellular]` Handling incoming calls and sms in sleep mode.
* `[audio][bluetooth]` Added Bluetooth A2DP playback
* `[settings][bluetooth]` Create "Add device" windows.
* `[hardware]` Slider driver (offline/online mode selection)

### Fixed

* `[meditation]` Fix time options display on RT1051.
* `[audio]` Fix headphones autodetection.
* `[audio]` Cumulative set of minor fixes and improvements.
* `[db][vfs]` Fixed database corruption issues on abrupt phone reset.

### Other

* `[audio]` Improve synchronization when switching operations.
* `[vfs]`  Use partition image file for Phone filesystem on Linux emulator.

## [0.42.2 2020-10-16]

### Added

* `[calculator]` Add calculator application.
* `[calendar]` Add screen notifications for events.
* `[settings]` Create window for messages settings.
* `[settings]` Add window for keypad light settings.
* `[doc]` Added open source license.

### Changed

* `[settings]` Removed the border around the settings menu.
* `[messages]` SMS bubble edges smoothing.
* `[cellular]` Temporarily removed SMS memory setting.
* `[phonebook]` Contacts list rebuilding in place, not from the start.

### Fixed

* `[audio]` Fix parsing audio meta tags during playback.
* `[simulator]` Fix crash on exit.
* `[listview]` Fix looping rebuild.
* `[messages]` Fix draft msg overwriting SMS input text.
* `[phonebook]` Fix crash on contact details application exit.
* `[gui]` Fix refresh on special character insert in a special input.

## [0.42.1 2020-10-12]

### Added

* `[messages]` Added network time synchronization.
* `[gui][desktop]` Added "How to unlock" screen info window.
* `[fonts]` Added multiplication and division sign to the font files.
* `[bluetooth]` Added storing of BT link keys.
* `[audio]` Added vibration/enable sound database logic.
* `[settings]` Added the main window for apps and tools.
* `[meditation]` Added a meditation progress bar a with simple implementation of meditation application.

### Changed

* `[messages]` Split SMS thread for a specific contact into separate ones if multiple phone numbers are available.

### Fixed

* `[db]` Fixed database corruption issues on abrupt phone reset.
* `[audio]` Fix audio looping.
* `[phonebook]` Fix phone freezing when contacts database is empty.

### Other

* `[audio]` Added vibration logic (the effect is visible in logs only).
* `[audio]` Improvements to API and logic.
* `[messages]` Refactored messages SMS thread window to use list view. 
* `[settings]` Improvements to API and logic.
* `[gui]` Added timers.
* `[doc]` Several updates to user and developer documentation.

## [0.40.1 2020-10-02]

### Added

* `[antenna]` Add parameter history window.
* `[cellular]` Added USSD codes handling.
* `[desktop]` Add SIM and PUK lock-related windows.
* `[calendar]` Add handling of notifications from the database.
* `[cellular]` Add DTMF tone generation.
* `[settings]` Add basic display and keypad options window.
* `[renderer]` Add ability to draw circles and arcs.
* `[bluetooth]` Add support for scanning and pairing.

### Changed

* `[specialinput]` Added emoji selector and changed special character selector.
* `[messages]` Save a new message as a message draft, if not sent.
* `[messages]` Save unsent messages as SMS drafts.
* `[audio]` Enabled sounds priorities, looping and merging.
* `[phonebook]` Updated scrolling contacts list with a particular letter.

### Fixed

* `[calendar]` Fixed custom repeat option.
* `[settings]` Fixed a system crash on language change.

### Other

* `[audio]` Audio service and API cleanup/refactor.
* `[audio]` Add audio volume control per functionality.
* `[doc]` Updated user documentation.
* `[system]` Improved destroying of worker threads.

## [0.39.1 2020-09-25]

### Added

* `[gui]` Add rich text parsing for text styling needs.
* `[audio]` New volume buttons features.
* `[settings]` Add basic Bluetooth options window.
* `[phonebook]`Scrolling contacts list with a particular letter.
* `[antenna-app]` Added parameter history window.

### Changed

* `[phonenumber]` Change default country to unknown.
* `[messages]` Save unsent messages as SMS drafts.
* `[settings]` Update main screen.

### Fixed

* `[calendar]` Fixed custom repeat option.
* `[settings]` Fixed items sizes and positions in Date/Time window.
* `[phonebook]` Fixed duplication of a phone number when adding or editing an ex-temporary contact.
* `[messages]` Fixed sending SMS to new or deleted contact.
* `[cellular]` Fixed outgoing call stuck in the calling state.
* `[settings]` Fixed a system crash on language change.

### Other

* `[listview]` Added ability to rebuild on provided data offset.
* `[messages]` Cleanup of database service API.
* `[audio]` Add audio volume control per functionality.

## [0.38.2 2020-09-18]

### Added

* `[music player]` Add layout for the playback window.

### Other

* `[build]` Move user data to SDRAM during linking.
* `[build]` RT1051's linker script cleanup.

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
