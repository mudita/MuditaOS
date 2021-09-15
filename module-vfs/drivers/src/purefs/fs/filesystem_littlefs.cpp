// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <purefs/fs/drivers/mount_point_littlefs.hpp>
#include <littlefs/volume_mapper.hpp>
#include <purefs/fs/drivers/file_handle_littlefs.hpp>
#include <purefs/fs/drivers/directory_handle_littlefs.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/fs/mount_flags.hpp>
#include <lfs.h>
#include <log.hpp>

#include <climits>
#include <syslimits.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <algorithm>
#include <sys/stat.h>

namespace
{
    // NOTE: lfs block size is configured during format
    static constexpr auto c_lfs_block_size = 32U * 1024U;

    template <typename T> auto lfs_to_errno(T error) -> T
    {
        if (error >= 0) {
            return error;
        }
        switch (error) {
        default:
        case LFS_ERR_IO: /* Error during device operation */
            return -EIO;
        case LFS_ERR_CORRUPT: /* Corrupted */
            return -EFAULT;
        case LFS_ERR_NOENT: /* No directory entry */
            return -ENOENT;
        case LFS_ERR_EXIST: /* Entry already exists */
            return -EEXIST;
        case LFS_ERR_NOTDIR: /* Entry is not a dir */
            return -ENOTDIR;
        case LFS_ERR_ISDIR: /* Entry is a dir */
            return -EISDIR;
        case LFS_ERR_NOTEMPTY: /* Dir is not empty */
            return -ENOTEMPTY;
        case LFS_ERR_BADF: /* Bad file number */
            return -EBADF;
        case LFS_ERR_FBIG: /* File too large */
            return -EFBIG;
        case LFS_ERR_INVAL: /* Invalid parameter */
            return -EINVAL;
        case LFS_ERR_NOSPC: /* No space left on device */
            return -ENOSPC;
        case LFS_ERR_NOMEM: /* No more memory available */
            return -ENOMEM;
        }
    }
    int translate_flags(unsigned flags)
    {
        int lfs_mode = 0;
        switch (flags & O_ACCMODE) {
        case O_RDONLY:
            lfs_mode |= LFS_O_RDONLY;
            break;
        case O_WRONLY:
            lfs_mode |= LFS_O_WRONLY;
            break;
        case O_RDWR:
            lfs_mode |= LFS_O_RDWR;
            break;
        }
        if (flags & O_APPEND) {
            lfs_mode |= LFS_O_APPEND;
        }
        if (flags & O_CREAT) {
            lfs_mode |= LFS_O_CREAT;
        }
        if (flags & O_TRUNC) {
            lfs_mode |= LFS_O_TRUNC;
        }
        if (flags & O_EXCL) {
            lfs_mode |= LFS_O_EXCL;
        }
        return lfs_mode;
    }

    auto translate_attrib_to_st_mode(uint8_t type, bool readOnly)
    {
        decltype(std::declval<struct stat *>()->st_mode) mode = S_IRUSR | S_IRGRP | S_IROTH;
        if (!readOnly) {
            mode |= S_IWUSR | S_IWGRP | S_IWOTH;
        }
        if (type == LFS_TYPE_REG) {
            mode |= S_IFREG;
        }
        else if (type == LFS_TYPE_DIR) {
            mode |= (S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH);
        }
        return mode;
    }

    void translate_lfsinfo_to_stat(const ::lfs_info &fs, const lfs_config &cfg, bool readOnly, struct stat &st)
    {
        std::memset(&st, 0, sizeof st);
        st.st_nlink   = 1;
        st.st_size    = fs.size;
        st.st_blksize = cfg.block_size;
        st.st_blocks  = fs.size / cfg.block_count;
        st.st_mode    = translate_attrib_to_st_mode(fs.type, readOnly);
    }

