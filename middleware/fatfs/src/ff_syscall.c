/*------------------------------------------------------------------------*/
/* Sample code of OS dependent controls for FatFs                         */
/* (C)ChaN, 2014                                                          */
/*------------------------------------------------------------------------*/
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#include "ff.h"

#if _FS_REENTRANT
/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called by f_mount() function to create a new
/  synchronization object, such as semaphore and mutex. When a 0 is
/  returned, the f_mount() function fails with FR_INT_ERR.
*/

int ff_cre_syncobj (	/* 1:Function succeeded, 0:Could not create due to any error */
	BYTE vol,	/* Corresponding logical drive being processed */
	_SYNC_t *sobj	/* Pointer to return the created sync object */
)
{
	int ret;

#ifdef OS_FREERTOS   /* FREERTOS */
	*sobj = xSemaphoreCreateMutex();	/* FreeRTOS */
	ret = (int)(*sobj != NULL);
#else
	ret = 1;
#endif

	return ret;
}



/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is
/  returned, the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj (	/* 1:Function succeeded, 0:Could not delete due to any error */
	_SYNC_t sobj	/* Sync object tied to the logical drive to be deleted */
)
{
	int ret;

#ifdef OS_FREERTOS   /* FREERTOS */
	xSemaphoreDelete(sobj);		/* FreeRTOS */
	ret = 1;
#else
	ret = 1;
#endif

	return ret;
}



/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a FALSE is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (	/* 1:Got a grant to access the volume, 0:Could not get a grant */
	_SYNC_t sobj	/* Sync object to wait */
)
{
	int ret;

#ifdef OS_FREERTOS   /* FREERTOS */
	ret = (int)(xSemaphoreTake(sobj, _FS_TIMEOUT) == pdTRUE);	/* FreeRTOS */
#else
	ret = 1;
#endif

	return ret;
}



/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
	_SYNC_t sobj	/* Sync object to be signaled */
)
{
#ifdef OS_FREERTOS   /* FREERTOS */
	xSemaphoreGive(sobj);	/* FreeRTOS */
#else

#endif
}

#endif

#if _USE_LFN == 3	/* LFN with a working buffer on the heap */
/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/
/* If a NULL is returned, the file function fails with FR_NOT_ENOUGH_CORE.
*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block */
	UINT msize	/* Number of bytes to allocate */
)
{
	void *p_buf = NULL;

#ifdef OS_FREERTOS   /* FREERTOS */
	p_buf = pvPortMalloc(msize);
#else
	p_buf = malloc(msize);	/* Allocate a new memory block with POSIX API */
#endif

	return p_buf;
}


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree (
	void* mblock	/* Pointer to the memory block to free */
)
{

#ifdef OS_FREERTOS	/* FREERTOS */
	vPortFree(mblock);
#else
	free(mblock);	/* Discard the memory block with POSIX API */
#endif

}

#endif
