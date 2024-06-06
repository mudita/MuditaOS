# MuditaOS changelog - Harmony

## Unreleased

### Fixed
* Fixed incorrect Power Management statistics in the logs

### Added
* Added custom alarms functionality
* Added update instruction screen at end of onboarding
* Added update instruction screen in Settings

### Changed / Improved
* Changed the refresh rate of the progress bar screen
* Extended range of supported chargers
* Changed order of options in Settings menu
* Improved device stability with low quality USB cables

## [2.7.0 2024-05-20]

### Fixed
* Fixed the backlight blink in pre-wake up
* Fixed missing tooltip text in Onboarding in French

### Added
* Added unique sound to indicate the meditation session has ended.
* Added Focus Timer application.

### Changed / Improved
* Updated button handling during pre-wake up
* Updated countdown timer in Meditation app according to the new design
* Updated countdown timer in Power Nap app according to the new design
* Updated countdown timer in Relaxation app according to the new design

## [2.6.1 2024-04-02]

### Fixed
* Fixed BIWIN eMMC memory errors
* Fixed UI freeze after changing any volume intensively

### Changed / Improved
* Updated FSL drivers from NXP
* Increased speed of update process

## [2.6.0 2024-03-07]

### Fixed
* Fixed device freeze after changing Relaxation volume intensively

### Added
* Added new 32px and 170px fonts
* Added new clock face with quotes
* Added a backend for quotes on home screen
* Added progress bar to update process
* Added low battery notification on home screen with quotes

### Changed / Improved
* Increased battery charger stack size

## [2.5.0 2024-02-09]

### Fixed
* Fixed source clock frequency computation for PWM module
* Fixed initial watchdog configuration
* Fixed alarm rings when deactivated during snooze
* Fixed popup about file deletion showing in Relaxation app even if no file was deleted
* Fixed displaying the menu after deep press on main screen
* Fixed display of "Connected" label on home screen

### Added
* Added setting onboarding year to build date year
* Added low battery notification on the home screen
* Added low battery notification before using the application
* Added entering WFI when CPU is idle to reduce power consumption
* Added switching SDRAM to self-refresh before entering WFI for further power consumption reduction
* Added watchdog protection to WFI mode
* Added low battery warning when the alarm is activated

## [2.4.0 2024-01-11]

### Added
* Added gradual alarm volume increase
* Added progress bar for all volume control windows

### Changed / Improved
* Increased clock font in Relaxation, Meditation, Power nap mode
* Changed countdown progress bar design in Relaxation, Meditation and Power Nap apps
* Removed minus sign in progress countdown timers in Relaxation, Meditation and Power Nap apps
* Extended volume scale from 10 to 15 point scale
* Modified volume control characteristic for better user experience
* Reduced power consumption for Meditation and Power Nap applications
* Improved factory reset procedure to remove user files

## [2.3.0 2023-12-20]

### Fixed
* Fixed eink crash while refreshing
* Fixed problem with occasional background playback after song has finished

### Added
* Added brightness fade in functionality
* Added labels to Relaxation
* Added date format setting
* Added colors of noise to Relaxation

### Changed / Improved
* Optimize the way Relaxation is loading music files
* Disabled Address Sanitizer for the Linux simulator
* Changed misleading factory reset translations in Polish
* Time setting updated

## [2.2.3 2023-11-30]

### Fixed

* Fixed eink errors in logs
* Fixed alarm when the onboarding is in progress
* Fixed relaxation start delay when trying to play MP3 files with large metadata

### Added

* Added shortcuts instruction to settings

## [2.2.2 2023-11-14]

### Fixed

* Fixed CPU frequency setting when dumping logs to a file
* Fixed incorrect calculation of requested CPU frequency
* Fixed boot error screen text alignment

### Added
* Added notification when charger is connected

### Changed / Improved
* Disabled USB MTP protocol

## [2.2.1 2023-10-30]

### Fixed
* Fixed meditation countdown timer when a deep press is performed
* Fixed uneven screen refresh when turning on the device
* Fixed not disappearing snooze icon when an alarm is deactivated
* Fixed incorrect message after new alarm setting in some scenarios
* Fixed the possibility of OS crash during update package size check
* Fixed errors that occur when uploading a music file with graphics in the metadata via the Center
* Fixed frequency lock during user activity

### Added
* Files not fully transferred via Center will be now removed when USB cable is unplugged

