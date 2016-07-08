#ifndef __KERN_NETWORK_LWIP_ARCH_SYS_ARCH__
#define __KERN_NETWORK_LWIP_ARCH_SYS_ARCH__

#include <sync.h>
#include <sem.h>
#include <mbox.h>
#include "assert.h"
#include "lwip/sys.h"

typedef int sys_thread_t;

typedef semaphore_t sys_mutex_t;

typedef struct {
  semaphore_t sem;
  bool valid;
} sys_sem_t;

typedef struct sys_msg {
  void* data;
  list_entry_t list_entry;
};

typedef struct {
  sys_mutex_t queue_mutex;
  semaphore_t fill_sem;
  semaphore_t unused_sem;
  list_entry_t queue;
  bool valid;
} sys_mbox_t;

#define BYTE_ORDER LITTLE_ENDIAN
//TODO: need to ensure uCore doesn't have big-endian architecture support.

#endif /* __KERN_NETWORK_LWIP_ARCH_SYS_ARCH__ */
