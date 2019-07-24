/*****< btpskrnl.c >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSKRNL - Stonestreet One Bluetooth Stack Kernel Implementation for      */
/*             FreeRTOS.                                                      */
/*                                                                            */
/*  Author:  Marcus Funk                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/08/12  M. Funk        Initial creation.                               */
/******************************************************************************/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "BTPSKRNL.h"         /* BTPS Kernel Prototypes/Constants.            */
#include "BTTypes.h"          /* BTPS internal data types.                    */

#include "FreeRTOS.h"         /* FreeRTOS Kernel Prototypes/Constants.        */
#include "task.h"             /* FreeRTOS Task Prototypes/Constants.          */
#include "semphr.h"           /* FreeRTOS Semaphore Prototypes/Constants.     */

   /* The following MACRO maps Timeouts (specified in Milliseconds) to  */
   /* System Ticks that are required by the Operating System Timeout    */
   /* functions (Waiting only).                                         */
#define MILLISECONDS_TO_TICKS(_x)      ((_x)/(portTICK_RATE_MS))

   /* Denotes the priority of the thread being created using the thread */
   /* create function.                                                  */
#define DEFAULT_THREAD_PRIORITY        (3)

   /* The following type declaration represents the entire state        */
   /* information for an Event_t.  This structure is used with all of   */
   /* the Event functions contained in this module.                     */
typedef struct _tagEventHeader_t
{
   Boolean_t        IsSet;
   xSemaphoreHandle SemaphoreHandle;
   unsigned int     Count;
} EventHeader_t;

   /* The following structure is a container structure that exists to   */
   /* map the OS Thread Function to the BTPS Kernel Thread Function (it */
   /* doesn't totally map in a one to one relationship/mapping).        */
typedef struct _tagThreadWrapperInfo_t
{
   char           Name[4];
   ThreadHandle_t Thread;
   Thread_t       ThreadFunction;
   void          *ThreadParameter;
} ThreadWrapperInfo_t;

   /* The following type declaration represents the entire state        */
   /* information for a Mailbox.  This structure is used with all of the*/
   /* Mailbox functions contained in this module.                       */
typedef struct _tagMailboxHeader_t
{
   xQueueHandle  QueueHandle;
   unsigned int  SlotSize;
} MailboxHeader_t;

/************************************************************************/
/* Heap Manager definitions.                                            */
/************************************************************************/

   /* Defines the maximum number of bytes that will be allocated by the */
   /* kernel abstraction module to support allocations.                 */
   /* * NOTE * This module declares a memory array of this size (in     */
   /*          bytes) that will be used by this module for memory       */
   /*          allocation.                                              */
#define BTPS_MEMORY_BUFFER_SIZE        (20 * 1024)

   /* The following defines a type that is the size in bytes of the     */
   /* desired alignment of each data fragment.                          */
typedef unsigned int Alignment_t;

   /* The following defines the size that memory needs to be aligned to.*/
   /* For correct operation, this value must be a power of two.         */
#define ALIGNMENT_SIZE                 sizeof(Alignment_t)

   /* The following structure contains the information for a single     */
   /* memory block.  Its members include the size of the previous block,*/
   /* and the size of the block, and the start of the data region of the*/
   /* The size of the block also includes a flag to indicate if the     */
   /* block is allocated.                                               */
typedef struct _tagBlockInfo_t
{
   Word_t      PrevSize;
   Word_t      Size;
   Alignment_t Data[1];
} BlockInfo_t;

#define BLOCK_INFO_SIZE(_x)            ((BTPS_STRUCTURE_OFFSET(BlockInfo_t, Data) / ALIGNMENT_SIZE) + (_x))

   /* The following constants are used with the Size member of the      */
   /* BlockInfo_t to denote various information about the memory block. */
#define SEGMENT_SIZE_BITMASK           ((Word_t)(((Word_t)-1) >> 1))
#define SEGMENT_ALLOCATED_BITMASK      (Word_t)(~SEGMENT_SIZE_BITMASK)

   /* The following defines the size in bytes of a data fragment that is*/
   /* considered a large value.  Allocations that are equal to and      */
   /* larger than this value will be allocated from the end of the heap.*/
#define LARGE_SIZE                     (256 / ALIGNMENT_SIZE)

   /* The following defines the minimum and maximum sizes of a block (in*/
   /* Alignment_t units) that can be allocated.                         */
#define MINIMUM_MEMORY_SIZE            (BLOCK_INFO_SIZE(1))
#define MAXIMUM_MEMORY_SIZE            (SEGMENT_SIZE_BITMASK)

   /* The following structure provides the information for a heap. Its  */
   /* members include a flag to indicate if it has been initializes, the*/
   /* current and maximum amount of the heap used (in Alignment_t       */
   /* units), a pointer to the end of the heap and the start of the     */
   /* heap.                                                             */
typedef struct _tagHeapInfo_t
{
   Boolean_t     Initialized;
   unsigned int  CurrentHeapUsed;
   unsigned int  MaximumHeapUsed;
   BlockInfo_t  *HeapTail;
   BlockInfo_t   HeapHead[1];
} HeapInfo_t;

#define HEAP_INFO_SIZE(_x)             (BTPS_STRUCTURE_OFFSET(HeapInfo_t, HeapHead) + (_x))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

   /* Declare a buffer to use for the Heap.  Note that we declare this  */
   /* as an Alignment_t so that we can force alignment to be correct.   */
static Alignment_t                  MemoryBuffer[(BTPS_MEMORY_BUFFER_SIZE / ALIGNMENT_SIZE)];

/************************************************************************/
/* End of Heap Manager definitions.                                     */
/************************************************************************/

   /* Variable which holds the current Debug Zone Mask.                 */
static unsigned long                DebugZoneMask;

   /* The following mutex is used to guard access to the function that  */
   /* sends Messages to the console.                                    */
static Mutex_t                      IOMutex;

   /* The following mutex is used to guard access to BTPSKRNL resources.*/
static Mutex_t                      KernelMutex;

   /* Variable that stores the number of threads that have been created */
   /* by BTPS_CreateThread().                                           */
static char                         ThreadCount;

   /* The following buffer is used when writing Debug Messages to Debug */
   /* UART.                                                             */
static BTPS_MessageOutputCallback_t MessageOutputCallback;

   /* Internal Function Prototypes.                                     */
static void HeapInit(void *Heap, unsigned long Size);
static void *MemAlloc(void *Heap, unsigned long Size);
static void MemFree(void *Heap, void *MemoryPtr);
static int GetHeapStatistics(void *Heap, BTPS_MemoryStatistics_t *MemoryStatistics, Boolean_t AdvancedStatitics);
static void ThreadWrapper(void *UserData);

   /* The following function is used to initialize the heap structure.  */
   /* The function takes no parameters and returns no status.           */
static void HeapInit(void *Heap, unsigned long Size)
{
   HeapInfo_t *HeapInfo;

   HeapInfo = (HeapInfo_t *)Heap;

   /* Confirm that the parameters are valid and that the heap has not   */
   /* already been initialized.                                         */
   if((HeapInfo) && (!(HeapInfo->Initialized)) && (Size > HEAP_INFO_SIZE(MINIMUM_MEMORY_SIZE)))
   {
      Size = (Size - HEAP_INFO_SIZE(0)) / ALIGNMENT_SIZE;

      /* Confirm that the size is valid.                                */
      if((Size >= MINIMUM_MEMORY_SIZE) && (Size <= MAXIMUM_MEMORY_SIZE))
      {
         /* Initialize the Heap information.                            */
         BTPS_MemInitialize(HeapInfo, 0, sizeof(HeapInfo_t));
         HeapInfo->HeapHead->PrevSize = Size;
         HeapInfo->HeapHead->Size     = Size;
         HeapInfo->HeapTail           = (BlockInfo_t *)(((Alignment_t *)HeapInfo->HeapHead) + Size);

         /* Indicate the heap has been initialized.                     */
         HeapInfo->Initialized        = TRUE;
      }
   }
}

   /* The following function is used to allocate a fragment of memory   */
   /* from a large buffer.  The function takes as its parameter the size*/
   /* in bytes of the fragment to be allocated.  The function tries to  */
   /* avoid fragmentation by obtaining memory requests larger than      */
   /* LARGE_SIZE from the end of the buffer, while small fragments are  */
   /* taken from the start of the buffer.                               */
