#ifndef __MODULES_H
#define __MODULES_H

#include <lib/types.h>

#define KLDR_DATA_TABLE_ENTRY_Flink(x) (x)
#define KLDR_DATA_TABLE_ENTRY_Blink(x) ((x)+8)
#define KLDR_DATA_TABLE_ENTRY_DllBase(x) ((x)+0x30)
#define KLDR_DATA_TABLE_ENTRY_BaseDllName(x) ((x)+0x58)

#define UNICODE_STRING_Length(x) (x)
#define UNICODE_STRING_Buffer(x) ((x)+8)

#endif