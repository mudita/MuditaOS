// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <purefs/fs/drivers/mount_point_littlefs.hpp>
#include <littlefs/volume_mapper.hpp>
#include <purefs/fs/drivers/file_handle_littlefs.hpp>
#include <lfs.h>
#include <log/log.hpp>

#include <sys/statvfs.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>

/** NOTE: LITTLEFS is not thread safe like ELM FAT, so it will require global fs mutex lock :(
 */

namespace
{
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
        if (flags & O_APPEND)
            lfs_mode |= LFS_O_APPEND;
        if (flags & O_CREAT)
            lfs_mode |= LFS_O_CREAT;
        if (flags & O_TRUNC)
            lfs_mode |= LFS_O_TRUNC;
        if (flags & O_EXCL)
            lfs_mode |= LFS_O_EXCL;
        return lfs_mode;
    }

    auto translate_attrib_to_st_mode(uint8_t type)
    {

        decltype(static_cast<struct stat *>(nullptr)->st_mode) mode =
            S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;
        if (type == LFS_TYPE_REG) {
            mode |= (S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH);
        }
        else if (type == LFS_TYPE_DIR) {
            mode |= S_IFREG;
        }
        return mode;
    }

    void translate_lfsinfo_to_stat(const ::lfs_info &fs, const lfs_config &cfg, struct stat &st)
    {
        std::memset(&st, 0, sizeof st);
        st.st_nlink   = 1;
        st.st_size    = fs.size;
        st.st_blksize = cfg.block_size;
        st.st_blocks  = fs.size / cfg.block_count;
        st.st_mode    = translate_attrib_to_st_mode(fs.type);
    }

    void setup_lfs_config(lfs_config *cfg)
    {
        // TODO: LFS config default parameters
    }
} // namespace

namespace purefs::fs::drivers
{
    namespace
    {
        template <typename T, typename... Args>
        auto invoke_lfs(cpp_freertos::MutexRecursive &mtx, filesystem_littlefs::fsfile zfil, T lfs_fun, Args &&... args)
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
            decltype(lfs_fun(nullptr, nullptr, std::forward<Args>(args)...)) lerr;
            {
                cpp_freertos::LockGuard _lck(mtx);
                lerr = lfs_fun(mntp->lfs_mount(), vfile->lfs_filp(), std::forward<Args>(args)...);
            }
            return lfs_to_errno(lerr);
        }

        template <typename T, typename... Args>
        auto invoke_lfs(cpp_freertos::MutexRecursive &mtx,
                        filesystem_littlefs::fsmount fmnt,
                        T lfs_fun,
                        Args &&... args) -> decltype(lfs_fun(nullptr, std::forward<Args>(args)...))
        {
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(fmnt);
            if (!mntp) {
                LOG_ERROR("Non LITTLEFS mount point");
                return -EBADF;
            }
            decltype(lfs_fun(nullptr, std::forward<Args>(args)...)) lerr;
            {
                cpp_freertos::LockGuard _lck(mtx);
                lerr = lfs_fun(mntp->lfs_mount(), std::forward<Args>(args)...);
            }
            return lfs_to_errno(lerr);
        }

