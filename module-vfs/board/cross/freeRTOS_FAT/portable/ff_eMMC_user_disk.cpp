/*
 *  @file ff_eMMC_user_disk.c
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 01.03.2018
 *  @brief
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */



/* Standard includes. */
#include "ff_eMMC_user_disk.hpp"
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
#include "ff_sys.h"

#include "fsl_cache.h"
#include "macros.h"

#include "log/log.hpp"

#define eMMCHIDDEN_SECTOR_COUNT		8
#define eMMCPRIMARY_PARTITIONS		1
#define eMMCHUNDRED_64_BIT			100ULL
#define eMMCPARTITION_NUMBER			0 /* Only a single partition is used. */
#define eMMCBYTES_PER_MB				( 1024ull * 1024ull )
#define eMMCSECTORS_PER_MB			( eMMCBYTES_PER_MB / 512ull )

/* Used as a magic number to indicate that an FF_Disk_t structure is a RAM
disk. */
#define eMMCSIGNATURE				0x61606362
#define mainIO_MANAGER_CACHE_SIZE	( 15UL * FSL_SDMMC_DEFAULT_BLOCK_SIZE )


static ALIGN_(32) uint8_t emmc_user_CacheBuffer[mainIO_MANAGER_CACHE_SIZE];

/*
 * The function that writes to the media.
 */
static int32_t prvWriteeMMC( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk );

/*
 * The function that reads from the media - as this is implementing a RAM disk
 * the media is just a RAM buffer.
 */
static int32_t prvReadeMMC( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk );

static FF_Error_t prvPartitionAndFormatDisk( FF_Disk_t *pxDisk );

