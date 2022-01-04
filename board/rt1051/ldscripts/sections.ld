/*
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2018
 * Generated linker script file for MIMXRT1052xxxxB
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v10.2.0 [Build 759] [2018-05-15]  on 2018-07-30 10:43:20
 *
 * Modified by Mudita
 */

ENTRY(ResetISR)

__ocram_noncached_start = ORIGIN(SRAM_OC);
__ocram_noncached_end = ORIGIN(SRAM_OC) + LENGTH(SRAM_OC);

__dtcm_ram_start = ORIGIN(SRAM_DTC);
__dtcm_ram_end = ORIGIN(SRAM_DTC) + LENGTH(SRAM_DTC);

__sdram_cached_start = ORIGIN(BOARD_SDRAM_HEAP);
__sdram_cached_end = ORIGIN(BOARD_SDRAM_HEAP) + LENGTH(BOARD_SDRAM_HEAP);

EXTERN(image_vector_table);
EXTERN(boot_data);
EXTERN(hyperflash_config);

/* Ensure that newlib runs the finalizers.  */
__libc_fini = _fini;

SECTIONS
{
    /* Image Vector Table and Boot Data for booting from external flash */
    .boot_hdr : ALIGN(4)
    {
        FILL(0x00)
        __boot_hdr_start__ = ABSOLUTE(.) ;
        KEEP(*(.boot_hdr.conf))
        . = 0x400 ;
        KEEP(*(.boot_hdr.ivt))
        . = 0x420 ;
        KEEP(*(.boot_hdr.boot_data))
        . = 0x430 ;
        KEEP(*(.boot_hdr.dcd_data))
        __boot_hdr_end__ = ABSOLUTE(.) ;
        . = 0x1000 ;
    } > BOARD_SDRAM_TEXT

    /* Data sections descriptors for startup initialization */
    .text : ALIGN(4)
    {
        FILL(0x00)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        /* System data */
        LONG(LOADADDR(.sysdata));
        LONG(    ADDR(.sysdata));
        LONG(  SIZEOF(.sysdata));
        /* User data */
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        /* System bss */
        LONG(    ADDR(.sysbss));
        LONG(  SIZEOF(.sysbss));
        /* User bss */
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */
    } > BOARD_SDRAM_TEXT

    /* Init code */
    .text : ALIGN(4)
    {
        *(.after_vectors*)
    } > BOARD_SDRAM_TEXT

    /* System uninitialized data */
    .sysbss : ALIGN(4)
    {
        *(NonCacheable)
        *(COMMON)
        *libmodule-os.a:*(.bss*)
        *libmodule-sys.a:*(.bss*)
        *libmodule-bsp.a:*(.bss*)
        *libusb_stack.a:*(.bss*)
    } > SRAM_DTC

    /* System initialized data */
    .sysdata : ALIGN(4)
    {
        *(NonCacheable.init)
        *(.ramfunc*)
        *libmodule-os.a:*(.data*)
        *libmodule-sys.a:*(.data*)
        *libmodule-bsp.a:*(.data*)
        *libusb_stack.a:*(.data*)
    } > SRAM_DTC AT > BOARD_SDRAM_TEXT

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);

        /* C++ constructors etc */
        . = ALIGN(4);
        KEEP(*(.init))

        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;
        
        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        . = ALIGN(4);
        /* End of C++ support */
    } > BOARD_SDRAM_TEXT

    /* Exception handling */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > BOARD_SDRAM_TEXT
    .ARM.exidx : ALIGN(4)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > BOARD_SDRAM_TEXT

    /* Main DATA section (SDRAM) */
    .data : ALIGN(4)
    {
        FILL(0xff)
        _data = . ;
        *(vtable)
        *(.data*)
        . = ALIGN(4) ;
        _edata = . ;
    } > BOARD_SDRAM_TEXT

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > BOARD_SDRAM_TEXT

    /* ext SDRAM */
    .sdram (NOLOAD) : ALIGN(4)
    {
        *(.sdram)
        *(.sdram.*)
    } >BOARD_SDRAM_HEAP

    /* DMA buffers */
    .intramnoncacheable (NOLOAD) : ALIGN(4)
    {
        *(.intramnoncacheable)
        *(.intramnoncacheable.*)
    } > SRAM_OC

    /* Place system stack at the very end of DTCM */
    _StackSize = 0x1000;
    .stack ORIGIN(SRAM_DTC) + LENGTH(SRAM_DTC) - _StackSize - 0 : ALIGN(4)
    {
        _vStackBase = .;
        . = ALIGN(4);
        _vStackTop = . + _StackSize;
    } > SRAM_DTC
}
