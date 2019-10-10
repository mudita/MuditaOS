bluetooth stack documentation
=============================

# Introduction

There were a few libraries checked (any of possible at the time):
* serun (Previous PurePhone iteration)
* bluetopia
* bluekitchen

Generaly speaking from checks and experiences Bluetopia prove to be the best.

# Code layout

Code layout presented is bottom to top in order from lowest layer, to highest

* module-bsp
    Our bluetooth chip works on UART with HCI protocol (in H4 mode), code in this directory
    is resposible for communication purposes
    * board/`<arch>`/bluetooth/
        architecture dependent hal layer
    * bluetooth/
        architecture independent hal code for bsp
* module-bluetooth
    Bluetooth library interface code
* module-services/
    * service-bluetooth
        Code for bluetooth devices high level management
* module-ip/
    LwIP interface code for processor
