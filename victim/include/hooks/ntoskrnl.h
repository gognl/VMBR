#ifndef __NTOSKRNL_H
#define __NTOSKRNL_H

#include <lib/types.h>
#include <vmm/vmm.h>

extern void handle_lstar_write(uint64_t lstar);
extern void handle_MiDriverLoadSucceeded_hook(vmexit_data_t *state);

#endif