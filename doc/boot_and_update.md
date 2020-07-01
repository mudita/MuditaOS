1. ecoboot reads /.boot.ini and /.boot.ini.crc32
   and verifies if crc32 in /.boot.ini.crc32 matches
   the actual crc32 of /.boot.ini

2. if ecoboot can't read /.boot.ini and/or /.boot.ini.crc32
   it tries to read /.boot.ini.bak and /.boot.ini.bak.crc32
   and verifies the checksum of /.boot.ini.bak.
   If the /.boot.ini.bak file passes the checksum test ecoboot
   should fix /.boot.ini and /.boot.ini.crc32 files so that
   PureOS can pick up what version is booted.

3. if **(1)** and **(2)** fails ecoboot reads `/boot.bin` and loads it 
   (failsafe)

4. `boot.ini` contains the filename to load in a
   simple INI format  
```
[global]  
boot = current/boot.bin  
type = current  
```

There should be 2 instances of the OS on the
phone (`/sys` is assumed at vfs class creation time)

```  
"current"  -> /sys/current  
"previous" -> /sys/previous  
```

The type variable in `boot.ini` is for the PureOS
this will indicate a subdirectory name to append
for all file operations (loading assets dbs etc.) 
When the option becoms possible this should be passed
as a variable to the boot.bin (PureOS) as an argument

4. ecoboot boots the "**boot**" filename in `boot.ini`.

   PureOS reads `/boot.ini` to find it's root directory
   and reads all assets and files from it. 
   
5. updating from old style partitioning (1 partition)
   to new partition scheme (2 partitions)
   
switch the phone to MSC mode (bootloader option 4)

unmount disk from the OS so that it's not used
find the disk name that is assigned to the phone (sda, sdb etc.)

cd into project root dir and run the partitioning script,

``replace /dev/sdX with the disk name you found for your phone``
```
$ sudo ./config/partition_emmc.sh /dev/sdX -f
```
   example output would be:
```
[atom@urethra:~/devel/PurePhone_10]$ sudo ./config/partition_emmc.sh /dev/sda -f
/dev/sda is not mounted
/dev/sda has 2 partitions
Checking that no-one is using this disk right now ... OK

Disk /dev/sda: 14,62 GiB, 15678308352 bytes, 30621696 sectors
Disk model: NXP MASS STORAGE
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x09650eb4

Old situation:

Device     Boot    Start      End  Sectors  Size Id Type
/dev/sda1  *        2048 28524543 28522496 13,6G  b W95 FAT32
/dev/sda2       28524544 30621695  2097152    1G  b W95 FAT32

>>> Script header accepted.
>>> Script header accepted.
>>> Script header accepted.
>>> Script header accepted.
>>> Created a new DOS disklabel with disk identifier 0x09650eb4.
/dev/sda1: Created a new partition 1 of type 'W95 FAT32' and of size 13,6 GiB.
Partition #1 contains a vfat signature.
/dev/sda2: Created a new partition 2 of type 'W95 FAT32' and of size 1 GiB.
Partition #2 contains a vfat signature.
/dev/sda3: Done.

New situation:
Disklabel type: dos
Disk identifier: 0x09650eb4

Device     Boot    Start      End  Sectors  Size Id Type
/dev/sda1  *        2048 28524543 28522496 13,6G  b W95 FAT32
/dev/sda2       28524544 30621695  2097152    1G  b W95 FAT32

The partition table has been altered.
Calling ioctl() to re-read partition table.
Syncing disks.
create FATs
FAT: PUREOS /dev/sda1
mkfs.fat 4.1 (2017-01-24)
FAT: RECOVER /dev/sda2
mkfs.fat 4.1 (2017-01-24)
probe new partitions to OS
```
once completed, flash the phone with the latest os build, to do that
mount the first partition of the phone to any location (ex. /mnt)

`` replace /dev/sdX1 with the disk name``

``$ sound mount /dev/sdX1 /mnt``

