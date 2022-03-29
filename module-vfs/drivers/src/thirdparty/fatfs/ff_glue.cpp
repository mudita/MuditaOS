// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
#include <log/log.hpp>

namespace purefs::fs::drivers::ffat::internal
{
    namespace
    {
        cpp_freertos::MutexRecursive g_lock;
        // Device manager part / ffat LUN map
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

    int append_volume(blkdev::disk_fd diskh)
    {
        cpp_freertos::LockGuard _lck(g_lock);
        const auto vol = g_registered_vol.find(std::string(diskh->name()));
        if (vol != std::end(g_registered_vol)) {
            return vol->second;
        }
        if (g_disk_handles.size() >= FF_VOLUMES) {
            return -EOVERFLOW;
        }
        auto diskm = g_disk_mm.lock();
        if (!diskm) {
            return -EIO;
        }
        const auto hwnd = g_disk_handles.insert(diskh);
        g_registered_vol.emplace(std::make_pair(diskh->name(), hwnd));
        return hwnd;
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
        DSTATUS disk_initialize(BYTE pdrv)
        {
            return disk_handle_from_pdrive(pdrv) ? (0) : (STA_NOINIT);
        }

        DSTATUS disk_status(BYTE pdrv)
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
                return 0;
            case blkdev::media_status::error:
                return STA_NOINIT;
            case blkdev::media_status::nomedia:
                return STA_NODISK;
            case blkdev::media_status::uninit:
                return STA_NOINIT;
            case blkdev::media_status::wprotect:
                return RES_WRPRT;
            }
            return STA_NOINIT;
        }

        DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
        {
            const auto diskh = disk_handle_from_pdrive(pdrv);
            if (!diskh) {
                return RES_NOTRDY;
            }
            const auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return RES_NOTRDY;
            }
            const auto res = diskmm->read(diskh, buff, sector, count);
            if (res < 0) {
                LOG_ERROR("write error %i", res);
                return (res == -ERANGE) ? (RES_PARERR) : (RES_ERROR);
            }
            else {
                return RES_OK;
            }
        }

        DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
        {
            const auto diskh = disk_handle_from_pdrive(pdrv);
            if (!diskh) {
                return RES_NOTRDY;
            }
            const auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return RES_NOTRDY;
            }
            const auto res = diskmm->write(diskh, buff, sector, count);
            if (res < 0) {
                LOG_ERROR("write error %i", res);
                return (res == -ERANGE) ? (RES_PARERR) : (RES_ERROR);
            }
            else {
                return RES_OK;
            }
        }

        DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
        {
            const auto diskh = disk_handle_from_pdrive(pdrv);
            if (!diskh) {
                return RES_NOTRDY;
            }
            const auto diskmm = g_disk_mm.lock();
            if (!diskmm) {
                return RES_NOTRDY;
            }
            blkdev::scount_t ret;
            switch (cmd) {
            case CTRL_SYNC:
                ret = diskmm->sync(diskh);
                break;
            case GET_SECTOR_COUNT:
                ret = diskmm->get_info(diskh, blkdev::info_type::sector_count);
                if (ret >= 0) {
                    if (!buff) {
                        ret = -EINVAL;
                    }
                    else {
                        *reinterpret_cast<LBA_t *>(buff) = ret;
                        ret                              = 0;
                    }
                }
                break;
            case GET_SECTOR_SIZE:
                ret = diskmm->get_info(diskh, blkdev::info_type::sector_size);
                if (ret >= 0) {
                    if (!buff) {
                        ret = -EINVAL;
                    }
                    else {
                        *reinterpret_cast<WORD *>(buff) = ret;
                    }
                }
                break;
            case GET_BLOCK_SIZE:
                ret = diskmm->get_info(diskh, blkdev::info_type::erase_block);
                if (ret >= 0) {
                    if (!buff) {
                        ret = -EINVAL;
                    }
                    else {
                        *reinterpret_cast<DWORD *>(buff) = ret;
                    }
                }
                break;
            case CTRL_TRIM:
                if (!buff) {
                    ret = -EINVAL;
                }
                else {
                    auto lba             = reinterpret_cast<LBA_t *>(buff);
                    const auto lba_start = lba[0];
                    const auto lba_end   = lba[1];
                    // Only when trim is supported call method
                    ret = diskmm->get_info(diskh, blkdev::info_type::erase_block);
                    if (ret > 0) {
                        ret = diskmm->erase(diskh, lba_start, lba_end - lba_start + 1);
                    }
                    else {
                        ret = 0;
                    }
                }
                break;
            default:
                ret = -EINVAL;
                break;
            }
            // Finally translate error
            switch (ret) {
            case 0:
                return RES_OK;
            case -EINVAL:
                return RES_PARERR;
            default:
                return RES_ERROR;
            }
        }
    }

} // namespace purefs::fs::drivers::ffat::internal
