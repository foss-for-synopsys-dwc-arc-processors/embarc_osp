/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

#include <string.h>

#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"

#include "arc_exception.h"

#define LWIP_MIN_STACKSIZE    128

#if !NO_SYS

/* Mutex functions: */
/** Define LWIP_COMPAT_MUTEX if the port has no mutexes and binary semaphores
    should be used instead */
#if !LWIP_COMPAT_MUTEX
/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
	err_t ercd = ERR_MEM;

	*mutex = xSemaphoreCreateMutex();

	if (sys_mutex_valid(mutex)) {
		ercd = ERR_OK;
		SYS_STATS_INC_USED( mutex );
	} else {
		SYS_STATS_INC( mutex.err );
	}

	return ercd;
}
/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{
	if (sys_mutex_valid(mutex)) {
		while (xSemaphoreTake(*mutex, portMAX_DELAY) != pdTRUE);
	}
}
/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
	if (sys_mutex_valid(mutex)) {
		xSemaphoreGive(*mutex);
	}
}
/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
	if (sys_mutex_valid(mutex)) {
		SYS_STATS_DEC(mutex.used);
		vSemaphoreDelete(*mutex);
	}
}
#ifndef sys_mutex_valid
/** Check if a mutex is valid/allocated: return 1 for valid, 0 for invalid */
int sys_mutex_valid(sys_mutex_t *mutex)
{
	if (*mutex == SYS_MUTEX_NULL) {
		return 0;
	} else {
		return 1;
	}
}
#endif
#ifndef sys_mutex_set_invalid
/** Set a mutex invalid so that sys_mutex_valid returns 0 */
void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
	*mutex = SYS_MUTEX_NULL;
}
#endif
#endif /* !LWIP_COMPAT_MUTEX */

/* Semaphore functions: */

/** Create a new semaphore
 * @param sem pointer to the semaphore to create
 * @param count initial count of the semaphore
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	err_t ercd = ERR_MEM;

	vSemaphoreCreateBinary((*sem));

	if (sys_sem_valid(sem)) {
		if (count == 0) {
			if (xSemaphoreTake(*sem, 1) == pdPASS) {
				ercd = ERR_OK;
				SYS_STATS_INC_USED(sem);
			}
		} else {
			ercd = ERR_OK;
			SYS_STATS_INC_USED(sem);
		}
	} else {
		SYS_STATS_INC(sem.err);
	}

	return ercd;
}
/** Signals a semaphore
 * @param sem the semaphore to signal */
void sys_sem_signal(sys_sem_t *sem)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (sys_sem_valid(sem)) {
		if( exc_sense() ) { /** in interrupt */
			xSemaphoreGiveFromISR( *sem, &xHigherPriorityTaskWoken );
		} else {
			xSemaphoreGive( *sem );
		}
	}
}
/** Wait for a semaphore for the specified timeout
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 * @return time (in milliseconds) waited for the semaphore
 *         or SYS_ARCH_TIMEOUT on timeout */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	TickType_t xStartTime, xEndTime, xElapsed;
	u32_t xReturn;

	if (!sys_sem_valid(sem)) { /** NON-VALID SEM */
		return SYS_ARCH_TIMEOUT;
	}

	xStartTime = xTaskGetTickCount();

	if( timeout != 0UL ) {
		if( xSemaphoreTake( *sem, timeout / portTICK_PERIOD_MS ) == pdTRUE ) {
			xEndTime = xTaskGetTickCount();
			xElapsed = (xEndTime - xStartTime) * portTICK_PERIOD_MS;
			xReturn = xElapsed;
		} else {
			xReturn = SYS_ARCH_TIMEOUT;
		}
	} else {
		while( xSemaphoreTake( *sem, portMAX_DELAY ) != pdTRUE );
		xEndTime = xTaskGetTickCount();
		xElapsed = ( xEndTime - xStartTime ) * portTICK_PERIOD_MS;

		if( xElapsed == 0UL ) {
			xElapsed = 1UL;
		}
		xReturn = xElapsed;
	}

	return xReturn;
}
/** Delete a semaphore
 * @param sem semaphore to delete */
void sys_sem_free(sys_sem_t *sem)
{
	if (sys_sem_valid(sem)) {
		SYS_STATS_DEC(sem.used);
		vSemaphoreDelete(*sem);
	}
}

