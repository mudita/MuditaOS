/*
 *  @file ff_eMMC_user_disk.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 01.03.2018
 *  @brief
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#ifndef FREERTOS_PLUS_FAT_PORTABLE_FF_EMMC_USER_DISK_H_
#define FREERTOS_PLUS_FAT_PORTABLE_FF_EMMC_USER_DISK_H_

#include "vfs.hpp"
#include "board/cross/eMMC/eMMC.hpp"

FF_Disk_t *FF_eMMC_user_DiskInit( const char *pcName,bsp::eMMC* emmc);
BaseType_t FF_eMMC_user_DiskDelete( FF_Disk_t *pxDisk );
BaseType_t FF_eMMC_user_DiskShowPartition( FF_Disk_t *pxDisk );
void FF_eMMC_user_DiskFlush( FF_Disk_t *pxDisk );
uint8_t FF_eMMC_user_DiskIsPresent(void);

#endif /* FREERTOS_PLUS_FAT_PORTABLE_FF_EMMC_USER_DISK_H_ */