static void *MemAlloc(void *Heap, unsigned long Size)
{
   void        *ret_val;
   HeapInfo_t  *HeapInfo;
   BlockInfo_t *BlockInfo;
   BlockInfo_t *TempBlockInfo;
   Word_t       RemainingSize;

   HeapInfo = (HeapInfo_t *)Heap;

   /* Convert the requested memory allocation in bytes to alignment     */
   /* size, rounding up, and add the block info header size to it.      */
   Size = BLOCK_INFO_SIZE((Size + (ALIGNMENT_SIZE - 1)) / ALIGNMENT_SIZE);

   /* Verify that the parameters are valid.                             */
   if((HeapInfo) && (HeapInfo->Initialized) && (Size >= MINIMUM_MEMORY_SIZE) && (Size <= MAXIMUM_MEMORY_SIZE))
   {
      /* Start at the beginning of the heap for small segments and the  */
      /* end for large segments.                                        */
      if(Size < LARGE_SIZE)
         BlockInfo = HeapInfo->HeapHead;
      else
         BlockInfo = (BlockInfo_t *)(((Alignment_t *)(HeapInfo->HeapTail)) - HeapInfo->HeapHead->PrevSize);

      /* Loop until we have walked the entire list.                     */
      while(((Size < LARGE_SIZE) || (BlockInfo != HeapInfo->HeapHead)) && (BlockInfo != HeapInfo->HeapTail))
      {
         /* Check to see if the current entry is free and is large      */
         /* enough to hold the data being requested.                    */
         if((BlockInfo->Size & SEGMENT_ALLOCATED_BITMASK) || (BlockInfo->Size < Size))
         {
            /* If the requested size is larger than the limit then      */
            /* search backwards for an available buffer, else go        */
            /* forward.  This will hopefully help to reduce             */
            /* fragmentataion problems.                                 */
            if(Size >= LARGE_SIZE)
               BlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) - (BlockInfo->PrevSize));
            else
               BlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) + (BlockInfo->Size & SEGMENT_SIZE_BITMASK));
         }
         else
         {
            /* Suitable memory block found.                             */
            break;
         }
      }

      /* Check to see if we found a segment large enough for the        */
      /* request.                                                       */
      if((BlockInfo != HeapInfo->HeapTail) && (BlockInfo->Size >= Size) && (!(BlockInfo->Size & SEGMENT_ALLOCATED_BITMASK)))
      {
         /* Check to see if we need to split this into two entries.     */
         /* * NOTE * If there is not enough room to make another entry  */
         /*          then we will not adjust the size of this entry to  */
         /*          match the amount requested.                        */
         if((RemainingSize = BlockInfo->Size - Size) >= MINIMUM_MEMORY_SIZE)
         {
            /* If this is a large segment allocation, then split the    */
            /* segment so that the free segment is at the beginning.    */
            if(Size >= LARGE_SIZE)
            {
               /* Re-size the current block for the remaining space.    */
               BlockInfo->Size = RemainingSize;

               /* Initialize the new block, setting it to allocated.    */
               BlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) + RemainingSize);
               BlockInfo->PrevSize = RemainingSize;
               BlockInfo->Size     = Size | SEGMENT_ALLOCATED_BITMASK;

               /* Set the TmpBlock pointer to the current block and the */
               /* RemainingSize to its size for setting the previous    */
               /* size of the next block.                               */
               TempBlockInfo = BlockInfo;
               RemainingSize = Size;
            }
            else
            {
               /* Re-size the current block and set it as allocated.    */
               BlockInfo->Size = Size | SEGMENT_ALLOCATED_BITMASK;

               /* Initialize the new block.                             */
               TempBlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) + Size);
               TempBlockInfo->PrevSize = Size;
               TempBlockInfo->Size     = RemainingSize;
            }

            /* Calculate the pointer to the next segment, checking for a*/
            /* wrap condition, and update the next segment's PrevSize   */
            /* field.                                                   */
            if((TempBlockInfo = (BlockInfo_t *)(((Alignment_t *)TempBlockInfo) + RemainingSize)) != HeapInfo->HeapTail)
               TempBlockInfo->PrevSize = RemainingSize;
            else
            {
               /* This block is the last in the Heap so set the previous*/
               /* size of the heap head instead.                        */
               HeapInfo->HeapHead->PrevSize = RemainingSize;
            }
         }
         else
         {
            /* Update the allocated size to be that of the block that   */
            /* was found and set the block to allocated.                */
            Size             = BlockInfo->Size;
            BlockInfo->Size |= SEGMENT_ALLOCATED_BITMASK;
         }

         /* Get the address of the start of the allocated memory.       */
         ret_val = (void *)(BlockInfo->Data);

         /* Adjust the memory statistics.                               */
         HeapInfo->CurrentHeapUsed += Size;
         if(HeapInfo->MaximumHeapUsed < HeapInfo->CurrentHeapUsed)
            HeapInfo->MaximumHeapUsed = HeapInfo->CurrentHeapUsed;
      }
      else
      {
         /* No suitable memory block found.                             */
         ret_val = NULL;
      }
   }
   else
      ret_val = NULL;

   return(ret_val);
}

   /* The following function is used to free memory that was previously */
   /* allocated with MemAlloc.  The function takes as its parameter a   */
   /* pointer to the memory that was allocated.  The pointer is used to */
   /* locate the structure of information that describes the allocated  */
   /* fragment.  The function tries to a verify that the structure is a */
   /* valid fragment structure before the memory is freed.  When a      */
   /* fragment is freed, it may be combined with adjacent fragments to  */
   /* produce a larger free fragment.                                   */
static void MemFree(void *Heap, void *MemoryPtr)
{
   HeapInfo_t  *HeapInfo;
   BlockInfo_t *BlockInfo;
   BlockInfo_t *TempBlockInfo;

   HeapInfo = (HeapInfo_t *)Heap;

   /* Verify that the parameter passed in appears valid.                */
   if((HeapInfo) && (HeapInfo->Initialized) && (MemoryPtr) && (MemoryPtr >= (void *)(HeapInfo->HeapHead->Data)) && (MemoryPtr < (void *)(HeapInfo->HeapTail)))
   {
      /* Get a pointer to the Block Info.                               */
      BlockInfo = (BlockInfo_t *)(((Alignment_t *)MemoryPtr) - BLOCK_INFO_SIZE(0));

      /* Verify that this segment is allocated.                         */
      if(BlockInfo->Size & SEGMENT_ALLOCATED_BITMASK)
      {
         /* Set the current block as un-allocated.                      */
         BlockInfo->Size &= ~SEGMENT_ALLOCATED_BITMASK;

         /* Update the Heap Statistics.                                 */
         HeapInfo->CurrentHeapUsed -= BlockInfo->Size;

         /* Try to combine this segment with the previous segment.      */
         if(BlockInfo != HeapInfo->HeapHead)
         {
            TempBlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) - BlockInfo->PrevSize);

            if(!(TempBlockInfo->Size & SEGMENT_ALLOCATED_BITMASK))
            {
               /* Combine this segment with the newly freed segment.    */
               TempBlockInfo->Size += BlockInfo->Size;
               BlockInfo = TempBlockInfo;
            }
         }

         /* Try to combine this segment with the following segment.     */
         if((TempBlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) + BlockInfo->Size)) < HeapInfo->HeapTail)
         {
            if(!(TempBlockInfo->Size & SEGMENT_ALLOCATED_BITMASK))
            {
               BlockInfo->Size += TempBlockInfo->Size;

            }
         }

         /* Update the previous size of the next block.                 */
         if((TempBlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) + BlockInfo->Size)) != HeapInfo->HeapTail)
            TempBlockInfo->PrevSize = BlockInfo->Size;
         else
         {
            /* This is the last block so adjust the previous size of the*/
            /* first block instead.                                     */
            HeapInfo->HeapHead->PrevSize = BlockInfo->Size;
         }

      }
   }
}

   /* The following function will find statistics for current heap      */
   /* usage.  This function accepts as its parameter a pointer to a     */
   /* memory statistics structure and a flag to indicate if fragment    */
   /* information will be determined.  The function will return zero if */
   /* successful or a negative value if there is an error.              */
   /* * NOTE * If the advanced statitistics flag is set to FALSE, then  */
   /*          the largest free fragment and free fragment count will be*/
   /*          set to zero.                                             */