### Changed / Improved
* Improved the display time of the "turn off" window.
* Hardware faults during device startup now result in shutdown and a more user-friendly message
* Discharged device screen changed

## [2.2.0 2023-09-28]

### Fixed

* Fixed hard fault handling

### Added

* Added a log when manual reset is performed

### Changed / Improved

* Improved device stability related to frequency scaling


## [2.1.0 2023-08-30]

### Fixed

* Fixed problem with an unresponsive device after playing specific WAV files.
* Fixed problem with deleting files during Relaxation session.
* Fixed occasional USB crash when USB cable was disconnected during files upload
* Fixed back button power off timer changed to 10s
* Fixed alarm problems when it was re-set while snooze was still active
* Fixed the problem with the not appearing system closing window in some cases
* Fixed problem with displaying some filenames in Relaxation
* Fixed disabling the alarm on the system shutdown screen
* Fixed "Next alarm will ring in 24h" popup on shutdown screen
* Fixed redundant clock face display while shutting down Harmony
* Fixed problem with disabling the frontlight in pre-wake up
* Fixed occasional crash when a deep press occurs during popups
* Fixed diacritics in translations
* Fixed problem with sounds in relaxation and power nap applications
* Fixed the logic in onboarding screens
* Fixed issues with file uploads with low disk space.
* Fixed information about next alarm ring

### Added

* Added serial number to About section
* Added possibility to detect device's case colour
* Added extended information to crashdump filename
* Added extended information to log filename
* Added Harmony version information in about section
* Added system shutdown if there is no user activity for 180 seconds on the language selection screen

### Changed / Improved

* Information about device memory is now sent to MC in floating points numbers
* General improvement in Eink display and error handling
* Changed the "Turn off Harmony" popup display time to 10 seconds
* The default year has been updated to 2023
* Improved device stability related to frequency scaling

## [2.0.0 2023-06-29]

### Fixed

* Fixed canceling of alarm editing after 10s of inactivity
* Fixed yes/no behavior in factory reset window
* Fixed missing software version in French language
* Fixed problems with displaying file names in Relaxation
* Fixed polish Meditation summary text
* Fixed problems with copying files via Mudita Center to Relaxation
* Fixed problem with long Relaxation loading when titles were too long
* Fixed backlight behavior after returning to the main window
* Fixed settings frontlight intensity in on demand mode
* Fixed problem that UI could be occasionally broken
* Fixed problem with displaying end of title when playing song in loop was selected.
* Fixed USB charging port detection.
* Fixed occasional crash on system startup.
* Fixed the buttons sometimes don't respond on press or release
* Fixed no clock update

### Added

* Added error handling for incorrect audio formats and corrupted files inside Relaxation app
* Added error message when files limit is exceeded in Relaxation app
* Added label informing about PC connection.

### Changed / Improved

* Added new field to deviceInfo endpoint
* Changed order in which files are displayed in Relaxation
* Optimized E-Ink energy consumption
* Changed the layout of the battery status

## [1.9.0 2023-04-03]

### Changed / Improved

#### General:

* Changed filesystem to ext4
* Improved update processes
* Changed updater utility

### Fixed

#### Relaxation:

* Fixed a problem with the pause function not working correctly during relaxation.

#### Pre-wake up:

* Fixed problems with the intensity of the frontlight - the brightness of the backlight differs from that indicated digitally.

#### General:

* Fixed displayed device name when connected to Windows
* Minor firmware fixes/optimizations.
* Reinitialization of eMMC driver after failure

#### UI/UX:

* Added missing translations in French.

## [1.8.2 2023-04-03]

### Added

* Changes introduced in this release are enablers to the release Mudita Harmony 1.9.0.

## [1.8.1 2023-03-07]

### Added

#### UI/UX:

* Added notification that the device needs to be charged before first use.

### Fixed

#### General:

* Fixed an issue where Harmony could not be switched off when the battery was low.
* Fixed a problem with Harmony's power management, causing the alarm clock to discharge in several hours.
* Fixed a problem with Harmony permanently freezing.

## [1.8.0 2022-12-14]

### Added

#### Home Screen:

* Font size increased.
* Clock faces with dates.

#### General:

* Bedside lamp mode.

#### UI/UX:

* Improve refreshing of the display.

#### Onboarding

* Shortcuts instruction.

#### Alarm

* Front light intensity during the main alarm and pre-wake up

### Changed

#### Home Screen:

* Placement of battery and time format indicators.

#### Relaxation:

* New circular progress bar
* Add loop mode

### Fixed

