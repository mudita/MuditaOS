// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <littlefs/volume_mapper.hpp>
#include <lfs.h>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <log/log.hpp>

namespace purefs::fs::drivers::littlefs::internal
{

    // LFS io API
    namespace
    {
        namespace lfs_io
        {

            class io_context
            {
              public:
                io_context(std::shared_ptr<blkdev::disk_manager> diskmm, blkdev::disk_fd diskh, size_t _sector_size)
                    : disk(diskmm), disk_h(diskh), sector_size(_sector_size)
                {}
                const std::weak_ptr<blkdev::disk_manager> disk;
                const blkdev::disk_fd disk_h;
                const size_t sector_size;
            };

            int read(const struct lfs_config *lfsc, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
            {
                auto ctx = reinterpret_cast<io_context *>(lfsc->context);
                if (!ctx) {
                    return LFS_ERR_IO;
                }
                auto diskmm = ctx->disk.lock();
                if (!diskmm) {
                    return LFS_ERR_IO;
                }
                const auto lba = (uint64_t(lfsc->block_size) * block + off) / ctx->sector_size;
                if (off % ctx->sector_size) {
                    LOG_ERROR("Partial offset not supported");
                    return LFS_ERR_IO;
                }
                const std::size_t lba_sz = (size * lfsc->block_size) / ctx->sector_size;
                const auto err           = diskmm->read(ctx->disk_h, buffer, lba, lba_sz);
                if (err) {
                    LOG_ERROR("Sector read error %i", err);
                }
                return err;
            }

            int prog(
                const struct lfs_config *lfsc, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
            {
                auto ctx = reinterpret_cast<io_context *>(lfsc->context);
                if (!ctx) {
                    return LFS_ERR_IO;
                }
                auto diskmm = ctx->disk.lock();
                if (!diskmm) {
                    return LFS_ERR_IO;
                }
                const auto lba = (uint64_t(lfsc->block_size) * block + off) / ctx->sector_size;
                if (off % ctx->sector_size) {
                    LOG_ERROR("Partial offset not supported");
                    return LFS_ERR_IO;
                }
                const std::size_t lba_sz = (size * lfsc->block_size) / ctx->sector_size;
                const auto err           = diskmm->write(ctx->disk_h, buffer, lba, lba_sz);
                if (err) {
                    LOG_ERROR("Sector read error %i", err);
                }
                return err;
            }

            int erase(const struct lfs_config *lfsc, lfs_block_t block)
            {
                auto ctx = reinterpret_cast<io_context *>(lfsc->context);
                if (!ctx) {
                    return LFS_ERR_IO;
                }
                auto diskmm = ctx->disk.lock();
                if (!diskmm) {
                    return LFS_ERR_IO;
                }
                const auto lba    = (uint64_t(lfsc->block_size) * block) / ctx->sector_size;
                const auto lba_sz = lfsc->block_size / ctx->sector_size;
                int err           = diskmm->erase(ctx->disk_h, lba, lba_sz);
                if (err) {
                    LOG_ERROR("Unable to erase area ret: %i", err);
                }
                return err;
            }

            int sync(const struct lfs_config *lfsc)
            {
                auto ctx = reinterpret_cast<io_context *>(lfsc->context);
                if (!ctx) {
                    return LFS_ERR_IO;
                }
                auto diskmm = ctx->disk.lock();
                if (!diskmm) {
                    return LFS_ERR_IO;
                }
                int err = diskmm->sync(ctx->disk_h);
                if (err) {
                    LOG_ERROR("Unable to sync %i", err);
                }
                return err;
            }
        } // namespace lfs_io
    }     // namespace

    int append_volume(lfs_config &lfsc, std::shared_ptr<blkdev::disk_manager> diskmm, blkdev::disk_fd diskh)
    {
        const auto sect_size = diskmm->get_info(diskh, blkdev::info_type::sector_size);
        if (sect_size < 0) {
            LOG_ERROR("Unable to get sector size %li", sect_size);
            return sect_size;
        }
        if (sect_size % lfsc.block_size || lfsc.block_size < sect_size) {
            LOG_ERROR("Sector size doesn't match block size");
            return -ERANGE;
        }
        auto ctx     = new lfs_io::io_context(diskmm, diskh, sect_size);
        lfsc.context = ctx;
        lfsc.read    = lfs_io::read;
        lfsc.prog    = lfs_io::prog;
        lfsc.erase   = lfs_io::erase;
        lfsc.sync    = lfs_io::sync;
        lfsc.context = ctx;
        return 0;
    }

    void remove_volume(lfs_config &lfsc)
    {
        if (lfsc.context) {
            delete reinterpret_cast<lfs_io::io_context *>(lfsc.context);
        }
    }

} // namespace purefs::fs::drivers::littlefs::internal
