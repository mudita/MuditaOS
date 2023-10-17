// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*             ----> DO NOT REMOVE THE FOLLOWING NOTICE <----

                  Copyright (c) 2014-2021 Tuxera US Inc.
                      All Rights Reserved Worldwide.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; use version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but "AS-IS," WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
/*  Businesses and individuals that for commercial or other reasons cannot
    comply with the terms of the GPLv2 license must obtain a commercial license
    before incorporating Reliance Edge into proprietary software for
    distribution in any form.  Visit http://www.datalight.com/reliance-edge for
    more information.
*/
/** @file
    @brief Implements block device I/O.
*/
#include <FreeRTOS.h>

#include <redfs.h>
#include <redvolume.h>
#include <redbdev.h>

/*------------------------------------------------------------------------------
    Porting Note:

    Several example implementations of this module for FreeRTOS are available.
    If you are lucky, you can use one of these implementations; otherwise, these
    can serve as examples of how to implement this service.
------------------------------------------------------------------------------*/

/** @brief A custom implementation, initially stubbed out.

    This the default setting: it is initially stubbed out (does nothing) and
    produces an error when compiled so that it is obvious that this file needs
    to be modified.  You can edit this code to provide your own implementation
    of the block device.  Alternatively, you can delete all of the Disk*()
    functions and put the custom code directly into the RedOsBDev*() functions.
*/
#define BDEV_CUSTOM (0U)

/** @brief Tuxera FlashFX Tera driver implementation.

    This implementation uses Tuxera's FlashFX Tera driver to use raw flash
    storage with Reliance Edge.

    This option is only available in commercial releases of Reliance Edge.
*/
#define BDEV_FLASHFX (1U)

/** @brief The FatFs example implementation.

    This implementation is designed to reuse an existing block device driver
    that was written for FatFs.  If you have such a driver, it can be linked
    in and used immediately.  The FatFs `diskio.h` header must be in the include
    directory path.
*/
#define BDEV_FATFS (3U)

/** @brief The Atmel Studio Framework SD/MMC driver example implementation.

    This implementation uses a modified version of the open source SD/MMC driver
    included in the Atmel Studio Framework (ASF) and will work as-is for many
    varieties of Atmel hardware.  This example assumes relatively minor
    modifications to the ASF SD/MMC driver to make it support multi-sector read
    and write requests, which greatly improves performance.  The modified driver
    is distributed with the Reliance Edge commercial kit and is included in
    FreeRTOS Atmel projects that come with the commercial kit (such as in
    projects/freertos/atmel/sam4e-ek/src/ASF).

    This example can easily be modified to work with an unmodified version of
    the ASF SD/MMC driver.  Simply replace sd_mmc_mem_2_ram_multi() and
    sd_mmc_ram_2_mem_multi() with sd_mmc_mem_2_ram() and sd_mmc_ram_2_mem()
    respectively, and add a for loop to loop over each sector in the request.
    However, as described in the manual, there are considerable performance
    advantages to issuing real multi-sector requests, so using the modified
    driver is recommended.
*/
#define BDEV_ATMEL_SDMMC (4U)

/** @brief The ST Microelectronics STM32 SDIO driver example implementation.

    This implementation accesses the microSD card through the BSP utilities
    provided as part of the STM32Cube package, used with the STM32 HAL drivers.
    The STM3240G-EVAL and STM32F746NG-Discovery boards are currently supported.
*/
#define BDEV_STM32_SDIO (5U)

/** @brief The RAM disk example implementation.

    This implementation uses a RAM disk.  It will allow you to compile and test
    Reliance Edge even if your storage driver is not yet ready.  On typical
    target hardware, the amount of spare RAM will be limited so generally only
    very small disks will be available.
*/
#define BDEV_RAM_DISK (6U)

/** @brief Pick which example implementation is compiled.

    Must be one of:
    - #BDEV_CUSTOM
    - #BDEV_FLASHFX
    - #BDEV_FATFS
    - #BDEV_ATMEL_SDMMC
    - #BDEV_STM32_SDIO
    - #BDEV_RAM_DISK
*/
#ifndef BDEV_EXAMPLE_IMPLEMENTATION
#define BDEV_EXAMPLE_IMPLEMENTATION BDEV_CUSTOM
#endif