* Brightness calculation for alarms

#### Home Screen:

* Centering of battery indicator.

#### Settings:

* Incorrect time format of the alarm in clock face settings.

## [1.7.0 2022-11-14]

### Added

#### Home Screen:

* Battery status icon.

#### Meditation:

* Option to see Meditation statistics from the last 7, 30, and 365 days.
* Preview a countdown before the Meditation starts.
* New meditation progress layout.
* Additional start delay interval - up to 90s.

#### PowerNap:

* New circular progress bar
* Ringing bell image at the end of the Power Nap

#### Settings:

* Option to set date.

### Fixed

#### Mudita Center:

* Fixed problem with errors while connecting the device to Mudita Center.

#### Snooze:

* Fixed the problem with the alarm light flashing during snooze time.

#### Bedtime reminder:

* Fixed an issue with not pausing music notifications at bedtime.

#### Alarm:

* Fixed problem with wrong alarm status message when setting alarm from the menu.
* Fixed the spelling of the titles of two soundtracks in the alarms application.
* Fixed the issue with the welcome message not being displayed when the alarm is turned off.

#### Meditation:

* Fixed view of diacritic signs.
* Fixed resetting meditation settings with deep press.
* Fixed an issue with saving meditation settings before going through the entire setup process.

#### Relaxation:

* Fixed the navigation of the back button during relaxation.

#### General:

* Fixed inconsistencies in welcome messages.

#### UX/UI:

* Updated screens according to design.

#### Translations:

* Fixed gaps in translations.

## [1.6.0 2022-06-14]

### Added

#### Home Screen:

* Possibility to choose Clock Face from 8 options (3 for 24h format, 5 for 12h format).

#### Meditation:

* Possibility to set a Meditation Timer start delay.

#### UI/UX:

* UI update (Home Screen settings).

#### Translations:

* Completed missing translations.

### Fixed

* Fixed problem with confirmation of language change action.
* Fixed problem with mixed translations appearing after language change.
* Fixed missing texts.
* Fixed issue with the device restarting when trying to turn it off.
* Fixed incorrect unwanted shutdown of device in transport mode.
* Fixed problem with duplicated action after pressing back and lightclick button.
* Fixed problem with blank screen when approving certain settings.
* Fixed problem with Home Screen flashing during startup.
* Fixed problem with relaxation sounds turning off after alarm or bed time deactivation.

## [1.5.1 2022-04-07]

### Fixed

* Fixed issue with the device appearing in service mode when connected via USB.
* Fixed problems with clock desynchronization.

## [1.5.0 2022-02-25]

### Added

#### Settings:

* Ability to scroll between Harmony settings.
* The order of alarm sounds has been defined.

### Changed / Improved

* The default alarm volume has been changed.
* Updated alarm sounds.
* The welcome screen has been updated.

### Fixed

* Fixed problem with Pre wake-up light.
* Fixed problems with turning on the device despite high battery level.
* Fixed problems with incorrect display of battery charge status.
* Fixed issue with the turning on the device when the battery is critical, even if a charger is plugged.
* Fixed problem with the occurrence of duplicate texts.
* Fixed problem with alarm sounds appearing after changing device language.
* Fixed problem with opening the Menu section when confirming the alarm setting.
* Fixed problem with resetting some settings after changing language.
* Fixed gaps in translations.
* Fixed a problem with setting the new alarm sound.

## [1.4.0 2022-02-01]

### Added

#### General:

* Transportation mode.

#### PowerNap:

* Add front light functionality to PowerNap app.

### Changed/Improved

#### UI/UX:

* Changed exit screen with instruction to turn on.
* Update Mudita logo icon.
* Updated fonts.
* Updated the screen at the end of the Onboarding process.

#### Meditation:

* Updated texts.
* The time of appearance of messages has been extended.

#### Alarm:

* Updated the sound of the default alarm.

#### Translations:

* Added missing translations.
* Corrected mistakes in translations.

### Fixed

* Fixed errors in the translation of the incoming alarm message.
* Fixed the operation of the alarm when using other alarm clock functions.
* Fixed problem with the truncated relaxation screen.
* Fixed problem with unwanted change in volume of relaxation sounds.
* Fixed an issue with the Onboarding process.
* Fixed problem with booting when USB is connected.
* Fixed the layout of on-screen messages.
* Fixed problem with displaying wrong screen after power nap.
* Updated screens according to design.
* Fixed function of automatically returning to the home screen after meditation is completed.