        template <typename T, typename... Args>
        auto invoke_lfs(cpp_freertos::MutexRecursive &mtx,
                        filesystem_littlefs::fsmount fmnt,
                        std::string_view path,
                        T lfs_fun,
                        Args &&... args) -> decltype(lfs_fun(nullptr, "", std::forward<Args>(args)...))
        {
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(fmnt);
            if (!mntp) {
                LOG_ERROR("Non LITTLEFS mount point");
                return -EBADF;
            }
            decltype(lfs_fun(nullptr, "", std::forward<Args>(args)...)) lerr;
            const auto native_path = mntp->native_path(path);
            {
                cpp_freertos::LockGuard _lck(mtx);
                lerr = lfs_fun(mntp->lfs_mount(), native_path.c_str(), std::forward<Args>(args)...);
            }
            return lfs_to_errno(lerr);
        }
    } // namespace

    auto filesystem_littlefs::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                             std::string_view path,
                                             unsigned int flags) -> fsmount
    {
        return std::make_shared<mount_point_littlefs>(diskh, path, flags, shared_from_this());
    }

    auto filesystem_littlefs::mount(fsmount mnt) noexcept -> int
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
        cpp_freertos::LockGuard _lck(m_lock);
        auto err = littlefs::internal::append_volume(vmnt->lfs_config(), disk_mngr(), disk);
        if (err) {
            return err;
        }
        setup_lfs_config(vmnt->lfs_config());
        err = lfs_mount(vmnt->lfs_mount(), vmnt->lfs_config());
        if (err) {
            LOG_ERROR("LFS mount error %i", err);
        }
        if (!err) {
            filesystem_operations::mount(mnt);
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
        int lerr;
        {
            cpp_freertos::LockGuard _lck(m_lock);
            lerr = lfs_unmount(vmnt->lfs_mount());
        }
        if (!lerr) {
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
        const auto cfg = vmnt->lfs_config();
        stat.f_bsize   = cfg->prog_size;
        stat.f_frsize  = cfg->block_size;
        stat.f_blocks  = cfg->block_count;
        int lerr;
        {
            cpp_freertos::LockGuard _lck(m_lock);
            lerr = lfs_fs_size(vmnt->lfs_mount());
        }
        if (lerr >= 0) {
            stat.f_bfree = stat.f_blocks - lerr;
            lerr         = 0;
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
        const auto fsflag = translate_flags(mode);
        auto filep        = std::make_shared<file_handle_littlefs>(mnt, fspath, flags);
        int lerr;
        {
            cpp_freertos::LockGuard _lck(m_lock);
            lerr = lfs_file_open(vmnt->lfs_mount(), filep->lfs_filp(), fspath.c_str(), fsflag);
        }
        filep->error(lfs_to_errno(lerr));
        return filep;
    }

    auto filesystem_littlefs::close(fsfile zfile) noexcept -> int
    {
        return invoke_lfs(m_lock, zfile, ::lfs_file_close);
    }

    auto filesystem_littlefs::write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t
    {
        return invoke_lfs(m_lock, zfile, ::lfs_file_write, ptr, len);
    }

    auto filesystem_littlefs::read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t
    {
        return invoke_lfs(m_lock, zfile, ::lfs_file_read, ptr, len);
    }

    auto filesystem_littlefs::seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t
    {
        return invoke_lfs(m_lock, zfile, ::lfs_file_seek, pos, dir);
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
        const auto err  = invoke_lfs(m_lock, zfile->mntpoint(), ::lfs_stat, path.c_str(), &linfo);
        if (!err) {
            auto vmnt = std::static_pointer_cast<mount_point_littlefs>(vfile->mntpoint());
            translate_lfsinfo_to_stat(linfo, *vmnt->lfs_config(), st);
        }
        return err;
    }

    auto filesystem_littlefs::stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int
    {
        ::lfs_info linfo;
        const auto err = invoke_lfs(m_lock, mnt, file, ::lfs_stat, &linfo);
        if (!err) {
            auto mntp = std::static_pointer_cast<mount_point_littlefs>(mnt);
            translate_lfsinfo_to_stat(linfo, *mntp->lfs_config(), st);
        }
        return err;
    }

    auto filesystem_littlefs::unlink(fsmount mnt, std::string_view name) noexcept -> int
    {
        return invoke_lfs(m_lock, mnt, name, ::lfs_remove);
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
        int lerr;
        {
            cpp_freertos::LockGuard _lck(m_lock);
            lerr = lfs_rename(mntp->lfs_mount(), native_old.c_str(), native_new.c_str());
        }
        return lfs_to_errno(lerr);
    }

    auto filesystem_littlefs::ftruncate(fsfile zfile, off_t len) noexcept -> int
    {
        return invoke_lfs(m_lock, zfile, ::lfs_file_truncate, len);
    }

    auto filesystem_littlefs::fsync(fsfile zfile) noexcept -> int
    {
        return invoke_lfs(m_lock, zfile, ::lfs_file_sync);
    }

    auto filesystem_littlefs::isatty(fsfile zfile) noexcept -> int
    {
        // NOTE: Handle littlefs is always not a tty
        return 0;
    }

    auto filesystem_littlefs::mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int
    {
        return invoke_lfs(m_lock, mnt, path, ::lfs_mkdir);
    }

} // namespace purefs::fs::drivers
