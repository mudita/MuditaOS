Bluetooth interface layer
=========================

Right now it properly uses API from bsp/bsp-bluetooth and BlueKitchen stack.  
It's meant to provide interface layer for service-bluetooth like that:
>>> os calls <=> service-bluetooth <=> module-bluetooth <=> bluetooth library
>>>                                                     <=> bsp/bluetooth

## Layout
* Bluetooth
    * `interface` code - for other modules to use
    * `glucode` code needed for libraries to work
    * `profiles` bluetooth library function wrappers
* lib
    Libraries used with as little modifications as possible
