#ifndef __KERN_NETWORK_LWIP_ARCH_CC__
#define __KERN_NETWORK_LWIP_ARCH_CC__

#include <error.h>
#include <assert.h>
#include <proc.h>
#include <kio.h>
#include <slab.h>

#define LWIP_NO_INTTYPES_H 1
#define LWIP_PLATFORM_ASSERT(message) panic(message)

#define LWIP_PREFIX_BYTEORDER_FUNCS
#define LWIP_PROVIDE_ERRNO
#define LWIP_ARP 1
#define MEM_LIBC_MALLOC 1
//#define LWIP_DEBUG
//#define LWIP_PLATFORM_DIAG(message) kprintf(message)

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int8_t s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;
typedef uintptr_t mem_ptr_t;

#define LWIP_DHCP 1

#define sys_msleep(ms) do_sleep(ms)

static void free(void* ptr) {
  kfree(ptr);
}

static void* malloc(int size) {
  return kmalloc(size);
}

#endif /* __KERN_NETWORK_LWIP_ARCH_CC__ */