static int GetHeapStatistics(void *Heap, BTPS_MemoryStatistics_t *MemoryStatistics, Boolean_t AdvancedStatitics)
{
   int          ret_val;
   HeapInfo_t  *HeapInfo;
   BlockInfo_t *BlockInfo;

   HeapInfo = (HeapInfo_t *)Heap;

   /* Verify that the parameters that were passed in appear valid.      */
   if((HeapInfo) && (HeapInfo->Initialized) && (MemoryStatistics))
   {
      BTPS_MemInitialize(MemoryStatistics, 0, sizeof(BTPS_MemoryStatistics_t));

      /* Assign the basic heap statistics.                              */
      MemoryStatistics->HeapSize        = (unsigned int)(((unsigned char *)(HeapInfo->HeapTail)) - ((unsigned char *)(HeapInfo->HeapHead)));
      MemoryStatistics->CurrentHeapUsed = HeapInfo->CurrentHeapUsed * ALIGNMENT_SIZE;
      MemoryStatistics->MaximumHeapUsed = HeapInfo->MaximumHeapUsed * ALIGNMENT_SIZE;

      if(AdvancedStatitics)
      {
         /* Walk the heap and calculate the advanced statistics.        */
         BlockInfo = HeapInfo->HeapHead;

         while(BlockInfo < HeapInfo->HeapTail)
         {
            if(!(BlockInfo->Size & SEGMENT_ALLOCATED_BITMASK))
            {
               /* Block is un-allocated.                                */
               MemoryStatistics->FreeFragmentCount ++;

               if(MemoryStatistics->LargestFreeFragment < BlockInfo->Size)
                  MemoryStatistics->LargestFreeFragment = BlockInfo->Size;
            }

            /* Adjust the pointer to the next entry.                    */
            BlockInfo = (BlockInfo_t *)(((Alignment_t *)BlockInfo) + (BlockInfo->Size & SEGMENT_SIZE_BITMASK));
         }

         /* Convert the size of the largest free fragment to bytes.     */
         MemoryStatistics->LargestFreeFragment *= ALIGNMENT_SIZE;
      }


      ret_val = 0;
   }
   else
      ret_val = -1;

   return(ret_val);
}

   /* The following function is a function that represents the native   */
   /* thread type for the operating system.  This function does nothing */
   /* more than to call the BTPSKRNL Thread function of the specified   */
   /* format (parameters/calling convention/etc.).  The UserData        */
   /* parameter that is passed to this function is a pointer to a       */
   /* ThreadWrapperInfo_t structure which will contain the actual       */
   /* BTPSKRNL Thread Information.  This function will free this pointer*/
   /* using the BTPS_FreeMemory() function (which means that the Thread */
   /* Information structure needs to be allocated with the              */
   /* BTPS_AllocateMemory() function.                                   */
static void ThreadWrapper(void *UserData)
{
   if(UserData)
   {
      ((*(((ThreadWrapperInfo_t *)UserData)->ThreadFunction))(((ThreadWrapperInfo_t *)UserData)->ThreadParameter));

      /* If we exit a thread free the ThreadWrapperInfo structure, and  */
      /* then delete this thread.                                       */
      BTPS_FreeMemory(UserData);

      /* Delete the current thread.                                     */
      vTaskDelete(NULL);
   }
}

   /* The following function is responsible for the Memory Usage        */
   /* Information.  This function accepts as its parameter a pointer to */
   /* a memory statistics structure and a flag to indicate if fragment  */
   /* information will be determined.  The function will return zero if */
   /* successful or a negative value if there is an error.              */
   /* * NOTE * If the advanced statitistics flag is set to FALSE, then  */
   /*          the largest free fragment and free fragment count will be*/
   /*          set to zero.                                             */
int BTPSAPI BTPS_QueryMemoryUsage(BTPS_MemoryStatistics_t *MemoryStatistics, Boolean_t AdvancedStatitics)
{
   return(GetHeapStatistics(MemoryBuffer, MemoryStatistics, AdvancedStatitics));
}

   /* The following function is responsible for delaying the current    */
   /* task for the specified duration (specified in Milliseconds).      */
   /* * NOTE * Very small timeouts might be smaller in granularity than */
   /*          the system can support !!!!                              */
void BTPSAPI BTPS_Delay(unsigned long MilliSeconds)
{
   /* Simply wrap the OS supplied TaskDelay function.                   */
   if(MilliSeconds == BTPS_INFINITE_WAIT)
   {
      while(TRUE)
         vTaskDelay(portMAX_DELAY);
   }
   else
      vTaskDelay(MILLISECONDS_TO_TICKS(MilliSeconds));
}

   /* The following function is responsible for creating an actual Mutex*/
   /* (Binary Semaphore).  The Mutex is unique in that if a Thread      */
   /* already owns the Mutex, and it requests the Mutex again it will be*/
   /* granted the Mutex.  This is in Stark contrast to a Semaphore that */
   /* will block waiting for the second acquisition of the Sempahore.   */
   /* This function accepts as input whether or not the Mutex is        */
   /* initially Signalled or not.  If this input parameter is TRUE then */
   /* the caller owns the Mutex and any other threads waiting on the    */
   /* Mutex will block.  This function returns a NON-NULL Mutex Handle  */
   /* if the Mutex was successfully created, or a NULL Mutex Handle if  */
   /* the Mutex was NOT created.  If a Mutex is successfully created, it*/
   /* can only be destroyed by calling the BTPS_CloseMutex() function   */
   /* (and passing the returned Mutex Handle).                          */
Mutex_t BTPSAPI BTPS_CreateMutex(Boolean_t CreateOwned)
{
   Mutex_t ret_val;

   /* Simply create a Recursive Mutex.                                  */
   if((ret_val = (Mutex_t)xSemaphoreCreateRecursiveMutex()) != NULL)
   {
      /* Check to see if we need to own the Mutex.                      */
      if(CreateOwned)
      {
         if(xSemaphoreTakeRecursive((xSemaphoreHandle)ret_val, (portTickType)1) != pdTRUE)
         {
            /* Error with the Semaphore, go ahead and delete it and flag*/
            /* an error.                                                */
            vQueueDelete((xQueueHandle)ret_val);

            ret_val = NULL;
         }
      }
   }

   if(!ret_val)
      DBG_MSG(DBG_ZONE_BTPSKRNL,("Failed to create Mutex\r\n"));

   return(ret_val);
}

   /* The following function is responsible for waiting for the         */
   /* specified Mutex to become free.  This function accepts as input   */
   /* the Mutex Handle to wait for, and the Timeout (specified in       */
   /* Milliseconds) to wait for the Mutex to become available.  This    */
   /* function returns TRUE if the Mutex was successfully acquired and  */
   /* FALSE if either there was an error OR the Mutex was not acquired  */
   /* in the specified Timeout.  It should be noted that Mutexes have   */
   /* the special property that if the calling Thread already owns the  */
   /* Mutex and it requests access to the Mutex again (by calling this  */
   /* function and specifying the same Mutex Handle) then it will       */
   /* automatically be granted the Mutex.  Once a Mutex has been granted*/
   /* successfully (this function returns TRUE), then the caller MUST   */
   /* call the BTPS_ReleaseMutex() function.                            */
   /* * NOTE * There must exist a corresponding BTPS_ReleaseMutex()     */
   /*          function call for EVERY successful BTPS_WaitMutex()      */
   /*          function call or a deadlock will occur in the system !!! */
