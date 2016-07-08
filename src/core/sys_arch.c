#include "lwip/sys.h"
#include <time/time.h>
#include <clock.h>
#include <ipc.h>
#include <slab.h>
#include <stddef.h>

int errno;

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  kernel_thread(thread, arg, NULL);
}

void sys_init(void)
{

}

err_t sys_mutex_new(sys_mutex_t *mutex)
{
  sem_init(mutex, 1);
  return ERR_OK;
}

void sys_mutex_free(sys_mutex_t *mutex)
{
  //TODO: Seems nothing to do here?
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
  down(mutex);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
  up(mutex);
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  sem_init(&sem->sem, count);
  sem->valid = 1;
  kprintf("sem is valid");
  return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
  //TODO: Seems nothing to do here?
}

void sys_sem_signal(sys_sem_t *sem)
{
  up(&sem->sem);
}

u32_t __sem_wait(semaphore_t *sem, u32_t timeout)
{
  if(timeout == 0) {
    __down(sem, WT_KSEM, NULL);
    return 0;
  }
  //TODO: Workaround for invalid timer.
  timeout /= 100;
  if(timeout == 0) timeout = 1;
	unsigned long saved_ticks;
	timer_t timer;
	ipc_timer_init(timeout, &saved_ticks, &timer);

	uint32_t flags;
	if ((flags = __down(sem, WT_USEM, &timer)) == 0) {
		return ticks - saved_ticks;
	}
	return SYS_ARCH_TIMEOUT;
}

u32_t sys_arch_sem_wait(sys_sem_t *sys_sem, u32_t timeout)
{
  return __sem_wait(&sys_sem->sem, timeout);
}

int sys_sem_valid(sys_sem_t *sem)
{
  return sem->valid;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
  sem->valid = 0;
}

uint32_t sys_now()
{
  return ticks * 100;
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  //kprintf("sys_mbox_new = %lx\n", mbox);
  sys_mutex_new(&mbox->queue_mutex);
  sem_init(&mbox->fill_sem, 0);
  sem_init(&mbox->unused_sem, size);
  list_init(&mbox->queue);
  mbox->valid = 1;
  return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
  //TODO: free the message queue.
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  down(&mbox->unused_sem);
  sys_mutex_lock(&mbox->queue_mutex);
  struct sys_msg* sys_msg = kmalloc(sizeof(struct sys_msg));
  sys_msg->data = msg;
  list_add_after(&mbox->queue, &sys_msg->list_entry);
  sys_mutex_unlock(&mbox->queue_mutex);
  up(&mbox->fill_sem);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  //kprintf("sys_mbox_trypost = %lx\n", mbox);
  //TODO: This is buggy. It is possible that another thread modifiy value after
  //the check, causing the mutex to block. To fix this, a "__trydown"
  //have to be implemented.
  if(mbox->unused_sem.value == 0) return ERR_MEM;
  sys_mbox_post(mbox, msg);
  return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  if(__sem_wait(&mbox->fill_sem, timeout) == SYS_ARCH_TIMEOUT) {
    return SYS_ARCH_TIMEOUT;
  }
  sys_mutex_lock(&mbox->queue_mutex);
  list_entry_t* entry = list_prev(&mbox->queue);
  struct sys_msg* sys_msg = container_of(entry, struct sys_msg, list_entry);
  (*msg) = sys_msg->data;
  list_del(entry);
  kfree(sys_msg);
  sys_mutex_unlock(&mbox->queue_mutex);
  up(&mbox->unused_sem);
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  //TODO: This is buggy, the same reason as @sys_mbox_trypost
  if(mbox->fill_sem.value == 0) return ERR_MEM;
  sys_mbox_fetch(mbox, msg);
  return ERR_OK;
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
  return mbox->valid;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  mbox->valid = 0;
}
