# Memory configuration for the RT1051 platform

## Memory interfaces

There are 4 memory interfaces available for the platform. Three of them connect to FlexRAM inside the MCU, fourth is an external SDRAM.
Inside the chip there are:
* OCRAM - clocked with peripheral clock.
* TCM - clocked with CPU clock. TCM is a high speed memory interface which allows to access data via two 32bit DTCM interfaces and/or instructions via a single 64 bit interface.

Currently RT1051 offers:
* 64 kB of OCRAM memory;
* 448 kB of DTCM memory;
* 16 MB of SDRAM memory.

## Firmware in the memory

During linking firmware is split between memory regions. Data that
need to be accessed fast is placed inside the chip (system code and
data mostly), 
* data that cannot be cached (`NonCacheable*`),
* system data, including system heap in (see `heap_4.c`),
* common symbols (e.g. `errno` variable),
* system stack (used before starting scheduler and by scheduler itself),
* relocated "fast" functions (`.ramfunc`)

The rest is placed in the SDRAM split between two regions:
* user code, including exception handling (`BOARD_SDRAM_TEXT`),
* big buffers, including user heap (see `usermem.c`, region `BOARD_SDRAM_HEAP`)

Currently there is no data in the OCRAM region, though it could be used for DMA buffers.
