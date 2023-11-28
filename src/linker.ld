ENTRY(_start)

SECTIONS {
    . = 0x7c00;

    .boot :
    {
        /* ensure that the multiboot header is at the beginning */
        *(.mb2_header)
    }

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }

    vmbr_end = .;
    
}