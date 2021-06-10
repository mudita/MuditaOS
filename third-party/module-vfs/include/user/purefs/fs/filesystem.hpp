// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <memory>
#include <list>
#include <array>
#include <map>
#include <functional>
#include <ctime>
#include <unordered_set>
#include <purefs/fs/handle_mapper.hpp>
#include <purefs/fs/file_handle.hpp>
#include <purefs/fs/directory_handle.hpp>
#include <purefs/fs/mount_point.hpp>
#include <purefs/fs/mount_flags.hpp>
#include <purefs/fs/fsnotify.hpp>
#include <type_traits>

struct statvfs;
struct stat;

namespace purefs::blkdev
{
    class disk_manager;
}

namespace cpp_freertos
{
    class MutexRecursive;
}

namespace sys
{
    class Service;
}

namespace purefs::fs
{
    /** This is the filesystem class layer
     * All methods can be called from the user thread
     * but generally those functions are for internal use for
     * example for newlib or glibc syscalls
     */
    class filesystem_operations;
    namespace internal
    {
        class directory_handle;
        class notifier;
    }
    class filesystem
    {
        static constexpr auto path_separator = '/';
        /** NOTE: Current implementation doesn't allow to open
         * device and return fd. In the next release it will be fixed
         * so default stdin,stdout,stderr will be registered so this
         * first fd will not be longer needed.
         */
        static constexpr auto first_file_descriptor = 3;

      public:
        using fsdir                    = std::shared_ptr<internal::directory_handle>;
        using fsfile                         = std::shared_ptr<internal::file_handle>;
        explicit filesystem(std::shared_ptr<blkdev::disk_manager> diskmm);
        ~filesystem();
        filesystem(const filesystem &) = delete;
        auto operator=(const filesystem &) = delete;
        /** Utility API */
        /** Register filesystem driver
         * @param[in] fsname Unique filesystem name for example fat
         * @param[in] fops Filesystem operation structure
         * @return zero on sucess otherwise error
         */
        template <typename T> auto register_filesystem(std::string_view fsname, std::shared_ptr<T> fops) -> int
        {
            if (!fops || !std::is_convertible_v<T *, filesystem_operations *>) {
                return -EINVAL;
            }
            return register_filesystem(fsname, std::shared_ptr<filesystem_operations>(fops));
        }
        auto register_filesystem(std::string_view fsname, std::shared_ptr<filesystem_operations> fops) -> int;
        /** Unregister filesystem driver
         * @param[in] fsname Unique filesystem name for example fat
         * @return zero on success otherwise error
         **/
        auto unregister_filesystem(std::string_view fsname) -> int;

        /** Mount filesystem to the the specified mount point
         * see man(2) mount
         * @param[in] dev_or_part Device or partition for mount
         * @param[in] target Target path where the fs will be mounted
         * @param[in] flags  Mount flags
         * @param[in] data   Filesystem specific configuration
         * @return zero on success otherwise error
         */
        auto mount(std::string_view dev_or_part,
                   std::string_view target,
                   std::string_view fs_type,
                   unsigned flags   = 0,
                   const void *data = nullptr) -> int;
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
        auto stat_vfs(std::string_view path, struct statvfs &stat) const noexcept -> int;
        /** Standard file access API */
        auto open(std::string_view path, int flags, int mode) noexcept -> int;
        auto close(int fd) noexcept -> int;
        auto write(int fd, const char *ptr, size_t len) noexcept -> ssize_t;
        auto read(int fd, char *ptr, size_t len) noexcept -> ssize_t;
        auto seek(int fd, off_t pos, int dir) noexcept -> off_t;
        auto fstat(int fd, struct stat &st) noexcept -> int;
        auto stat(std::string_view file, struct stat &st) noexcept -> int;
        auto link(std::string_view existing, std::string_view newlink) noexcept -> int;
        auto symlink(std::string_view existing, std::string_view newlink) noexcept -> int;
        auto unlink(std::string_view name) noexcept -> int;
        auto rename(std::string_view oldname, std::string_view newname) noexcept -> int;
        auto mkdir(std::string_view path, int mode) noexcept -> int;

        /** Directory support API */
        auto diropen(std::string_view path) noexcept -> fsdir;
        auto dirreset(fsdir) noexcept -> int;
        auto dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) noexcept -> int;
        auto dirclose(fsdir dirstate) noexcept -> int;

        /** Other fops API */
        auto ftruncate(int fd, off_t len) noexcept -> int;
        auto fsync(int fd) noexcept -> int;
        auto ioctl(std::string_view path, int cmd, void *arg) noexcept -> int;
        auto utimens(std::string_view path, std::array<timespec, 2> &tv) noexcept -> int;
        auto flock(int fd, int cmd) noexcept -> int;
        auto isatty(int fd) noexcept -> int;

        auto chmod(std::string_view path, mode_t mode) noexcept -> int;
        auto fchmod(int fd, mode_t mode) noexcept -> int;

        auto getcwd() noexcept -> std::string_view;
        auto chdir(std::string_view name) noexcept -> int;

