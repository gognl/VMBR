ENTRY(_start)

SECTIONS {
    . = 0x3300000;

    .boot :
    {
        /* ensure that the multiboot header is at the beginning */
        *(.mb2_header)
    }

    .text :
    {
        *(.text)
    }

    .real :
    {
        *(.real)
    }

    .rodata :
    {
        *(.rodata)
    }

    .bss :
    {
        *(.bss)
    }

    .data :
    {
        *(.data)
    }

    vmbr_end = .;
    
}