    [[gnu::nonnull(1)]] int setup_lfs_config(lfs_config *cfg,
                                             size_t sector_size,
                                             size_t part_sectors_count,
                                             const void *data)
    {
        if (data) {
            // NOTE: block size from mount param
            cfg->block_size = *(reinterpret_cast<const uint32_t *>(data));
        }
        else {
            cfg->block_size = c_lfs_block_size;
            LOG_WARN("LFS: mount block size not specified using default value");
        }
        cfg->block_cycles    = 512;
        cfg->block_count     = 0; // Read later from super block
        const auto total_siz = uint64_t(sector_size) * uint64_t(part_sectors_count);
        if (total_siz % cfg->block_size) {
            LOG_ERROR("Block size doesn't match partition size");
            return -ERANGE;
        }
        cfg->block_count = total_siz / cfg->block_size - 1;
        cfg->lookahead_size = std::min<lfs_size_t>(131072U, ((cfg->block_count >> 3U) + 1U) << 3U);
        cfg->read_size  = cfg->block_size;
        cfg->prog_size  = cfg->block_size;
        cfg->cache_size = cfg->block_size;
        LOG_INFO("LFS: block count %u block size %u", unsigned(cfg->block_count), unsigned(cfg->block_size));
        return 0;
    }

} // namespace

namespace purefs::fs::drivers
{
    namespace
    {
        template <typename T, typename... Args>
        auto invoke_lfs(filesystem_littlefs::fsfile zfil, T lfs_fun, Args &&... args)
            -> decltype(lfs_fun(nullptr, nullptr, std::forward<Args>(args)...))
        {
            auto vfile = std::dynamic_pointer_cast<file_handle_littlefs>(zfil);
            if (!vfile) {
                LOG_ERROR("Non LITTLEFS filesystem file pointer");
                return -EBADF;
            }
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(vfile->mntpoint());
            if (!mntp) {
                LOG_ERROR("Non LITTLEFS mount point");
                return -EBADF;
            }
            auto lerr = lfs_fun(mntp->lfs_mount(), vfile->lfs_filp(), std::forward<Args>(args)...);
            return lfs_to_errno(lerr);
        }
        template <typename T, typename... Args>
        auto invoke_lfs(filesystem_littlefs::fsdir zdir, T lfs_fun, Args &&... args)
            -> decltype(lfs_fun(nullptr, nullptr, std::forward<Args>(args)...))
        {
            auto vdir = std::dynamic_pointer_cast<directory_handle_littlefs>(zdir);
            if (!vdir) {
                LOG_ERROR("Non LITTLEFS filesystem directory pointer");
                return -EBADF;
            }
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(vdir->mntpoint());
            if (!mntp) {
                LOG_ERROR("Non LITTLEFS mount point");
                return -EBADF;
            }
            auto lerr = lfs_fun(mntp->lfs_mount(), vdir->lfs_dirp(), std::forward<Args>(args)...);
            return lfs_to_errno(lerr);
        }

        template <typename T, typename... Args>
        auto invoke_lfs(filesystem_littlefs::fsmount fmnt, T lfs_fun, Args &&... args)
            -> decltype(lfs_fun(nullptr, std::forward<Args>(args)...))
        {
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(fmnt);
            if (!mntp) {
                LOG_ERROR("Non LITTLEFS mount point");
                return -EBADF;
            }
            auto lerr = lfs_fun(mntp->lfs_mount(), std::forward<Args>(args)...);
            return lfs_to_errno(lerr);
        }

        template <typename T, typename... Args>
        auto invoke_lfs(filesystem_littlefs::fsmount fmnt, std::string_view path, T lfs_fun, Args &&... args)
            -> decltype(lfs_fun(nullptr, "", std::forward<Args>(args)...))
        {
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(fmnt);
            if (!mntp) {
                LOG_ERROR("Non LITTLEFS mount point");
                return -EBADF;
            }
            const auto native_path = mntp->native_path(path);
            auto lerr              = lfs_fun(mntp->lfs_mount(), native_path.c_str(), std::forward<Args>(args)...);
            return lfs_to_errno(lerr);
        }
    } // namespace