Boolean_t BTPSAPI BTPS_WaitMutex(Mutex_t Mutex, unsigned long Timeout)
{
   Boolean_t     ret_val = FALSE;
   unsigned long DecrementWait;

   /* Before proceeding any further we need to make sure that the       */
   /* parameters that were passed to us appear semi-valid.              */
   if(Mutex)
   {
      /* The event is set to wait for a finite amount of time, in order */
      /* to do this we must poll the event for that amount of time.     */
      ret_val = FALSE;

      if(Timeout == BTPS_INFINITE_WAIT)
         DecrementWait = portMAX_DELAY;
      else
      {
         Timeout = (unsigned long)MILLISECONDS_TO_TICKS(Timeout);

         if(Timeout > portMAX_DELAY)
            DecrementWait = portMAX_DELAY;
         else
            DecrementWait = Timeout;
      }

      while((Timeout) && (!ret_val))
      {
         /* Attempt to get the semaphore for the timeout duration.      */
         if(xSemaphoreTakeRecursive(((xSemaphoreHandle)Mutex), DecrementWait) == pdPASS)
         {
            /* Mutex acquired successfully.                             */
            ret_val = TRUE;
         }
         else
         {
            /* Decrement the timeout.                                   */
            if(Timeout != BTPS_INFINITE_WAIT)
            {
               if(Timeout >= DecrementWait)
                  Timeout -= DecrementWait;
               else
                  DecrementWait = Timeout;
            }
         }
      }
   }

   /* Return the result to the caller.                                  */
   return(ret_val);
}

   /* The following function is responsible for releasing a Mutex that  */
   /* was successfully acquired with the BTPS_WaitMutex() function.     */
   /* This function accepts as input the Mutex that is currently owned. */
   /* * NOTE * There must exist a corresponding BTPS_ReleaseMutex()     */
   /*          function call for EVERY successful BTPS_WaitMutex()      */
   /*          function call or a deadlock will occur in the system !!! */
void BTPSAPI BTPS_ReleaseMutex(Mutex_t Mutex)
{
   /* Before proceeding any further we need to make sure that the       */
   /* parameters that were passed to us appear semi-valid.              */
   if(Mutex)
   {
      /* Simply give back the Semaphore.                                */
      xSemaphoreGiveRecursive((xSemaphoreHandle)Mutex);
   }
}

   /* The following function is responsible for destroying a Mutex that */
   /* was created successfully via a successful call to the             */
   /* BTPS_CreateMutex() function.  This function accepts as input the  */
   /* Mutex Handle of the Mutex to destroy.  Once this function is      */
   /* completed the Mutex Handle is NO longer valid and CANNOT be used. */
   /* Calling this function will cause all outstanding BTPS_WaitMutex() */
   /* functions to fail with an error.                                  */
void BTPSAPI BTPS_CloseMutex(Mutex_t Mutex)
{
   /* Before proceeding any further we need to make sure that the       */
   /* parameters that were passed to us appear semi-valid.              */
   if(Mutex)
   {
      /* Note that there is no explicit mechanism to delete a           */
      /* semaphore/Mutex, thus we will free the Queue memory that is    */
      /* at a low level to implement semaphore/Mutex in FreeRTOS.       */
      vQueueDelete((xQueueHandle)Mutex);
   }
}

   /* The following function is responsible for creating an actual      */
   /* Event.  The Event is unique in that it only has two states.  These*/
   /* states are Signalled and Non-Signalled.  Functions are provided to*/
   /* allow the setting of the current state and to allow the option of */
   /* waiting for an Event to become Signalled.  This function accepts  */
   /* as input whether or not the Event is initially Signalled or not.  */
   /* If this input parameter is TRUE then the state of the Event is    */
   /* Signalled and any BTPS_WaitEvent() function calls will immediately*/
   /* return.  This function returns a NON-NULL Event Handle if the     */
   /* Event was successfully created, or a NULL Event Handle if the     */
   /* Event was NOT created.  If an Event is successfully created, it   */
   /* can only be destroyed by calling the BTPS_CloseEvent() function   */
   /* (and passing the returned Event Handle).                          */
Event_t BTPSAPI BTPS_CreateEvent(Boolean_t CreateSignalled)
{
   Event_t        ret_val     = NULL;
   EventHeader_t *EventHeader = NULL;

   /* Before preceding allocate memory for the event header and verify  */
   /* that the allocate was successful.                                 */
   if((EventHeader = (EventHeader_t *)BTPS_AllocateMemory(sizeof(EventHeader_t))) != NULL)
   {
      /* Create the semaphore to be used as the event.                  */
      vSemaphoreCreateBinary(EventHeader->SemaphoreHandle);
      if(EventHeader->SemaphoreHandle)
      {
         /* The semaphore to be used as the event was successfully      */
         /* created, now set the state of the semaphore.                */
         if(!CreateSignalled)
         {
            /* Go ahead and grab the Semaphore.                         */
            if(xSemaphoreTake(EventHeader->SemaphoreHandle, (portTickType)1) == pdTRUE)
               ret_val = EventHeader;
         }
         else
            ret_val = EventHeader;

         EventHeader->IsSet = CreateSignalled;
         EventHeader->Count = (CreateSignalled)?0:1;
      }
      else
      {
         DBG_MSG(DBG_ZONE_BTPSKRNL,("Create Semaphore Error\r\n"));
      }
   }

   /* Check if an error occurred while creating the Event Header, if so  */
   /* deallocate the Event Header.                                      */
   if(!ret_val)
   {
      /* Note there is currently no mechanism to delete a               */
      /* semaphore/mutex.                                               */
      if(EventHeader)
      {
         vQueueDelete((xQueueHandle)(EventHeader->SemaphoreHandle));

         BTPS_FreeMemory(EventHeader);
      }
   }

   return((Event_t)ret_val);
}

   /* The following function is responsible for waiting for the         */
   /* specified Event to become Signalled.  This function accepts as    */
   /* input the Event Handle to wait for, and the Timeout (specified in */
   /* Milliseconds) to wait for the Event to become Signalled.  This    */
   /* function returns TRUE if the Event was set to the Signalled State */
   /* (in the Timeout specified) or FALSE if either there was an error  */
   /* OR the Event was not set to the Signalled State in the specified  */
   /* Timeout.  It should be noted that Signals have a special property */
   /* in that multiple Threads can be waiting for the Event to become   */
   /* Signalled and ALL calls to BTPS_WaitEvent() will return TRUE      */
   /* whenever the state of the Event becomes Signalled.                */
