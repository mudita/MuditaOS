// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*------------------------------------------------------------------------*/
/* Sample Code of OS Dependent Functions for FatFs                        */
/* (C)ChaN, 2018                                                          */
/*------------------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "semphr.h"
#include "ff.h"
#include <cstdlib>

#if FF_USE_LFN == 3 /* Dynamic memory allocation */

/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/

extern "C" void *ff_memalloc(           /* Returns pointer to the allocated memory block (null if not enough core) */
                             UINT msize /* Number of bytes to allocate */
)
{
    return std::malloc(msize);
}

/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

extern "C" void ff_memfree(void *mblock /* Pointer to the memory block to free (nothing to do if null) */
)
{
    std::free(mblock); /* Free the memory block with POSIX API */
}

#endif

#if FF_FS_REENTRANT /* Mutual exclusion */

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is returned, the f_mount() function fails with FR_INT_ERR.
*/

// const osMutexDef_t Mutex[FF_VOLUMES];	/* Table of CMSIS-RTOS mutex */

extern "C" int ff_cre_syncobj(                /* 1:Function succeeded, 0:Could not create the sync object */
                              BYTE vol,       /* Corresponding volume (logical drive number) */
                              FF_SYNC_t *sobj /* Pointer to return the created sync object */
)
{
    /* FreeRTOS */
    *sobj = xSemaphoreCreateMutex();
    return static_cast<int>(*sobj != NULL);
}

/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

extern "C" int ff_del_syncobj(               /* 1:Function succeeded, 0:Could not delete due to an error */
                              FF_SYNC_t sobj /* Sync object tied to the logical drive to be deleted */
)
{
    /* FreeRTOS */
    vSemaphoreDelete(sobj);
    return 1;
}

/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

extern "C" int ff_req_grant(               /* 1:Got a grant to access the volume, 0:Could not get a grant */
                            FF_SYNC_t sobj /* Sync object to wait */
)
{
    /* FreeRTOS */
    return static_cast<int>(xSemaphoreTake(sobj, FF_FS_TIMEOUT) == pdTRUE);
}

/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
 */

extern "C" void ff_rel_grant(FF_SYNC_t sobj /* Sync object to be signaled */
)
{
    /* FreeRTOS */
    xSemaphoreGive(sobj);
}

#endif