#ifndef sys_sem_valid
/** Check if a sempahore is valid/allocated: return 1 for valid, 0 for invalid */
int sys_sem_valid(sys_sem_t *sem)
{
	if ((*sem) == SYS_SEM_NULL) {
		return 0;
	} else {
		return 1;
	}
}
#endif
#ifndef sys_sem_set_invalid
/** Set a semaphore invalid so that sys_sem_valid returns 0 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
	*sem = SYS_SEM_NULL;
}
#endif


/* Mailbox functions. */

/** Create a new mbox of specified size
 * @param mbox pointer to the mbox to create
 * @param size (miminum) number of messages in this mbox
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
	err_t ercd = ERR_MEM;

	*mbox = xQueueCreate( size, sizeof(void *) );
	if (sys_mbox_valid(mbox)) {
		ercd = ERR_OK;
		SYS_STATS_INC_USED(mbox);
	}
 	return ercd;
}
/** Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	if (sys_mbox_valid(mbox)) {
		while (xQueueSendToBack( *mbox, &msg, portMAX_DELAY ) != pdPASS);
	}
}
/** Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	err_t ercd = ERR_MEM;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (sys_mbox_valid(mbox)) {
		if( exc_sense() ) { /** in interrupt */
			ercd = xQueueSendToBackFromISR(*mbox, &msg, &xHigherPriorityTaskWoken);
		} else {
			ercd = xQueueSendToBack(*mbox, &msg, 0);
		}
	}
	if (ercd == pdPASS) {
		ercd = ERR_OK;
	} else {
		ercd = ERR_MEM;
		SYS_STATS_INC(mbox.err);
	}
	return ercd;
}

/** Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
err_t sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
/** NOTE: The sys_mbox_trypost_fromisr() function was introduced by LWIP
 * 8fc20142f7b1b18f24fc48187d390187d718ce30 (05.01.2018). */
	return sys_mbox_trypost(q, msg);
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message (0 = wait forever)
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter! */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	void *pvDummy;
	TickType_t xStartTime, xEndTime, xElapsed;
	u32_t xReturn;

	if (!sys_mbox_valid(mbox)) {
		return SYS_ARCH_TIMEOUT;
	}

	xStartTime = xTaskGetTickCount();

	if (msg == NULL) {
		msg = &pvDummy; /** just remove from mailbox */
	}

	if( timeout != 0UL ) {
		if( xQueueReceive( *mbox, &(*msg), timeout / portTICK_PERIOD_MS ) == pdTRUE ) {
			xEndTime = xTaskGetTickCount();
			xElapsed = (xEndTime - xStartTime) * portTICK_PERIOD_MS;
			xReturn = xElapsed;
		} else {
			*msg = NULL;
			xReturn = SYS_ARCH_TIMEOUT;
		}
	} else {
		while( xQueueReceive( *mbox, &(*msg), portMAX_DELAY) != pdTRUE );
		xEndTime = xTaskGetTickCount();
		xElapsed = ( xEndTime - xStartTime ) * portTICK_PERIOD_MS;

		if( xElapsed == 0UL ) {
			xElapsed = 1UL;
		}
		xReturn = xElapsed;
	}

	return xReturn;

}
/* Allow port to override with a macro, e.g. special timout for sys_arch_mbox_fetch() */
#ifndef sys_arch_mbox_tryfetch
/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	void *pvDummy;
	u32_t xReturn;
	err_t ercd;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (!sys_mbox_valid(mbox)) {
		return SYS_MBOX_EMPTY;
	}

	if (msg == NULL) {
		msg = &pvDummy;
	}

	if (exc_sense()) { /* interrupt in */
		ercd = xQueueReceiveFromISR(*mbox, &(*msg), &xHigherPriorityTaskWoken);
	} else {
		ercd = xQueueReceive(*mbox, &(*msg), 0);
	}
	if (ercd == pdPASS) {
		xReturn = ERR_OK;
	} else {
		xReturn = SYS_MBOX_EMPTY;
	}

	return xReturn;
}
#endif
/** Delete an mbox
 * @param mbox mbox to delete */
