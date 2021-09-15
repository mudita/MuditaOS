// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <purefs/fs/filesystem_operations.hpp>

namespace purefs::fs::drivers
{
    /** Filesystem specific driver base class */
    class filesystem_vfat final : public filesystem_operations
    {
      public:
        filesystem_vfat()                        = default;
        virtual ~filesystem_vfat()               = default;

      private:
        auto mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh, std::string_view path, unsigned flags)
            -> fsmount override;
        auto mount(fsmount mnt, const void *data) noexcept -> int override;
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
        auto unlink(fsmount mnt, std::string_view name) noexcept -> int override;
        auto rmdir(fsmount mnt, std::string_view name) noexcept -> int override;
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
        auto isatty(fsfile zfile) noexcept -> int override;

        auto chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int override;
        auto fchmod(fsfile zfile, mode_t mode) noexcept -> int override;
        auto filesystem_register_completed() const noexcept -> int override;
    };
} // namespace purefs::fs::drivers
