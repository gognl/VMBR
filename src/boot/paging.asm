bits 32
InitializePageTables:

    ; Point the first entry in the p4 table to the p3 table
    mov eax, p3_table
    or eax, PTE_P | PTE_W 
    mov dword [p4_table+0], eax

    ; Point the first entry in the p3 table to the p2 table
    mov eax, p2_table
    or eax, PTE_P | PTE_W
    mov dword [p3_table+0], eax

    ; Map the p2 table 
    xor ecx, ecx
    .map_p2_table:
        mov eax, 0x200000   ; 2MB
        mul ecx    ; now eax=eax*ecx, meaning eax=2MB*idx
        or eax, PTE_P | PTE_W | PTE_PS
        mov [p2_table + 8*ecx], eax

        inc ecx
        cmp ecx, MEM_SIZE
        jne .map_p2_table

    ret