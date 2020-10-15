//
// Created by lucck on 14.10.2020.
//
#include <vfs.hpp>
#include <ff_image_user_disk.hpp>
#include <filesystem>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

namespace
{
    constexpr auto FSL_SDMMC_DEFAULT_BLOCK_SIZE = 512UL;
    constexpr auto eMMCSIGNATURE                = 0x61606362;
    constexpr auto mainIO_MANAGER_CACHE_SIZE    = 15UL * FSL_SDMMC_DEFAULT_BLOCK_SIZE;
    constexpr auto eMMCPARTITION_NUMBER         = 0;

    constexpr auto eMMCHIDDEN_SECTOR_COUNT = 8;
    constexpr auto eMMCPRIMARY_PARTITIONS  = 1;

    constexpr auto eMMCHUNDRED_64_BIT = 100ULL;

    constexpr auto eMMCBYTES_PER_MB   = 1024ull * 1024ull;
    constexpr auto eMMCSECTORS_PER_MB = eMMCBYTES_PER_MB / 512ull;

    alignas(32) uint8_t emmc_user_CacheBuffer[mainIO_MANAGER_CACHE_SIZE];
} // namespace

namespace
{
    class file_operation
    {
        static constexpr auto SECT_SIZE = 512;

      public:
        file_operation(const file_operation &) = delete;
        file_operation &operator=(const file_operation &) = delete;
        // Constructor
        file_operation(const char file_name[])
        {
            m_fd = ::open(file_name, O_RDWR, O_SYNC);
            if (m_fd < 0) {
                throw std::system_error();
            }
        }
        // Destructor
        ~file_operation()
        {
            if (m_fd) {
                ::close(m_fd);
            }
        }
        // Read sector
        int read(void *buf, unsigned sector_number, unsigned sector_count)
        {
            int ret = ::lseek(m_fd, sector_number * SECT_SIZE, SEEK_SET);
            if (ret < 0) {
                return ret;
            }
            auto to_read = sector_count * SECT_SIZE;
            auto buf_b   = reinterpret_cast<uint8_t *>(buf);
            do {
                ret = ::read(m_fd, buf_b, to_read);
                if (ret < 0) {
                    return ret;
                }
                to_read -= ret;
                buf_b += ret;
            } while (to_read > 0);
            return 0;
        }
        // Write sector
        int write(const void *buf, unsigned sector_number, unsigned sector_count)
        {
            int ret = ::lseek(m_fd, sector_number * SECT_SIZE, SEEK_SET);
            if (ret < 0) {
                return ret;
            }
            auto to_write = sector_count * SECT_SIZE;
            auto buf_b    = reinterpret_cast<const uint8_t *>(buf);
            do {
                ret = ::write(m_fd, buf_b, to_write);
                if (ret < 0) {
                    return ret;
                }
                to_write -= ret;
                buf_b += ret;
            } while (to_write > 0);
            return 0;
        }

      private:
        int m_fd{};
    };

    int32_t writeBlocks(uint8_t *pucSource, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk)
    {
        int32_t lReturn = FF_ERR_NONE;
        auto mmc_card   = static_cast<file_operation *>(pxDisk->pvTag);

        if (pxDisk != NULL) {

            if (pxDisk->ulSignature != eMMCSIGNATURE) {
                /* The disk structure is not valid because it doesn't contain a
                magic number written to the disk when it was created. */
                lReturn = FF_ERR_IOMAN_DRIVER_FATAL_ERROR | FF_ERRFLAG;
            }
            else if (pxDisk->xStatus.bIsInitialised == pdFALSE) {
                /* The disk has not been initialised. */
                lReturn = FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG;
            }
            else if (ulSectorNumber >= pxDisk->ulNumberOfSectors) {
                /* The start sector is not within the bounds of the disk. */
                lReturn = (FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG);
            }
            else if ((pxDisk->ulNumberOfSectors - ulSectorNumber) < ulSectorCount) {
                /* The end sector is not within the bounds of the disk. */
                lReturn = (FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG);
            }
            else {
                if (mmc_card->write(pucSource, ulSectorNumber, ulSectorCount) < 0) {
                    lReturn = FF_ERR_DEVICE_DRIVER_FAILED;
                }
            }
        }
        else {
            lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
        }

        return lReturn;
    }

