// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <littlefs/volume_mapper.hpp>
#include <lfs.h>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <log/log.hpp>
#include <mutex.hpp>
#include <errno.h>

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
                io_context(std::shared_ptr<blkdev::disk_manager> diskmm,
                           blkdev::disk_fd diskh,
                           size_t _sector_size,
                           size_t _erase_block)
                    : disk(diskmm), disk_h(diskh), sector_size(_sector_size), erase_block(_erase_block)
                {}
                const std::weak_ptr<blkdev::disk_manager> disk;
                const blkdev::disk_fd disk_h;
                const size_t sector_size;
                const size_t erase_block;
                mutable cpp_freertos::MutexRecursive mutex;
            };

            int errno_to_lfs(int error)
            {
                if (error >= 0) {
                    return LFS_ERR_OK;
                }

                switch (error) {
                default:
                case -EIO: /* Error during device operation */
                    return LFS_ERR_IO;
                case -EFAULT: /* Corrupted */
                    return LFS_ERR_CORRUPT;
                case -ENOENT: /* No directory entry */
                    return LFS_ERR_NOENT;
                case -EEXIST: /* Entry already exists */
                    return LFS_ERR_EXIST;
                case -ENOTDIR: /* Entry is not a dir */
                    return LFS_ERR_NOTDIR;
                case -EISDIR: /* Entry is a dir */
                    return LFS_ERR_ISDIR;
                case -ENOTEMPTY: /* Dir is not empty */
                    return LFS_ERR_NOTEMPTY;
                case -EBADF: /* Bad file number */
                    return LFS_ERR_BADF;
                case -EFBIG: /* File too large */
                    return LFS_ERR_FBIG;
                case -EINVAL: /* Invalid parameter */
                    return LFS_ERR_INVAL;
                case -ENOSPC: /* No space left on device */
                    return LFS_ERR_NOSPC;
                case -ENOMEM: /* No more memory available */
                    return LFS_ERR_NOMEM;
                }
            }
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
                const auto lba = (uint64_t(lfsc->block_size) * block) / ctx->sector_size;
                if (off % ctx->sector_size) {
                    LOG_ERROR("Partial offset not supported");
                    return LFS_ERR_IO;
                }
                const std::size_t lba_sz = size / ctx->sector_size;
                if (size % ctx->sector_size) {
                    LOG_ERROR("Bounary read sz error");
                    return LFS_ERR_IO;
                }
                const auto err = diskmm->read(ctx->disk_h, buffer, lba, lba_sz);
                if (err) {
                    LOG_ERROR("Sector read error %i", err);
                }
                return errno_to_lfs(err);
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
                const auto lba = (uint64_t(lfsc->block_size) * block) / ctx->sector_size;
                if (off % ctx->sector_size) {
                    LOG_ERROR("Partial offset not supported");
                    return LFS_ERR_IO;
                }
                const std::size_t lba_sz = size / ctx->sector_size;
                const auto err           = diskmm->write(ctx->disk_h, buffer, lba, lba_sz);
                if (err) {
                    LOG_ERROR("Sector read error %i", err);
                }
                return errno_to_lfs(err);
            }

            int erase(const struct lfs_config *lfsc, lfs_block_t block)
            {
                auto ctx = reinterpret_cast<io_context *>(lfsc->context);
                if (!ctx) {
                    return LFS_ERR_IO;
                }
                if (ctx->erase_block == 0) {
                    // Erase not supported
                    return LFS_ERR_OK;
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
                return errno_to_lfs(err);
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
                return errno_to_lfs(err);
            }
            int lock(const struct lfs_config *lfsc)
            {
                auto ctx = reinterpret_cast<io_context *>(lfsc->context);
                return ctx->mutex.Lock() ? LFS_ERR_OK : LFS_ERR_IO;
            }
            int unlock(const struct lfs_config *lfsc)
            {
                auto ctx = reinterpret_cast<io_context *>(lfsc->context);
                return ctx->mutex.Unlock() ? LFS_ERR_OK : LFS_ERR_IO;
            }
        } // namespace lfs_io
    }     // namespace

    int append_volume(lfs_config *lfsc, std::shared_ptr<blkdev::disk_manager> diskmm, blkdev::disk_fd diskh)
    {
        if (!lfsc) {
            return -EINVAL;
        }
        const auto sect_size = diskmm->get_info(diskh, blkdev::info_type::sector_size);
        if (sect_size < 0) {
            LOG_ERROR("Unable to get sector size %li", long(sect_size));
            return sect_size;
        }
        const auto erase_size = diskmm->get_info(diskh, blkdev::info_type::erase_block);
        if (erase_size < 0) {
            LOG_ERROR("Unable to get erase block size %li", long(erase_size));
            return erase_size;
        }
        auto ctx      = new lfs_io::io_context(diskmm, diskh, sect_size, erase_size);
        lfsc->context = ctx;
        lfsc->read    = lfs_io::read;
        lfsc->prog    = lfs_io::prog;
        lfsc->erase   = lfs_io::erase;
        lfsc->sync    = lfs_io::sync;
        lfsc->lock    = lfs_io::lock;
        lfsc->unlock  = lfs_io::unlock;
        lfsc->context = ctx;
        return 0;
    }

    void remove_volume(lfs_config *lfsc)
    {
        if (!lfsc) {
            return;
        }
        if (lfsc->context) {
            delete reinterpret_cast<lfs_io::io_context *>(lfsc->context);
        }
    }

} // namespace purefs::fs::drivers::littlefs::internal