now run the flash script (assuming you built your OS in the build-rt1051-Debug folder)
```
[atom@urethra:~/devel/PurePhone_10]$ sudo ./flash_eMMC.sh ./build-rt1051-Debug
PurePhone remove all files
Create directories
PurePhone copy build files
'./build-rt1051-Debug/boot.bin' -> '/mnt/current/boot.bin'
Copying assets
'./build-rt1051-Debug/assets' -> '/mnt/current/assets'
'./build-rt1051-Debug/assets/profiles' -> '/mnt/current/assets/profiles'
'./build-rt1051-Debug/assets/profiles/template.kprof' -> '/mnt/current/assets/profiles/template.kprof'
'./build-rt1051-Debug/assets/profiles/phone.kprof' -> '/mnt/current/assets/profiles/phone.kprof'
'./build-rt1051-Debug/assets/profiles/lang_sp_upper.kprof' -> '/mnt/current/assets/profiles/lang_sp_upper.kprof'
'./build-rt1051-Debug/assets/profiles/lang_sp_lower.kprof' -> '/mnt/current/assets/profiles/lang_sp_lower.kprof'
'./build-rt1051-Debug/assets/profiles/lang_pl_upper.kprof' -> '/mnt/current/assets/profiles/lang_pl_upper.kprof'
'./build-rt1051-Debug/assets/profiles/lang_pl_lower.kprof' -> '/mnt/current/assets/profiles/lang_pl_lower.kprof'
'./build-rt1051-Debug/assets/profiles/lang_eng_upper.kprof' -> '/mnt/current/assets/profiles/lang_eng_upper.kprof'
'./build-rt1051-Debug/assets/profiles/lang_de_upper.kprof' -> '/mnt/current/assets/profiles/lang_de_upper.kprof'
'./build-rt1051-Debug/assets/profiles/numeric.kprof' -> '/mnt/current/assets/profiles/numeric.kprof'
'./build-rt1051-Debug/assets/profiles/lang_eng_lower.kprof' -> '/mnt/current/assets/profiles/lang_eng_lower.kprof'
'./build-rt1051-Debug/assets/profiles/lang_de_lower.kprof' -> '/mnt/current/assets/profiles/lang_de_lower.kprof'
'./build-rt1051-Debug/assets/images' -> '/mnt/current/assets/images'
'./build-rt1051-Debug/assets/images/phonebook_search_glass_grey.vpi' -> '/mnt/current/assets/images/phonebook_search_glass_grey.vpi'
'./build-rt1051-Debug/assets/images/menu_calendar_W_G.vpi' -> '/mnt/current/assets/images/menu_calendar_W_G.vpi'
'./build-rt1051-Debug/assets/images/menu_contacts_W_G.vpi' -> '/mnt/current/assets/images/menu_contacts_W_G.vpi'
'./build-rt1051-Debug/assets/images/mail.vpi' -> '/mnt/current/assets/images/mail.vpi'
'./build-rt1051-Debug/assets/images/logo.vpi' -> '/mnt/current/assets/images/logo.vpi'
'./build-rt1051-Debug/assets/images/left_label_arrow.vpi' -> '/mnt/current/assets/images/left_label_arrow.vpi'
'./build-rt1051-Debug/assets/images/signal1.vpi' -> '/mnt/current/assets/images/signal1.vpi'
'./build-rt1051-Debug/assets/images/enter_icon_alpha_W_M.vpi' -> '/mnt/current/assets/images/enter_icon_alpha_W_M.vpi'
'./build-rt1051-Debug/assets/images/calllog_arrow_out.mpi' -> '/mnt/current/assets/images/calllog_arrow_out.mpi'
'./build-rt1051-Debug/assets/images/calllog_arrow_in.mpi' -> '/mnt/current/assets/images/calllog_arrow_in.mpi'
'./build-rt1051-Debug/assets/images/messages_error_W_M.vpi' -> '/mnt/current/assets/images/messages_error_W_M.vpi'
'./build-rt1051-Debug/assets/images/mute.vpi' -> '/mnt/current/assets/images/mute.vpi'
'./build-rt1051-Debug/assets/images/homescreen-locked.vpi' -> '/mnt/current/assets/images/homescreen-locked.vpi'
'./build-rt1051-Debug/assets/images/battery1.vpi' -> '/mnt/current/assets/images/battery1.vpi'
'./build-rt1051-Debug/assets/images/battery5.vpi' -> '/mnt/current/assets/images/battery5.vpi'
'./build-rt1051-Debug/assets/images/battery4.vpi' -> '/mnt/current/assets/images/battery4.vpi'
'./build-rt1051-Debug/assets/images/battery3.vpi' -> '/mnt/current/assets/images/battery3.vpi'
'./build-rt1051-Debug/assets/images/lock.vpi' -> '/mnt/current/assets/images/lock.vpi'
'./build-rt1051-Debug/assets/images/dot_12px_hard_alpha_W_G.vpi' -> '/mnt/current/assets/images/dot_12px_hard_alpha_W_G.vpi'
'./build-rt1051-Debug/assets/images/battery2.vpi' -> '/mnt/current/assets/images/battery2.vpi'
'./build-rt1051-Debug/assets/images/signal5.vpi' -> '/mnt/current/assets/images/signal5.vpi'
'./build-rt1051-Debug/assets/images/small_circle.vpi' -> '/mnt/current/assets/images/small_circle.vpi'
'./build-rt1051-Debug/assets/images/menu_settings_W_G.vpi' -> '/mnt/current/assets/images/menu_settings_W_G.vpi'
'./build-rt1051-Debug/assets/images/simunknown.vpi' -> '/mnt/current/assets/images/simunknown.vpi'
'./build-rt1051-Debug/assets/images/speaker.vpi' -> '/mnt/current/assets/images/speaker.vpi'
'./build-rt1051-Debug/assets/images/menu_meditation_W_G.vpi' -> '/mnt/current/assets/images/menu_meditation_W_G.vpi'
'./build-rt1051-Debug/assets/images/circle_bottom.mpi' -> '/mnt/current/assets/images/circle_bottom.mpi'
'./build-rt1051-Debug/assets/images/phonebook_empty_grey_circle_W_G.vpi' -> '/mnt/current/assets/images/phonebook_empty_grey_circle_W_G.vpi'
'./build-rt1051-Debug/assets/images/calllog_arrow_den.mpi' -> '/mnt/current/assets/images/calllog_arrow_den.mpi'
'./build-rt1051-Debug/assets/images/info_big_circle_W_G.vpi' -> '/mnt/current/assets/images/info_big_circle_W_G.vpi'
'./build-rt1051-Debug/assets/images/dot_12px_hard_alpha_W_M.vpi' -> '/mnt/current/assets/images/dot_12px_hard_alpha_W_M.vpi'
'./build-rt1051-Debug/assets/images/menu_phone_W_G.vpi' -> '/mnt/current/assets/images/menu_phone_W_G.vpi'
'./build-rt1051-Debug/assets/images/menu_tools_calculator_W_G.vpi' -> '/mnt/current/assets/images/menu_tools_calculator_W_G.vpi'
'./build-rt1051-Debug/assets/images/signal0.vpi' -> '/mnt/current/assets/images/signal0.vpi'
'./build-rt1051-Debug/assets/images/menu_alarm_W_G.vpi' -> '/mnt/current/assets/images/menu_alarm_W_G.vpi'
'./build-rt1051-Debug/assets/images/arrow_left.vpi' -> '/mnt/current/assets/images/arrow_left.vpi'
'./build-rt1051-Debug/assets/images/menu_messages_W_G.vpi' -> '/mnt/current/assets/images/menu_messages_W_G.vpi'
'./build-rt1051-Debug/assets/images/battery0.vpi' -> '/mnt/current/assets/images/battery0.vpi'
'./build-rt1051-Debug/assets/images/menu_music_player_W_G.vpi' -> '/mnt/current/assets/images/menu_music_player_W_G.vpi'
'./build-rt1051-Debug/assets/images/menu_tools_W_G.vpi' -> '/mnt/current/assets/images/menu_tools_W_G.vpi'
'./build-rt1051-Debug/assets/images/right_label_arrow.vpi' -> '/mnt/current/assets/images/right_label_arrow.vpi'
'./build-rt1051-Debug/assets/images/menu_tools_recorder_W_G.vpi' -> '/mnt/current/assets/images/menu_tools_recorder_W_G.vpi'
'./build-rt1051-Debug/assets/images/microphone_off.vpi' -> '/mnt/current/assets/images/microphone_off.vpi'
'./build-rt1051-Debug/assets/images/pin_lock_info.vpi' -> '/mnt/current/assets/images/pin_lock_info.vpi'
'./build-rt1051-Debug/assets/images/microphone_on.vpi' -> '/mnt/current/assets/images/microphone_on.vpi'
'./build-rt1051-Debug/assets/images/cross.vpi' -> '/mnt/current/assets/images/cross.vpi'
'./build-rt1051-Debug/assets/images/phone.vpi' -> '/mnt/current/assets/images/phone.vpi'
'./build-rt1051-Debug/assets/images/phonebook_search_arrow_grey.vpi' -> '/mnt/current/assets/images/phonebook_search_arrow_grey.vpi'
'./build-rt1051-Debug/assets/images/phonebook_blocked_large.mpi' -> '/mnt/current/assets/images/phonebook_blocked_large.mpi'
'./build-rt1051-Debug/assets/images/speaker_on.vpi' -> '/mnt/current/assets/images/speaker_on.vpi'
'./build-rt1051-Debug/assets/images/phonebook_blocked_large.vpi' -> '/mnt/current/assets/images/phonebook_blocked_large.vpi'
'./build-rt1051-Debug/assets/images/phonebook_contact_delete_trashcan.vpi' -> '/mnt/current/assets/images/phonebook_contact_delete_trashcan.vpi'
'./build-rt1051-Debug/assets/images/phonebook_info.vpi' -> '/mnt/current/assets/images/phonebook_info.vpi'
'./build-rt1051-Debug/assets/images/search_big.mpi' -> '/mnt/current/assets/images/search_big.mpi'
'./build-rt1051-Debug/assets/images/phonebook_phone_ringing.vpi' -> '/mnt/current/assets/images/phonebook_phone_ringing.vpi'
'./build-rt1051-Debug/assets/images/circle_top.mpi' -> '/mnt/current/assets/images/circle_top.mpi'
'./build-rt1051-Debug/assets/images/pin_lock.vpi' -> '/mnt/current/assets/images/pin_lock.vpi'
'./build-rt1051-Debug/assets/images/phonebook_small.vpi' -> '/mnt/current/assets/images/phonebook_small.vpi'
'./build-rt1051-Debug/assets/images/search.vpi' -> '/mnt/current/assets/images/search.vpi'
'./build-rt1051-Debug/assets/images/signal2.vpi' -> '/mnt/current/assets/images/signal2.vpi'
'./build-rt1051-Debug/assets/images/signal3.vpi' -> '/mnt/current/assets/images/signal3.vpi'
'./build-rt1051-Debug/assets/images/signal4.vpi' -> '/mnt/current/assets/images/signal4.vpi'
'./build-rt1051-Debug/assets/images/phonebook_search_arrow_down_grey.vpi' -> '/mnt/current/assets/images/phonebook_search_arrow_down_grey.vpi'
'./build-rt1051-Debug/assets/images/sim2.vpi' -> '/mnt/current/assets/images/sim2.vpi'
'./build-rt1051-Debug/assets/images/sim1.vpi' -> '/mnt/current/assets/images/sim1.vpi'
'./build-rt1051-Debug/assets/images/arrow_right.vpi' -> '/mnt/current/assets/images/arrow_right.vpi'
'./build-rt1051-Debug/assets/images/simfail.vpi' -> '/mnt/current/assets/images/simfail.vpi'
'./build-rt1051-Debug/assets/images/small_heart.vpi' -> '/mnt/current/assets/images/small_heart.vpi'
'./build-rt1051-Debug/assets/images/small_tick.vpi' -> '/mnt/current/assets/images/small_tick.vpi'
'./build-rt1051-Debug/assets/images/call_message.vpi' -> '/mnt/current/assets/images/call_message.vpi'
'./build-rt1051-Debug/assets/images/menu_tools_notes_W_G.vpi' -> '/mnt/current/assets/images/menu_tools_notes_W_G.vpi'
'./build-rt1051-Debug/assets/images/homescreen_lock.mpi' -> '/mnt/current/assets/images/homescreen_lock.mpi'
'./build-rt1051-Debug/assets/images/speaker_off.vpi' -> '/mnt/current/assets/images/speaker_off.vpi'
'./build-rt1051-Debug/assets/fonts' -> '/mnt/current/assets/fonts'
'./build-rt1051-Debug/assets/fonts/gt_pressura_regular_30.mpf' -> '/mnt/current/assets/fonts/gt_pressura_regular_30.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_light_30.mpf' -> '/mnt/current/assets/fonts/gt_pressura_light_30.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_light_27.mpf' -> '/mnt/current/assets/fonts/gt_pressura_light_27.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_bold_30.mpf' -> '/mnt/current/assets/fonts/gt_pressura_bold_30.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_bold_27.mpf' -> '/mnt/current/assets/fonts/gt_pressura_bold_27.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_bold_32.mpf' -> '/mnt/current/assets/fonts/gt_pressura_bold_32.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_bold_20.mpf' -> '/mnt/current/assets/fonts/gt_pressura_bold_20.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_regular_20.mpf' -> '/mnt/current/assets/fonts/gt_pressura_regular_20.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_regular_27.mpf' -> '/mnt/current/assets/fonts/gt_pressura_regular_27.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_regular_24.mpf' -> '/mnt/current/assets/fonts/gt_pressura_regular_24.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_bold_24.mpf' -> '/mnt/current/assets/fonts/gt_pressura_bold_24.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_light_90.mpf' -> '/mnt/current/assets/fonts/gt_pressura_light_90.mpf'
'./build-rt1051-Debug/assets/fonts/gt_pressura_light_46.mpf' -> '/mnt/current/assets/fonts/gt_pressura_light_46.mpf'
'./build-rt1051-Debug/assets/lang' -> '/mnt/current/assets/lang'
'./build-rt1051-Debug/assets/lang/lang_en.json' -> '/mnt/current/assets/lang/lang_en.json'
'./build-rt1051-Debug/assets/lang/lang_sp.json' -> '/mnt/current/assets/lang/lang_sp.json'
'./build-rt1051-Debug/assets/lang/lang_de.json' -> '/mnt/current/assets/lang/lang_de.json'
'./build-rt1051-Debug/assets/lang/lang_pl.json' -> '/mnt/current/assets/lang/lang_pl.json'
'./build-rt1051-Debug/assets/audio' -> '/mnt/current/assets/audio'
'./build-rt1051-Debug/assets/audio/ringtone.wav' -> '/mnt/current/assets/audio/ringtone.wav'
'./build-rt1051-Debug/assets/audio/alarm_clock.mp3' -> '/mnt/current/assets/audio/alarm_clock.mp3'
Copying country-codes.db
'./build-rt1051-Debug/country-codes.db' -> '/mnt/current/country-codes.db'
Copying Luts.bin
'./build-rt1051-Debug/Luts.bin' -> '/mnt/current/Luts.bin'
Copying user
'./build-rt1051-Debug/user' -> '/mnt/current/user'
'./build-rt1051-Debug/user/settings.db' -> '/mnt/current/user/settings.db'
'./build-rt1051-Debug/user/calllog.db' -> '/mnt/current/user/calllog.db'
'./build-rt1051-Debug/user/contacts.db' -> '/mnt/current/user/contacts.db'
'./build-rt1051-Debug/user/sms.db' -> '/mnt/current/user/sms.db'
'./build-rt1051-Debug/user/notes.db' -> '/mnt/current/user/notes.db'
Copying .boot.ini
'./build-rt1051-Debug/.boot.ini' -> '/mnt/current/.boot.ini'
Copying .boot.ini.crc32
'./build-rt1051-Debug/.boot.ini.crc32' -> '/mnt/current/.boot.ini.crc32'
Moving /mnt/current/user to /mnt
Moving /mnt/current/.boot.ini to /mnt
Moving /mnt/current/.boot.ini.crc32 to /mnt
PurePhone copied

PurePhone unmouted
PurePhone ejected
Done. You can reset PurePhone now

```