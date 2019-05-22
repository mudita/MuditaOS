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

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"

/* FreeRTOS+FAT includes. */
#include "ff_headers.h"
#include "ff_ramdisk.h"
#include "ff_sys.h"

#define ramHIDDEN_SECTOR_COUNT		8
#define ramPRIMARY_PARTITIONS		1
#define ramHUNDRED_64_BIT			100ULL
#define ramSECTOR_SIZE				512UL
#define ramPARTITION_NUMBER			0 /* Only a single partition is used. */
#define ramBYTES_PER_MB				( 1024ull * 1024ull )
#define ramSECTORS_PER_MB			( ramBYTES_PER_MB / 512ull )

/* Used as a magic number to indicate that an FF_Disk_t structure is a RAM
disk. */
#define ramSIGNATURE				0x41404342

/*-----------------------------------------------------------*/

/*
 * The function that writes to the media - as this is implementing a RAM disk
 * the media is just a RAM buffer.
 */
static int32_t prvWriteRAM( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk );

/*
 * The function that reads from the media - as this is implementing a RAM disk
 * the media is just a RAM buffer.
 */
static int32_t prvReadRAM( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk );

/*
 * This is the driver for a RAM disk.  Unlike most media types, RAM disks are
 * volatile so are created anew each time the system is booted.  As the disk is
 * new and just created, it must also be partitioned and formatted.
 */
static FF_Error_t prvPartitionAndFormatDisk( FF_Disk_t *pxDisk );

/*-----------------------------------------------------------*/

