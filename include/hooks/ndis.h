#ifndef __NDIS_H
#define __NDIS_H

#include <lib/types.h>
#include <vmm/vmm.h>

extern void handle_NdisSendNetBufferLists_hook(vmexit_data_t *state);
#endif