Boolean_t BTPSAPI BTPS_WaitEvent(Event_t Event, unsigned long Timeout)
{
   Boolean_t     ret_val;
   unsigned long DecrementWait;

   /* Before proceeding any further we need to make sure that the       */
   /* parameters that were passed to us appear semi-valid.              */
   if(Event)
   {
      /* Lock the scheduler.                                            */
      vTaskSuspendAll();

      /* Check to see if the current state indicates that we are        */
      /* currently reset.                                               */
      if(!((EventHeader_t *)Event)->IsSet)
      {
         /* The event is in the reset state.  Increment the count.      */
         ((EventHeader_t *)Event)->Count++;

         /* Resume the Scheduler.                                       */
         xTaskResumeAll();

         if(Timeout == BTPS_INFINITE_WAIT)
            DecrementWait = portMAX_DELAY;
         else
         {
            Timeout = (unsigned long)MILLISECONDS_TO_TICKS(Timeout);

            if(Timeout > portMAX_DELAY)
               DecrementWait = portMAX_DELAY;
            else
               DecrementWait = Timeout;
         }

         ret_val = FALSE;
         do
         {
            /* Attempt to get the semaphore for the timeout duration.   */
            if(xSemaphoreTake((((EventHeader_t *)Event)->SemaphoreHandle), DecrementWait) == pdPASS)
            {
               /* Semaphore acquired successfully.                      */
               ret_val = TRUE;
            }
            else
            {
               if(Timeout != BTPS_INFINITE_WAIT)
               {
                  if(Timeout >= DecrementWait)
                     Timeout -= DecrementWait;
                  else
                     DecrementWait = Timeout;
               }
            }
         } while((Timeout) && (!ret_val));

         if(!ret_val)
         {
            /* A timeout or an error occurred, Lock the scheduler so we  */
            /* can modify the count variable.                           */
            vTaskSuspendAll();

            /* Decrement the count.                                     */
            ((EventHeader_t *)Event)->Count--;

            /* Resume the Scheduler.                                    */
            xTaskResumeAll();
         }
      }
      else
      {
         /* Resume the Scheduler.                                       */
         xTaskResumeAll();

         /* The event is in the set state, set the return value to true.*/
         ret_val = TRUE;
      }
   }
   else
      ret_val = FALSE;

   return(ret_val);
}

   /* The following function is responsible for changing the state of   */
   /* the specified Event to the Non-Signalled State.  Once the Event is*/
   /* in this State, ALL calls to the BTPS_WaitEvent() function will    */
   /* block until the State of the Event is set to the Signalled State. */
   /* This function accepts as input the Event Handle of the Event to   */
   /* set to the Non-Signalled State.                                   */
void BTPSAPI BTPS_ResetEvent(Event_t Event)
{
   /* Before proceeding any further we need to make sure that the       */
   /* parameter that was passed to us appears to be semi-valid.         */
   if(Event)
   {
      /* Lock the scheduler.                                            */
      vTaskSuspendAll();

      /* We have successfully acquired the mutex, now check to see if the*/
      /* event is currently signaled.                                   */
      if(((EventHeader_t *)Event)->IsSet)
      {
         /* The event is currently set, change the flag and update the  */
         /* count.                                                      */
         ((EventHeader_t *)Event)->IsSet = FALSE;
         ((EventHeader_t *)Event)->Count++;

         /* Resume the Scheduler.                                       */
         xTaskResumeAll();

         /* Reset the event semaphore.                                  */
         xSemaphoreTake(((EventHeader_t *)Event)->SemaphoreHandle, (portTickType)1);
      }
      else
      {
         /* Resume the Scheduler.                                       */
         xTaskResumeAll();
      }
   }
}

   /* The following function is responsible for changing the state of   */
   /* the specified Event to the Signalled State.  Once the Event is in */
   /* this State, ALL calls to the BTPS_WaitEvent() function will       */
   /* return.  This function accepts as input the Event Handle of the   */
   /* Event to set to the Signalled State.                              */
void BTPSAPI BTPS_SetEvent(Event_t Event)
{
   /* Before proceeding any further we need to make sure that the       */
   /* parameter that was passed to us appears to be semi-valid.         */
   if(Event)
   {
      /* Lock the scheduler.                                            */
      vTaskSuspendAll();

      /* We have successfully acquired the mutex, now check to see if   */
      /* the event is currently signaled.                               */
      if(!((EventHeader_t *)Event)->IsSet)
      {
         ((EventHeader_t *)Event)->IsSet = TRUE;

         /* The event is not currently set, change the flag and post the*/
         /* semaphore count times.                                      */
         while(((EventHeader_t *)Event)->Count)
         {
            ((EventHeader_t *)Event)->Count--;
            xSemaphoreGive(((EventHeader_t *)Event)->SemaphoreHandle);
         }
     }

      /* Resume the Scheduler.                                          */
      xTaskResumeAll();
   }
}


   /* The following function is responsible for destroying an Event that*/
   /* was created successfully via a successful call to the             */
   /* BTPS_CreateEvent() function.  This function accepts as input the  */
   /* Event Handle of the Event to destroy.  Once this function is      */
   /* completed the Event Handle is NO longer valid and CANNOT be used. */
   /* Calling this function will cause all outstanding BTPS_WaitEvent() */
   /* functions to fail with an error.                                  */
void BTPSAPI BTPS_CloseEvent(Event_t Event)
{
   /* Set the event to force any outstanding calls to return.           */
   BTPS_SetEvent(Event);

   /* Call FreeRTOS to free kernel allocated memory.                    */
   vQueueDelete((xQueueHandle)(((EventHeader_t *)Event)->SemaphoreHandle));

   /* Free memory allocated by BTPS_CreateEvent.                        */
   BTPS_FreeMemory(Event);
}

   /* The following function is provided to allow a mechanism to        */
   /* actually allocate a Block of Memory (of at least the specified    */
   /* size).  This function accepts as input the size (in Bytes) of the */
   /* Block of Memory to be allocated.  This function returns a NON-NULL*/
   /* pointer to this Memory Buffer if the Memory was successfully      */
   /* allocated, or a NULL value if the memory could not be allocated.  */