/*  The DiskOpen(), DiskClose(), DiskRead(), DiskWrite(), DiskFlush(), and
    DiskDiscard() functions used below are defined in these header files:
*/
#if BDEV_EXAMPLE_IMPLEMENTATION == BDEV_CUSTOM
#include "osbdev_custom.h"
#elif BDEV_EXAMPLE_IMPLEMENTATION == BDEV_FLASHFX
#if RED_KIT == RED_KIT_GPL
#error "FlashFX block device only supported in commercial versions of Reliance Edge."
#endif
#include "osbdev_flashfx.h"
#elif BDEV_EXAMPLE_IMPLEMENTATION == BDEV_FATFS
#include "osbdev_fatfs.h"
#elif BDEV_EXAMPLE_IMPLEMENTATION == BDEV_ATMEL_SDMMC
#include "osbdev_asfsdmmc.h"
#elif BDEV_EXAMPLE_IMPLEMENTATION == BDEV_STM32_SDIO
#include "osbdev_stm32sdio.h"
#elif BDEV_EXAMPLE_IMPLEMENTATION == BDEV_RAM_DISK
#include "osbdev_ramdisk.h"
#else
#error "Invalid BDEV_EXAMPLE_IMPLEMENTATION value"
#endif /* BDEV_EXAMPLE_IMPLEMENTATION == ... */

/** @brief Initialize a block device.

    This function is called when the file system needs access to a block
    device.

    Upon successful return, the block device should be fully initialized and
    ready to service read/write/flush/close requests.

    The behavior of calling this function on a block device which is already
    open is undefined.

    @param bVolNum  The volume number of the volume whose block device is being
                    initialized.
    @param mode     The open mode, indicating the type of access required.

    @return A negated ::REDSTATUS code indicating the operation result.

    @retval 0           Operation was successful.
    @retval -RED_EINVAL @p bVolNum is an invalid volume number.
    @retval -RED_EIO    A disk I/O error occurred.
*/
REDSTATUS RedOsBDevOpen(uint8_t bVolNum, BDEVOPENMODE mode)
{
    REDSTATUS ret;

    if (bVolNum >= REDCONF_VOLUME_COUNT) {
        ret = -RED_EINVAL;
    }
    else {
        ret = DiskOpen(bVolNum, mode);
    }

    return ret;
}

/** @brief Uninitialize a block device.

    This function is called when the file system no longer needs access to a
    block device.  If any resource were allocated by RedOsBDevOpen() to service
    block device requests, they should be freed at this time.

    Upon successful return, the block device must be in such a state that it
    can be opened again.

    The behavior of calling this function on a block device which is already
    closed is undefined.

    @param bVolNum  The volume number of the volume whose block device is being
                    uninitialized.

    @return A negated ::REDSTATUS code indicating the operation result.

    @retval 0           Operation was successful.
    @retval -RED_EINVAL @p bVolNum is an invalid volume number.
*/
REDSTATUS RedOsBDevClose(uint8_t bVolNum)
{
    REDSTATUS ret;

    if (bVolNum >= REDCONF_VOLUME_COUNT) {
        ret = -RED_EINVAL;
    }
    else {
        ret = DiskClose(bVolNum);
    }

    return ret;
}

/** @brief Return the block device geometry.

    The behavior of calling this function is undefined if the block device is
    closed.

    @param bVolNum  The volume number of the volume whose block device geometry
                    is being queried.
    @param pInfo    On successful return, populated with the geometry of the
                    block device.

    @return A negated ::REDSTATUS code indicating the operation result.

    @retval 0               Operation was successful.
    @retval -RED_EINVAL     @p bVolNum is an invalid volume number, or @p pInfo
                            is `NULL`.
    @retval -RED_EIO        A disk I/O error occurred.
    @retval -RED_ENOTSUPP   The geometry cannot be queried on this block device.
*/
REDSTATUS RedOsBDevGetGeometry(uint8_t bVolNum, BDEVINFO *pInfo)
{
    REDSTATUS ret;

    if ((bVolNum >= REDCONF_VOLUME_COUNT) || (pInfo == NULL)) {
        ret = -RED_EINVAL;
    }
    else {
        ret = DiskGetGeometry(bVolNum, pInfo);
    }

    return ret;
}

