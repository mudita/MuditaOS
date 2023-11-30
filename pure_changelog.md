# MuditaOS changelog - PurePhone

## Unreleased

### Added

* Added VoLTE support in Denmark, United Kingdom, Netherlands, Canada and Austria

### Changed / Improved

* Hide the SOS label during onboarding when the modem is not ready for an emergency call.
* Anonymization sensitive data in logs added.

### Fixed

* Fixed playback start delay when trying to play MP3 files with large metadata.
* Fixed memory leak in contacts list view.

## [1.10.0 2023-11-24]

### Added

* Added VoLTE support in Germany as an experimental feature.

### Changed / Improved

* Disabled USB MTP protocol.
* Changed UI flow on discharged device window.
* Changed VoLTE label from 'beta' to 'experimental'.
* Changed signal strength update mechanism to avoid unnecessary screen refreshes.

### Fixed

* Fixed possible crash when entering phone number.
* Fixed incorrect calculation of requested CPU frequency.
* Fixed CPU frequency setting when dumping logs to a file.

## [1.9.0 2023-10-19]

### Added

* Added VoLTE support in Poland as an experimental feature.
* Added "beta" label to VoLTE switch for selected countries.
* Added confirmation window when closing an app with unsaved changes.
* Added preserving message text and recipient number when closing new message window.
* Added confirmation window when switching to tethering from an app with unsaved changes.
* Added displaying number entered on the keypad during a call.
* Added automatic change of input language when changing system language.

### Changed / Improved

* Changed roaming indicator to show domestic roaming as home network.
* Improved device stability related to frequency scaling.
* Enabled screen autolock in meditation app when idle.

### Fixed

* Fixed multiple UI issues with input fields in "New alarm" window.
* Fixed misleading "Save" button behavior in "Date and time" window.
* Fixed invalid elapsed time in music player after playing and pausing track from songs list view.
* Fixed disappearing "Call" label in Phonebook app.
* Fixed disappearing selections when setting custom alarm after a popup is shown.
* Fixed alarm preview playback behavior.
* Fixed CPU minimum frequency lock during user activity.
* Fixed crash on transferring audio file with big metadata.
* Fixed possibility of OS crash during update package size check.
* Fixed cut off VoLTE checking label in some languages.
* Fixed inability to save a contact that did not previously have a phone number

## [1.8.0 2023-09-27]

### Added

* Added extended information to crashdump filename.
* Added extended information to log filename.
* Added GUI screens informing about failed MMI/USSD request.
* Files not fully transferred via Center will be now removed when USB cable is unplugged.

### Changed / Improved

* General improvement in Eink display and error handling.
* Optimized ServiceAudio power management.
* Blocked call execution with empty phone number field.

### Fixed

* Fixed unsupported character in several quotes.
* Fixed unwanted autolock on SMS template selection window when rejecting call.
* Fixed scenario when Alarm not being handled properly during a phone call.
* Fixed handsfree device still ringing after caller has hung up.
* Fixed missing contact entries when scrolling through the contacts list.
* Fixed misleading holes in SMS threads.
* Fixed occasional crash when unplugging Pure from PC when connected with Center.
* Fixed ability to create contact with both numbers identical.
* Fixed diacritics in translations.
* Fixed issues with file uploads with low disk space.
* Fixed hard fault handling.
* Fixed issue with uneven first screen redraw after turning the phone on.
* Fixed issue with bypassing phone lock window after unplugging USB cable on tethering popup.
* Fixed Alarm disappearance during end of call window.
* Fixed passcode behavior when blocked from Center.
* Fixed missing tethering icon on "Tethering is on" window.
* Fixed showing "Copy text" option in empty note.
* Fixed "Copy" option missing from the options list in "New message" window.
* Fixed unneeded case sensitiveness in the note search

## [1.7.2 2023-07-28]

### Fixed

* Fixed issue with frequent crashes on some devices that appeared in MuditaOS 1.7.1.

## [1.7.1 2023-07-13]

### Added

* Added missing translations for Bluetooth connection status.
* Added missing translation in French.
* Made EULA window scroll by a few lines at once.