    auto filesystem_littlefs::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                             std::string_view path,
                                             unsigned int flags) -> fsmount
    {
        return std::make_shared<mount_point_littlefs>(diskh, path, flags, shared_from_this());
    }

    auto filesystem_littlefs::mount(fsmount mnt, const void *data) noexcept -> int
    {
        auto disk = mnt->disk();
        if (!disk) {
            return -EIO;
        }
        auto vmnt = std::dynamic_pointer_cast<mount_point_littlefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non LITTLEFS mount point");
            return -EIO;
        }
        auto err = littlefs::internal::append_volume(vmnt->lfs_config(), disk_mngr(), disk);
        if (err) {
            return lfs_to_errno(err);
        }
        {
            auto diskmm = disk_mngr();
            const auto ssize = diskmm->get_info(disk, blkdev::info_type::sector_size);
            if (ssize < 0) {
                LOG_ERROR("Unable to read sector size %i", int(ssize));
                err = ssize;
            }
            else {
                auto sect_count = diskmm->get_info(disk, blkdev::info_type::sector_count);
                if (sect_count > 0) {
                    err = setup_lfs_config(vmnt->lfs_config(), ssize, sect_count, data);
                }
                else {
                    LOG_ERROR("Unable to read sector count %i", int(sect_count));
                    err = sect_count;
                }
            }
        }
        if (!err) {
            err = lfs_mount(vmnt->lfs_mount(), vmnt->lfs_config());
        }
        else {
            LOG_ERROR("LFS mount error %i", err);
        }
        if (!err) {
            filesystem_operations::mount(mnt, data);
        }
        else {
            littlefs::internal::remove_volume(vmnt->lfs_config());
        }
        return lfs_to_errno(err);
    }

    auto filesystem_littlefs::umount(fsmount mnt) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_littlefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non LITTLEFS mount point");
            return -EIO;
        }
        auto lerr = lfs_unmount(vmnt->lfs_mount());
        if (!lerr) {
            littlefs::internal::remove_volume(vmnt->lfs_config());
            filesystem_operations::umount(mnt);
        }
        else {
            LOG_ERROR("LFS unable umount %i", lerr);
        }
        return lfs_to_errno(lerr);
    }

    auto filesystem_littlefs::stat_vfs(fsmount mnt, std::string_view path, struct statvfs &stat) const noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_littlefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non LITTLEFS mount point");
            return -EIO;
        }
        auto lerr = lfs_fs_size(vmnt->lfs_mount());
        if (lerr >= 0) {
            const auto cfg = vmnt->lfs_config();
            std::memset(&stat, 0, sizeof stat);
            stat.f_blocks  = cfg->block_count;
            stat.f_bfree   = stat.f_blocks - lerr;
            stat.f_bavail  = stat.f_bfree;
            stat.f_bsize   = cfg->prog_size;
            stat.f_frsize  = cfg->block_size;
            stat.f_namemax = PATH_MAX;
            stat.f_flag    = vmnt->flags();
            lerr           = 0;
        }
        return lfs_to_errno(lerr);
    }

    auto filesystem_littlefs::open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_littlefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non LITTLEFS mount point");
            return nullptr;
        }
        const auto fspath = vmnt->native_path(path);
        const auto fsflag = translate_flags(flags);
        auto filep        = std::make_shared<file_handle_littlefs>(mnt, fspath, flags);
        auto lerr         = lfs_file_open(vmnt->lfs_mount(), filep->lfs_filp(), fspath.c_str(), fsflag);
        filep->error(lfs_to_errno(lerr));
        return filep;
    }

    auto filesystem_littlefs::close(fsfile zfile) noexcept -> int
    {
        return invoke_lfs(zfile, ::lfs_file_close);
    }

    auto filesystem_littlefs::write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t
    {
        return invoke_lfs(zfile, ::lfs_file_write, ptr, len);
    }

    auto filesystem_littlefs::read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t
    {
        return invoke_lfs(zfile, ::lfs_file_read, ptr, len);
    }

    auto filesystem_littlefs::seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t
    {
        return invoke_lfs(zfile, ::lfs_file_seek, pos, dir);
    }

    auto filesystem_littlefs::fstat(fsfile zfile, struct stat &st) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_littlefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non LITTLEFS filesystem file pointer");
            return -EBADF;
        }
        ::lfs_info linfo;
        const auto path = vfile->open_path();
        const auto err  = invoke_lfs(zfile->mntpoint(), ::lfs_stat, path.c_str(), &linfo);
        if (!err) {
            auto vmnt = std::static_pointer_cast<mount_point_littlefs>(vfile->mntpoint());
            translate_lfsinfo_to_stat(linfo, *vmnt->lfs_config(), vmnt->is_ro(), st);
        }
        return err;
    }

    auto filesystem_littlefs::stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int
    {
        ::lfs_info linfo;
        const auto err = invoke_lfs(mnt, file, ::lfs_stat, &linfo);
        if (!err) {
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(mnt);
            translate_lfsinfo_to_stat(linfo, *mntp->lfs_config(), mntp->is_ro(), st);
        }
        return err;
    }

    auto filesystem_littlefs::unlink(fsmount mnt, std::string_view name) noexcept -> int
    {
        return invoke_lfs(mnt, name, ::lfs_remove);
    }

    auto filesystem_littlefs::rmdir(fsmount mnt, std::string_view name) noexcept -> int
    {
        return invoke_lfs(mnt, name, ::lfs_remove);
    }

    auto filesystem_littlefs::rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int
    {
        auto mntp = std::static_pointer_cast<mount_point_littlefs>(mnt);
        if (!mntp) {
            LOG_ERROR("Non LITTLEFS mount point");
            return -EBADF;
        }
        const auto native_old = mntp->native_path(oldname);
        const auto native_new = mntp->native_path(newname);
        auto lerr             = lfs_rename(mntp->lfs_mount(), native_old.c_str(), native_new.c_str());
        return lfs_to_errno(lerr);
    }

    auto filesystem_littlefs::ftruncate(fsfile zfile, off_t len) noexcept -> int
    {
        return invoke_lfs(zfile, ::lfs_file_truncate, len);
    }

    auto filesystem_littlefs::fsync(fsfile zfile) noexcept -> int
    {
        return invoke_lfs(zfile, ::lfs_file_sync);
    }

    auto filesystem_littlefs::isatty(fsfile zfile) noexcept -> int
    {
        // NOTE: Handle littlefs is always not a tty
        return 0;
    }

    auto filesystem_littlefs::mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int
    {
        return invoke_lfs(mnt, path, ::lfs_mkdir);
    }

    auto filesystem_littlefs::diropen(fsmount mnt, std::string_view path) noexcept -> fsdir
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_littlefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non LFS mount point");
            return nullptr;
        }
        const auto fspath = vmnt->native_path(path);
        const auto dirp   = std::make_shared<directory_handle_littlefs>(mnt, 0);
        const auto lret   = lfs_dir_open(vmnt->lfs_mount(), dirp->lfs_dirp(), fspath.c_str());
        dirp->error(lfs_to_errno(lret));
        return dirp;
    }

    auto filesystem_littlefs::dirreset(fsdir dirstate) noexcept -> int
    {
        return invoke_lfs(dirstate, ::lfs_dir_rewind);
    }

    auto filesystem_littlefs::dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) -> int
    {
        lfs_info linfo;
        int err = invoke_lfs(dirstate, ::lfs_dir_read, &linfo);
        if (err == 1) {
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(dirstate->mntpoint());
            translate_lfsinfo_to_stat(linfo, *mntp->lfs_config(), mntp->is_ro(), filestat);
            filename = linfo.name;
            err      = 0;
        }
        else if (err == 0) {
            err = -ENODATA;
        }
        return err;
    }

    auto filesystem_littlefs::dirclose(fsdir dirstate) noexcept -> int
    {
        return invoke_lfs(dirstate, ::lfs_dir_close);
    }

    auto filesystem_littlefs::fchmod(fsfile zfile, mode_t mode) noexcept -> int
    {
        // Littlefs doesn't support chmod() so we need that path exists
        struct stat filestat;
        return fstat(zfile, filestat);
    }

} // namespace purefs::fs::drivers