void *BTPSAPI BTPS_AllocateMemory(unsigned long MemorySize)
{
   void *ret_val;

   if(BTPS_WaitMutex(KernelMutex, BTPS_INFINITE_WAIT))
   {
      ret_val = MemAlloc(MemoryBuffer, MemorySize);

      BTPS_ReleaseMutex(KernelMutex);
   }
   else
      ret_val = NULL;

   if(!ret_val)
      DBG_MSG(DBG_ZONE_BTPSKRNL, ("Malloc Failed: %d.\r\n", MemorySize));

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for de-allocating a Block of*/
   /* Memory that was successfully allocated with the                   */
   /* BTPS_AllocateMemory() function.  This function accepts a NON-NULL */
   /* Memory Pointer which was returned from the BTPS_AllocateMemory()  */
   /* function.  After this function completes the caller CANNOT use ANY*/
   /* of the Memory pointed to by the Memory Pointer.                   */
void BTPSAPI BTPS_FreeMemory(void *MemoryPointer)
{
   if(BTPS_WaitMutex(KernelMutex, BTPS_INFINITE_WAIT))
   {
      MemFree(MemoryBuffer, MemoryPointer);

      BTPS_ReleaseMutex(KernelMutex);
   }
}

   /* The following function is responsible for copying a block of      */
   /* memory of the specified size from the specified source pointer to */
   /* the specified destination memory pointer.  This function accepts  */
   /* as input a pointer to the memory block that is to be Destination  */
   /* Buffer (first parameter), a pointer to memory block that points to*/
   /* the data to be copied into the destination buffer, and the size   */
   /* (in bytes) of the Data to copy.  The Source and Destination Memory*/
   /* Buffers must contain AT LEAST as many bytes as specified by the   */
   /* Size parameter.                                                   */
   /* * NOTE * This function does not allow the overlapping of the      */
   /*          Source and Destination Buffers !!!!                      */
void BTPSAPI BTPS_MemCopy(void *Destination, BTPSCONST void *Source, unsigned long Size)
{
   /* Simply wrap the C Run-Time memcpy() function.                     */
   memcpy(Destination, Source, Size);
}

   /* The following function is responsible for moving a block of memory*/
   /* of the specified size from the specified source pointer to the    */
   /* specified destination memory pointer.  This function accepts as   */
   /* input a pointer to the memory block that is to be Destination     */
   /* Buffer (first parameter), a pointer to memory block that points to*/
   /* the data to be copied into the destination buffer, and the size   */
   /* (in bytes) of the Data to copy.  The Source and Destination Memory*/
   /* Buffers must contain AT LEAST as many bytes as specified by the   */
   /* Size parameter.                                                   */
   /* * NOTE * This function DOES allow the overlapping of the Source   */
   /*          and Destination Buffers.                                 */
void BTPSAPI BTPS_MemMove(void *Destination, BTPSCONST void *Source, unsigned long Size)
{
   /* Simply wrap the C Run-Time memmove() function.                    */
   memmove(Destination, Source, Size);
}

   /* The following function is provided to allow a mechanism to fill a */
   /* block of memory with the specified value.  This function accepts  */
   /* as input a pointer to the Data Buffer (first parameter) that is to*/
   /* filled with the specified value (second parameter).  The final    */
   /* parameter to this function specifies the number of bytes that are */
   /* to be filled in the Data Buffer.  The Destination Buffer must     */
   /* point to a Buffer that is AT LEAST the size of the Size parameter.*/
void BTPSAPI BTPS_MemInitialize(void *Destination, unsigned char Value, unsigned long Size)
{
   /* Simply wrap the C Run-Time memset() function.                     */
   memset(Destination, Value, Size);
}

   /* The following function is provided to allow a mechanism to Compare*/
   /* two blocks of memory to see if the two memory blocks (each of size*/
   /* Size (in bytes)) are equal (each and every byte up to Size bytes).*/
   /* This function returns a negative number if Source1 is less than   */
   /* Source2, zero if Source1 equals Source2, and a positive value if  */
   /* Source1 is greater than Source2.                                  */
int BTPSAPI BTPS_MemCompare(BTPSCONST void *Source1, BTPSCONST void *Source2, unsigned long Size)
{
   /* Simply wrap the C Run-Time memcmp() function.                     */
   return(memcmp(Source1, Source2, Size));
}

   /* The following function is provided to allow a mechanism to Compare*/
   /* two blocks of memory to see if the two memory blocks (each of size*/
   /* Size (in bytes)) are equal (each and every byte up to Size bytes) */
   /* using a Case-Insensitive Compare.  This function returns a        */
   /* negative number if Source1 is less than Source2, zero if Source1  */
   /* equals Source2, and a positive value if Source1 is greater than   */
   /* Source2.                                                          */
int BTPSAPI BTPS_MemCompareI(BTPSCONST void *Source1, BTPSCONST void *Source2, unsigned long Size)
{
   int           ret_val = 0;
   unsigned char Byte1;
   unsigned char Byte2;
   unsigned int  Index;

   /* Simply loop through each byte pointed to by each pointer and check*/
   /* to see if they are equal.                                         */
   for(Index = 0; ((Index<Size) && (!ret_val)); Index++)
   {
      /* Note each Byte that we are going to compare.                   */
      Byte1 = ((unsigned char *)Source1)[Index];
      Byte2 = ((unsigned char *)Source2)[Index];

      /* If the Byte in the first array is lower case, go ahead and make*/
      /* it upper case (for comparisons below).                         */
      if((Byte1 >= 'a') && (Byte1 <= 'z'))
         Byte1 = Byte1 - ('a' - 'A');

      /* If the Byte in the second array is lower case, go ahead and    */
      /* make it upper case (for comparisons below).                    */
      if((Byte2 >= 'a') && (Byte2 <= 'z'))
         Byte2 = Byte2 - ('a' - 'A');

      /* If the two Bytes are equal then there is nothing to do.        */
      if(Byte1 != Byte2)
      {
         /* Bytes are not equal, so set the return value accordingly.   */
         if(Byte1 < Byte2)
            ret_val = -1;
         else
            ret_val = 1;
      }
   }

   /* Simply return the result of the above comparison(s).              */
   return(ret_val);
}

   /* The following function is provided to allow a mechanism to copy a */
   /* source NULL Terminated ASCII (character) String to the specified  */
   /* Destination String Buffer.  This function accepts as input a      */
   /* pointer to a buffer (Destination) that is to receive the NULL     */
   /* Terminated ASCII String pointed to by the Source parameter.  This */
   /* function copies the string byte by byte from the Source to the    */
   /* Destination (including the NULL terminator).                      */
void BTPSAPI BTPS_StringCopy(char *Destination, BTPSCONST char *Source)
{
   /* Simply wrap the C Run-Time strcpy() function.                     */
   strcpy(Destination, Source);
}

   /* The following function is provided to allow a mechanism to        */
   /* determine the Length (in characters) of the specified NULL        */
   /* Terminated ASCII (character) String.  This function accepts as    */
   /* input a pointer to a NULL Terminated ASCII String and returns the */
   /* number of characters present in the string (NOT including the     */
   /* terminating NULL character).                                      */
unsigned int BTPSAPI BTPS_StringLength(BTPSCONST char *Source)
{
   /* Simply wrap the C Run-Time strlen() function.                     */
   return(strlen(Source));
}

   /* The following function is provided to allow a mechanism for a C   */
   /* Run-Time Library sprintf() function implementation.  This function*/
   /* accepts as its imput the output buffer, a format string and a     */
   /* variable number of arguments determined by the format string.     */
int BTPSAPI BTPS_SprintF(char *Buffer, BTPSCONST char *Format, ...)
{
   int     ret_val;
   va_list args;

   va_start(args, Format);
   ret_val = vsprintf(Buffer, Format, args);
   va_end(args);

   return(ret_val);
}

   /* The following function is provided to allow a means for the       */
   /* programmer to create a separate thread of execution.  This        */
   /* function accepts as input the Function that represents the Thread */
   /* that is to be installed into the system as its first parameter.   */
   /* The second parameter is the size of the Threads Stack (in bytes)  */
   /* required by the Thread when it is executing.  The final parameter */
   /* to this function represents a parameter that is to be passed to   */
   /* the Thread when it is created.  This function returns a NON-NULL  */
   /* Thread Handle if the Thread was successfully created, or a NULL   */
   /* Thread Handle if the Thread was unable to be created.  Once the   */
   /* thread is created, the only way for the Thread to be removed from */
   /* the system is for the Thread function to run to completion.       */
   /* * NOTE * There does NOT exist a function to Kill a Thread that is */
   /*          present in the system.  Because of this, other means     */
   /*          needs to be devised in order to signal the Thread that it*/
   /*          is to terminate.                                         */
ThreadHandle_t BTPSAPI BTPS_CreateThread(Thread_t ThreadFunction, unsigned int StackSize, void *ThreadParameter)
{
   long                 Result;
   ThreadHandle_t       ret_val;
   ThreadWrapperInfo_t *ThreadWrapperInfo;

   /* Wrap the OS Thread Creation function.                             */
   /* * NOTE * Because the OS Thread function and the BTPS Thread       */
   /*          function are not necessarily the same, we will wrap the  */
   /*          BTPS Thread within the real OS thread.                   */
   if(ThreadFunction)
   {
      /* Make sure that we allocate at least the minimal stack size     */
      if(StackSize < (configMINIMAL_STACK_SIZE * sizeof(portSTACK_TYPE)))
         StackSize = configMINIMAL_STACK_SIZE * sizeof(portSTACK_TYPE);

      /* First we need to allocate memory for a ThreadWrapperInfo_t     */
      /* structure to hold the data we are going to pass to the thread. */
      if((ThreadWrapperInfo = (ThreadWrapperInfo_t *)BTPS_AllocateMemory(sizeof(ThreadWrapperInfo_t))) != NULL)
      {
         /* Memory allocated, populate the structure with the correct   */
         /* information.                                                */
         BTPS_SprintF(ThreadWrapperInfo->Name, "BT%d", ThreadCount++);

         ThreadWrapperInfo->ThreadFunction  = ThreadFunction;
         ThreadWrapperInfo->ThreadParameter = ThreadParameter;

         /* Next attempt to create a thread using the default priority. */
         Result = xTaskCreate(ThreadWrapper, (char const *)ThreadWrapperInfo->Name, StackSize / sizeof(portSTACK_TYPE), ThreadWrapperInfo, DEFAULT_THREAD_PRIORITY, &ThreadWrapperInfo->Thread);
         if(Result != pdPASS)
         {
            /* An error occurred while attempting to create the thread. */
            /* Free any previously allocated resources and set the      */
            /* return value to indicate and error has occurred.         */
            DBG_MSG(DBG_ZONE_BTPSKRNL, ("xTaskCreate failed.\r\n"));
            BTPS_FreeMemory(ThreadWrapperInfo);
            ret_val = NULL;
         }
         else
         {
            ret_val = ThreadWrapperInfo->Thread;
         }
      }
      else
         ret_val = NULL;
   }
   else
      ret_val = NULL;

   /* Return the result to the caller.                                  */
   return(ret_val);
}

   /* The following function is provided to allow a mechanism to        */
   /* retrieve the handle of the thread which is currently executing.   */
   /* This function require no input parameters and will return a valid */
   /* ThreadHandle upon success.                                        */
ThreadHandle_t BTPSAPI BTPS_CurrentThreadHandle(void)
{
   /* Simply return the Current Thread Handle that is executing.        */
   return(xTaskGetCurrentTaskHandle());
}

   /* The following function is provided to allow a mechanism to create */
   /* a Mailbox.  A Mailbox is a Data Store that contains slots (all of */
   /* the same size) that can have data placed into (and retrieved      */
   /* from).  Once Data is placed into a Mailbox (via the               */
   /* BTPS_AddMailbox() function, it can be retrieved by using the      */
   /* BTPS_WaitMailbox() function.  Data placed into the Mailbox is     */
   /* retrieved in a FIFO method.  This function accepts as input the   */
   /* Maximum Number of Slots that will be present in the Mailbox and   */
   /* the Size of each of the Slots.  This function returns a NON-NULL  */
   /* Mailbox Handle if the Mailbox is successfully created, or a NULL  */
   /* Mailbox Handle if the Mailbox was unable to be created.           */
Mailbox_t BTPSAPI BTPS_CreateMailbox(unsigned int NumberSlots, unsigned int SlotSize)
{
   Mailbox_t        ret_val;
   xQueueHandle     Queue;
   MailboxHeader_t *MailboxHeader;

   /* Before proceeding any further we need to make sure that the       */
   /* parameters that were passed to us appear semi-valid.              */
   if((NumberSlots) && (SlotSize))
   {
      /* Parameters appear semi-valid, so now let's allocate enough     */
      /* Memory to hold the Mailbox Header.                             */
      if((MailboxHeader = (MailboxHeader_t *)BTPS_AllocateMemory(sizeof(MailboxHeader_t))) != NULL)
      {
         /* If the xQueueCreate returns Non-ZERO then it was            */
         /* successfully.                                               */
         if((Queue = xQueueCreate(NumberSlots,SlotSize)) != NULL)
         {
            MailboxHeader->SlotSize    = SlotSize;
            MailboxHeader->QueueHandle = Queue;
            ret_val                    = (Mailbox_t)MailboxHeader;
         }
         else
         {
            BTPS_FreeMemory(MailboxHeader);

            ret_val = NULL;
         }
      }
      else
         ret_val = NULL;
   }
   else
      ret_val = NULL;

   /* Return the result to the caller.                                  */
   return(ret_val);
}

   /* The following function is provided to allow a means to Add data to*/
   /* the Mailbox (where it can be retrieved via the BTPS_WaitMailbox() */
   /* function.  This function accepts as input the Mailbox Handle of   */
   /* the Mailbox to place the data into and a pointer to a buffer that */
   /* contains the data to be added.  This pointer *MUST* point to a    */
   /* data buffer that is AT LEAST the Size of the Slots in the Mailbox */
   /* (specified when the Mailbox was created) and this pointer CANNOT  */
   /* be NULL.  The data that the MailboxData pointer points to is      */
   /* placed into the Mailbox where it can be retrieved via the         */
   /* BTPS_WaitMailbox() function.                                      */
   /* * NOTE * This function copies from the MailboxData Pointer the    */
   /*          first SlotSize Bytes.  The SlotSize was specified when   */
   /*          the Mailbox was created via a successful call to the     */
   /*          BTPS_CreateMailbox() function.                           */
Boolean_t BTPSAPI BTPS_AddMailbox(Mailbox_t Mailbox, void *MailboxData)
{
   xQueueHandle  QueueHandle;
   Boolean_t     ret_val;

   /* Before proceeding any further make sure that the Mailbox Handle   */
   /* and the MailboxData pointer that was specified appears semi-valid.*/
   if((Mailbox) && (MailboxData))
   {
      vTaskSuspendAll();
      QueueHandle = ((MailboxHeader_t *)Mailbox)->QueueHandle;
      xTaskResumeAll();

      /* Loop until we read the data out of the queue.                  */
      ret_val     = FALSE;
      while(!ret_val)
      {
         if(xQueueSend(QueueHandle,MailboxData,portMAX_DELAY) == pdTRUE)
            ret_val = TRUE;
      }
   }
   else
      ret_val = FALSE;

   /* Return the result to the caller.                                  */
   return(ret_val);
}

   /* The following function is provided to allow a means to retrieve   */
   /* data from the specified Mailbox.  This function will block until  */
   /* either Data is placed in the Mailbox or an error with the Mailbox */
   /* was detected.  This function accepts as its first parameter a     */
   /* Mailbox Handle that represents the Mailbox to wait for the data   */
   /* with.  This function accepts as its second parameter, a pointer to*/
   /* a data buffer that is AT LEAST the size of a single Slot of the   */
   /* Mailbox (specified when the BTPS_CreateMailbox() function was     */
   /* called).  The MailboxData parameter CANNOT be NULL.  This function*/
   /* will return TRUE if data was successfully retrieved from the      */
   /* Mailbox or FALSE if there was an error retrieving data from the   */
   /* Mailbox.  If this function returns TRUE then the first SlotSize   */
   /* bytes of the MailboxData pointer will contain the data that was   */
   /* retrieved from the Mailbox.                                       */
   /* * NOTE * This function copies to the MailboxData Pointer the data */
   /*          that is present in the Mailbox Slot (of size SlotSize).  */
   /*          The SlotSize was specified when the Mailbox was created  */
   /*          via a successful call to the BTPS_CreateMailbox()        */
   /*          function.                                                */
Boolean_t BTPSAPI BTPS_WaitMailbox(Mailbox_t Mailbox, void *MailboxData)
{
   xQueueHandle QueueHandle;
   Boolean_t    ret_val;

   /* Before proceeding any further make sure that the Mailbox Handle   */
   /* and the MailboxData pointer that was specified appears semi-valid.*/
   if((Mailbox) && (MailboxData))
   {
      vTaskSuspendAll();
      QueueHandle = ((MailboxHeader_t *)Mailbox)->QueueHandle;
      xTaskResumeAll();

      /* Loop until we read the data out of the queue.                  */
      ret_val     = FALSE;
      while(!ret_val)
      {
         if(xQueueReceive(QueueHandle,MailboxData,portMAX_DELAY) == pdTRUE)
            ret_val = TRUE;
      }
   }
   else
      ret_val = FALSE;

   /* Return the result to the caller.                                  */
   return(ret_val);
}

   /* The following function is responsible for destroying a Mailbox    */
   /* that was created successfully via a successful call to the        */
   /* BTPS_CreateMailbox() function.  This function accepts as input the*/
   /* Mailbox Handle of the Mailbox to destroy.  Once this function is  */
   /* completed the Mailbox Handle is NO longer valid and CANNOT be     */
   /* used.  Calling this function will cause all outstanding           */
   /* BTPS_WaitMailbox() functions to fail with an error.  The final    */
   /* parameter specifies an (optional) callback function that is called*/
   /* for each queued Mailbox entry.  This allows a mechanism to free   */
   /* any resources that might be associated with each individual       */
   /* Mailbox item.                                                     */
void BTPSAPI BTPS_DeleteMailbox(Mailbox_t Mailbox, BTPS_MailboxDeleteCallback_t MailboxDeleteCallback)
{
   unsigned int SlotSize;
   xQueueHandle QueueHandle;
   Boolean_t    Done;
   void        *Buffer;
   int          Result;

   /* Before proceeding any further make sure that the Mailbox Handle   */
   /* that was specified appears semi-valid.                            */
   if(Mailbox)
   {
      vTaskSuspendAll();
      SlotSize    = ((MailboxHeader_t *)Mailbox)->SlotSize;
      QueueHandle = ((MailboxHeader_t *)Mailbox)->QueueHandle;
      xTaskResumeAll();

      /* Check to see if a Mailbox Delete Item Callback was specified.  */
      if(MailboxDeleteCallback)
      {
         /* Allocate a buffer to hold the data in the queue slots.      */
         if((Buffer = BTPS_AllocateMemory(SlotSize)) != NULL)
         {
            /* Now loop though all of the occupied slots and call the   */
            /* callback with the slot data.                             */
            Done = FALSE;
            while(!Done)
            {
               /* Poll the queue for any data.                          */
               Result = xQueueReceive(QueueHandle,Buffer,0);

               /* If we read from the Queue then call the Mailbox Delete*/
               /* Callback otherwise we can exit the loop.              */
               if(Result == pdTRUE)
               {
                  __BTPSTRY
                  {
                     (*MailboxDeleteCallback)(Buffer);
                  }
                  __BTPSEXCEPT(1)
                  {
                     /* Do Nothing.                                     */
                  }
               }
               else
                  Done = TRUE;
            }

            BTPS_FreeMemory(Buffer);
         }
      }

      /* All finished cleaning up the Mailbox, simply free all          */
      /* memory that was allocated for the Mailbox.                     */
      vQueueDelete(QueueHandle);
      BTPS_FreeMemory(Mailbox);
   }
}

   /* The following function is used to initialize the Platform module. */
   /* The Platform module relies on some static variables that are used */
   /* to coordinate the abstraction.  When the module is initially      */
   /* started from a cold boot, all variables are set to the proper     */
   /* state.  If the Warm Boot is required, then these variables need to*/
   /* be reset to their default values.  This function sets all static  */
   /* parameters to their default values.                               */
   /* * NOTE * The implementation is free to pass whatever information  */
   /*          required in this parameter.                              */
void BTPSAPI BTPS_Init(void *UserParam)
{
   /* Input parameter represents the Debug Message Output Callback      */
   /* function.                                                         */
   if(UserParam)
   {
      if(((BTPS_Initialization_t *)UserParam)->MessageOutputCallback)
      {
         if((IOMutex = BTPS_CreateMutex(FALSE)) != NULL)
            MessageOutputCallback = ((BTPS_Initialization_t *)UserParam)->MessageOutputCallback;
         else
            MessageOutputCallback = NULL;
      }
   }
   else
      MessageOutputCallback = NULL;

   /* Initailize the Heap.                                              */
   HeapInit(MemoryBuffer, sizeof(MemoryBuffer));

   KernelMutex = BTPS_CreateMutex(FALSE);

   /* Initialize the static variables for this module.                  */
   DebugZoneMask              = DEBUG_ZONES;
   ThreadCount                = 0;
}

   /* The following function is used to cleanup the Platform module.    */
void BTPSAPI BTPS_DeInit(void)
{
   if(KernelMutex)
   {
      BTPS_CloseMutex(KernelMutex);
      KernelMutex = NULL;
   }
   if(IOMutex)
   {
      MessageOutputCallback = NULL;

      BTPS_CloseMutex(IOMutex);
      IOMutex = NULL;
   }
}

   /* Write out the specified NULL terminated Debugging String to the   */
   /* Debug output.                                                     */
void BTPSAPI BTPS_OutputMessage(BTPSCONST char *DebugString, ...)
{
   va_list args;
   int     Length;
   char    MsgBuffer[256];

   if(MessageOutputCallback)
   {
      /* Grab the I/O Mutex and send the Message.                       */
      if(BTPS_WaitMutex(IOMutex, BTPS_INFINITE_WAIT))
      {
         /* Write out the Data.                                         */
         va_start(args, DebugString);
         Length = vsnprintf(MsgBuffer, (sizeof(MsgBuffer) - 1), DebugString, args);
         va_end(args);

         MessageOutputCallback(Length, MsgBuffer);

         BTPS_ReleaseMutex(IOMutex);
      }
   }
}

   /* The following function is used to set the Debug Mask that controls*/
   /* which debug zone messages get displayed.  The function takes as   */
   /* its only parameter the Debug Mask value that is to be used.  Each */
   /* bit in the mask corresponds to a debug zone.  When a bit is set,  */
   /* the printing of that debug zone is enabled.                       */
void BTPSAPI BTPS_SetDebugMask(unsigned long DebugMask)
{
   DebugZoneMask = DebugMask;
}

   /* The following function is a utility function that can be used to  */
   /* determine if a specified Zone is currently enabled for debugging. */
int BTPSAPI BTPS_TestDebugZone(unsigned long Zone)
{
   return(DebugZoneMask & Zone);
}

   /* The following function is responsible for displaying binary debug */
   /* data.  The first parameter to this function is the length of data */
   /* pointed to by the next parameter.  The final parameter is a       */
   /* pointer to the binary data to be displayed.                       */
int BTPSAPI BTPS_DumpData(unsigned int DataLength, BTPSCONST unsigned char *DataPtr)
{
   int          ret_val;
   unsigned int Index;
   unsigned int Offset;

   /* Before proceeding any further, lets make sure that the parameters */
   /* passed to us appear semi-valid.                                   */
   if((DataLength > 0) && (DataPtr != NULL))
   {
      /* Print out the header.                                          */
      BTPS_OutputMessage("\r\n");
      BTPS_OutputMessage("  -OFFSET- | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
      BTPS_OutputMessage(" ----------+-------------------------------------------------\r\n");

      Offset = 0;

      /* Output the Debug Data, 16 bytes per line.                      */
      while(DataLength)
      {
         /* Print the line header.                                      */
         BTPS_OutputMessage("  %08X |", Offset);
         Offset += 16;

         /* Print the row of data.                                      */
         for(Index = 0; (Index < 16) && (DataLength); Index ++)
         {
            BTPS_OutputMessage(" %02X", *DataPtr);

            DataLength --;
            DataPtr    ++;
         }

         BTPS_OutputMessage("\r\n");
      }

      BTPS_OutputMessage("\r\n");

      /* Finally, set the return value to indicate success.             */
      ret_val = 0;
   }
   else
      ret_val = -1;

   return(ret_val);
}

