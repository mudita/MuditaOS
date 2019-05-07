/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2018
 * Generated linker script file for MIMXRT1052xxxxB
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v10.2.0 [Build 759] [2018-05-15]  on 2018-07-30 10:43:20
 */

ENTRY(ResetISR)

__sdram_non_cached_start = ORIGIN(BOARD_SDRAM_NOCACHE);
__sdram_non_cached_end = ORIGIN(BOARD_SDRAM_NOCACHE) + LENGTH(BOARD_SDRAM_NOCACHE);

__ocram_cached_start = ORIGIN(SRAM_OC);
__ocram_cached_end = ORIGIN(SRAM_OC) + LENGTH(SRAM_OC);

__dtcm_ram_start = ORIGIN(SRAM_DTC);
__dtcm_ram_end = ORIGIN(SRAM_DTC) + LENGTH(SRAM_DTC);

__sdram_cached_start = ORIGIN(BOARD_SDRAM_HEAP);
__sdram_cached_end = ORIGIN(BOARD_SDRAM_HEAP) + LENGTH(BOARD_SDRAM_HEAP);

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
        . = 0x2000 ;
    } > BOARD_SDRAM_TEXT

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0x00)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __textram_section_table = .;
        LONG(LOADADDR(.freeRTOS));
        LONG(    ADDR(.freeRTOS));
        LONG(  SIZEOF(.freeRTOS));
        __textram_section_table_end = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        *(.after_vectors*)

    } > BOARD_SDRAM_TEXT

    /* Put FreeRTOS code into internal DTC RAM */
    .freeRTOS : ALIGN(4)  
    { 
        *core/amazon-freertos/*.c.obj (.text .text*) 
    }  > SRAM_DTC_TEXT AT>BOARD_SDRAM_TEXT

    .text : ALIGN(4)    
    {
       /**(.text*)*/
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
            
            KEEP(*(.fini));
            
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
            /* End C++ */
    } > BOARD_SDRAM_TEXT


    
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4) 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > BOARD_SDRAM_TEXT

    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > BOARD_SDRAM_TEXT
    __exidx_end = .;

    _etext = .;

    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > SRAM_DTC AT>BOARD_SDRAM_TEXT

    /* Main DATA section (SRAM_OC) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(NonCacheable.init)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > SRAM_DTC AT>BOARD_SDRAM_TEXT

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(NonCacheable)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > SRAM_DTC

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > SRAM_DTC
    
    
    /* ext SDRAM */
    .sdram (NOLOAD) : ALIGN(4)
    {
        *(.sdram)
        *(.sdram.*)
    } >BOARD_SDRAM_HEAP
    
    /* No cacheable region in ext SDRAM */
    .sdramnoncacheable (NOLOAD) : ALIGN(4)
    {
        *(.sdramnoncacheable)
        *(.sdramnoncacheable.*)
    } >BOARD_SDRAM_NOCACHE

    /* Reserve and place Heap within memory map */
    _HeapSize = 0x1000;
    .heap :  ALIGN(4)
    {
        _pvHeapStart = .;
        . += _HeapSize;
        . = ALIGN(4);
        _pvHeapLimit = .;
    } > SRAM_DTC

     _StackSize = 0x1000;
     /* Reserve space in memory for Stack */
    .heap2stackfill  :
    {
        . += _StackSize;
    } > SRAM_DTC
    
    /* Locate actual Stack in memory map */
    .stack ORIGIN(SRAM_DTC) + LENGTH(SRAM_DTC) - _StackSize - 0:  ALIGN(4)
    {
        _vStackBase = .;
        . = ALIGN(4);
        _vStackTop = . + _StackSize;
    } > SRAM_DTC

    /* Provide basic symbols giving location and size of main text
     * block, including initial values of RW data sections. Note that
     * these will need extending to give a complete picture with
     * complex images (e.g multiple Flash banks).
     */
    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}