    int32_t readBlocks(uint8_t *pucDestination, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk)
    {
        int32_t lReturn = FF_ERR_NONE;

        auto mmc_card = static_cast<file_operation *>(pxDisk->pvTag);

        if (pxDisk != NULL) {
            if (pxDisk->ulSignature != eMMCSIGNATURE) {
                /* The disk structure is not valid because it doesn't contain a
                magic number written to the disk when it was created. */
                lReturn = FF_ERR_IOMAN_DRIVER_FATAL_ERROR | FF_ERRFLAG;
            }
            else if (pxDisk->xStatus.bIsInitialised == pdFALSE) {
                /* The disk has not been initialised. */
                lReturn = FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG;
            }
            else if (ulSectorNumber >= pxDisk->ulNumberOfSectors) {
                /* The start sector is not within the bounds of the disk. */
                lReturn = (FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG);
            }
            else if ((pxDisk->ulNumberOfSectors - ulSectorNumber) < ulSectorCount) {
                /* The end sector is not within the bounds of the disk. */
                lReturn = (FF_ERR_IOMAN_OUT_OF_BOUNDS_WRITE | FF_ERRFLAG);
            }
            else {
                if (mmc_card->read(pucDestination, ulSectorNumber, ulSectorCount) < 0) {
                    lReturn = FF_ERR_DEVICE_DRIVER_FAILED;
                }
            }
        }
        else {
            lReturn = FF_ERR_NULL_POINTER | FF_ERRFLAG;
        }

        return lReturn;
    }

    FF_Error_t prvPartitionAndFormatDisk(FF_Disk_t *pxDisk)
    {
        FF_PartitionParameters_t xPartition;
        FF_Error_t xError;

        /* Create a single partition that fills all available space on the disk. */
        memset(&xPartition, '\0', sizeof(xPartition));
        xPartition.ulSectorCount   = pxDisk->ulNumberOfSectors;
        xPartition.ulHiddenSectors = eMMCHIDDEN_SECTOR_COUNT;
        xPartition.xPrimaryCount   = eMMCPRIMARY_PARTITIONS;
        xPartition.eSizeType       = eSizeIsQuota;

        /* Partition the disk */
        xError = FF_Partition(pxDisk, &xPartition);
        LOG_PRINTF("FF_Partition: %s\r\n", (const char *)FF_GetErrMessage(xError));

        if (FF_isERR(xError) == pdFALSE) {
            /* Format the partition. */
            xError = FF_Format(pxDisk, eMMCPARTITION_NUMBER, pdFALSE, pdFALSE);
            LOG_PRINTF("FF_eMMC_user_DiskInit: FF_Format: %s\r\n", (const char *)FF_GetErrMessage(xError));
        }

        return xError;
    }

    FF_Error_t FF_InvalidateCache(FF_IOManager_t *pxIOManager)
    {
        BaseType_t xIndex;
        FF_Error_t xError;

        if (pxIOManager == NULL) {
            xError = FF_ERR_NULL_POINTER | FF_FLUSHCACHE;
        }
        else {
            xError = FF_ERR_NONE;

            FF_PendSemaphore(pxIOManager->pvSemaphore);
            {
                for (xIndex = 0; xIndex < pxIOManager->usCacheSize; xIndex++) {
                    /* If a buffers has no users and if it has been modified... */
                    if ((pxIOManager->pxBuffers[xIndex].usNumHandles != 0) ||
                        (pxIOManager->pxBuffers[xIndex].bModified == pdTRUE)) {
                        /* Can not flush all caches. */
                        xError++;
                    }
                    else {
                        /* Mark the buffer as invalid so that it won't be used again. */
                        pxIOManager->pxBuffers[xIndex].bValid = pdFALSE;
                    }
                }
            }

            FF_ReleaseSemaphore(pxIOManager->pvSemaphore);
        }

        /* Function successful if it returns 0. */
        return xError;
    }
} // namespace

