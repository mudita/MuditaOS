// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/fs/filesystem.hpp>
#include <purefs/fs/filesystem_operations.hpp>
#include <purefs/fs/mount_point.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <log/log.hpp>
#include <errno.h>

namespace purefs::fs
{
    filesystem::filesystem(std::shared_ptr<blkdev::disk_manager> diskmm) : m_diskmm(diskmm)
    {}
    auto filesystem::register_filesystem(std::string_view fsname, std::shared_ptr<filesystem_operations> fops) -> int
    {
        cpp_freertos::LockGuard _lck(m_lock);
        const auto ret = m_fstypes.find(std::string(fsname));
        if (ret != std::end(m_fstypes)) {
            LOG_ERROR("Disc: %.*s already registered.", int(fsname.length()), fsname.data());
            return -EEXIST;
        }
        else {
            m_fstypes.emplace(std::make_pair(fsname, fops));
            return 0;
        }
    }
    auto filesystem::unregister_filesystem(std::string_view fsname) -> int
    {
        cpp_freertos::LockGuard _lck(m_lock);
        const auto it = m_fstypes.find(std::string(fsname));
        if (it == std::end(m_fstypes)) {
            LOG_ERROR("VFS: filesystem %.*s doesn't exists in manager.", int(fsname.length()), fsname.data());
            return -ENOENT;
        }
        if (it->second->mount_count() > 0) {
            LOG_ERROR("VFS: fileystem %.*s  is already used", int(fsname.length()), fsname.data());
            return -EBUSY;
        }
        m_fstypes.erase(it);
        return 0;
    }
    auto filesystem::mount(std::string_view dev_or_part,
                           std::string_view target,
                           std::string_view fs_type,
                           unsigned flags) -> int
    {
        // Sanity check input data
        if (target.size() <= 1 || target[0] != '/') {
            LOG_ERROR("VFS: Invalid target mountpoint path %.*s", int(target.length()), target.data());
            return -EINVAL;
        }
        {
            cpp_freertos::LockGuard _lock(m_lock);
            const auto mpi = m_mounts.find(std::string(target));
            if (mpi != std::end(m_mounts)) {
                LOG_ERROR("VFS: mount point already exists %.*s", int(target.length()), target.data());
                return -EBUSY;
            }
            const auto vsi = m_fstypes.find(std::string(fs_type));
            if (vsi == std::end(m_fstypes)) {
                LOG_ERROR("VFS: requested filesystem %.*s not registered", int(fs_type.length()), fs_type.data());
            }
            // Trying to open disk or part by manager
            blkdev::disk_fd_t diskh;
            {
                auto disk_mgr = m_diskmm.lock();
                if (disk_mgr) {
                    diskh = disk_mgr->device_handle(dev_or_part);
                }
                else {
                    LOG_ERROR("VFS: Unable to lock device manager");
                    return -EIO;
                }
            }
            if (diskh) {
                auto mnt_point = std::make_shared<internal::mount_point>(diskh, target, vsi->second);
            }
        }
        return -1;
    }
    auto filesystem::umount(std::string_view mount_point) -> int
    {
        return -1;
    }
    auto filesystem::read_mountpoints(std::list<std::string> &mountpoints) const -> int
    {
        return -1;
    }
} // namespace purefs::fs
