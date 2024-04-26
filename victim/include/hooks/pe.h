#ifndef __PE_H
#define __PE_H

#include <lib/types.h>

#define DOS_SIGNATURE 0x5a4d
#define PE_SIGNATURE 0x00004550
#define PE32PLUS_SIGNATURE 0x020b

#define DOS_HEADER_e_lfanew 0x3c

#define OPTIONAL_HEADER_OFFSET 0x18

#define OPTIONAL_HEADER_NumberOfRvaAndSizes 108
#define OPTIONAL_HEADER_DebugDirectory 160
#define DEBUG_SECTION_AddressOfRawData 20
#define DEBUG_DATA_StringOffset 24

#define RVA(module, address) ((module)+(address))

typedef struct _IMAGE_DATA_DIRECTORY {
    dword_t VirtualAddress;
    dword_t Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

extern BOOL check_for_module(byte_t *module, byte_t *name, byte_t name_len);

#endif