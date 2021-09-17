// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <lwext4/ext4_bdev.hpp>
#include <ext4_config.h>
#include <ext4_blockdev.h>
#include <ext4_errno.h>
#include <unordered_map>
#include <log.hpp>
#include <mutex.hpp>
#include <cstring>
#include <purefs/blkdev/disk_manager.hpp>

namespace purefs::fs::drivers::ext4::internal
{
    namespace
    {
        //! Global structure for single mount
        struct ext4_config
        {
            ::ext4_blockdev bdev;
            std::unique_ptr<uint8_t[]> buf;
            ::ext4_blockdev_iface ifc;
        };

        // Global volumes list
        std::unordered_map<::ext4_blockdev *, std::unique_ptr<ext4_config>> g_volumes;
        // Global lock for recursive mutex
        cpp_freertos::MutexRecursive g_lock;

        namespace io
        {
            //! Global io context for the partition
            struct context
            {
                context(std::shared_ptr<blkdev::disk_manager> diskmm, blkdev::disk_fd _disk_h)
                    : disk(diskmm), disk_h(_disk_h)
                {}
                const std::weak_ptr<blkdev::disk_manager> disk;
                const blkdev::disk_fd disk_h;
                mutable cpp_freertos::MutexRecursive mutex;
            };

            int write(struct ext4_blockdev *bdev, const void *buf, uint64_t blk_id, uint32_t blk_cnt)
            {
                auto ctx = reinterpret_cast<context *>(bdev->bdif->p_user);
                if (!ctx) {
                    return -EIO;
                }
                auto diskmm = ctx->disk.lock();
                if (!diskmm) {
                    return -EIO;
                }
                const auto err = diskmm->write(ctx->disk_h, buf, blk_id, blk_cnt);
                if (err) {
                    LOG_ERROR("Sector write error errno: %i", err);
                }
                return err;
            }
            int read(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id, uint32_t blk_cnt)
            {
                auto ctx = reinterpret_cast<context *>(bdev->bdif->p_user);
                if (!ctx) {
                    return -EIO;
                }
                auto diskmm = ctx->disk.lock();
                if (!diskmm) {
                    return -EIO;
                }
                const auto err = diskmm->read(ctx->disk_h, buf, blk_id, blk_cnt);
                if (err) {
                    LOG_ERROR("Sector write error errno: %i", err);
                }
                return err;
            }

            int open(struct ext4_blockdev *bdev)
            {
                return 0;
            }
            int close(struct ext4_blockdev *bdev)
            {
                return 0;
            }

        } // namespace io

    } // namespace

    // Append volume
    std::pair<ext4_blockdev *, int> append_volume(std::shared_ptr<blkdev::disk_manager> diskmm, blkdev::disk_fd diskh)
    {
        const auto sect_size = diskmm->get_info(diskh, blkdev::info_type::sector_size);
        if (sect_size < 0) {
            LOG_ERROR("Unable to get sector size %li", long(sect_size));
            return {nullptr, sect_size};
        }
        const auto sect_count = diskmm->get_info(diskh, blkdev::info_type::sector_count);
        if (sect_count < 0) {
            LOG_ERROR("Unable to get sector count %li", long(sect_size));
            return {nullptr, sect_count};
        }

        // Insert into the container
        auto cfg              = std::make_unique<ext4_config>();
        std::memset(cfg.get(), 0, sizeof(ext4_config));
        cfg->ifc.open         = io::open;
        cfg->ifc.bread        = io::read;
        cfg->ifc.bwrite       = io::write;
        cfg->ifc.close        = io::close;
        cfg->buf              = std::make_unique<uint8_t[]>(sect_size);
        cfg->ifc.ph_bbuf      = cfg->buf.get();
        cfg->ifc.ph_bcnt      = sect_count;
        cfg->ifc.ph_bsize     = sect_size;
        cfg->ifc.p_user       = new io::context(diskmm, diskh);
        cfg->bdev.bdif        = &cfg->ifc;
        cfg->bdev.part_offset = 0;
        cfg->bdev.part_size   = uint64_t(sect_size) * sect_count;

        cpp_freertos::LockGuard _lck(g_lock);
        g_volumes.insert(std::make_pair(&cfg->bdev, std::move(cfg)));
        return {&cfg->bdev, {}};
    }

    // Remove volume
    int remove_volume(ext4_blockdev *ext4_block)
    {
        cpp_freertos::LockGuard _lck(g_lock);
        auto vol = g_volumes.find(ext4_block);
        if (vol == std::end(g_volumes)) {
            LOG_ERROR("Unable to find LFS ");
            return -ENXIO;
        }
        delete reinterpret_cast<io::context *>(vol->second->ifc.p_user);
        g_volumes.erase(vol);
        return 0;
    }

} // namespace purefs::fs::drivers::ext4::internal