FF_Disk_t *FF_eMMC_user_DiskInit(const char *pcName,bsp::eMMC* emmc)
{
FF_Error_t xError;
FF_Disk_t *pxDisk = NULL;
FF_CreationParameters_t xParameters;

	/* Attempt to allocated the FF_Disk_t structure. */
	pxDisk = ( FF_Disk_t * ) malloc( sizeof( FF_Disk_t ) );

	if( pxDisk != NULL )
	{
		/* Start with every member of the structure set to zero. */
		memset( pxDisk, '\0', sizeof( FF_Disk_t ) );

		/* The signature is used by the disk read and disk write functions to
		ensure the disk being accessed is a eMMC disk. */
		pxDisk->ulSignature = eMMCSIGNATURE;

		/* The number of sectors is recorded for bounds checking in the read and
		write functions. */
		pxDisk->ulNumberOfSectors = emmc->userPartitionBlocks;
		/* Store pointer to mmc_card_t structure */
		pxDisk->pvTag = emmc;

		/* Create the IO manager that will be used to control the eMMC disk. */
		memset( &xParameters, '\0', sizeof( xParameters ) );
		xParameters.pucCacheMemory = emmc_user_CacheBuffer;
		xParameters.ulMemorySize = mainIO_MANAGER_CACHE_SIZE;
		xParameters.ulSectorSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;
		xParameters.fnWriteBlocks = prvWriteeMMC;
		xParameters.fnReadBlocks = prvReadeMMC;
		xParameters.pxDisk = pxDisk;

		/* Driver is reentrant so xBlockDeviceIsReentrant can be set to pdTRUE.
		In this case the semaphore is only used to protect FAT data
		structures. */
		xParameters.pvSemaphore = ( void * ) xSemaphoreCreateRecursiveMutex();
		xParameters.xBlockDeviceIsReentrant = pdFALSE;


		/* Check the validity of the xIOManagerCacheSize parameter. */
		configASSERT( ( mainIO_MANAGER_CACHE_SIZE % xParameters.ulSectorSize ) == 0 );
		configASSERT( ( mainIO_MANAGER_CACHE_SIZE >= (size_t)( 2 * xParameters.ulSectorSize ) ) );

		pxDisk->pxIOManager = FF_CreateIOManger( &xParameters, &xError );

		if( ( pxDisk->pxIOManager != NULL ) && ( FF_isERR( xError ) == pdFALSE ) )
		{
			/* Record that the eMMC disk has been initialised. */
			pxDisk->xStatus.bIsInitialised = pdTRUE;


			/* Record the partition number the FF_Disk_t structure is, then
			mount the partition. */
			pxDisk->xStatus.bPartitionNumber = eMMCPARTITION_NUMBER;

#if 1
			/* Mount the partition. */
			xError = FF_Mount( pxDisk, pxDisk->xStatus.bPartitionNumber);
			LOG_PRINTF( "FF_eMMC_user_DiskInit: FF_Mount: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );

			if( FF_isERR( xError ) == pdFALSE )
			{
				/* The partition mounted successfully, add it to the virtual
				file system - where it will appear as a directory off the file
				system's root directory. */
				FF_FS_Add( pcName, pxDisk );
			}
			else
#endif
			{
				/* This is completely new eMMC disk so at first it needs proper formatting. */
				xError = prvPartitionAndFormatDisk(pxDisk);

				/* Mount the partition again, it should complete without fault. */
				if( FF_isERR( xError ) == pdFALSE )
				{
					/* Record the partition number the FF_Disk_t structure is, then
					mount the partition. */
					pxDisk->xStatus.bPartitionNumber = eMMCPARTITION_NUMBER;

					/* Mount the partition. */
					xError = FF_Mount( pxDisk, eMMCPARTITION_NUMBER );
					LOG_PRINTF( "FF_eMMC_user_DiskInit: FF_Mount: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );
				}

				if( FF_isERR( xError ) == pdFALSE )
				{
					/* The partition mounted successfully, add it to the virtual
					file system - where it will appear as a directory off the file
					system's root directory. */
					FF_FS_Add( pcName, pxDisk );
				}

			}
		}
		else
		{
			LOG_PRINTF( "FF_eMMC_user_DiskInit: FF_CreateIOManger: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );

			/* The disk structure was allocated, but the disk's IO manager could
			not be allocated, so free the disk again. */
			FF_eMMC_user_DiskDelete( pxDisk );
			pxDisk = NULL;
		}
	}
	else
	{
		LOG_PRINTF( "FF_eMMC_user_DiskInit: Malloc failed\r\n" );
	}

	return pxDisk;
}

BaseType_t FF_eMMC_user_DiskDelete( FF_Disk_t *pxDisk )
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

BaseType_t FF_eMMC_user_DiskShowPartition( FF_Disk_t *pxDisk )
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

		LOG_PRINTF( "Reading FAT and calculating Free Space\r\n" );

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
		iPercentageFree = ( int ) ( ( eMMCHUNDRED_64_BIT * ullFreeSectors + pxIOManager->xPartition.ulDataSectors / 2 ) /
			( ( uint64_t )pxIOManager->xPartition.ulDataSectors ) );

		ulTotalSizeMB = pxIOManager->xPartition.ulDataSectors / eMMCSECTORS_PER_MB;
		ulFreeSizeMB = ( uint32_t ) ( ullFreeSectors / eMMCSECTORS_PER_MB );

		/* It is better not to use the 64-bit format such as %Lu because it
		might not be implemented. */
		LOG_PRINTF( "Partition Nr   %8u\r\n", pxDisk->xStatus.bPartitionNumber );
		LOG_PRINTF( "Type           %8u (%s)\r\n", pxIOManager->xPartition.ucType, pcTypeName );
		LOG_PRINTF( "VolLabel       '%8s' \r\n", pxIOManager->xPartition.pcVolumeLabel );
		LOG_PRINTF( "TotalSectors   %8lu\r\n", pxIOManager->xPartition.ulTotalSectors );
		LOG_PRINTF( "SecsPerCluster %8lu\r\n", pxIOManager->xPartition.ulSectorsPerCluster );
		LOG_PRINTF( "Size           %8lu MB\r\n", ulTotalSizeMB );
		LOG_PRINTF( "FreeSize       %8lu MB ( %d perc free )\r\n", ulFreeSizeMB, iPercentageFree );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void FF_eMMC_user_DiskFlush( FF_Disk_t *pxDisk )
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

uint8_t FF_eMMC_user_DiskIsPresent(void)
{
	return 1;
}


static int32_t prvReadeMMC( uint8_t *pucDestination, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturn = FF_ERR_NONE;

 bsp::eMMC* mmc_card = static_cast<bsp::eMMC*>(pxDisk->pvTag);

	if( pxDisk != NULL )
	{
		if( pxDisk->ulSignature != eMMCSIGNATURE )
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
			if(mmc_card->ReadBlocks(pucDestination, ulSectorNumber, ulSectorCount) != bsp::RetCode::Success){
			    lReturn = FF_ERR_DEVICE_DRIVER_FAILED;
			}
		}
	}
	else
	{
		lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

static int32_t prvWriteeMMC( uint8_t *pucSource, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturn = FF_ERR_NONE;
    bsp::eMMC* mmc_card = static_cast<bsp::eMMC*>(pxDisk->pvTag);

	if( pxDisk != NULL )
	{

		if( pxDisk->ulSignature != eMMCSIGNATURE )
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
            if(mmc_card->WriteBlocks(pucSource, ulSectorNumber, ulSectorCount) != bsp::RetCode::Success){
                lReturn = FF_ERR_DEVICE_DRIVER_FAILED;
            }
		}
	}
	else
	{
		lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
	}

	return lReturn;
}

static FF_Error_t prvPartitionAndFormatDisk( FF_Disk_t *pxDisk )
{
FF_PartitionParameters_t xPartition;
FF_Error_t xError;

	/* Create a single partition that fills all available space on the disk. */
	memset( &xPartition, '\0', sizeof( xPartition ) );
	xPartition.ulSectorCount = pxDisk->ulNumberOfSectors;
	xPartition.ulHiddenSectors = eMMCHIDDEN_SECTOR_COUNT;
	xPartition.xPrimaryCount = eMMCPRIMARY_PARTITIONS;
	xPartition.eSizeType = eSizeIsQuota;

	/* Partition the disk */
	xError = FF_Partition( pxDisk, &xPartition );
	LOG_PRINTF( "FF_Partition: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );

	if( FF_isERR( xError ) == pdFALSE )
	{
		/* Format the partition. */
		xError = FF_Format( pxDisk, eMMCPARTITION_NUMBER, pdFALSE, pdFALSE );
		LOG_PRINTF( "FF_eMMC_user_DiskInit: FF_Format: %s\r\n", ( const char * ) FF_GetErrMessage( xError ) );
	}

	return xError;
}