### Fixed
* Fixed occasional USB crash when USB cable was disconnected during files upload.
* Fixed Template window clearing after all templates are removed.
* Fixed problem with an unresponsive device after playing specific WAV files.
* Fixed unresponsive Templates window for user input after templates were changed via MC.
* Fixed redirection to the dial number view by pressing BACK from add new number view.
* Fixed for a ghost call after a quick click back key to end a call.
* Fixed handling USSD started by #.
* Fixed an issue with displaying a previously closed USSD session using a new USSD code.
* Fixed problems occurring with MMI/USSD code handling.
* Fixed misleading SMS notification text indicating the number of unread messages.
* Fixed automatic pasting into the text input field of forwarded messages.
* Fixed missing new message notifications.
* Matched the time limit of the pop-up volume window with the pop-up phone mode window.
* Fixed random crashes when turning off the phone.
* Fixed unneeded deep refresh in the empty list (Alarm/Message/Contacts/Notes).
* Fixed support for shortening multi-line strings in dialogue titles.
* Fixed search engine labels.
* Fixed access to contacts during onboarding in progress.
* Fixed abbreviations of days of the week in Polish.
* Fixed marking new message as read in Messages app main window.

## [1.7.0 2023-03-23]
### Changed / Improved

* Shortened duration of phone modes pop-up from 3s to 1s
* Changed responses in contacts and messages endpoints used to communicate with Center
* Added SMS template call rejection window for case when no templates are defined
* Changed unknown character glyph to the one matching the design
* Changed tethering icon on status bar
* Unified grey color tones used while displaying texts
* Always display network access technology and signal strength on the status bar
* Added days of the week to the list of SMS, calls and notes
* Added date formatting of received/sent SMS
* Added meditation parameters to non-volatile memory
* Added missing translations for legal documents in French, Spanish and Swedish

### Fixed

* Fixed not marking thread as read when new message arrives in the opened thread
* Fixed alarm rings on the low battery screen
* Fixed the notification of unread messages are not deleted with the thread
* Fixed several MTP issues
* Fixed notifications of unread SMS threads on the home screen
* Fixed false dotted notification above Calls app in case of answered call
* Fixed time disappearing in SMS thread
* Fixed scrollbar behavior in call log view
* Fixed contacts imported from SIM not showing up in Mudita Center
* Fixed broken events counter on main screen for more than 99 events
* Fixed incorrect signal strength displayed in Center
* Fixed inability to enter text in search field after clicking right arrow
* Fixed displaying wrong screen after contact removal
* Fixed improper font weight used in navbar
* Fix for country code in new contact based on deleted one
* Fixed the data displayed on snoozed alarms screen
* Fixed displaying an incorrect window after double ending a phone call
* Fixed templates list not looping
* Fixed inability to import contacts from Orange SIM cards
* Fixed improper asterisk button behavior when adding new contact
* Fixed for contacts removed and imported from SIM card once again were added to database without names
* Fixed (removed) redundant leading zero from time representation unless exactly midnight
* Fixed inability to type characters other than digits in USSD replies
* Fixed screen ghosting after emoji selection
* Fixed incorrect loudspeaker icon in call window when headset was connected or disconnected during a call
* Fixed invalid screen displayed after missed call
* Fixed minor issues in the Calculator Application
* Fixed displaying usual SMS template call rejection window when no templates were defined
* Fixed misleading popup on SMS send when modem is rebooting
* Fixed window redirection when clicking SMS icon
* Fixed crash when pressing button after turning off the phone with USB cable connected
* Fixed notification on the home screen shouldn't be bolded
* Fixed long phone shutdown time
* Fixed missing translation for words "show" and "replace"
* Fixed music playback disablement after currently played audio file gets deleted
* Fixed incorrectly displayed Swedish translations in Onboarding
* Fixed missing "No calls yet" text in call log
* Fixed phonebook is not handling database notification about contacts action done by Center
* Fixed contact deleted via Center cannot be edited by Phonebook app and saved again
* Fixed returning to call screen from message template
* Fixed displaying wrong information on screen after rejecting call with SMS template
* Fixed lack of contact search list update when returning to list after contact edition

## [1.6.0 2023-02-27]

### Changed / Improved

* Changed filesystem to ext4
* Improved update/restore/backup processes
* Changed updater utility
* Improved dialog with network via USSD
* Added serial number and timestamp to crashdump filename
* Changed order of starting services, ServiceDesktop moved to the back

### Fixed

