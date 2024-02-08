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
        . = ALIGN(4096);
        vmm_start = .;
        *(.vmm)
        vmm_end = .;
        . = ALIGN(4096);
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