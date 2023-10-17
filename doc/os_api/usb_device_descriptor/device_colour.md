# Device colour

Device colour is added as metadata about the device to the USB descriptor.
The colour of the device is coded in **bcdDevice** field.
Each device colour has assigned a value:
```
none  - 0x0111
black - 0x0112
gray  - 0x0113
``` 
*none* - is used when the information about device colour is not available for the device or is different than specified.

