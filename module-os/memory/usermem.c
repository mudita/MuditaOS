/*
 *  @file usermem.c
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 30 lip 2018
 *  @brief DYnamic memory allocator for user space
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "macros.h"

/**
 * Sets heap size in SDRAM
 */
#ifndef PROJECT_CONFIG_USER_DYNMEM_SIZE
#error "Define user heap size!"
#else
#define USERMEM_TOTAL_HEAP_SIZE	PROJECT_CONFIG_USER_DYNMEM_SIZE
#endif

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
	#error This file must not be used if configSUPPORT_DYNAMIC_ALLOCATION is 0
#endif

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE		( ( size_t ) 8 )
/**
 * Document specified below is must to read. It explains cached memory regions alignment requirements
 * and so on.
 * https://www.avrfreaks.net/sites/default/files/forum_attachments/AN-15679_SAMS70_E70_Cache_Coherence.pdf
 */

#define usermemBYTE_ALIGNMENT		32
#define usermemBYTE_ALIGNMENT_MASK ( 0x001f )


#if( PROJECT_CONFIG_HEAP_INTEGRITY_CHECKS != 0 )
    #define INTEGRITY_STAMP_TAKEN   33333
    #define INTEGRITY_STAMP_FREE    11111
#endif


/* Allocate the memory for the heap. */
CACHEABLE_SECTION_SDRAM_ALIGN(static uint8_t userUcHeap[ USERMEM_TOTAL_HEAP_SIZE ],usermemBYTE_ALIGNMENT) ;


/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK
{
#if( PROJECT_CONFIG_HEAP_INTEGRITY_CHECKS != 0 )
    uint32_t ulStamp;
#endif
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
#if (configUSER_HEAP_STATS == 1)
    TaskHandle_t xAllocatingTask; /*<< The task allocating the memory block. */
    TickType_t xTimeAllocated;    /*<< The timestamp of memory block allocation. */
#endif
} BlockLink_t;

/*-----------------------------------------------------------*/

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert );

/*
 * Called automatically to setup the required heap structures the first time
 * pvPortMalloc() is called.
 */
static void prvHeapInit( void );

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize = (sizeof(BlockLink_t)
        + ((size_t) ( usermemBYTE_ALIGNMENT - 1)))
        & ~((size_t) usermemBYTE_ALIGNMENT_MASK);

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t userxStart, *userpxEnd = NULL;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t userxFreeBytesRemaining = 0U;
static size_t userxMinimumEverFreeBytesRemaining = 0U;


/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;

