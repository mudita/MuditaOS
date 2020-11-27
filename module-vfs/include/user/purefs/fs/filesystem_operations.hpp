// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <list>
#include <tuple>
#include <memory>

namespace purefs::blkdev::internal
{
    class disk_handle;
}

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
    class filesystem_operations
    {
      public:
        using fsfile                                         = std::shared_ptr<internal::file_handle>;
        using fsdir                                          = std::shared_ptr<internal::directory_handle>;
        using fsmount                                        = std::shared_ptr<internal::mount_point>;
        filesystem_operations(const filesystem_operations &) = delete;
        virtual ~filesystem_operations()                     = default;
        auto operator=(const filesystem_operations &) = delete;
        /** Allocate mount point class specify to the VFS
         * @return Allocated mount point structure
         */
        virtual auto mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh, std::string_view path)
            -> fsmount                                   = 0;
        virtual auto mount(fsmount mnt) noexcept -> int  = 0;
        virtual auto umount(fsmount mnt) noexcept -> int = 0;
        virtual auto stat_vfs(fsmount mnt, std::string_view path, statvfs &stat) const noexcept -> int;

        /** Standard file access API */
        virtual auto open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile = 0;
        virtual auto close(fsfile zfile) noexcept -> int                                              = 0;
        virtual auto write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t             = 0;
        virtual auto read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t                    = 0;
        virtual auto seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t;
        virtual auto fstat(fsfile zfile, struct stat *st) noexcept -> int;
        virtual auto stat(fsmount mnt, std::string_view file, struct stat *st) noexcept -> int;
        virtual auto link(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int;
        virtual auto symlink(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int;
        virtual auto unlink(fsmount mnt, std::string_view name) noexcept -> int;
        virtual auto rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int;
        virtual auto mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int;

        /** Directory support API */
        virtual auto diropen(fsmount mnt, std::string_view path) noexcept -> fsdir;
        virtual auto dirreset(fsdir dirstate) noexcept -> int;
        virtual auto dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) -> int;
        virtual auto dirclose(fsdir dirstate) noexcept -> int;

        /** Other fops API */
        virtual auto ftruncate(fsfile zfile, off_t len) noexcept -> int;
        virtual auto fsync(fsfile zfile) noexcept -> int;
        virtual auto ioctl(fsmount mnt, std::string_view path, int cmd, void *arg) noexcept -> int;
        virtual auto utimens(fsmount mnt, std::string_view path, std::array<timespec, 2> &tv) noexcept -> int;
        virtual auto flock(fsfile zfile, int cmd) noexcept -> int;
        virtual auto isatty(fsfile zfile) noexcept -> int = 0;

        virtual auto chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int;
        virtual auto fchmod(fsfile zfile, mode_t mode) noexcept -> int;

        /** Internal use */
        auto mount_count() const noexcept
        {
            return m_mount_count;
        }

      private:
        std::size_t m_mount_count{};
    };
} // namespace purefs::fs
