# MuditaOS changelog

** Not suitable for the T7 hardware version. **

To install the new version of MuditaOS please follow the steps described in this guide:
- [Linux](https://github.com/mudita/MuditaOS/blob/master/doc/boot_and_update.md#creating-a-storage-partition)
- [Windows and macOS](https://github.com/mudita/MuditaOS/blob/master/doc/flashing_win_macos.md)

## [0.75.1 2021-08-02]

* [BH-380] Librarize application-calendar
* [BH-564] Add Time&Units application
* [BH-564] Time&Units scaling fix
* [BH-663] Fix CMake in service-antenna
* [BH-669] Fix CMake in service-appmgr
* [BH-671] Fixed CMake in service-audio
* [BH-675] Add TextSpinner
* [BH-677] Add widget AlarmSetSpinner
* [BH-700] Add minimal Alarm app
* [CP-207] Add option to disable dev mode endpoint
* [CP-225] Create contact using post method, and update it using put
* [CP-261] Print crash occurences to log
* [CP-371] Updater miscelanous developer mode and logs changes
* [CP-434] Restore checksums.txt for legacy update
* [EGD-6723] Music Player application skeleton
* [EGD-6881] Fix HSP no sound
* [EGD-7031] Remove UT deps to the production image
* [EGD-7083] Fixed earspeaker bt HSP
* [EGD-7127] Store imported contacts in DB
* [EGD-7161] Remove image name reference in vfs
* [EGD-7163] Disable auto locking when playing music
* [EGD-7204] Revert of msp register and clear IRQ flags
* [EGD-7204] Set msp register and clear IRQ flags
* [EGD-7206] Fix audio profiles priorities
* [EGD-7211] Sim contacts import selector UI part
* [EGD-7227] Fix update package generation

## [0.74.1 2021-07-26]

* [BH-362] Move segger to a separate directory
* [BH-375] Clean `module-utils/third-party`
* [BH-377] Liberalize application special input
* [BH-381] Turned application call into lib
* [BH-382] Rebase and cleaning
* [BH-384] Turned application desktop to library
* [BH-385] Turned application meditation into lib
* [BH-464] Fix make all
* [BH-628] Cleaned up module-vfs cmake
* [BH-655] Add SideListView
* [BH-672] SideListView integration with ListViewEngine
* [BH-674] TimeSetSpinner
* [CP-215] Expose current operator name in service desktop API
* [CP-254] Use Send File for Update package
* [CP-260] Use gdb with py to debug crash dumps
* [CP-264] Ensure bluetooth logging
* [CP-265] Extend cellular logging
* [CP-267] Ensure cellular logging
* [CP-280] Implement send file procedure
* [CP-281] Block restoration of mock messages
* [CP-282] Add md5sums to the version.json file
* [CP-282] Set correct standalone image dependencies
* [CP-288] Update secureboot documentation
* [CP-316] Tool to get MuditaOS log
* [CP-319] Extends reboot reason code
* [CP-325] Request reboot code adjusted to work with harness
* [CP-326] Reformat recovery partition as littlefs
* [CP-327] Fix UB in QueryResult
* [CP-335] Fix formatting of SN
* [CP-355] Pure update full flow implemented
* [EGD-3171] Fix key shortRelease on app switch
* [EGD-5006] Add French localizations
* [EGD-5659] Cpu frequency for audio
* [EGD-6212] Add tethering doc
* [EGD-6609] Separated out sms logic
* [EGD-6624] Audio support for linux simulator
* [EGD-6672] Fix missing key after slider
* [EGD-6889] Fix Fixed network time handling
* [EGD-6928] Fix system powe instability
* [EGD-6971] TextLine breaking on space or with dash
* [EGD-6973] Fix Auto date is on by default
* [EGD-7005] Removed sensitive data cellular
* [EGD-7010] Remove sensitive data from logs
* [EGD-7024] Keep the state of SMS on phone lock
* [EGD-7036] Audio tracks updated
* [EGD-7052] Generate unique window name
* [EGD-7074] Fix date and time unit tests
* [EGD-7088] SDRAM configuration on T7 board
* [EGD-7096] Add reaction after changing BT device name
* [EGD-7097] Introduce BT name length limit
* [EGD-7108] Fix disconnecting all devices during unpairing
* [EGD-7114] Updated reboot messages
* [EGD-7120] Remove old settings application
* [EGD-7124] Add import contacts from sim
* [EGD-7130] Renamed settings application
* [EGD-7131] Flashing instruction updated
* [EGD-7133] Bluetooth setting cleanup
* [EGD-7134] Network setting cleanup and fixes
* [EGD-7135] Display and keypad setting cleanup and fixes
* [EGD-7136] Apps setting cleanup and fixes
* [EGD-7136] Phone modes setting cleanup and fixes
* [EGD-7138] Security setting cleanup and fixes
* [EGD-7139] System setting cleanup and fixes
* [EGD-7141] Fix GUI googletest
* [EGD-7142] Fix compilation issues with __PRETTY_FUNCTION__
* [EGD-7145] Fix mp3 tags fetch
* [EGD-7149] Fix connecting BT devices
* [EGD-7162] Removed sensitive data from settings app
* [EGD-7164] ListViewEngine separation from ListView
* [EGD-7169] Changed genlfs path
* [EGD-7176] Switch GH Actions docker image to company's one
* [EGD-7187] Update docker maintainer
* [EGD-7199] Switch weekly releases to T6

## [0.73.1 2021-07-09]

* [BH-386] Turned app messages into lib
* [CP-312] Add reboot to update function
* [CP-314] Allow genlittlefs to create empty part
* [CP-315] Remove old fat only partition layout
* [EGD-6697] Fix DateTime::isYesterday() tests
* [EGD-6955] Add calls notifications in DND
* [EGD-6956] Add offline calls notifications
* [EGD-6957] Add sms notifications in DnD
* [EGD-7006] Removed sensitive data from db
* [EGD-7007] Removed sensitive data from gui
* [EGD-7026] Fix nullptr calls and unlock behaviour
* [EGD-7040] Notes autosave
* [EGD-7043] Added 3 localizations
* [EGD-7043] Fixes in German JSON
* [EGD-7043] Fixes in Polish localization
* [EGD-7043] Reverted the decimal separator for Polish
* [EGD-7044] Strip down calendar code
* [EGD-7047] Fixed navigation down through input texts
* [EGD-7048] Remove "Automatic time zone" from settings
* [EGD-7050] Fixed reboot copy
* [EGD-7051] Fixed saving multiple notes at once
* [EGD-7071] Text paste fixed
* [EGD-7074] Removed incorrect and failing unit tests
* [EGD-7077] Don't allow for connecting more than 1 BT device at once
* [EGD-7081] Disconnect forgotten BT device
* [EGD-7084] No scrollbar on All devices list
* [EGD-7089] Fix torch color change when locked
* [EGD-7091] Forgetting forgets incorrect BT device
* [EGD-7100] Fix shuttered audio
* [EGD-7105] Fix window with timeout
* [EGD-7111] Restore calendarEvents endpoint number

## [0.72.1 2021-07-05]

* [BH-363] Move libphonenumber to a separate directory
* [BH-379] Librarize application-calculator
* [BH-390] Turned notes app into library
* [BH-464] Moved USB stack to third-party dir
* [BH-591] Add Spinner Widget
* [CP-183] Do not log sensitive data in service-desktop
* [CP-248] Harness script to transfer Update package to Pure
* [CP-253] Add checksum to package transfer
* [CP-270] Replace CRC32 library
* [EGD-6697] Fix DateTime tests
* [EGD-6699] Add timezone selecting window
* [EGD-6978] Outgoing calls fixes
* [EGD-6990] No autolock when battery is low
* [EGD-7003] Removed sensitive data bt
* [EGD-7004] Removed sensitive data

## [0.71.1 2021-06-30]

* [BH-372] Turned i18n into separate library
* [BH-374] Fix ENABLE\_APP mechanism + adjust includes policy
* [BH-374] Librarize application-antenna
* [BH-388] Turned music player app into library
* [BH-392] Turned application phonebook into lib
* [BH-459] Moved fatfs to third-party v2
* [BH-495] Cleaned up Bell Hybrid App cmake
* [BH-495] Created Bell Hybrid home screen
* [BH-501] MainMenu
* [CP-163] Implement FS Endpoint read file
* [CP-166] Update Harness Tests
* [CP-174] Remove limit of maximum attempts of unlocking phone
* [CP-194] Intercept all input when OS update window is displayed
* [CP-200] Delete file with old OS version
* [CP-200] Revert Delete file with old OS version
* [CP-206] Report Pure serial number in USB descriptors
* [EGD-5709] Add full actions support in call application
* [EGD-5709] Fixed a crash on switching phone modes
* [EGD-5709] Fixed actions support in Call application
* [EGD-6041] Fix unlock windows behaviour
* [EGD-6514] HSP volume control
* [EGD-6697] Date and time formatting
* [EGD-6745] Add tool to create JIRA version
* [EGD-6797] Remove color from SAR
* [EGD-6876] Fix Factory Settings Test
* [EGD-6882] Fix call hangup after BT device disconnect
* [EGD-6883] Change time settings cleanup
* [EGD-6947] Fix SQL vulnerabilities
* [EGD-6950] Added header layout
* [EGD-6951] Removed old header implementation
* [EGD-6952] Fix tethering on low battery
* [EGD-6954] Enable all unit tests
* [EGD-6958] Fix rssibar oor exception
* [EGD-6960] Header addition related style cleanups
* [EGD-6966] Fixed a switch from call window back
* [EGD-6970] Autolock default value
* [EGD-6972] Update BT stack to 1.4.1 version
* [EGD-6979] Fix for "Unable to catch hard faults on rt1051 via GDB"
* [EGD-6986] Add Store timezone in settings db
* [EGD-6994] Remove sensitive data logging
* [EGD-6995] Remove sensitive data from logs
* [EGD-6999] Removed sensitive data
* [EGD-7000] Removed sensitive data
* [EGD-7015] Revert "test"
* [EGD-7018] Fixed text empty block removal at end
* [EGD-7029] Fix running unit tests in release script

## [0.70.1 2021-06-21]

* [BH-393] Create separate math lib
* [BH-515] Create main Bell app
* [CP-142] Update tests with 204 response code
* [CP-143] Set 204 code for responses with empty body
* [EGD-2949] Fix fallback image
* [EGD-3243] Fix cellular memory leaks
* [EGD-4384] Status bar roaming indicator
* [EGD-5011] Set headphones maximal sound level
* [EGD-5689] Add clang tidy to jenkins
* [EGD-6702] Add time zone library
* [EGD-6790] Fix cellular sleep mode
* [EGD-6916] Fixed makefile build linking issue
* [EGD-6918] Timestamp clean up
* [EGD-6932] Small cellular refactor

## [0.69.1 2021-06-16]

### Added

* [EGD-6896] Sanitize Tray event handling
* [BH-353] Add minimal build target for BH
* [CP-84] Implement Security API
* [CP-145] Implement Security Endpoint helper
* [EGD-6907] Introduce ModemState in Cellular
* [BH-353] Add Documentation for main targets

### Changed

* [EGD-6897] Add force flag to the pureflash
* [EGD-6897] Change force flag to disable check
* [BH-460] Moved TPLIB littlefs
* [BH-371] Replace gsl with mainline submodule and move to third-party
* [EGD-6867] Renamed TopBar into StatusBar
* [EGD-6898] Remove dead SimProcedure code
* [BH-396] Move CrashDebug to a separate directory
* [BH-360] Move TPLIB re2 to separate dir
* [CP-83] Update tests for unlocking phone and usb
* [BH-378] Utils move log to library
* [BH-458] Pureflash turned to ExternalProject
* [EGD-6906] Updated OnBoarding update info window
* [CP-140] Deactivate autolock while connected to MC
* [BH-370] Convert utils common_data into a libs
* [BH-369] Turned UTF8 into separate library
* [BH-368] Added missing CMAkeLists
* [BH-368] Moved board to library
* [BH-387] Move utils/state to service-antenna
* [BH-367] Convert UCS2 to library
* [BH-376] Time module unit test decoupled
* [BH-376] Utils time turned to separate library
* [EGD-6544] Switch for VoLTE
* [EGD-6917] Screen light in automode does not turnoff itself
* [BH-373] Convert utils json into a lib

### Fixed

* [EGD-6035] Fix support for glibc-2.32
* [EGD-6899] Fix AppCall access
* [EGD-6446] Fix notes delete window
* [CP-148] Fix setting of response in the Parser Context
* [EGD-6684] Fix call ended message
* [EGD-6875] Fix UT check script
* [BH-369] Fixed UTF8 unit test CI run
* [EGD-6884] Fix autolock in sms template
* [CP-187] Fix stack overflow on passcode decoding

## [0.68.1 2021-06-04]

### Added

* Add Onboarding application.
* Add iNotify mechanism for the VFS.

### Changed

* Hide calls and messages notifications while tethering is on.
* SIM handling refactored.
* Automatic locking disabled when torch is on.
* Automatic locking disabled during OS update.

### Fixed

* Fix preserving automatic display light setting between phone starts.
* Fix current time displaying in "Date and time" window.

## [0.67.1 2021-05-28]

### Added

* Add a per-application auto-locking policy.
* Add documentation on auto-locking mechanism.
* Add RC oscillator self-calibrating to increase the precision of the system clock.
* Add support for Bluetooth pairing with a PIN.
* Add time updating with a value obtained from a GSM network.
* Add using voice transcoding during phone calls if needed.

### Changed

* Block building commits that are not rebased to reduce CI workload.
* Move CI builds to a RAM disk to reduce build times.
* Move several third-party libraries to a single `third-party` subdirectory.

### Fixed

* Fix displaying currently selected ringtone in the ringtone settings.
* Fix excessive ATA command when answering a phone call using headset buttons.
* Fix invalid integer to string conversion used in the call records.
* Fix reconnecting Pure to the Mudita Center when unlocked.
* Fix RTC clock not being retained between resets.

## [0.66.1 2021-05-21]

### Added

* Add autolocking settings.
* Add EEPROM block device driver.
* Add enabling screen light on a keypress.
* Add front light brightness automatic adjustment.
* Add key bindings documentation for Bell's simulator.
* Add settings guidelines documentation.
* Add SIM select window in the onboarding process.
* Add single number message notifications.

### Changed

* Add several code quality improvements to the build system.
* Cancel Jenkins builds on PR update.
* Disable logs and allocations in interrupts to prevent crashing.
* Disable WAL mechanism in databases.
* Don't show contents of PurePhone in Center when PurePhone is locked.
* Enable changing simulator's screen size at compilation time.
* Reject uploads of non-audio files via MTP.
* Remove logging database's pragma errors.
* Update notifications icons.

### Fixed

* Fix building for Glibc v2.33.
* Fix crashing progress bar during an update.
* Fix NAT widget to be visible only during a call.
* Fix parsing of boot descriptor file.

## [0.65.1 2021-05-19]

### Added

* Add build system options to the quickstart documentation.
* Add EEPROM low-level driver.
* Add primary code owners configuration.

### Fixed

* Fix changing Bluetooth device name.
* Fix communication with GSM modem on Linux,
* Fix display backlight turning on in the off state.
* Fix issues with sending messages from the new message window.
* Fix network settings window rebuilding on SIM selection.
* Fix returning to the correct screen from the SOS call window.
* Fix simulator build for Valgrind.

## [0.64.1 2021-05-12]

### Added

* Add a warning when adding a calendar event is invalid, e.g., has an empty title.
* Add Bluetooth notifications playback when Bluetooth audio device is connected.
* Add compilation caching to the Jenkins CI.
* Add displaying factory data in the technical information window.
* Add documentation for home notifications.
* Add minimum documentation on filesystem support.
* Add timeout on meditation end screen to automatically return to the main window of meditation application.

### Changed

* Improve audio block size calculation algorithm to reduce voice latency in a phone call.
* Remove excessive logs from the USB stack.
* Tweak call window alignment.

### Fixed

* Fix Bluetooth default entries not being loaded at startup.
* Fix crash when searching for notes.
* Fix crash when trying to set a new calendar event and change its date or time.
* Fix database and GUI unit tests.
* Fix displaying Bluetooth name in the Bluetooth settings window.
* Fix labels in Calls and Meditation applications.
* Fix numeric inputs.
* Fix repeated tethering notification.
* Fix switching to a loudspeaker with headphones plugged in.
* Fix switching window after editing a contact.

## [0.63.2 2021-05-05]

### Added

* Add "connecting" and "pairing" statuses to the Bluetooth connectivity settings window.
* Add audio transcoding framework with basic sample rate transcoding.
* Add handling of Pause/Resume/Stop headset controls while in the music player application.
* Add Out-of-Memory hooks to the system for better protection against failures related to memory handling.
* Add settings initialization from factory data.
* Add tethering notification.
* Add tool to generate EEPROM image with factory data.
* Add volume level feedback for Bluetooth audio devices.

### Changed

* Add a distinction between rejected and ended the call when presenting finished call status to the user.
* Adjust OS to be used with Secure Boot.
* Change calculator behavior to finish the existing equation before starting the next one.
* Enable cyclic database writes.
* Enable power saving for filesystem operations.
* Enable power saving for the USB peripheral.
* Limit digits to 7 in the calculator.
* Remake cellular flow for better error handling.
* Switch CI process from Github actions to Jenkins server.

### Fixed

* Fix audio assets' tags not being displayed in the list view.
* Fix invalid "block contact" icon.
* Fix keypad backlight random state changes when on battery power.
* Fix meditation start sound.
* Fix no "send" text on the bottom bar while receiving a call.
* Fix phone crashes after playing ringtone in the new alarm window.
* Fix several issues with APN settings.
* Fix the SIM card icon in the top bar.

## [0.63.1 2021-04-26]

### Added

* Add "On when active" keypad light setting handling.
* Add `clang-tidy` to the CI process.
* Add answering or rejecting phone calls with wired headset controls.
* Add API for controlling disks' power usage.
* Add audio format checking before audio playback.
* Add clearing user input in the calculator on the `#` key press.
* Add code coverage reporting.
* Add connecting to GSM network in user-defined intervals.
* Add controlling charge current depending on the temperature of the battery.
* Add current value selection depending on the USB connection type.
* Add EEPROM emulation in the Linux simulator.
* Add HSP ringing on a call with the ability to reject or answer the call with headset controls.
* Add keypad backlight turning on during a call.
* Add logic to change SIM PIN.
* Add low power mode to magnetometer handling.
* Add low-power handling during Bluetooth communication.
* Add mechanism to automount the `mfgconf` partition.
* Add per-module unit testing.
* Add ringtone selection UI.
* Add support for eMMC system partitions.
* Add windows for setting passcode during the onboarding process.

### Changed

* Add only last project changes during project release creation by the CI.
* Change all `emulator` references in the documentation to `simulator`.
* Cleanup database API.
* Cleanup localization files.
* Disable phone mode changes when tethering is enabled.
* Don't allow to edit exponent output in the calculator.
* Limit calculator decimals to 6 digits.
* Refactor audio settings API.
* Refactor handling of phone modes in applications.
* Turn audio on only when the user is using audio to save energy.
* Unify localization API.
* Update audio assets.
* Update tests in the tests harness.
* Update tethering icon.

### Fixed

* Fix assets copying issues during the build process.
* Fix building Linux simulator with `glibc` older than `2.28`. 
* Fix building SystemView.
* Fix calculator operations on empty input.
* Fix crash when navigating left on search results in the phonebook.
* Fix crashes in list view related to options handling.
* Fix crashing on single char removal in the text input widget.
* Fix device unresponsiveness when opening settings window.
* Fix display backlight settings not being persistent.
* Fix displaying complete contact list after ICE call.
* Fix displaying wrong contact info in the call log.
* Fix ghosting in the meditation application.
* Fix handling unit tests in IDEs by removing unnecessary messages from `libiosyscalls`.
* Fix inability to find a contact with its full name.
* Fix inability to unlock the phone with proper PIN due to race mode with settings.
* Fix invalid initialization of Bluetooth profiles.
* Fix issues with displaying grey colors.
* Fix phone mode discovery in the tethering mode.
* Fix race between focus gain and the user pressing asterisk button in the SMS thread view.
* Fix several typos in shell scripts.
* Fix SIM card not being detected.
* Fix turning off the phone.
* Fix unnecessary displaying applications' main windows on user actions.

## [0.62.2 2021-04-09]

### Added

* Add factory reset mode.
* Add SIM PIN settings.
* Add tethering confirmation popup.
* Add volume control of Bluetooth headsets.
* Add windows for audio volume and vibration settings.

### Changed

* Disable functional tests with test harness during CI builds.

### Fixed

* Fix CrashDump repository path causing build issues.
* Fix database initialization's unit test.
* Fix phone call muting.

## [0.62.1 2021-04-07]

### Added

* Add a window to display technical information about the device for certification purposes.
* Add calls rejecting in the 'Do not Disturb mode.
* Add crash dumping with CrashCatcher.
* Add detection of headset buttons.
* Add factory reset window.
* Add offline mode handling by the cellular modem.
* Add test harness for cellular in the sleep mode.
* Add window to display SAR parameter value.

### Changed

* Change USB charging current to 500 mA.
* Disable USB security temporarily to prevent connectivity issues with Mudita Center.
* Improve boot time by optimizing the start time of the desktop application.
* Move quotes from JSON file to the database.
* Prevent starting the phone if the battery is below shutdown level.
* Run unit tests on Release configuration instead of Debug during CI checks.
* Split CI build types for unit tests and test harness.
* Update documentation on the cellular modem's low power mode.

### Fixed

* Fix backup/restore API.
* Fix crash in the calendar application when focusing on an all-day event.
* Fix crashing unit tests of the desktop service.
* Fix execution time of test harness during CI runs.
* Fix inability to unlock SIM with PIN.
* Fix invalid language fallback on assets load error.
* Fix issues with date and time in the onboarding application.
* Fix linking with GCC10.
* Fix several minor issues in automated testing.
* Fix silent boot fail when there is no valid image available.
* Fix storing quotation marks in the notes.
* Fix text addition and deletion for the text line widget.
* Fix USB disconnect time.

## [0.61.1 2021-03-26]

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

## [0.60.1 2021-03-19]

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

## [0.59.1 2021-03-09]

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

## [0.58.1 2021-02-19]

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

## [0.57.1 2021-02-11]

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

## [0.56.1 2021-02-03]

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

## [0.55.1 2021-01-28]

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

## [0.54.3 2021-01-22]

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

## [0.54.2 2021-01-19]

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

## [0.54.1 2021-01-18]

### Fixed

* Fix settings broken by the accidentally removed script.

## [0.53.2 2021-01-15]

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

## [0.53.1 2021-01-11]

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
