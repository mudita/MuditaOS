/**
 * @file ff_sddisk.h
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date Feb 8, 2018
 * @brief SD card driver for FreeRTOS+FAT
 * @copyright Copyright (C) 2018 mudita.com.
 * @details
 */

#ifndef ECOS_MODULES_FREERTOS_FAT_INCLUDE_FF_SDDISK_H_
#define ECOS_MODULES_FREERTOS_FAT_INCLUDE_FF_SDDISK_H_

typedef struct
{
  uint32_t CardType;                     /*!< Specifies the card Type                         */

  uint32_t CardVersion;                  /*!< Specifies the card version                      */

  uint32_t Class;                        /*!< Specifies the class of the card class           */

  uint32_t RelCardAdd;                   /*!< Specifies the Relative Card Address             */

  uint32_t BlockNbr;                     /*!< Specifies the Card Capacity in blocks           */

  uint32_t BlockSize;                    /*!< Specifies one block size in bytes               */

  uint32_t LogBlockNbr;                  /*!< Specifies the Card logical Capacity in blocks   */

  uint32_t LogBlockSize;                 /*!< Specifies logical block size in bytes           */

}FF_SDDisk_CardInfo_t;

/**
 * Initializes SD FAT disk.
 * @param pcName Disk name, it will be used for accesing disk i.e /sd
 * @param xIOManagerCacheSize
 * @return
 */
FF_Disk_t *FF_SDDiskInit( char *pcName,size_t xIOManagerCacheSize );
/**
 * Deletes SD FAT disk.
 * @param pxDisk Disk handle obtained from Init function.
 * @return
 */
BaseType_t FF_SDDiskDelete( FF_Disk_t *pxDisk );
/**
 * Prints inforamtion about mounted SD disk.
 * @param pxDisk Disk handle obtained from Init function.
 * @return
 */
BaseType_t FF_SDDiskShowPartition( FF_Disk_t *pxDisk );
/**
 * Flush internal buffers and writes them to disk.
 * @param pxDisk Disk handle obtained from Init function.
 */
void FF_SDDiskFlush( FF_Disk_t *pxDisk );
/**
 * Check if SD Card is in slot.
 * @return
 */
uint8_t FF_SDDiskIsPresent(void);
/**
 * Needs to be invoked when SD card is removed from slot.
 */
void FF_SDDiskCardRemovedCallback(void);


/**
 * Low-level API for FAT SDDisk. Need to be implemented for specific architecture/platform.
 */
uint8_t FF_SDDisk_LL_Init(void);
uint8_t FF_SDDisk_LL_IsCardPresent(void);
uint8_t FF_SDDisk_LL_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t FF_SDDisk_LL_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t FF_SDDisk_LL_GetCardInfo(FF_SDDisk_CardInfo_t* info);

#endif /* ECOS_MODULES_FREERTOS_FAT_INCLUDE_FF_SDDISK_H_ */