* Fixed disappearing "confirm" button in PIN entering screen
* Fixed looping on the SIM card selection screen
* Fixed notes paste option showing for empty clipboard
* Fixed notes window title
* Fixed adding and deleting country code prefix to existing contact
* Fixed French translation for SIM cards texts
* Fixed crash on music file with invalid tags
* Fixed backspace behavior in text edit
* Fixed text not showing when adding/editing contact if text began with 'j' glyph
* Fixed VoLTE switch availability after taking out SIM card
* Fixed improper message text displaying after pasting from clipboard
* Fixed USSD handling crash
* Fixed calls to emergency contact when typed manually
* Fixed message content being deleted when phone mode changed on new message window
* Fixed wrong navigation bar state after exit from custom repeat window
* Fixed OS crash when editing contact by adding country prefix to number
* Fixed unwanted Mudita Center passcode prompt after long press '#' while dialing
* Fixed inability to unblock SIM card when previously ejected during slot switching
* Fixed French translations for Unlock Screen
* Fixed crash when syncing with Mudita Center
* Fixed inactive alarms after timezone change and reboot
* Fixed no sound when Bluetooth audio device connected/disconnected during call
* Fixed full filesystem path displayed in music player for invalid files instead of just filename
* Fixed problem with track info not being displayed correctly
* Fixed crash when trying to play 96kHz FLAC with USB cable connected
* Fixed broken French translation on 'Configure passcode' screen in Onboarding
* Fixed navigation through the ringtone preview list to automatically switch playback to the currently selected option

## [1.5.1 2023-02-27]

### Added

* Added bootstrapped utilities to allow performing next (1.6.0) update

## [1.5.0 2022-12-20]

### Added

#### Messages:

* Added text message support via VoLTE modem.

#### Calls:

* Call support via VoLTE for users in the US

#### Home Screen:

* Added tethering info on status bar.

### Changed / Improved

#### SIM Cards:

* Change windows flow in SIM settings.
* Improved support for reinserting the SIM card into the device during SIM setup.

### Fixed

* Fixed MTP integration.
* Fixed incorrect total CPU usage in logs.
* Fixed disappearing button in PIN entering screen.
* Restored notifications about PIN changes.
* Fixed windows flow after PIN mistakes.
* Fixed music files extensions case sensitivity.
* Fixed locked password wallpaper behavior.
* Fixed access to the phone before going onboarding.
* Fixed the phone hanging up after receiving an empty SMS message.

## [1.4.0 2022-10-06]

### Added

#### MMS:

* Added basic MMS handling.

#### Bluetooth:

* Added state refresh when BT automatically turns off.

### Changed / Improved

#### Calendar:

* Added Polish translation to a calendar title.

#### General:

* Changed USB logging.
* Improved keyboard responsiveness.
* Improved lock screen power consumption.

#### Bluetooth:

* Separated system volume from Bluetooth device volume for A2DP.

### Fixed

* Fixed issue with music not pausing when BT device is disconnected.
* Fixed music player behaviour when connecting/disconnecting audio devices.
* Fixed issue with message handling in which characters not supported by Pure appear.
* Fixed problem with message display on Pure for Verizon network.
* Fixed issue with inability to send SMS.
* Fixed mixed SMS messages.
* Fixed SIM card pop-ups not showing.
* Fixed disappearing manual alarm and vibration volume setting in German.
* Fixed an issue with the screen not displaying a message about changing the phone mode from DND/Offline/Connected.
* Fixed USB connection/disconnection detection.
* Fixed passcode lock time discrepancy between lock screen and 'Wrong password' popup.
* Fixed lost bytes in logs.
* Fixed cellular DMA errors.
* Fixed order of the services while closing system.
* Fixed removing wrong sentinels.
* Fixed crash during phone turn off.
* Fixed PLAY label translation in German.
* Fixed memory leaks in APN settings.
* Fixed poor phone responsiveness.
* Fixed crash on entering PIN settings.
* Fixed crash of the Eink service while restoring system data.
* Fixed dropping the call during the DND mode.
* Fixed improper duration of the rejected outgoing call shown in calls log.
* Fixed turning on loudspeaker before outgoing call is answered.
* Fixed no calllog entries in DND mode.
* Fixed wrong tethering popup order.
* Fixed Bluetooth volume control issues.
* Fixed Bluetooth state first rendering off.
* Fixed crash on importing unprocessable contact.
* Fixed the order in which contacts are displayed in the list.
* Fixed incorrect navigation text in onboarding for timezone and date time.
* Fixed wrong time displayed on password locked screen with 'Quotes' or 'Logo' wallpaper.

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