/** @brief Read sectors from a physical block device.

    The behavior of calling this function is undefined if the block device is
    closed or if it was opened with ::BDEV_O_WRONLY.

    @param bVolNum          The volume number of the volume whose block device
                            is being read from.
    @param ullSectorStart   The starting sector number.
    @param ulSectorCount    The number of sectors to read.
    @param pBuffer          The buffer into which to read the sector data.

    @return A negated ::REDSTATUS code indicating the operation result.

    @retval 0           Operation was successful.
    @retval -RED_EINVAL @p bVolNum is an invalid volume number, @p pBuffer is
                        `NULL`, or @p ullStartSector and/or @p ulSectorCount
                        refer to an invalid range of sectors.
    @retval -RED_EIO    A disk I/O error occurred.
*/
REDSTATUS RedOsBDevRead(uint8_t bVolNum, uint64_t ullSectorStart, uint32_t ulSectorCount, void *pBuffer)
{
    REDSTATUS ret;

    if ((bVolNum >= REDCONF_VOLUME_COUNT) || !VOLUME_SECTOR_RANGE_IS_VALID(bVolNum, ullSectorStart, ulSectorCount) ||
        (pBuffer == NULL)) {
        ret = -RED_EINVAL;
    }
    else {
        ret = DiskRead(bVolNum, ullSectorStart, ulSectorCount, pBuffer);
    }

    return ret;
}

#if REDCONF_READ_ONLY == 0

/** @brief Write sectors to a physical block device.

    The behavior of calling this function is undefined if the block device is
    closed or if it was opened with ::BDEV_O_RDONLY.

    @param bVolNum          The volume number of the volume whose block device
                            is being written to.
    @param ullSectorStart   The starting sector number.
    @param ulSectorCount    The number of sectors to write.
    @param pBuffer          The buffer from which to write the sector data.

    @return A negated ::REDSTATUS code indicating the operation result.

    @retval 0           Operation was successful.
    @retval -RED_EINVAL @p bVolNum is an invalid volume number, @p pBuffer is
                        `NULL`, or @p ullStartSector and/or @p ulSectorCount
                        refer to an invalid range of sectors.
    @retval -RED_EIO    A disk I/O error occurred.
*/
REDSTATUS RedOsBDevWrite(uint8_t bVolNum, uint64_t ullSectorStart, uint32_t ulSectorCount, const void *pBuffer)
{
    REDSTATUS ret;

    if ((bVolNum >= REDCONF_VOLUME_COUNT) || !VOLUME_SECTOR_RANGE_IS_VALID(bVolNum, ullSectorStart, ulSectorCount) ||
        (pBuffer == NULL)) {
        ret = -RED_EINVAL;
    }
    else {
        ret = DiskWrite(bVolNum, ullSectorStart, ulSectorCount, pBuffer);
    }

    return ret;
}

/** @brief Flush any caches beneath the file system.

    This function must synchronously flush all software and hardware caches
    beneath the file system, ensuring that all sectors written previously are
    committed to permanent storage.

    If the environment has no caching beneath the file system, the
    implementation of this function can do nothing and return success.

    The behavior of calling this function is undefined if the block device is
    closed or if it was opened with ::BDEV_O_RDONLY.

    @param bVolNum  The volume number of the volume whose block device is being
                    flushed.

    @return A negated ::REDSTATUS code indicating the operation result.

    @retval 0           Operation was successful.
    @retval -RED_EINVAL @p bVolNum is an invalid volume number.
    @retval -RED_EIO    A disk I/O error occurred.
*/
REDSTATUS RedOsBDevFlush(uint8_t bVolNum)
{
    REDSTATUS ret;

    if (bVolNum >= REDCONF_VOLUME_COUNT) {
        ret = -RED_EINVAL;
    }
    else {
        ret = DiskFlush(bVolNum);
    }

    return ret;
}

#endif /* REDCONF_READ_ONLY == 0 */