/* This is the prototype of the function used to initialise the RAM disk driver.
Other media drivers do not have to have the same prototype.

In this example:
 + pcName is the name to give the disk within FreeRTOS+FAT's virtual file system.
 + pucDataBuffer is the start of the RAM to use as the disk.
 + ulSectorCount is effectively the size of the disk, each sector is 512 bytes.
 + xIOManagerCacheSize is the size of the IO manager's cache, which must be a
   multiple of the sector size, and at least twice as big as the sector size.
*/
FF_Disk_t *FF_RAMDiskInit( char *pcName, uint8_t *pucDataBuffer, uint32_t ulSectorCount, size_t xIOManagerCacheSize )
{
FF_Error_t xError;
FF_Disk_t *pxDisk = NULL;
FF_CreationParameters_t xParameters;

	/* Check the validity of the xIOManagerCacheSize parameter. */
	configASSERT( ( xIOManagerCacheSize % ramSECTOR_SIZE ) == 0 );
	configASSERT( ( xIOManagerCacheSize >= ( 2 * ramSECTOR_SIZE ) ) );

	/* Attempt to allocated the FF_Disk_t structure. */
	pxDisk = ( FF_Disk_t * ) pvPortMalloc( sizeof( FF_Disk_t ) );

	if( pxDisk != NULL )
	{
		/* Start with every member of the structure set to zero. */
		memset( pxDisk, '\0', sizeof( FF_Disk_t ) );

		/* Clear the entire space. */
		memset( pucDataBuffer, '\0', ulSectorCount * ramSECTOR_SIZE );

		/* The pvTag member of the FF_Disk_t structure allows the structure to be
		extended to also include media specific parameters.  The only media
		specific data that needs to be stored in the FF_Disk_t structure for a
		RAM disk is the location of the RAM buffer itself - so this is stored
		directly in the FF_Disk_t's pvTag member. */
		pxDisk->pvTag = ( void * ) pucDataBuffer;

		/* The signature is used by the disk read and disk write functions to
		ensure the disk being accessed is a RAM disk. */
		pxDisk->ulSignature = ramSIGNATURE;

		/* The number of sectors is recorded for bounds checking in the read and
		write functions. */
		pxDisk->ulNumberOfSectors = ulSectorCount;

		/* Create the IO manager that will be used to control the RAM disk. */
		memset( &xParameters, '\0', sizeof( xParameters ) );
		xParameters.pucCacheMemory = NULL;
		xParameters.ulMemorySize = xIOManagerCacheSize;
		xParameters.ulSectorSize = ramSECTOR_SIZE;
		xParameters.fnWriteBlocks = prvWriteRAM;
		xParameters.fnReadBlocks = prvReadRAM;
		xParameters.pxDisk = pxDisk;

		/* Driver is reentrant so xBlockDeviceIsReentrant can be set to pdTRUE.
		In this case the semaphore is only used to protect FAT data
		structures. */
		xParameters.pvSemaphore = ( void * ) xSemaphoreCreateRecursiveMutex();
		xParameters.xBlockDeviceIsReentrant = pdFALSE;

		pxDisk->pxIOManager = FF_CreateIOManger( &xParameters, &xError );

		if( ( pxDisk->pxIOManager != NULL ) && ( FF_isERR( xError ) == pdFALSE ) )
		{
			/* Record that the RAM disk has been initialised. */
			pxDisk->xStatus.bIsInitialised = pdTRUE;

			/* Create a partition on the RAM disk.  NOTE!  The disk is only
			being partitioned here because it is a new RAM disk.  It is
			known that the disk has not been used before, and cannot already
			contain any partitions.  Most media drivers will not perform
			this step because the media will have already been partitioned. */
			xError = prvPartitionAndFormatDisk( pxDisk );

			if( FF_isERR( xError ) == pdFALSE )
			{
				/* Record the partition number the FF_Disk_t structure is, then
				mount the partition. */
				pxDisk->xStatus.bPartitionNumber = ramPARTITION_NUMBER;

				/* Mount the partition. */
				xError = FF_Mount( pxDisk, ramPARTITION_NUMBER );
				FF_PRINTF( "FF_RAMDiskInit: FF_Mount: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );
			}

			if( FF_isERR( xError ) == pdFALSE )
			{
				/* The partition mounted successfully, add it to the virtual
				file system - where it will appear as a directory off the file
				system's root directory. */
				FF_FS_Add( pcName, pxDisk );
			}
		}
		else
		{
			FF_PRINTF( "FF_RAMDiskInit: FF_CreateIOManger: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );

			/* The disk structure was allocated, but the disk's IO manager could
			not be allocated, so free the disk again. */
			FF_RAMDiskDelete( pxDisk );
			pxDisk = NULL;
		}
	}
	else
	{
		FF_PRINTF( "FF_RAMDiskInit: Malloc failed\r\n" );
	}

	return pxDisk;
}
/*-----------------------------------------------------------*/

BaseType_t FF_RAMDiskDelete( FF_Disk_t *pxDisk )
{
	if( pxDisk != NULL )
	{
		pxDisk->ulSignature = 0;
		pxDisk->xStatus.bIsInitialised = 0;
		if( pxDisk->pxIOManager != NULL )
		{
			FF_DeleteIOManager( pxDisk->pxIOManager );
		}

		vPortFree( pxDisk );
	}

	return pdPASS;
}
/*-----------------------------------------------------------*/

static int32_t prvReadRAM( uint8_t *pucDestination, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturn;
uint8_t *pucSource;

	if( pxDisk != NULL )
	{
		if( pxDisk->ulSignature != ramSIGNATURE )
		{
			/* The disk structure is not valid because it doesn't contain a
			magic number written to the disk when it was created. */
			lReturn = FF_ERR_IOMAN_DRIVER_FATAL_ERROR | FF_ERRFLAG;
		}
		else if( pxDisk->xStatus.bIsInitialised == pdFALSE )
		{
			/* The disk has not been initialised. */
			lReturn = FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG;
		}
		else if( ulSectorNumber >= pxDisk->ulNumberOfSectors )
		{
			/* The start sector is not within the bounds of the disk. */
			lReturn = ( FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG );
		}
		else if( ( pxDisk->ulNumberOfSectors - ulSectorNumber ) < ulSectorCount )
		{
			/* The end sector is not within the bounds of the disk. */
			lReturn = ( FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG );
		}
		else
		{
			/* Obtain the pointer to the RAM buffer being used as the disk. */
			pucSource = ( uint8_t * ) pxDisk->pvTag;

			/* Move to the start of the sector being read. */
			pucSource += ( ramSECTOR_SIZE * ulSectorNumber );

			/* Copy the data from the disk.  As this is a RAM disk this can be
			done using memcpy(). */
			memcpy( ( void * ) pucDestination,
					( void * ) pucSource,
					( size_t ) ( ulSectorCount * ramSECTOR_SIZE ) );

			lReturn = FF_ERR_NONE;
		}
	}
	else
	{
		lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

static int32_t prvWriteRAM( uint8_t *pucSource, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturn = FF_ERR_NONE;
uint8_t *pucDestination;

	if( pxDisk != NULL )
	{
		if( pxDisk->ulSignature != ramSIGNATURE )
		{
			/* The disk structure is not valid because it doesn't contain a
			magic number written to the disk when it was created. */
			lReturn = FF_ERR_IOMAN_DRIVER_FATAL_ERROR | FF_ERRFLAG;
		}
		else if( pxDisk->xStatus.bIsInitialised == pdFALSE )
		{
			/* The disk has not been initialised. */
			lReturn = FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG;
		}
		else if( ulSectorNumber >= pxDisk->ulNumberOfSectors )
		{
			/* The start sector is not within the bounds of the disk. */
			lReturn = ( FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG );
		}
		else if( ( pxDisk->ulNumberOfSectors - ulSectorNumber ) < ulSectorCount )
		{
			/* The end sector is not within the bounds of the disk. */
			lReturn = ( FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG );
		}
		else
		{
			/* Obtain the location of the RAM being used as the disk. */
			pucDestination = ( uint8_t * ) pxDisk->pvTag;

			/* Move to the sector being written to. */
			pucDestination += ( ramSECTOR_SIZE * ulSectorNumber );

			/* Write to the disk.  As this is a RAM disk the write can use a
			memcpy(). */
			memcpy( ( void * ) pucDestination,
					( void * ) pucSource,
					( size_t ) ulSectorCount * ( size_t ) ramSECTOR_SIZE );

			lReturn = FF_ERR_NONE;
		}
	}
	else
	{
		lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

static FF_Error_t prvPartitionAndFormatDisk( FF_Disk_t *pxDisk )
{
FF_PartitionParameters_t xPartition;
FF_Error_t xError;

	/* Create a single partition that fills all available space on the disk. */
	memset( &xPartition, '\0', sizeof( xPartition ) );
	xPartition.ulSectorCount = pxDisk->ulNumberOfSectors;
	xPartition.ulHiddenSectors = ramHIDDEN_SECTOR_COUNT;
	xPartition.xPrimaryCount = ramPRIMARY_PARTITIONS;
	xPartition.eSizeType = eSizeIsQuota;

	/* Partition the disk */
	xError = FF_Partition( pxDisk, &xPartition );
	FF_PRINTF( "FF_Partition: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );

	if( FF_isERR( xError ) == pdFALSE )
	{
		/* Format the partition. */
		xError = FF_Format( pxDisk, ramPARTITION_NUMBER, pdTRUE, pdTRUE );
		FF_PRINTF( "FF_RAMDiskInit: FF_Format: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );
	}

	return xError;
}
/*-----------------------------------------------------------*/

BaseType_t FF_RAMDiskShowPartition( FF_Disk_t *pxDisk )
{
FF_Error_t xError;
uint64_t ullFreeSectors;
uint32_t ulTotalSizeMB, ulFreeSizeMB;
int iPercentageFree;
FF_IOManager_t *pxIOManager;
const char *pcTypeName = "unknown type";
BaseType_t xReturn = pdPASS;

	if( pxDisk == NULL )
	{
		xReturn = pdFAIL;
	}
	else
	{
		pxIOManager = pxDisk->pxIOManager;

		FF_PRINTF( "Reading FAT and calculating Free Space\r\n" );

		switch( pxIOManager->xPartition.ucType )
		{
			case FF_T_FAT12:
				pcTypeName = "FAT12";
				break;

			case FF_T_FAT16:
				pcTypeName = "FAT16";
				break;

			case FF_T_FAT32:
				pcTypeName = "FAT32";
				break;

			default:
				pcTypeName = "UNKOWN";
				break;
		}

		FF_GetFreeSize( pxIOManager, &xError );

		ullFreeSectors = pxIOManager->xPartition.ulFreeClusterCount * pxIOManager->xPartition.ulSectorsPerCluster;
		iPercentageFree = ( int ) ( ( ramHUNDRED_64_BIT * ullFreeSectors + pxIOManager->xPartition.ulDataSectors / 2 ) /
			( ( uint64_t )pxIOManager->xPartition.ulDataSectors ) );

		ulTotalSizeMB = pxIOManager->xPartition.ulDataSectors / ramSECTORS_PER_MB;
		ulFreeSizeMB = ( uint32_t ) ( ullFreeSectors / ramSECTORS_PER_MB );

		/* It is better not to use the 64-bit format such as %Lu because it
		might not be implemented. */
		FF_PRINTF( "Partition Nr   %8u\r\n", pxDisk->xStatus.bPartitionNumber );
		FF_PRINTF( "Type           %8u (%s)\r\n", pxIOManager->xPartition.ucType, pcTypeName );
		FF_PRINTF( "VolLabel       '%8s' \r\n", pxIOManager->xPartition.pcVolumeLabel );
		FF_PRINTF( "TotalSectors   %8lu\r\n", pxIOManager->xPartition.ulTotalSectors );
		FF_PRINTF( "SecsPerCluster %8lu\r\n", pxIOManager->xPartition.ulSectorsPerCluster );
		FF_PRINTF( "Size           %8lu MB\r\n", ulTotalSizeMB );
		FF_PRINTF( "FreeSize       %8lu MB ( %d perc free )\r\n", ulFreeSizeMB, iPercentageFree );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void FF_RAMDiskFlush( FF_Disk_t *pxDisk )
{
	if( ( pxDisk != NULL ) && ( pxDisk->xStatus.bIsInitialised != 0 ) && ( pxDisk->pxIOManager != NULL ) )
	{
		FF_FlushCache( pxDisk->pxIOManager );
		if( FF_InvalidateCache( pxDisk->pxIOManager ) != 0 )
		{
			/* Not all buffers could be invalidated. */
		}
	}
}
/*-----------------------------------------------------------*/

