MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x100000
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x30000
}

SECTIONS
{
    /* Vector table + code in flash */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text*)              /* all text sections */
        *(.rodata*)            /* const data */
    } > FLASH

    .data : AT (ADDR(.text) + SIZEOF(.text))
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM 

    _sidata = LOADADDR(.data);  /* Initial values in flash */

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM); /* Stack pointer */
}
