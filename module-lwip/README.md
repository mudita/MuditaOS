# LwIP system integration
========================

# Why?

Bluetooth APN needs IP & routing and level 2 TCP/IP stack.

# How?

LwIP sources needed are added as _shallow_ submodule in lib.
Our needed sources, not from libraries are in interfaces (now only there)

* module-lwip
    One place for LwIP code
    * lib 
        Place for LwIP submodule
    * interfaces
        Our defined LwIP interfaces place. We need:
        * BNEP - bluetooth network encapsulation protocol - interface
        * PPP  - gsm interface for PPP PDP communication