void sys_mbox_free(sys_mbox_t *mbox)
{
	u32_t msg_wait_cnt = 0;

	if (!sys_mbox_valid(mbox)) {
		return;
	}
	msg_wait_cnt = uxQueueMessagesWaiting(*mbox);

	if (msg_wait_cnt) {
  		LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_free: mbox 0x%x, %d msgs waited\n", (uint32_t)(*mbox), msg_wait_cnt));
  		SYS_STATS_INC(mbox.err);
	}
	SYS_STATS_DEC(mbox.used);

	vQueueDelete( *mbox );
}
#ifndef sys_mbox_valid
/** Check if an mbox is valid/allocated: return 1 for valid, 0 for invalid */
int sys_mbox_valid(sys_mbox_t *mbox)
{
	if (*mbox == SYS_MBOX_NULL) {
		return 0;
	} else {
		return 1;
	}
}
#endif
#ifndef sys_mbox_set_invalid
/** Set an mbox invalid so that sys_mbox_valid returns 0 */
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	*mbox = SYS_MBOX_NULL;
}
#endif

static const char* lwip_thread_default_name = "lwip_task";
/** The only thread function:
 * Creates a new thread
 * @param name human-readable name for the thread (used for debugging purposes)
 * @param thread thread-function
 * @param arg parameter passed to 'thread'
 * @param stacksize stack size in bytes for the new thread (may be ignored by ports)
 * @param prio priority of the new thread (may be ignored by ports) */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	sys_thread_t xTask;

	LWIP_DEBUGF(SYS_DEBUG, ("Create LwIP related task"));
	if (name) {
		LWIP_DEBUGF(SYS_DEBUG, (":%s\r\n", name));
	} else {
		name = lwip_thread_default_name;
		LWIP_DEBUGF(SYS_DEBUG, ("\r\n"));
	}
	if (stacksize <= 0) {
		LWIP_DEBUGF(SYS_DEBUG, ("StackSize of %s can't set to %d bytes, now set it to %d bytes\r\n", name, stacksize, LWIP_MIN_STACKSIZE));
		stacksize = LWIP_MIN_STACKSIZE;
	}
	if (prio > configMAX_PRIORITIES) {
    		LWIP_DEBUGF(SYS_DEBUG, ("priority must be smaller than %d, so set it to %d\r\n", configMAX_PRIORITIES, configMAX_PRIORITIES));
    		prio = configMAX_PRIORITIES;
  	}
	if (xTaskCreate( thread, name, stacksize, arg, prio, &xTask) != pdPASS) {
		if (name) {
			LWIP_DEBUGF(SYS_DEBUG, ("Create LwIP task %s failed\r\n", name));
		} else {
			LWIP_DEBUGF(SYS_DEBUG, ("Create new LwIP task failed\r\n"));
		}
		xTask = SYS_THREAD_NULL;
	}
	return xTask;
}

#endif /* !NO_SYS */

/* sys_init() must be called before anthing else. */
void sys_init(void)
{

}

#ifndef sys_jiffies
/** Ticks/jiffies since power up. */
u32_t sys_jiffies(void)
{
	return (u32_t)xTaskGetTickCount();
}
#endif

/** Returns the current time in milliseconds,
 * may be the same as sys_jiffies or at least based on it. */
u32_t sys_now(void)
{
	return (u32_t)xTaskGetTickCount();
}

/* Critical section */
#if SYS_LIGHTWEIGHT_PROT
/** sys_prot_t sys_arch_protect(void)

This optional function does a "fast" critical region protection and returns
the previous protection level. This function is only called during very short
critical regions. An embedded system which supports ISR-based drivers might
want to implement this function by disabling interrupts. Task-based systems
might want to implement this by using a mutex or disabling tasking. This
function should support recursive calls from the same task or interrupt. In
other words, sys_arch_protect() could be called while already protected. In
that case the return value indicates that it is already protected.

sys_arch_protect() is only required if your port is supporting an operating
system.
*/
sys_prot_t sys_arch_protect(void)
{
    return (sys_prot_t)cpu_lock_save();
}

/** void sys_arch_unprotect(sys_prot_t pval)

This optional function does a "fast" set of critical region protection to the
value specified by pval. See the documentation for sys_arch_protect() for
more information. This function is only required if your port is supporting
an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{
    cpu_unlock_restore(pval);
}
#endif /* SYS_LIGHTWEIGHT_PROT */