#include <hardware/serial.h>
#include <boot/addresses.h>
#include <lib/util.h>

void read_first_sector(byte_t drive){
    dap_t *dap_ptr = (dap_t*)DAP_ADDRESS;
    dap_ptr->size = sizeof(dap_t);
    dap_ptr->unused = 0;
    dap_ptr->amount = 1;
    dap_ptr->offset = MBR_ADDRESS;
    dap_ptr->segment = 0;
    dap_ptr->sector = 0;
    *(byte_t*)DRIVE_IDX_ADDRESS = drive;
    CallReal(ReadDisk);
}

void load_guest(){
    mbr_t *mbr_ptr = (mbr_t*)MBR_ADDRESS;

    memcpy(REAL_START+low_functions_end-low_functions_start, CallReal, call_real_end-CallReal);
    void (*CallRealCopy)(void(*)(void)) = (void (*)(void(*)(void)))(REAL_START+low_functions_end-low_functions_start);

    for (byte_t drive_index = 0x80; drive_index < 0xff; drive_index++){
        read_first_sector(drive_index);
        if (mbr_ptr->signature == BIOS_SIGNATURE){
            *(byte_t*)DRIVE_IDX_ADDRESS = drive_index;
            memcpy(0x7c00, (byte_t*)mbr_ptr, sizeof(mbr_t));
            break;
        }
    }
    
    CallRealCopy(JumpToGuest);
}

