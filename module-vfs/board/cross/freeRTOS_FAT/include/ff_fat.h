/*
 * FreeRTOS+FAT Labs Build 160919 (C) 2016 Real Time Engineers ltd.
 * Authors include James Walmsley, Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   FREERTOS+FAT IS STILL IN THE LAB:                                     ***
 ***                                                                         ***
 ***   This product is functional and is already being used in commercial    ***
 ***   products.  Be aware however that we are still refining its design,    ***
 ***   the source code does not yet fully conform to the strict coding and   ***
 ***   style standards mandated by Real Time Engineers ltd., and the         ***
 ***   documentation and testing is not necessarily complete.                ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact  Active early adopters may, at   ***
 ***   the sole discretion of Real Time Engineers Ltd., be offered versions  ***
 ***   under a license other than that described below.                      ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
 *
 * FreeRTOS+FAT can be used under two different free open source licenses.  The
 * license that applies is dependent on the processor on which FreeRTOS+FAT is
 * executed, as follows:
 *
 * If FreeRTOS+FAT is executed on one of the processors listed under the Special
 * License Arrangements heading of the FreeRTOS+FAT license information web
 * page, then it can be used under the terms of the FreeRTOS Open Source
 * License.  If FreeRTOS+FAT is used on any other processor, then it can be used
 * under the terms of the GNU General Public License V2.  Links to the relevant
 * licenses follow:
 *
 * The FreeRTOS+FAT License Information Page: http://www.FreeRTOS.org/fat_license
 * The FreeRTOS Open Source License: http://www.FreeRTOS.org/license
 * The GNU General Public License Version 2: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * FreeRTOS+FAT is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+FAT unless you agree that you use the software 'as is'.
 * FreeRTOS+FAT is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/plus
 * http://www.FreeRTOS.org/labs
 *
 */

/**
 *	@file		ff_fat.h
 *	@ingroup	FAT
 **/

#ifndef _FF_FAT_H_
#define _FF_FAT_H_

#ifndef PLUS_FAT_H
	#error this header will be included from "plusfat.h"
#endif

/*---------- ERROR CODES */


/*---------- PROTOTYPES */

/* HT statistics Will be taken away after testing: */
#if( ffconfigFAT_USES_STAT != 0 )
struct SFatStat
{
	unsigned initCount;
	unsigned clearCount;
	unsigned getCount[2];	/* Index 0 for READ counts, index 1 for WRITE counts. */
	unsigned reuseCount[2];
	unsigned missCount[2];
};

extern struct SFatStat fatStat;
#endif

#if( ffconfigWRITE_BOTH_FATS != 0 )
	#define ffconfigBUF_STORE_COUNT 2
#else
	#define ffconfigBUF_STORE_COUNT 1
#endif

typedef struct _FatBuffers
{
	FF_Buffer_t *pxBuffers[ffconfigBUF_STORE_COUNT];
	uint8_t ucMode; /* FF_MODE_READ or WRITE. */
} FF_FATBuffers_t;

uint32_t FF_getClusterPosition( FF_IOManager_t *pxIOManager, uint32_t ulEntry, uint32_t ulEntrySize );
uint32_t FF_getClusterChainNumber( FF_IOManager_t *pxIOManager, uint32_t ulEntry, uint32_t ulEntrySize );
uint32_t FF_getMajorBlockNumber( FF_IOManager_t *pxIOManager, uint32_t ulEntry, uint32_t ulEntrySize );
uint32_t FF_getMinorBlockNumber( FF_IOManager_t *pxIOManager, uint32_t ulEntry, uint32_t ulEntrySize );
uint32_t FF_getMinorBlockEntry( FF_IOManager_t *pxIOManager, uint32_t ulEntry, uint32_t ulEntrySize );

/* A partition may define a block size larger than 512 bytes (at offset 0x0B of the PBR).
This function translates a block address to an address based on 'pxIOManager->usBlkSize',
which is usually 512 bytes.
*/
static portINLINE uint32_t FF_getRealLBA( FF_IOManager_t *pxIOManager, uint32_t LBA )
{
	return LBA * pxIOManager->xPartition.ucBlkFactor;
}

uint32_t FF_Cluster2LBA( FF_IOManager_t *pxIOManager, uint32_t ulCluster );
uint32_t FF_LBA2Cluster( FF_IOManager_t *pxIOManager, uint32_t ulAddress );
uint32_t FF_getFATEntry( FF_IOManager_t *pxIOManager, uint32_t ulCluster, FF_Error_t *pxError, FF_FATBuffers_t *pxFATBuffers );
FF_Error_t FF_putFATEntry( FF_IOManager_t *pxIOManager, uint32_t ulCluster, uint32_t ulValue, FF_FATBuffers_t *pxFATBuffers );
BaseType_t FF_isEndOfChain( FF_IOManager_t *pxIOManager, uint32_t ulFatEntry );
uint32_t FF_FindFreeCluster( FF_IOManager_t *pxIOManager, FF_Error_t *pxError, BaseType_t aDoClaim );
uint32_t FF_ExtendClusterChain( FF_IOManager_t *pxIOManager, uint32_t ulStartCluster, uint32_t ulCount );
FF_Error_t FF_UnlinkClusterChain( FF_IOManager_t *pxIOManager, uint32_t ulStartCluster, BaseType_t xDoTruncate );
uint32_t FF_TraverseFAT( FF_IOManager_t *pxIOManager, uint32_t ulStart, uint32_t ulCount, FF_Error_t *pxError );
uint32_t FF_CreateClusterChain( FF_IOManager_t *pxIOManager, FF_Error_t *pxError );
uint32_t FF_GetChainLength( FF_IOManager_t *pxIOManager, uint32_t pa_nStartCluster, uint32_t *piEndOfChain, FF_Error_t *pxError );
uint32_t FF_FindEndOfChain( FF_IOManager_t *pxIOManager, uint32_t Start, FF_Error_t *pxError );
FF_Error_t FF_ClearCluster( FF_IOManager_t *pxIOManager, uint32_t ulCluster  );

#if( ffconfig64_NUM_SUPPORT != 0 )
	uint64_t FF_GetFreeSize( FF_IOManager_t *pxIOManager, FF_Error_t *pxError );
#else
	uint32_t FF_GetFreeSize( FF_IOManager_t *pxIOManager, FF_Error_t *pxError );
#endif

/* WARNING: If this prototype changes, it must be updated in ff_ioman.c also! */
uint32_t FF_CountFreeClusters( FF_IOManager_t *pxIOManager, FF_Error_t *pxError );

FF_Error_t FF_ReleaseFATBuffers( FF_IOManager_t *pxIOManager, FF_FATBuffers_t *pxFATBuffers );

static portINLINE void FF_InitFATBuffers( FF_FATBuffers_t *pxFATBuffers, uint8_t ucMode )
{
	pxFATBuffers->pxBuffers[ 0 ] = NULL;
#if ffconfigBUF_STORE_COUNT > 1
	pxFATBuffers->pxBuffers[ 1 ] = NULL;
#endif
#if ffconfigBUF_STORE_COUNT > 2
	#error Please check this code, maybe it is time to use memset
#endif
	pxFATBuffers->ucMode = ucMode; /* FF_MODE_READ/WRITE */
	#if ffconfigFAT_USES_STAT
	{
		fatStat.initCount++;
	}
	#endif
}

#endif

