1. ecoboot reads /boot.ini
 
2. if ecoboot can't read `/boot.ini` it tries to 
   read `/boot.bin` and loads it 
   
3. `boot.ini` contains the filename to load in a
   simple INI format  
```
[global]  
boot = current/boot.bin  
type = current  
```

There should be 3 instances of the OS on the
phone (`/sys` is assumed at vfs class creation time)

```  
"current"  -> /sys/current  
"previous" -> /sys/previous  
"failsafe" -> /sys  
```

The type variable in `boot.ini` is for the PureOS
this will indicate a subdirectory name to append
for all file operations (loading assets dbs etc.)

4. ecoboot boots the "**boot**" filename in `boot.ini`.

   PureOS reads `/boot.ini` to find it's root directory
   and reads all assets and files from it