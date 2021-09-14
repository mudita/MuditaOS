// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
// ffFAT glue layer code

#include <errno.h>
#include <cstring>
#include <string>
#include <unordered_map>
#include "services/osbdev_custom.h"
#include <reedgefs/volume_mapper.hpp>
#include <mutex.hpp>
#include <purefs/blkdev/defs.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/fs/handle_mapper.hpp>
#include <redvolume.h>

namespace purefs::fs::drivers::reedgefs::internal
{
    extern "C"
    {
        extern VOLCONF gaRedVolConf[REDCONF_VOLUME_COUNT];
        extern BDEVINFO gaRedBdevInfo[REDCONF_VOLUME_COUNT];
    }
    namespace
    {
        cpp_freertos::MutexRecursive g_lock;
        // Device manager part / LUN map
        std::unordered_map<std::string, int> g_registered_vol;
        fs::internal::handle_mapper<std::shared_ptr<blkdev::internal::disk_handle>> g_disk_handles;
        std::weak_ptr<blkdev::disk_manager> g_disk_mm;
    } // namespace
    namespace
    {
        inline auto disk_handle_from_pdrive(size_t pdrive)
        {
            cpp_freertos::LockGuard _lck(g_lock);
            return (g_disk_handles.exists(pdrive)) ? g_disk_handles[pdrive] : nullptr;
        }
    } // namespace

    std::pair<std::string, int> append_volume(blkdev::disk_fd diskh, const std::string &path)
    {
        cpp_freertos::LockGuard _lck(g_lock);
        const auto vol = g_registered_vol.find(std::string(diskh->name()));
        if (vol != std::end(g_registered_vol)) {
            return {"", vol->second};
        }
        if (g_disk_handles.size() >= REDCONF_VOLUME_COUNT) {
            return {"", -EOVERFLOW};
        }
        auto diskmm = g_disk_mm.lock();
        if (!diskmm) {
            return {"", -EIO};
        }

        if (path.length() > sizeof(VOLCONF::pszPathPrefix)) {
            return {"", -EIO};
        }

        const auto hwnd = g_disk_handles.insert(diskh);
        g_registered_vol.emplace(std::make_pair(diskh->name(), hwnd));
        gaRedVolConf[hwnd].ullSectorOffset = diskmm->get_info(diskh, blkdev::info_type::start_sector);
        return {std::string(gaRedVolConf[hwnd].pszPathPrefix), hwnd};
    }

    int remove_volume(blkdev::disk_fd diskh)
    {
        cpp_freertos::LockGuard _lck(g_lock);
        const auto vol_it = g_registered_vol.find(std::string(diskh->name()));
        if (vol_it == std::end(g_registered_vol)) {
            return -EBADF;
        }
        g_disk_handles.remove(vol_it->second);
        g_registered_vol.erase(vol_it);
        return 0;
    }

    void reset_volumes(std::shared_ptr<blkdev::disk_manager> diskmm)
    {
        cpp_freertos::LockGuard _lck(g_lock);
        g_disk_mm = diskmm;
        g_registered_vol.clear();
        g_disk_handles.clear();
    }

    extern "C"
    {

        REDSTATUS DiskOpen(uint8_t bVolNum, BDEVOPENMODE mode)
        {
            return disk_handle_from_pdrive(bVolNum) ? (0) : (-RED_EIO);
        }

        REDSTATUS DiskClose(uint8_t bVolNum)
        {
            (void)bVolNum;
            return 0;
        }

        REDSTATUS DiskGetGeometry(uint8_t bVolNum, BDEVINFO *pInfo)
        {
            const auto diskh = disk_handle_from_pdrive(bVolNum);
            if (!diskh) {
                return -RED_EIO;
            }
            const auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return -RED_EIO;
            }
            auto sectorSize  = diskmm->get_info(diskh, blkdev::info_type::sector_size);
            auto sectorCount = diskmm->get_info(diskh, blkdev::info_type::sector_count);

            if (sectorSize > 0 && sectorCount > 0) {
                pInfo->ulSectorSize   = sectorSize;
                pInfo->ullSectorCount = sectorCount;
                return 0;
            }

            return -RED_EIO;
        }

        REDSTATUS DiskRead(uint8_t bVolNum, uint64_t ullSectorStart, uint32_t ulSectorCount, void *pBuffer)
        {
            uint32_t ulSectorIdx  = 0U;
            uint32_t ulSectorSize = gaRedBdevInfo[bVolNum].ulSectorSize;
            auto pbBuffer         = static_cast<uint8_t *>(pBuffer);
            const auto diskh      = disk_handle_from_pdrive(bVolNum);
            if (!diskh) {
                return -RED_EIO;
            }
            const auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return -RED_EIO;
            }

            while (ulSectorIdx < ulSectorCount) {
                uint32_t ulTransfer = REDMIN(ulSectorCount - ulSectorIdx, MAX_SECTOR_TRANSFER);

                auto result = diskmm->read(
                    diskh, &pbBuffer[ulSectorIdx * ulSectorSize], (ullSectorStart + ulSectorIdx), ulTransfer);
                if (result < 0) {
                    LOG_ERROR("write error %i", result);
                    return (result == -ERANGE) ? (-RED_ERANGE) : (-RED_EIO);
                }

                ulSectorIdx += ulTransfer;
            }

            return 0;
        }

        REDSTATUS DiskWrite(uint8_t bVolNum, uint64_t ullSectorStart, uint32_t ulSectorCount, const void *pBuffer)
        {
            uint32_t ulSectorIdx  = 0U;
            uint32_t ulSectorSize = gaRedBdevInfo[bVolNum].ulSectorSize;
            auto pbBuffer         = static_cast<const uint8_t *>(pBuffer);
            const auto diskh      = disk_handle_from_pdrive(bVolNum);
            if (!diskh) {
                return -RED_EIO;
            }
            const auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return -RED_EIO;
            }

            while (ulSectorIdx < ulSectorCount) {
                uint32_t ulTransfer = REDMIN(ulSectorCount - ulSectorIdx, MAX_SECTOR_TRANSFER);

                auto result = diskmm->write(
                    diskh, &pbBuffer[ulSectorIdx * ulSectorSize], (ullSectorStart + ulSectorIdx), ulTransfer);
                if (result < 0) {
                    LOG_ERROR("write error %i", result);
                    return (result == -ERANGE) ? (-RED_ERANGE) : (-RED_EIO);
                }

                ulSectorIdx += ulTransfer;
            }

            return 0;
        }

        REDSTATUS DiskFlush(uint8_t bVolNum)
        {
            const auto diskh = disk_handle_from_pdrive(bVolNum);
            if (!diskh) {
                return -RED_EIO;
            }
            const auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return -RED_EIO;
            }

            auto result = diskmm->sync(diskh);
            if (result < 0) {
                LOG_ERROR("sync error %i", result);
                return -RED_EIO;
            }

            return 0;
        }
    }

} // namespace purefs::fs::drivers::reedgefs::internal
