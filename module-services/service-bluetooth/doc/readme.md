# Bluetooth Service
The Bluetooth Service is responsible for communicating the Bluetooth subsystem with rest of the operating system. 

## Connecting to the Bluetooth device
To be able to connect to the device with more than one [audio profile](https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles), the Profile Manager has been developed.
It allows connecting to the A2DP and HSP profiles at once while maintaining the flexibility of switching between them without any additional lag, which could happen during reconnecting to the device.
It provides the following functionalities:
- connecting/disconnecting to the device
- switching between A2DP (high-quality audio) and HSP (headset profile) back and forth
- stream control: pause, resume, start, stop

A temporary user interface is located in place of the old Bluetooth settings.

The Bluetooth message API will be unified shortly to allow integrating it with the audio service.
