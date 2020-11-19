// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <memory>
#include <list>
#include <array>

namespace purefs::fs
{

    /** This is the filesystem class layer
     * All methods can be called from the user thread
     * but generally those functions are for internal use for
     * example for newlib or glibc syscalls
     */
    class filesystem_operation;
    struct statvfs;
    struct DIR_ITER;
    struct timespec;

    class filesystem
    {
      public:
        filesystem(const filesystem &) = delete;
        auto operator=(const filesystem &) = delete;
        /** Utility API */
        /** Register filesystem driver
         * @param[in] fsname Unique filesystem name for example fat
         * @param[in] fops Filesystem operation structure
         * @return zero on sucess otherwise error
         */
        auto register_filesystem(std::string_view fsname, std::shared_ptr<filesystem_operation> fops) -> int;
        /** Unregister filesystem driver
         * @param[in] fsname Unique filesystem name for example fat
         * @return zero on success otherwise error
         **/
        auto unregister_filesystem(std::string_view fsname) -> int;

        /** Mount filesystem to the the specified mount point
         * @param[in] dev_or_part Device or partition for mount
         * @param[in] target Target path where the fs will be mounted
         * @param[in] flags  Mount flags
         * @return zero on success otherwise error
         */
        auto mount(std::string_view dev_or_part, std::string_view target, std::string_view fs_type, unsigned flags)
            -> int;
        /** Unmont filesystem from selected mount point
         * @param[in] mount_point Mount point where the fs is mounted
         * @return zero on success otherwise error
         */
        auto umount(std::string_view mount_point) -> int;
        /** Return actually mounted filesystem list
         * @param[out] mountpoints List of mount points
         * @return zero on success otherwise error
         */
        auto read_mountpoints(std::list<std::string> &mountpoints) const -> int;
        /** Get actual filesystem statistics *  @see man statvfs
         * @param[in] path Pathname of any file within the mounted filesystem
         * @param[out] stat Pointer to a statvfs structure
         * @return zero on success otherwise error
         */
        auto stat_vfs(std::string_view path, statvfs &stat) const -> int;
        /** Standard file access API */
        auto open(std::string_view path, int flags, int mode) noexcept -> int;
        auto close(int fd) noexcept -> int;
        auto write(int fd, const char *ptr, size_t len) noexcept -> ssize_t;
        auto read(int fd, char *ptr, size_t len) noexcept -> ssize_t;
        auto seek(int fd, off_t pos, int dir) noexcept -> off_t;
        auto fstat(int fd, struct stat *st) noexcept -> int;
        auto stat(std::string_view file, struct stat *st) noexcept -> int;
        auto link(std::string_view existing, std::string_view newlink) noexcept -> int;
        auto symlink(std::string_view existing, std::string_view newlink) noexcept -> int;
        auto unlink(std::string_view name) noexcept -> int;
        auto rename(std::string_view oldname, std::string_view newname) noexcept -> int;
        auto mkdir(std::string_view path, int mode) noexcept -> int;

        /** Directory support API */
        auto diropen(int &error, DIR_ITER *dirstate, std::string_view path) noexcept -> DIR_ITER *;
        auto dirreset(DIR_ITER *dirstate) noexcept -> int;
        auto dirnext(DIR_ITER *dirstate, std::string &filename, struct stat *filestat) noexcept -> int;
        auto dirclose(DIR_ITER *dirState) noexcept -> int;

        /** Other fops API */
        auto ftruncate(int fd, off_t len) noexcept -> int;
        auto fsync(int fd) noexcept -> int;
        auto ioctl(std::string_view path, int cmd, void *arg) noexcept -> int;
        auto utimens(std::string_view path, std::array<timespec, 2> &tv) noexcept -> int;
        auto flock(std::string_view path, int cmd) noexcept -> int;

        auto chmod(struct _reent *r, const char *path, mode_t mode) noexcept -> int;
        auto fchmod(int fd, mode_t mode) noexcept -> int;

        auto getcwd() noexcept -> std::string;
        auto chdir(std::string_view name) noexcept -> int;
    };
} // namespace purefs::fs
