// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
// ffFAT glue layer code

#include <errno.h>
#include <cstring>
#include <string>
#include <unordered_map>
#include <volume_mapper.hpp>
#include <ff.h>
#include <diskio.h>
#include <mutex.hpp>
#include <purefs/blkdev/defs.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/fs/handle_mapper.hpp>
#include <algorithm>

/** NOTE: FF_FAT require global symbol partition to
 * drive map so it is ugly
 */
extern "C" PARTITION VolToPart[FF_VOLUMES];

namespace purefs::fs::drivers::ffat::internal
{
    namespace
    {
        cpp_freertos::MutexRecursive g_lock;
        unsigned g_vol_to_part_index{};
        // Device manager part / ffat LUN map
        std::unordered_map<std::string, int> g_registered_vol;
        // Physical drive to device manager
        fs::internal::handle_mapper<std::weak_ptr<blkdev::internal::disk_handle>> g_diskh_map;
        std::weak_ptr<blkdev::disk_manager> g_disk_mm;
    } // namespace
    namespace
    {
        auto disk_handle_from_pdrive(size_t pdrive) -> blkdev::disk_fd
        {
            cpp_freertos::LockGuard _lck(g_lock);
            return g_diskh_map.exists(pdrive) ? g_diskh_map[pdrive].lock() : nullptr;
        }
    } // namespace

    int append_volume(blkdev::disk_fd diskh)
    {
        cpp_freertos::LockGuard _lck(g_lock);
        const auto vol = g_registered_vol.find(std::string(diskh->name()));
        if (vol != std::end(g_registered_vol)) {
            return vol->second;
        }
        if (g_vol_to_part_index >= FF_VOLUMES) {
            return -EOVERFLOW;
        }
        auto diskm = g_disk_mm.lock();
        if (!diskm) {
            return -EIO;
        }
        static constexpr auto invalid_pdrv = -1;
        auto pdrv{invalid_pdrv};
        {
            //? Is disk registered in the ffat
            for (size_t i = 0; i < g_diskh_map.size(); ++i) {
                const auto dh   = g_diskh_map[i].lock();
                const auto disk = diskh->disk();
                if (!disk)
                    break;
                if (!dh)
                    continue;
                if (dh->disk() == disk) {
                    pdrv = i;
                    break;
                }
            }
            // No register it as a handle
            if (pdrv == invalid_pdrv) {
                pdrv = g_diskh_map.insert(diskh);
                if (pdrv > std::numeric_limits<BYTE>::max()) {
                    g_diskh_map.remove(pdrv);
                    return -ERANGE;
                }
            }
        }
        auto part = diskm->partitions(diskh)[diskh->partition()];
        if (part.mbr_number < 1) {
            LOG_ERROR("vfat: FF_FAT driver support only MBR parts");
            return -ENOTSUP;
        }

        VolToPart[g_vol_to_part_index] = {BYTE(pdrv), BYTE(part.mbr_number - 1)};
        g_registered_vol.emplace(std::make_pair(diskh->name(), g_vol_to_part_index));

        return g_vol_to_part_index++;
    }

    void reset_volumes(std::shared_ptr<blkdev::disk_manager> diskmm)
    {
        cpp_freertos::LockGuard _lck(g_lock);
        g_disk_mm = diskmm;
        std::memset(VolToPart, 0, sizeof(VolToPart));
        g_vol_to_part_index = 0;
        g_registered_vol.clear();
        g_diskh_map.clear();
    }

    extern "C"
    {

        DSTATUS disk_initialize(BYTE pdrv)
        {
            const auto diskh = disk_handle_from_pdrive(pdrv);
            if (!diskh) {
                return RES_ERROR;
            }
            auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return RES_ERROR;
            }
            const auto status = diskmm->status(diskh);
            switch (status) {
            case blkdev::media_status::healthly:
                return RES_OK;
            case blkdev::media_status::error:
                return RES_ERROR;
            case blkdev::media_status::nomedia:
            case blkdev::media_status::uninit:
                return RES_NOTRDY;
            case blkdev::media_status::wprotect:
                return RES_WRPRT;
            }
            return RES_ERROR;
        }

        DSTATUS disk_status(BYTE pdrv)
        {
            return RES_ERROR;
        }
        DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
        {
            return RES_ERROR;
        }
        DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
        {
            return RES_ERROR;
        }
        DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
        {
            return RES_ERROR;
        }
    }
} // namespace purefs::fs::drivers::ffat::internal