namespace freertos_fat::internals
{
    // *** ***
    FF_Disk_t *diskInit(const char *pcName, const char img_path[])
    {
        FF_Error_t xError;
        FF_Disk_t *pxDisk = NULL;
        FF_CreationParameters_t xParameters;

        /* Attempt to allocated the FF_Disk_t structure. */
        pxDisk = (FF_Disk_t *)malloc(sizeof(FF_Disk_t));

        if (pxDisk != NULL) {
            /* Start with every member of the structure set to zero. */
            memset(pxDisk, '\0', sizeof(FF_Disk_t));

            /* The signature is used by the disk read and disk write functions to
            ensure the disk being accessed is a eMMC disk. */
            pxDisk->ulSignature = eMMCSIGNATURE;

            /* The number of sectors is recorded for bounds checking in the read and
            write functions. */
            pxDisk->ulNumberOfSectors = std::filesystem::file_size(img_path) / FSL_SDMMC_DEFAULT_BLOCK_SIZE;
            /* Store pointer to mmc_card_t structure */
            pxDisk->pvTag = new file_operation(img_path);

            /* Create the IO manager that will be used to control the eMMC disk. */
            memset(&xParameters, '\0', sizeof(xParameters));
            xParameters.pucCacheMemory = emmc_user_CacheBuffer;
            xParameters.ulMemorySize   = mainIO_MANAGER_CACHE_SIZE;
            xParameters.ulSectorSize   = FSL_SDMMC_DEFAULT_BLOCK_SIZE;
            xParameters.fnWriteBlocks  = writeBlocks;
            xParameters.fnReadBlocks   = readBlocks;
            xParameters.pxDisk         = pxDisk;

            /* Driver is reentrant so xBlockDeviceIsReentrant can be set to pdTRUE.
            In this case the semaphore is only used to protect FAT data
            structures. */
            xParameters.pvSemaphore             = (void *)xSemaphoreCreateRecursiveMutex();
            xParameters.xBlockDeviceIsReentrant = pdFALSE;

            /* Check the validity of the xIOManagerCacheSize parameter. */
            configASSERT((mainIO_MANAGER_CACHE_SIZE % xParameters.ulSectorSize) == 0);
            configASSERT((mainIO_MANAGER_CACHE_SIZE >= (size_t)(2 * xParameters.ulSectorSize)));

            pxDisk->pxIOManager = FF_CreateIOManger(&xParameters, &xError);

            if ((pxDisk->pxIOManager != NULL) && (FF_isERR(xError) == pdFALSE)) {
                /* Record that the eMMC disk has been initialised. */
                pxDisk->xStatus.bIsInitialised = pdTRUE;

                /* Record the partition number the FF_Disk_t structure is, then
                mount the partition. */
                pxDisk->xStatus.bPartitionNumber = eMMCPARTITION_NUMBER;

#if 1
                /* Mount the partition. */
                xError = FF_Mount(pxDisk, pxDisk->xStatus.bPartitionNumber);
                LOG_PRINTF("FF_eMMC_user_DiskInit: FF_Mount: %s\r\n", (const char *)FF_GetErrMessage(xError));

                if (FF_isERR(xError) == pdFALSE) {
                    /* The partition mounted successfully, add it to the virtual
                    file system - where it will appear as a directory off the file
                    system's root directory. */
                    FF_FS_Add(pcName, pxDisk);
                }
                else
#endif
                {
                    /* This is completely new eMMC disk so at first it needs proper formatting. */
                    xError = prvPartitionAndFormatDisk(pxDisk);

                    /* Mount the partition again, it should complete without fault. */
                    if (FF_isERR(xError) == pdFALSE) {
                        /* Record the partition number the FF_Disk_t structure is, then
                        mount the partition. */
                        pxDisk->xStatus.bPartitionNumber = eMMCPARTITION_NUMBER;

                        /* Mount the partition. */
                        xError = FF_Mount(pxDisk, eMMCPARTITION_NUMBER);
                        LOG_PRINTF("FF_eMMC_user_DiskInit: FF_Mount: %s\r\n", (const char *)FF_GetErrMessage(xError));
                    }

                    if (FF_isERR(xError) == pdFALSE) {
                        /* The partition mounted successfully, add it to the virtual
                        file system - where it will appear as a directory off the file
                        system's root directory. */
                        FF_FS_Add(pcName, pxDisk);
                    }
                }
            }
            else {
                LOG_PRINTF("FF_eMMC_user_DiskInit: FF_CreateIOManger: %s\r\n", (const char *)FF_GetErrMessage(xError));

                /* The disk structure was allocated, but the disk's IO manager could
                not be allocated, so free the disk again. */
                diskDelete(pxDisk);
                pxDisk = NULL;
            }
        }
        else {
            LOG_PRINTF("FF_eMMC_user_DiskInit: Malloc failed\r\n");
        }

        return pxDisk;
    }
    BaseType_t diskDelete(FF_Disk_t *pxDisk)
    {
        if (pxDisk != NULL) {
            pxDisk->ulSignature            = 0;
            pxDisk->xStatus.bIsInitialised = 0;
            if (pxDisk->pvTag) {
                auto fops = reinterpret_cast<file_operation *>(pxDisk->pvTag);
                delete fops;
            }
            if (pxDisk->pxIOManager != NULL) {
                FF_DeleteIOManager(pxDisk->pxIOManager);
            }

            vPortFree(pxDisk);
        }
        return pdPASS;
    }

