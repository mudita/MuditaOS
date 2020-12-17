// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/filesystem_operations.hpp>
#include <mutex.hpp>

namespace purefs::fs::drivers
{

    /** Filesystem specific driver base class */
    class filesystem_littlefs final : public filesystem_operations
    {
      public:
        using fsfile                                       = std::shared_ptr<internal::file_handle>;
        using fsdir                                        = std::shared_ptr<internal::directory_handle>;
        using fsmount                                      = std::shared_ptr<internal::mount_point>;
        filesystem_littlefs()                              = default;
        filesystem_littlefs(const filesystem_operations &) = delete;
        virtual ~filesystem_littlefs()                     = default;
        auto operator=(const filesystem_operations &) = delete;
        /** Allocate mount point class specify to the VFS
         * @return Allocated mount point structure
         */
        auto mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh, std::string_view path, unsigned flags)
            -> fsmount override;
        auto mount(fsmount mnt) noexcept -> int override;
        auto umount(fsmount mnt) noexcept -> int override;
        auto stat_vfs(fsmount mnt, std::string_view path, statvfs &stat) const noexcept -> int override;

        /** Standard file access API */
        auto open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile override;
        auto close(fsfile zfile) noexcept -> int override;
        auto write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t override;
        auto read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t override;
        auto seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t override;
        auto fstat(fsfile zfile, struct stat &st) noexcept -> int override;
        auto stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int override;
        auto link(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int override;
        auto symlink(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int override;
        auto unlink(fsmount mnt, std::string_view name) noexcept -> int override;
        auto rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int override;
        auto mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int override;

        /** Directory support API */
        auto diropen(fsmount mnt, std::string_view path) noexcept -> fsdir override;
        auto dirreset(fsdir dirstate) noexcept -> int override;
        auto dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) -> int override;
        auto dirclose(fsdir dirstate) noexcept -> int override;

        /** Other fops API */
        auto ftruncate(fsfile zfile, off_t len) noexcept -> int override;
        auto fsync(fsfile zfile) noexcept -> int override;
        auto ioctl(fsmount mnt, std::string_view path, int cmd, void *arg) noexcept -> int override;
        auto utimens(fsmount mnt, std::string_view path, std::array<timespec, 2> &tv) noexcept -> int override;
        auto flock(fsfile zfile, int cmd) noexcept -> int;
        auto isatty(fsfile zfile) noexcept -> int override;

        auto chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int override;
        auto fchmod(fsfile zfile, mode_t mode) noexcept -> int override;

      private:
        mutable cpp_freertos::MutexRecursive m_lock;
    };
} // namespace purefs::fs::drivers
