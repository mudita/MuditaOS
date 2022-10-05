# MuditaOS changelog - PurePhone

## Unreleased

### Changed / Improved
* Added Polish translation to a calendar title
* Changed USB logging
* Separated system volume from Bluetooth device volume for A2DP

### Fixed
* Fixed wrong time displayed on password locked screen with 'Quotes' or 'Logo' wallpaper
* Fixed issue with inability to send SMS
* Fixed mixed SMS messages
* Fixed disappearing manual alarm and vibration volume setting in German
* Fixed SIM card pop-ups not showing
* Fixed lost bytes in logs
* Fixed passcode lock time discrepancy between lock screen and 'Wrong password' popup
* Fixed cellular DMA errors
* Fixed order of the services while closing system
* Fixed crash of the E-ink service while restoring system data
* Fixed removing wrong sentinels
* Fixed music player behaviour when connecting/disconnecting audio devices
* Fixed dropping the call during the DND mode
* Fixed wrong tethering popup order
* Fixed crash during phone turn off
* Fixed improper duration of the rejected outgoing call shown in calls log
* Fixed Bluetooth volume control issues
* Fixed turning on loudspeaker before outgoing call is answered
* Fixed PLAY label translation in German
* Fixed USB connection/disconnection detection
* Fixed memory leaks in APN settings

### Added
* Added basic MMS handling

## [1.3.0 2022-08-04]

### Added

#### Bluetooth:

* Battery and network status notifications for Hands-Free Profile (HFP).

#### Call:

* Option to remove all calls from the call list.

#### Quotes:

* Added missing translations.

### Changed / Improved

#### Pure’s life on battery:

* Reduced USB stack's power consumption.
* Pure battery life tweaks.
* Power management improvements.

#### Call:

* Updated translations.
* Improved phone number handling.

#### Bluetooth:

* Improved device pairing process.
* Improved infotainment compatibility.

#### Contacts:

* Contacts UI change (fonts).
* Improved audio stability.

### Fixed

* Fixed bug of disappearing text while editing contacts.
* Fixed issue of Bluetooth not turning off after 10 minutes of inactivity.
* Fixed issue with devices appearing in the list of paired devices.
* Fixed issue with the failure to disconnect the call using the hardware buttons of the BT device.
* Fixed issue with needing to enter a code when pairing Pure with wireless headphones.
* Fixed problem with Pure reconnecting to Bluetooth device after disconnecting it.
* Fixed problem with disconnecting Pure with Bluetooth devices.
* Fixed problem with switching to the ongoing call screen immediately when connection is in progress.
* Fixed problem with rejecting calls using message template.
* Fixed problem of no audio for incoming calls/messages.
* Fixed issue with the display of a notification on a call from private number.
* Fixed problem with the failure to reject an incoming call after selecting a message template.
* Fixed problem with the failure to finish an outgoing call when it is rejected.
* Fixed problem with handling calls using a headset.
* Fixed issue with playback pausing when an unsupported format file appears in the playlist.
* Fixed problem with unable to resume audio playback after connecting a headset.
* Fixed problem with phone’s reboots while listening .wav/mp3.
* Fixed problem of loading quotes when displayed as wallpaper.
* Fixed issue with Pure screen refresh after the phone is inactive for a long time.
* Fixed issue with the phone freezing when the battery is very low while charging.
* Fixed issues with the phone connecting to the network after turning it on.
* Fixed flashlight shutdown in critical low-power event.
* Fixed issue with the alarm list not updating after adding a new alarm.
* Fixed functionality of adding contacts from the message screen.
* Fixed bug of incoming call notifications appearing during tethering.
* Fixed problem with tethering popup showing after plugging in USB even if no SIM is present.
* Fixed problem with Pure unlocking after a missed call from favorite contact.
* Fixed problem with Pure’s freezes on the Onboarding screen.
* Fixed no refresh of cellular signal bar after startup.
* Fixed problem with phone spontaneously unlocking when switching from DND to Connected mode.
* Fixed factory reset from bootloader.
* Fixed Mudita Center synchronization failure.
* Fixed problem with SIM card reading when switching it in settings.
* Fixed wrong calendar dates.
* Fixed problem with saving notes.
* Fixed problem with restoring the backup.
* Fixed problem with keyboard buttons hanging up.