    BaseType_t diskShowPartition(FF_Disk_t *pxDisk)
    {
        FF_Error_t xError;
        uint64_t ullFreeSectors;
        uint32_t ulTotalSizeMB, ulFreeSizeMB;
        int iPercentageFree;
        FF_IOManager_t *pxIOManager;
        const char *pcTypeName = "unknown type";
        BaseType_t xReturn     = pdPASS;

        if (pxDisk == NULL) {
            xReturn = pdFAIL;
        }
        else {
            pxIOManager = pxDisk->pxIOManager;

            LOG_PRINTF("Reading FAT and calculating Free Space\r\n");

            switch (pxIOManager->xPartition.ucType) {
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

            FF_GetFreeSize(pxIOManager, &xError);

            ullFreeSectors  = pxIOManager->xPartition.ulFreeClusterCount * pxIOManager->xPartition.ulSectorsPerCluster;
            iPercentageFree = (int)((eMMCHUNDRED_64_BIT * ullFreeSectors + pxIOManager->xPartition.ulDataSectors / 2) /
                                    ((uint64_t)pxIOManager->xPartition.ulDataSectors));

            ulTotalSizeMB = pxIOManager->xPartition.ulDataSectors / eMMCSECTORS_PER_MB;
            ulFreeSizeMB  = (uint32_t)(ullFreeSectors / eMMCSECTORS_PER_MB);

            /* It is better not to use the 64-bit format such as %Lu because it
            might not be implemented. */
            LOG_PRINTF("Type           %8u (%s)\r\n", pxIOManager->xPartition.ucType, pcTypeName);
            LOG_PRINTF("VolLabel       '%8s' \r\n", pxIOManager->xPartition.pcVolumeLabel);
            LOG_PRINTF("TotalSectors   %8u\r\n", pxIOManager->xPartition.ulTotalSectors);
            LOG_PRINTF("SecsPerCluster %8u\r\n", pxIOManager->xPartition.ulSectorsPerCluster);
            LOG_PRINTF("Size           %8u MB\r\n", ulTotalSizeMB);
            LOG_PRINTF("FreeSize       %8u MB ( %d perc free )\r\n", ulFreeSizeMB, iPercentageFree);
        }

        return xReturn;
    }
    void diskFlush(FF_Disk_t *pxDisk)
    {
        FF_FlushCache(pxDisk->pxIOManager);

        if (FF_InvalidateCache(pxDisk->pxIOManager) != 0) {
            /* Not all buffers could be invalidated. */
        }
    }
    uint8_t diskIsPresent(void)
    {
        return 1;
    }
} // namespace freertos_fat::internals
