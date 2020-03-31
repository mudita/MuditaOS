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