// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/fs/filesystem_operations.hpp>
#include <purefs/fs/directory_handle.hpp>

#include <errno.h>

namespace purefs::fs
{

    auto filesystem_operations::mount(fsmount mnt, const void *data) noexcept -> int
    {
        ++m_mount_count;
        return -ENOTSUP;
    }
    auto filesystem_operations::umount(fsmount mnt) noexcept -> int
    {
        if (m_mount_count > 0) {
            --m_mount_count;
        }
        return -ENOTSUP;
    }
    auto filesystem_operations::stat_vfs(fsmount mntp, std::string_view path, statvfs &stat) const noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::fstat(fsfile zfile, struct stat &st) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::link(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::symlink(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept
        -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::unlink(fsmount mnt, std::string_view name) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::rmdir(fsmount mnt, std::string_view name) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::diropen(fsmount mnt, std::string_view path) noexcept -> fsdir
    {
        return std::make_shared<internal::directory_handle>(nullptr, -ENOTSUP);
    }
    auto filesystem_operations::dirreset(fsdir dirstate) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::dirclose(fsdir dirstate) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::ftruncate(fsfile zfile, off_t len) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::fsync(fsfile zfile) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::ioctl(fsmount mnt, std::string_view path, int cmd, void *arg) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::utimens(fsmount mnt, std::string_view path, std::array<timespec, 2> &tv) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::flock(fsfile zfile, int cmd) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_operations::fchmod(fsfile zfile, mode_t mode) noexcept -> int
    {
        return -ENOTSUP;
    }
} // namespace purefs::fs
