Bluetooth interface layer
=========================

Right now it properly uses API from bsp/bsp-bluetooth and only Bluetopia stack.  
It's meant to provide interface layer for service-bluetooth like that:
>>> os calls <=> service-bluetooth <=> module-bluetooth <=> bluetooth library
>>>                                                     <=> bsp/bluetooth

For now Bluetopia initiates properly in A2DP mode.
