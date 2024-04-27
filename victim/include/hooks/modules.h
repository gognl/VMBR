#ifndef __MODULES_H
#define __MODULES_H

#include <lib/types.h>

#define KLDR_DATA_TABLE_ENTRY_Flink(x) (x)
#define KLDR_DATA_TABLE_ENTRY_Blink(x) ((x)+8)
#define KLDR_DATA_TABLE_ENTRY_DllBase(x) ((x)+0x30)
#define KLDR_DATA_TABLE_ENTRY_BaseDllName(x) ((x)+0x58)

#define UNICODE_STRING_Length(x) (x)
#define UNICODE_STRING_Buffer(x) ((x)+8)

extern qword_t find_windows_module(wchar_t *name, uint16_t len);
extern uint64_t get_node_dllbase(qword_t node);
extern uint16_t get_node_dllname_length(qword_t node);
extern qword_t get_node_dllname_buffer(qword_t node);
extern qword_t get_previous_node(qword_t node);

#endif