## [1.2.1 2022-06-10]

### Improved
* Improved audio quality

## [1.2.0 2022-04-01]

### Added

#### Quotes:
* Possibility to set periodically drawn quotes as wallpaper - predefined and custom.

#### Messages:
* Text writing option - "Abc" mode.

#### Contacts:
* Functionality to support the display of long contact names.

#### General:
* Extended time for displaying instructions to unlock the device.

#### Music Player:
* Extended audio files format support.

### Changed / Improved

#### General:
* Changed the default text input type from ABC mode to Abc mode.

### Fixed

* Fixed problem with missing gong at the beginning and the end of meditation process.
* Fixed issue with setting a time of 0 minutes for meditation.
* Fixed issue with the inability to delete characters when entering the meditation duration.
* Fixed problem with missing interval gong in default settings in Meditation.
* Fixed problem with receiving messages in offline mode.
* Fixed problem with previewing longer messages as they are entered.
* Fixed problem with restarting a fully discharged Pure.
* Fixed a problem with navigating through longer lists available in the system.
* Fixed problem with no sound for incoming calls/messages.
* Fixed problem with keyboard stuttering.
* Fixed problem with the phone hanging up after connecting/disconnecting the USB cable.
* Fixed problem with alarm turning off when Pure is locked.
* Fixed problem with alarm going off during active snooze time.
* Fixed problem with incorrect display of time on Home Screen.
* Fixed problem with leaving emergency call dialing screen on locked screen.
* Fixed problem with keyboard unlock instructions disappearing too quickly.
* Fixed an issue with unread messages/missed calls appearing on the lock screen when opened.
* Fixed problem with displaying the list of available languages in non-alphabetical order.
* Fixed problem with wrong order of items - date and time editing screen.
* Fixed issue with text in other languages being cut off too long in Contacts.
* Fixed problem with editing/adding numbers when there is a dash in the phone number.
* Fixed problem with alphabetical sorting of contacts that do not contain the last name.
* Fixed problem of being able to edit a deleted contact.
* Fixed issue with the text informing about the Mudita OS update that appears during onboarding.
* Fixed problem with using call log when more than 900 accounts are loaded.
* Fixed problem with missing call information from the private number in the notification.
* Fixed issue with the inability to interrupt an incoming quick call.
* Fixed translations when adding, editing and deleting contacts from the call log.
* Fixed issue with restoring data on Pure using Mudita Center.
* Fixed issue with FLAC file playback.
* Fixed problem with Music Player application crashing when trying to play files with unknown formats.
* Fixed problem with resuming music playback after connecting a headset.
* Fixed problem of battery discharging of phone without SIM card.

## [1.1.6 2022-01-20]

### Changed / Improved
* Changed speaker equalizer settings.

### Fixed

* Fixed battery charging critical issues.
* Fixed critical battery state window getting stuck.

## [1.1.5 2021-11-30]

### Changed

#### Bluetooth:

* The default device name has been changed.

#### Keyboard:

* Keyboard backlight settings changed.

#### UI/UX:

* Mudita logo display on turned off phone disabled.

### Fixed

* Fixed design of icons in Calls app.
* Fixed issue with incorrect display of battery status after device restart.
* Fixed issue with the screen backlight turning off too quickly.
* Fixed problem with inability to set alarm snooze.
* Fixed problem with data reset when custom alarm settings are entered.
* Fixed problem with receiving messages when phone is offline mode.
* Fixed errors in text translations in Messages.
* Fixed problem with the incorrect message when trying to enter a PIN code.
* Improved the sound quality of headphones during a phone call over HSP.
* Fixed problem with music playing after disconnecting and reconnecting BT device.
* Fixed problem with contacts disappearing after editing them.

## [1.0.2 2021-11-08]
## [1.0.1 2021-11-08]

1.0.2 has the same scope as 1.0.1, we released it due to the minor issue with the version name in 1.0.1.

### Fixed
* Fixed problem with contacts disappearing after editing them.
* Fixed problem with MTP on Windows.
