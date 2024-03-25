#ifndef __NDIS_H
#define __NDIS_H

#include <lib/types.h>
#include <vmm/vmm.h>

#define NET_BUFFER_LIST_Next(_NET_BUFFER_LIST) (_NET_BUFFER_LIST)               // 64 bits (ptr)
#define NET_BUFFER_LIST_FirstNetBuffer(_NET_BUFFER_LIST) ((_NET_BUFFER_LIST)+8) // 64 bits (ptr)

#define NET_BUFFER_Next(_NET_BUFFER) (_NET_BUFFER)                          // 64 bits (ptr)
#define NET_BUFFER_CurrentMdl(_NET_BUFFER) ((_NET_BUFFER)+8)                // 64 bits (ptr)
#define NET_BUFFER_CurrentMdlOffset(_NET_BUFFER) ((_NET_BUFFER)+0x10)       // 32 bits (val)
#define NET_BUFFER_DataLength(_NET_BUFFER) ((_NET_BUFFER)+0x18)             // 32 bits (val)
#define NET_BUFFER_DataOffset(_NET_BUFFER) ((_NET_BUFFER)+0x28)             // 32 bits (val)

#define MDL_Next(_MDL) (_MDL)                       // 64 bits (ptr)
#define MDL_Size(_MDL) ((_MDL)+8)                   // 16 bits (val)
#define MDL_MappedSystemVa(_MDL) ((_MDL)+0x18)      // 64 bits (ptr)
#define MDL_StartVa(_MDL) ((_MDL)+0x20)             // 64 bits (ptr)
#define MDL_ByteCount(_MDL) ((_MDL)+0x28)           // 32 bits (val)
#define MDL_ByteOffset(_MDL) ((_MDL)+0x2c)          // 32 bits (val)
#define MDL_PhysicalPage(_MDL) ((_MDL)+0x30)        // 64 bits (pfn)

extern void handle_NdisSendNetBufferLists_hook(vmexit_data_t *state);
#endif