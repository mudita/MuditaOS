/**
 * @file ff_sddisk.c
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date Feb 8, 2018
 * @brief 
 * @copyright Copyright (C) 2018 mudita.com.
 * @details
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
#include "ff_sddisk.h"
#include "ff_sys.h"


#define sdHUNDRED_64_BIT			100ULL
#define sdPARTITION_NUMBER			0 /* Only a single partition is used. */
#define sdBYTES_PER_MB				( 1024ull * 1024ull )
#define sdSECTORS_PER_MB			( sdBYTES_PER_MB / 512ull )
#define sdPresenceOK				1
#define sdPresenceFAIL				0

/* Used as a magic number to indicate that an FF_Disk_t structure is a RAM
disk. */
#define sdSIGNATURE				0x51505352

static uint8_t is_card_present = sdPresenceFAIL;

/*
 * The function that writes to the media.
 */
static int32_t prvWriteSD( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk );

/*
 * The function that reads from the media - as this is implementing a RAM disk
 * the media is just a RAM buffer.
 */
static int32_t prvReadSD( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk );


FF_Disk_t *FF_SDDiskInit( char *pcName,size_t xIOManagerCacheSize )
{
FF_Error_t xError;
FF_Disk_t *pxDisk = NULL;
FF_CreationParameters_t xParameters;

	/* Init SD card lower layers */
	if(FF_SDDisk_LL_Init() != FF_ERR_NONE){
		FF_PRINTF( "FF_SDDiskInit: Low level init failed.\r\n" );
		return pxDisk;
	}

	/* Check if card is in slot. */
	if(FF_SDDisk_LL_IsCardPresent())
	{
		is_card_present = sdPresenceOK;
	}
	else
	{
		FF_PRINTF( "FF_SDDiskInit: No SD Card in slot.\r\n" );
		return pxDisk;
	}

	/* Attempt to allocated the FF_Disk_t structure. */
	pxDisk = ( FF_Disk_t * ) malloc( sizeof( FF_Disk_t ) );

	if( pxDisk != NULL )
	{

		FF_SDDisk_CardInfo_t info;
	    FF_SDDisk_LL_GetCardInfo(&info);

		/* Start with every member of the structure set to zero. */
		memset( pxDisk, '\0', sizeof( FF_Disk_t ) );

		/* pvTag is generic pointer to user structure. In this case we're using it
		 * for storing information about card presence in slot. */
		pxDisk->pvTag = &is_card_present;

		/* The signature is used by the disk read and disk write functions to
		ensure the disk being accessed is a SD disk. */
		pxDisk->ulSignature = sdSIGNATURE;

		/* The number of sectors is recorded for bounds checking in the read and
		write functions. */
		pxDisk->ulNumberOfSectors = info.LogBlockNbr;

		/* Create the IO manager that will be used to control the SD disk. */
		memset( &xParameters, '\0', sizeof( xParameters ) );
		xParameters.pucCacheMemory = NULL;
		xParameters.ulMemorySize = xIOManagerCacheSize;
		xParameters.ulSectorSize = info.LogBlockSize;
		xParameters.fnWriteBlocks = prvWriteSD;
		xParameters.fnReadBlocks = prvReadSD;
		xParameters.pxDisk = pxDisk;

		/* Driver is reentrant so xBlockDeviceIsReentrant can be set to pdTRUE.
		In this case the semaphore is only used to protect FAT data
		structures. */
		xParameters.pvSemaphore = ( void * ) xSemaphoreCreateRecursiveMutex();
		xParameters.xBlockDeviceIsReentrant = pdTRUE;


		/* Check the validity of the xIOManagerCacheSize parameter. */
		configASSERT( ( xIOManagerCacheSize % xParameters.ulSectorSize ) == 0 );
		configASSERT( ( xIOManagerCacheSize >= (size_t)( 2 * xParameters.ulSectorSize ) ) );

		pxDisk->pxIOManager = FF_CreateIOManger( &xParameters, &xError );

		if( ( pxDisk->pxIOManager != NULL ) && ( FF_isERR( xError ) == pdFALSE ) )
		{
			/* Record that the SD disk has been initialised. */
			pxDisk->xStatus.bIsInitialised = pdTRUE;


			/* Record the partition number the FF_Disk_t structure is, then
			mount the partition. */
			pxDisk->xStatus.bPartitionNumber = 0;

			/* Mount the partition. */
			xError = FF_Mount( pxDisk, pxDisk->xStatus.bPartitionNumber);
			FF_PRINTF( "FF_SDDiskInit: FF_Mount: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );

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
			FF_PRINTF( "FF_SDDiskInit: FF_CreateIOManger: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );

			/* The disk structure was allocated, but the disk's IO manager could
			not be allocated, so free the disk again. */
			FF_SDDiskDelete( pxDisk );
			pxDisk = NULL;
		}
	}
	else
	{
		FF_PRINTF( "FF_SDDiskInit: Malloc failed\r\n" );
	}

	return pxDisk;
}

BaseType_t FF_SDDiskDelete( FF_Disk_t *pxDisk )
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

BaseType_t FF_SDDiskShowPartition( FF_Disk_t *pxDisk )
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
		iPercentageFree = ( int ) ( ( sdHUNDRED_64_BIT * ullFreeSectors + pxIOManager->xPartition.ulDataSectors / 2 ) /
			( ( uint64_t )pxIOManager->xPartition.ulDataSectors ) );

		ulTotalSizeMB = pxIOManager->xPartition.ulDataSectors / sdSECTORS_PER_MB;
		ulFreeSizeMB = ( uint32_t ) ( ullFreeSectors / sdSECTORS_PER_MB );

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

void FF_SDDiskFlush( FF_Disk_t *pxDisk )
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

uint8_t FF_SDDiskIsPresent(void)
{
	return FF_SDDisk_LL_IsCardPresent();
}

/* Need to be invoked upon detecting removal of sd card.*/
void FF_SDDiskCardRemovedCallback(void)
{
	is_card_present = sdPresenceFAIL;
}

static int32_t prvReadSD( uint8_t *pucDestination, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturn;

	if( pxDisk != NULL )
	{
		if(*(uint8_t*)pxDisk->pvTag != sdPresenceOK)
		{
			/* SD card is not instert into slot*/
			lReturn = FF_ERR_IOMAN_DRIVER_NOMEDIUM | FF_ERRFLAG;
		}
		else if( pxDisk->ulSignature != sdSIGNATURE )
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
			lReturn = FF_SDDisk_LL_ReadBlocks((uint32_t*)pucDestination,ulSectorNumber,ulSectorCount);
		}
	}
	else
	{
		lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

static int32_t prvWriteSD( uint8_t *pucSource, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturn = FF_ERR_NONE;

	if( pxDisk != NULL )
	{

		if(*(uint8_t*)pxDisk->pvTag != sdPresenceOK)
		{
			/* SD card is not instert into slot*/
			lReturn = FF_ERR_IOMAN_DRIVER_NOMEDIUM | FF_ERRFLAG;
		}
		else if( pxDisk->ulSignature != sdSIGNATURE )
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
			lReturn = FF_SDDisk_LL_WriteBlocks((uint32_t*)pucSource,ulSectorNumber,ulSectorCount);
		}
	}
	else
	{
		lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
	}

	return lReturn;
}






