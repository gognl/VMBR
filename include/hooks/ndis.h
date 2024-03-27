#ifndef __NDIS_H
#define __NDIS_H

#include <lib/types.h>
#include <vmm/vmm.h>

#define NET_BUFFER_LIST_Next(_NET_BUFFER_LIST) (_NET_BUFFER_LIST)               // 64 bits (ptr)
#define NET_BUFFER_LIST_FirstNetBuffer(_NET_BUFFER_LIST) ((_NET_BUFFER_LIST)+8) // 64 bits (ptr)
#define NET_BUFFER_LIST_INFO(_NBL, _Id) ((_NBL)+0x90+8*(_Id))

#define NDIS_NET_BUFFER_LIST_INFO_TcpIpChecksumNetBufferListInfo 0

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

typedef struct {
  union {
    struct {
      uint32_t IsIPv4 : 1;
      uint32_t IsIPv6 : 1;
      uint32_t TcpChecksum : 1;
      uint32_t UdpChecksum : 1;
      uint32_t IpHeaderChecksum : 1;
      uint32_t Reserved : 11;
      uint32_t TcpHeaderOffset : 10;
    } __attribute__((__packed__)) Transmit;
    struct {
      uint32_t TcpChecksumFailed : 1;
      uint32_t UdpChecksumFailed : 1;
      uint32_t IpChecksumFailed : 1;
      uint32_t TcpChecksumSucceeded : 1;
      uint32_t UdpChecksumSucceeded : 1;
      uint32_t IpChecksumSucceeded : 1;
      uint32_t Loopback : 1;
      uint32_t TcpChecksumValueInvalid : 1;
      uint32_t IpChecksumValueInvalid : 1;
    } __attribute__((__packed__)) Receive;
    qword_t Value;
  };
} __attribute__((__packed__)) NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO;

extern void handle_NdisSendNetBufferLists_hook(vmexit_data_t *state);

#endif