// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <list>
#include <purefs/blkdev/defs.hpp>
#include <purefs/fs/file_handle.hpp>
#include <purefs/fs/directory_handle.hpp>

namespace purefs::fs
{
    struct statvfs;

    /** Filesystem specific driver base class */
    class filesystem_operation
    {
      public:
        using fsfile                                       = std::shared_ptr<internal::file_handle>;
        using fsdir                                        = std::shared_ptr<internal::directory_handle>;
        filesystem_operation(const filesystem_operation &) = delete;
        virtual ~filesystem_operation()                    = default;
        auto operator=(const filesystem_operation &)                                                      = delete;
        virtual auto mount(std::string_view dev, std::string_view target, unsigned flags) noexcept -> int = 0;
        virtual auto umount(std::string_view dev) noexcept -> int                                         = 0;
        virtual auto stat_vfs(std::string_view path, statvfs &stat) const noexcept -> int                 = 0;

        /** Standard file access API */
        virtual auto open(std::string_view path, int flags, int mode) noexcept -> fsfile          = 0;
        virtual auto close(fsfile zfile) noexcept -> int                                          = 0;
        virtual auto write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t         = 0;
        virtual auto read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t                = 0;
        virtual auto seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t                     = 0;
        virtual auto fstat(fsfile zfile, struct stat *st) noexcept -> int                         = 0;
        virtual auto stat(std::string_view file, struct stat *st) noexcept -> int                 = 0;
        virtual auto link(std::string_view existing, std::string_view newlink) noexcept -> int    = 0;
        virtual auto symlink(std::string_view existing, std::string_view newlink) noexcept -> int = 0;
        virtual auto unlink(std::string_view name) noexcept -> int                                = 0;
        virtual auto rename(std::string_view oldname, std::string_view newname) noexcept -> int   = 0;
        virtual auto mkdir(std::string_view path, int mode) noexcept -> int                       = 0;

        /** Directory support API */
        virtual auto diropen(std::string_view path) noexcept -> fsdir                                      = 0;
        virtual auto dirreset(fsdir dirstate) noexcept -> int                                              = 0;
        virtual auto dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) noexcept -> int = 0;
        virtual auto dirclose(fsdir rdirState) noexcept -> int;

        /** Other fops API */
        virtual auto ftruncate(fsfile zfile, off_t len) noexcept -> int                          = 0;
        virtual auto fsync(fsfile zfile) noexcept -> int                                         = 0;
        virtual auto ioctl(fsfile zfile, int cmd, void *arg) noexcept -> int                     = 0;
        virtual auto utimens(std::string_view path, std::array<timespec, 2> &tv) noexcept -> int = 0;
        virtual auto flock(fsfile zfile, int cmd) noexcept -> int                                = 0;
        virtual auto isatty(fsfile zfile) noexcept -> int                                        = 0;

        virtual auto chmod(std::string_view path, mode_t mode) noexcept -> int = 0;
        virtual auto fchmod(fsfile zfile, mode_t mode) noexcept -> int         = 0;
    };
} // namespace purefs::fs
