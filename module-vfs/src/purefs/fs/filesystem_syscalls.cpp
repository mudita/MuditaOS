// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/fs/filesystem.hpp>
#include <errno.h>
#include <log/log.hpp>
#include <purefs/fs/filesystem_operations.hpp>
#include <purefs/fs/file_handle.hpp>
#include <purefs/fs/directory_handle.hpp>
#include <purefs/fs/thread_local_cwd.hpp>
#include <fcntl.h>

namespace purefs::fs
{
    auto filesystem::stat_vfs(std::string_view path, struct ::statvfs &stat) const noexcept -> int
    {
        return invoke_fops(iaccess::ro, &filesystem_operations::stat_vfs, path, stat);
    }

    auto filesystem::stat(std::string_view file, struct stat &st) noexcept -> int
    {
        return invoke_fops(iaccess::ro, &filesystem_operations::stat, file, st);
    }

    auto filesystem::unlink(std::string_view name) noexcept -> int
    {
        return invoke_fops(iaccess::rw, &filesystem_operations::unlink, name);
    }

    auto filesystem::mkdir(std::string_view path, int mode) noexcept -> int
    {
        return invoke_fops(iaccess::rw, &filesystem_operations::mkdir, path, mode);
    }

    auto filesystem::ioctl(std::string_view path, int cmd, void *arg) noexcept -> int
    {
        return invoke_fops(iaccess::ro, &filesystem_operations::ioctl, path, cmd, arg);
    }

    auto filesystem::utimens(std::string_view path, std::array<timespec, 2> &tv) noexcept -> int
    {
        return invoke_fops(iaccess::ro, &filesystem_operations::utimens, path, tv);
    }

    auto filesystem::flock(int fd, int cmd) noexcept -> int
    {
        return invoke_fops(&filesystem_operations::flock, fd, cmd);
    }

    auto filesystem::isatty(int fd) noexcept -> int
    {
        return invoke_fops(&filesystem_operations::isatty, fd);
    }

    auto filesystem::chmod(std::string_view path, mode_t mode) noexcept -> int
    {
        return invoke_fops(iaccess::rw, &filesystem_operations::chmod, path, mode);
    }

    auto filesystem::write(int fd, const char *ptr, size_t len) noexcept -> ssize_t
    {
        return invoke_fops(&filesystem_operations::write, fd, ptr, len);
    }

    auto filesystem::read(int fd, char *ptr, size_t len) noexcept -> ssize_t
    {
        return invoke_fops(&filesystem_operations::read, fd, ptr, len);
    }

    auto filesystem::seek(int fd, off_t pos, int dir) noexcept -> off_t
    {
        return invoke_fops(&filesystem_operations::seek, fd, pos, dir);
    }

    auto filesystem::fstat(int fd, struct stat &st) noexcept -> int
    {
        return invoke_fops(&filesystem_operations::fstat, fd, st);
    }

    auto filesystem::ftruncate(int fd, off_t len) noexcept -> int
    {
        return invoke_fops(&filesystem_operations::ftruncate, fd, len);
    }

    auto filesystem::fsync(int fd) noexcept -> int
    {
        return invoke_fops(&filesystem_operations::fsync, fd);
    }

    auto filesystem::fchmod(int fd, mode_t mode) noexcept -> int
    {
        return invoke_fops(&filesystem_operations::fchmod, fd, mode);
    }

    auto filesystem::symlink(std::string_view existing, std::string_view newlink) noexcept -> int
    {
        return invoke_fops_same_mp(&filesystem_operations::symlink, existing, newlink);
    }

    auto filesystem::link(std::string_view existing, std::string_view newlink) noexcept -> int
    {
        return invoke_fops_same_mp(&filesystem_operations::link, existing, newlink);
    }

    auto filesystem::rename(std::string_view oldname, std::string_view newname) noexcept -> int
    {
        return invoke_fops_same_mp(&filesystem_operations::rename, oldname, newname);
    }

    auto filesystem::open(std::string_view path, int flags, int mode) noexcept -> int
    {
        const auto abspath     = absolute_path(path);
        auto [mountp, pathpos] = find_mount_point(abspath);
        if (!mountp) {
            LOG_ERROR("VFS: Unable to find mount point: %s", std::string(path).c_str());
            return -ENOENT;
        }
        auto fsops = mountp->fs_ops();
        if (fsops) {
            if ((flags & O_ACCMODE) != O_RDONLY && (mountp->flags() & mount_flags::read_only)) {
                LOG_ERROR("Trying to open file %.*s with WR... flag on RO filesystem", int(path.size()), path.data());
                return -EACCES;
            }
            auto fh = fsops->open(mountp, abspath, flags, mode);
            if (!fh) {
                LOG_ERROR("VFS: Unable to get fops");
                return -EBADF;
            }
            const auto err = fh->error();
            if (err) {
                return err;
            }
            return add_filehandle(fh);
        }
        else {
            LOG_ERROR("VFS: Unable to lock fops");
            return -EIO;
        }
    }

    auto filesystem::close(int fd) noexcept -> int
    {
        auto ret = invoke_fops(&filesystem_operations::close, fd);
        if (!ret) {
            ret = (remove_filehandle(fd)) ? (0) : (-EBADF);
        }
        return ret;
    }

    auto filesystem::diropen(std::string_view path) noexcept -> fsdir
    {
        const auto abspath     = absolute_path(path);
        auto [mountp, pathpos] = find_mount_point(abspath);
        if (!mountp) {
            LOG_ERROR("VFS: Unable to find mount point: %s", std::string(path).c_str());
            return std::make_shared<internal::directory_handle>(nullptr, -ENOENT);
        }
        auto fsops = mountp->fs_ops();
        if (fsops) {
            auto dh = fsops->diropen(mountp, abspath);
            if (!dh) {
                LOG_ERROR("VFS: Unable to get diropen");
                return std::make_shared<internal::directory_handle>(nullptr, -ENXIO);
            }
            return dh;
        }
        else {
            LOG_ERROR("VFS: Unable to lock fops");
            return std::make_shared<internal::directory_handle>(nullptr, -EIO);
        }
    }

    auto filesystem::dirreset(fsdir dirstate) noexcept -> int
    {
        return invoke_fops(&filesystem_operations::dirreset, dirstate);
    }

    auto filesystem::dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) noexcept -> int
    {
        if (!dirstate) {
            LOG_ERROR("No directory handle");
            return -ENXIO;
        }
        return invoke_fops(&filesystem_operations::dirnext, dirstate, filename, filestat);
    }

    auto filesystem::dirclose(fsdir dirstate) noexcept -> int
    {
        if (!dirstate) {
            LOG_ERROR("No directory handle");
            return -ENXIO;
        }
        return invoke_fops(&filesystem_operations::dirclose, dirstate);
    }

} // namespace purefs::fs
