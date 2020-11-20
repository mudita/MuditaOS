// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <list>
#include <tuple>
#include <memory>

namespace purefs::fs
{
    struct statvfs;
    namespace internal
    {
        class file_handle;
        class directory_handle;
        class mount_point;
    }; // namespace internal

    /** Filesystem specific driver base class */
    class filesystem_operation
    {
      public:
        using fsfile                                       = std::shared_ptr<internal::file_handle>;
        using fsdir                                        = std::shared_ptr<internal::directory_handle>;
        using fsmount                                      = std::shared_ptr<internal::mount_point>;
        filesystem_operation(const filesystem_operation &) = delete;
        virtual ~filesystem_operation()                    = default;
        auto operator=(const filesystem_operation &)                                                      = delete;
        /** Allocate mount point class specify to the VFS
         * @return Allocated mount point structure
         */
        virtual auto mount_prealloc() -> fsmount                                                          = 0;
        virtual auto mount(fsmount mnt) noexcept -> int;
        virtual auto umount(fsmount mnt) noexcept -> int                                                  = 0;
        virtual auto stat_vfs(std::string_view path, statvfs &stat) const noexcept -> int                 = 0;

        /** Standard file access API */
        virtual auto open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile          = 0;
        virtual auto close(fsfile zfile) noexcept -> int                                          = 0;
        virtual auto write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t         = 0;
        virtual auto read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t                = 0;
        virtual auto seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t                     = 0;
        virtual auto fstat(fsfile zfile, struct stat *st) noexcept -> int                         = 0;
        virtual auto stat(fsmount mnt, std::string_view file, struct stat *st) noexcept -> int                 = 0;
        virtual auto link(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int    = 0;
        virtual auto symlink(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int = 0;
        virtual auto unlink(fsmount mnt, std::string_view name) noexcept -> int                                = 0;
        virtual auto rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int   = 0;
        virtual auto mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int                       = 0;

        /** Directory support API */
        virtual auto diropen(fsmount mnt, std::string_view path) noexcept -> fsdir                         = 0;
        virtual auto dirreset(fsdir dirstate) noexcept -> int                                              = 0;
        virtual auto dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) noexcept -> int = 0;
        virtual auto dirclose(fsdir dirstate) noexcept -> int;

        /** Other fops API */
        virtual auto ftruncate(fsfile zfile, off_t len) noexcept -> int                          = 0;
        virtual auto fsync(fsfile zfile) noexcept -> int                                         = 0;
        virtual auto ioctl(fsfile zfile, int cmd, void *arg) noexcept -> int                     = 0;
        virtual auto utimens(fsmount mnt, std::string_view path, std::array<timespec, 2> &tv) noexcept -> int = 0;
        virtual auto flock(fsfile zfile, int cmd) noexcept -> int                                = 0;
        virtual auto isatty(fsfile zfile) noexcept -> int                                        = 0;

        virtual auto chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int = 0;
        virtual auto fchmod(fsfile zfile, mode_t mode) noexcept -> int         = 0;
    };
} // namespace purefs::fs
