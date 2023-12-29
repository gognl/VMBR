
%define DAP_ADDRESS 0x2fef
%define DRIVE_IDX_ADDRESS 0x2fff

global ReadDisk

bits 16
ReadDisk:
    mov ah, 0x42
    mov dl, byte [DRIVE_IDX_ADDRESS]
    mov si, DAP_ADDRESS
    int 0x13
    jc ReadDisk.problem
    ret

    .problem:
        println "Error while reading from disk"
        mov byte [DRIVE_IDX_ADDRESS], ah
    ret

