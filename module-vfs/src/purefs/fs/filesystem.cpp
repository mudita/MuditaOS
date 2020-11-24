// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/fs/filesystem.hpp>
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
        if (it->second.use_count() > 1) {
            LOG_ERROR("VFS: fileystem %.*s  is already used", int(fsname.length()), fsname.data());
            return -EBUSY;
        }
        m_fstypes.erase(it);
        return 0;
    }
} // namespace purefs::fs