        /** Inotify API */
        [[nodiscard]] auto inotify_create(std::shared_ptr<sys::Service> svc) -> std::shared_ptr<inotify>;

      private:
        /** Unregister filesystem driver
         * @param[in] fsname Unique filesystem name for example fat
         * @return zero on success otherwise error
         **/
        /** Find the mount point object matching to the mount point path
         * @param[in] path Absolute input path
         * @return mount point object and the matching shortest path
         */
        auto find_mount_point(std::string_view path) const noexcept
            -> std::tuple<std::shared_ptr<internal::mount_point>, size_t>;
        /** Return absolute path from the relative path
         * @param[in] path Unormalized path
         * @return Full Normalized path
         */
        static auto absolute_path(std::string_view path) noexcept -> std::string;
        /** Normalize full path
         * @param[in] path Unnormalized full path
         * @param[out] Normalized path
         */
        static auto normalize_path(std::string_view path) noexcept -> std::string;
        /** Add handle to file descriptor
         */
        auto add_filehandle(fsfile file) noexcept -> int;
        auto remove_filehandle(int fds) noexcept -> fsfile;
        auto find_filehandle(int fds) const noexcept -> fsfile;
        auto autodetect_filesystem_type(std::string_view dev_or_part) const noexcept -> std::string;

        enum class iaccess : bool
        {
            ro, //! Syscall is RO
            rw  //! Syscall is RW
        };
        template <class Base, class T, typename... Args>
        inline auto invoke_fops(T Base::*method, int fds, Args &&... args)
            -> decltype((static_cast<Base *>(nullptr)->*method)(0, std::forward<Args>(args)...))
        {
            auto fil = find_filehandle(fds);
            if (!fil) {
                return -EBADF;
            }
            else {
                auto mp = fil->mntpoint();
                if (!mp) {
                    return -ENOENT;
                }
                auto fsops = mp->fs_ops();
                if (!fsops) {
                    return -EIO;
                }
                else {
                    return (fsops.get()->*method)(fil, std::forward<Args>(args)...);
                }
            }
        }

        template <class Base, class T, typename... Args>
        inline auto invoke_fops(iaccess acc, T Base::*method, std::string_view path, Args &&... args) const
            -> decltype((static_cast<Base *>(nullptr)->*method)(nullptr, {}, std::forward<Args>(args)...))
        {
            if (path.empty()) {
                return -ENOENT;
            }
            const auto abspath     = absolute_path(path);
            auto [mountp, pathpos] = find_mount_point(abspath);
            if (!mountp) {
                return -ENOENT;
            }
            if (acc == iaccess::rw && mountp->is_ro()) {
                return -EACCES;
            }
            auto fsops = mountp->fs_ops();
            if (fsops)
                return (fsops.get()->*method)(mountp, abspath, std::forward<Args>(args)...);
            else
                return -EIO;
        }

        template <class Base, class T, typename... Args>
        inline auto invoke_fops_same_mp(T Base::*method,
                                        std::string_view path,
                                        std::string_view path2,
                                        Args &&... args) const
            -> decltype((static_cast<Base *>(nullptr)->*method)(nullptr, {}, {}, std::forward<Args>(args)...))
        {
            if (path.empty() || path2.empty()) {
                return -ENOENT;
            }
            const auto abspath     = absolute_path(path);
            const auto abspath2    = absolute_path(path2);
            auto [mountp, pathpos] = find_mount_point(abspath);
            if (!mountp) {
                return -ENOENT;
            }
            if (mountp->is_ro()) {
                return -EACCES;
            }
            if (path.compare(0, pathpos, path2, 0, pathpos) != 0) {
                // Mount points are not the same
                return -EXDEV;
            }
            auto fsops = mountp->fs_ops();
            if (fsops)
                return (fsops.get()->*method)(mountp, abspath, abspath2, std::forward<Args>(args)...);
            else
                return -EIO;
        }
        template <class Base, class T, typename... Args>
        inline auto invoke_fops(T Base::*method, fsdir dirp, Args &&... args)
            -> decltype((static_cast<Base *>(nullptr)->*method)(nullptr, std::forward<Args>(args)...))
        {
            const auto err = dirp->error();
            if (err) {
                return err;
            }
            else {
                auto mp = dirp->mntpoint();
                if (!mp) {
                    return -ENOENT;
                }
                auto fsops = mp->fs_ops();
                if (!fsops) {
                    return -EIO;
                }
                else {
                    return (fsops.get()->*method)(dirp, std::forward<Args>(args)...);
                }
            }
        }
      private:
        std::weak_ptr<blkdev::disk_manager> m_diskmm;
        std::unordered_map<std::string, std::shared_ptr<filesystem_operations>> m_fstypes;
        std::map<std::string, std::shared_ptr<internal::mount_point>> m_mounts;
        std::unordered_set<std::string> m_partitions;
        internal::handle_mapper<fsfile> m_fds;
        std::unique_ptr<cpp_freertos::MutexRecursive> m_lock;
        std::shared_ptr<internal::notifier> m_notifier;
    };
} // namespace purefs::fs