void *usermalloc(size_t xWantedSize)
{
	BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
	void *pvReturn = NULL;

    // Preventing use of an allocator in an interrupt
    if (isIRQ()) {
        abort();
    }

		vTaskSuspendAll();
		{
			/* If this is the first call to malloc then the heap will require
			initialisation to setup the list of free blocks. */
			if( userpxEnd == NULL )
			{
				prvHeapInit();
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			/* Check the requested block size is not so large that the top bit is
			set.  The top bit of the block size member of the BlockLink_t structure
			is used to determine who owns the block - the application or the
			kernel, so it must be free. */
			if( ( xWantedSize & xBlockAllocatedBit ) == 0 )
			{
				/* The wanted size is increased so it can contain a BlockLink_t
				structure in addition to the requested amount of bytes. */
				if( xWantedSize > 0 )
				{
					xWantedSize += xHeapStructSize;

					/* Ensure that blocks are always aligned to the required number
					of bytes. */
					if( ( xWantedSize & usermemBYTE_ALIGNMENT_MASK ) != 0x00 )
					{
						/* Byte alignment required. */
						xWantedSize += ( usermemBYTE_ALIGNMENT - ( xWantedSize & usermemBYTE_ALIGNMENT_MASK ) );
						configASSERT( ( xWantedSize & usermemBYTE_ALIGNMENT_MASK ) == 0 );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}

				if( ( xWantedSize > 0 ) && ( xWantedSize <= userxFreeBytesRemaining ) )
				{
					/* Traverse the list from the start	(lowest address) block until
					one	of adequate size is found. */
					pxPreviousBlock = &userxStart;
					pxBlock = userxStart.pxNextFreeBlock;
					while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) )
					{
						pxPreviousBlock = pxBlock;
						pxBlock = pxBlock->pxNextFreeBlock;
					}

					/* If the end marker was reached then a block of adequate size
					was	not found. */
					if( pxBlock != userpxEnd )
					{
						/* Return the memory space pointed to - jumping over the
						BlockLink_t structure at its start. */
						pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );

						/* This block is being returned for use so must be taken out
						of the list of free blocks. */
						pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

						/* If the block is larger than required it can be split into
						two. */
						if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
						{
							/* This block is to be split into two.  Create a new
							block following the number of bytes requested. The void
							cast is used to prevent byte alignment warnings from the
							compiler. */
							pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );
							configASSERT( ( ( ( size_t ) pxNewBlockLink ) & usermemBYTE_ALIGNMENT_MASK ) == 0 );

							/* Calculate the sizes of two blocks split from the
							single block. */
							pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;

							#if( PROJECT_CONFIG_HEAP_INTEGRITY_CHECKS != 0 )
							{
                            	pxNewBlockLink->ulStamp = INTEGRITY_STAMP_FREE;
							}
							#endif

							pxBlock->xBlockSize = xWantedSize;

							/* Insert the new block into the list of free blocks. */
							prvInsertBlockIntoFreeList( pxNewBlockLink );
						}
						else
						{
							mtCOVERAGE_TEST_MARKER();
						}

						userxFreeBytesRemaining -= pxBlock->xBlockSize;

						if( userxFreeBytesRemaining < userxMinimumEverFreeBytesRemaining )
						{
							userxMinimumEverFreeBytesRemaining = userxFreeBytesRemaining;
						}
						else
						{
							mtCOVERAGE_TEST_MARKER();
						}

						/* The block is being returned - it is allocated and owned
						by the application and has no "next" block. */
						pxBlock->xBlockSize |= xBlockAllocatedBit;
#if (configUSER_HEAP_STATS == 1)
                        pxBlock->xAllocatingTask = xTaskGetCurrentTaskHandle();
                        pxBlock->xTimeAllocated  = xTaskGetTickCount();
#endif

#if (PROJECT_CONFIG_HEAP_INTEGRITY_CHECKS != 0)
                    	{
							/* First check if the block was still marked as 'free' */
							configASSERT( pxBlock->ulStamp == INTEGRITY_STAMP_FREE );
							/* And now mark it as 'taken' */
							pxBlock->ulStamp = INTEGRITY_STAMP_TAKEN;
                    	}
                    	#endif

						pxBlock->pxNextFreeBlock = NULL;
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			traceMALLOC( pvReturn, xWantedSize );
		}
		( void ) xTaskResumeAll();

		#if( configUSE_MALLOC_FAILED_HOOK == 1 )
		{
			if( pvReturn == NULL )
			{
				extern void vApplicationMallocFailedHook( void );
				vApplicationMallocFailedHook();
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		#endif

		configASSERT( ( ( ( size_t ) pvReturn ) & ( size_t ) usermemBYTE_ALIGNMENT_MASK ) == 0 );
		return pvReturn;
}

void userfree(void *pv)
{
	uint8_t *puc = ( uint8_t * ) pv;
	BlockLink_t *pxLink;

    // Preventing use of an allocator in an interrupt
    if (isIRQ()) {
        abort();
    }

		if( pv != NULL )
		{
			/* The memory being freed will have an BlockLink_t structure immediately
			before it. */
			puc -= xHeapStructSize;

			/* This casting is to keep the compiler from issuing warnings. */
			pxLink = ( void * ) puc;

			/* Check the block is actually allocated. */
			configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
			configASSERT( pxLink->pxNextFreeBlock == NULL );

			#if( PROJECT_CONFIG_HEAP_INTEGRITY_CHECKS != 0 )
        	{
            	configASSERT( pxLink->ulStamp == INTEGRITY_STAMP_TAKEN );
            	pxLink->ulStamp = INTEGRITY_STAMP_FREE;
        	}
        	#endif

			if( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 )
			{
				if( pxLink->pxNextFreeBlock == NULL )
				{
					/* The block is being returned to the heap - it is no longer
					allocated. */
					pxLink->xBlockSize &= ~xBlockAllocatedBit;

					vTaskSuspendAll();
					{
						/* Add this block to the list of free blocks. */
						userxFreeBytesRemaining += pxLink->xBlockSize;
						traceFREE( pv, pxLink->xBlockSize );
						prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
					}
					( void ) xTaskResumeAll();
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
}

void *userrealloc(void *pv, size_t xWantedSize) {
    uint8_t *puc = ( uint8_t * ) pv;
    BlockLink_t *pxLink;
    size_t curSize = 0;
    void *new = NULL;

    if( pv != NULL ) {
        /* The memory being freed will have an BlockLink_t structure immediately
                    before it. */
        puc -= xHeapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        pxLink = ( void * ) puc;

        /* Check the block is actually allocated. */
        configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
        configASSERT( pxLink->pxNextFreeBlock == NULL );

        if( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 ) {
            curSize = pxLink->xBlockSize & ~xBlockAllocatedBit;

            if (curSize >= xWantedSize)
                return pv;

            new = usermalloc(xWantedSize);
            if (!new) return NULL;
            vTaskSuspendAll();
            memcpy(new, pv, curSize);
            ( void ) xTaskResumeAll();
            userfree(pv);

            return new;
        }
        else
        {
        	mtCOVERAGE_TEST_MARKER();
        }
        return NULL;
    }
    else {
        return usermalloc(xWantedSize);
    }
    return NULL;
}

/*-----------------------------------------------------------*/

size_t usermemGetFreeHeapSize( void )
{
    return userxFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

size_t usermemGetMinimumEverFreeHeapSize( void )
{
    return userxMinimumEverFreeBytesRemaining;
}
/*-----------------------------------------------------------*/


static void prvHeapInit( void )
{
BlockLink_t *pxFirstFreeBlock;
uint8_t *pucAlignedHeap;
size_t uxAddress;

size_t xTotalHeapSize = USERMEM_TOTAL_HEAP_SIZE;

	/* Ensure the heap starts on a correctly aligned boundary. */
	uxAddress = ( size_t ) userUcHeap;

	if( ( uxAddress & usermemBYTE_ALIGNMENT_MASK ) != 0 )
	{
		uxAddress += ( usermemBYTE_ALIGNMENT - 1 );
		uxAddress &= ~( ( size_t ) usermemBYTE_ALIGNMENT_MASK );
		xTotalHeapSize -= uxAddress - ( size_t ) userUcHeap;
	}

	pucAlignedHeap = ( uint8_t * ) uxAddress;

	/* userxStart is used to hold a pointer to the first item in the list of free
	blocks.  The void cast is used to prevent compiler warnings. */
	userxStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;
	userxStart.xBlockSize = ( size_t ) 0;
#if (configUSER_HEAP_STATS == 1)
    userxStart.xTimeAllocated  = 0;
    userxStart.xAllocatingTask = 0;
#endif

    /* userpxEnd is used to mark the end of the list of free blocks and is inserted
	at the end of the heap space. */
	uxAddress = ( ( size_t ) pucAlignedHeap ) + xTotalHeapSize;
	uxAddress -= xHeapStructSize;
	uxAddress &= ~( ( size_t ) usermemBYTE_ALIGNMENT_MASK );
	userpxEnd = ( void * ) uxAddress;
	userpxEnd->xBlockSize = 0;
	userpxEnd->pxNextFreeBlock = NULL;
#if (configUSER_HEAP_STATS == 1)
    userpxEnd->xTimeAllocated  = 0;
    userpxEnd->xAllocatingTask = 0;
#endif

    /* To start with there is a single free block that is sized to take up the
	entire heap space, minus the space taken by userpxEnd. */
	pxFirstFreeBlock = ( void * ) pucAlignedHeap;
	pxFirstFreeBlock->xBlockSize = uxAddress - ( size_t ) pxFirstFreeBlock;
	pxFirstFreeBlock->pxNextFreeBlock = userpxEnd;
	#if( PROJECT_CONFIG_HEAP_INTEGRITY_CHECKS != 0 )
    {
        pxFirstFreeBlock->ulStamp = INTEGRITY_STAMP_FREE;
    }
    #endif

	/* Only one block exists - and it covers the entire usable heap space. */
	userxMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
	userxFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;

	/* Work out the position of the top bit in a size_t variable. */
	xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );
}
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert )
{
BlockLink_t *pxIterator;
uint8_t *puc;

	/* Iterate through the list until a block is found that has a higher address
	than the block being inserted. */
	for( pxIterator = &userxStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock )
	{
		/* Nothing to do here, just iterate to the right position. */
	}

	/* Do the block being inserted, and the block it is being inserted after
	make a contiguous block of memory? */
	puc = ( uint8_t * ) pxIterator;
	if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
	{
		pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
		pxBlockToInsert = pxIterator;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	/* Do the block being inserted, and the block it is being inserted before
	make a contiguous block of memory? */
	puc = ( uint8_t * ) pxBlockToInsert;
	if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock )
	{
		if( pxIterator->pxNextFreeBlock != userpxEnd )
		{
			/* Form one big block from the two blocks. */
			pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
			pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
		}
		else
		{
			pxBlockToInsert->pxNextFreeBlock = userpxEnd;
		}
	}
	else
	{
		pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
	}

	/* If the block being inserted plugged a gab, so was merged with the block
	before and the block after, then it's pxNextFreeBlock pointer will have
	already been set, and should not be set here as that would make it point
	to itself. */
	if( pxIterator != pxBlockToInsert )
	{
		pxIterator->pxNextFreeBlock = pxBlockToInsert;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}
}


