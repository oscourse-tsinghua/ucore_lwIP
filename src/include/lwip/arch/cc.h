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
#define LWIP_DEBUG 1
#define X32_F "%x"
#define X16_F "%x"
#define X8_F "%x"
#define S32_F "%d"
#define U16_F "%s"
#define S16_F "%s"
#define U32_F "%u"
#define SZT_F "%d"
#define LWIP_DEBUGF kprintf
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

void *memset(void *s, char c, size_t n);

static void* calloc (size_t num, size_t size) {
  size_t bytes = size * num;
  void* ret = malloc(bytes);
  memset(ret, 0, bytes);
  return ret;
}

#define INT_MAX 0x7FFFFFFF
#define INT_MIN 0x80000000

static int atoi(char* str) {
    int64_t ret = 0;
    int isNegative = 0;
    while(*str != '\0' && (*str == ' ' || *str == '\t')) {
        str++;
    }
    if(*str == '+') {
        str++;
    }
    else if(*str == '-') {
        isNegative = 1;
        str++;
    }
    while(*str >= '0' && *str <= '9') {
        ret = ret * 10 + (*str - '0');
        if(!isNegative && ret > INT_MAX) {
            ret = INT_MAX;
            break;
        }
        else if(isNegative && -ret < INT_MIN) {
            ret = -(int64_t)INT_MIN;
            break;
        }
        str++;
    }
    return isNegative ? (int)-ret : (int)ret;
}

#endif /* __KERN_NETWORK_LWIP_ARCH_CC__ */
