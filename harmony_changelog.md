# MuditaOS changelog - Harmony

## Unreleased

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

### Added

* Added error handling for incorrect audio formats and corrupted files inside Relaxation app
* Added error message when files limit is exceeded in Relaxation app

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

#### UI/UX:

* Added missing translations in French.

#### General:

* Minor firmware fixes/optimizations.

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

