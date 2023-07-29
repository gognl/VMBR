extern _start

;section .mb2_header
mb2_header_start:
    dd 0xE85250D6    ; magic
    dd 0    ; architecture
    dd mb2_header_end-mb2_header_start    ; header length
    dd 0x100000000 - (0xE85250D6 + 0 + mb2_header_end-mb2_header_start)    ; checksum
    ; the checksum, added to the magic fields, results in 0x100000000. 
    ; When read as a 32bit unsigned integer, it is 0 (little endian).
    mb2_entry_address_tag_start:
        dw 3      ; type
        dw 0      ; flags
        dd mb2_entry_address_tag_end - mb2_entry_address_tag_start ; size
        dd _start ; entry_addr
        dd 0    ; padding, needs to be aligned to 8
        mb2_entry_address_tag_end:
    ; end tag
    dw 0
    dw 0
    dd 8

mb2_header_end:
