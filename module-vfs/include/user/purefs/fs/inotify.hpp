// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once
#include <memory>
#include <purefs/vfs_subsystem.hpp>
#include <purefs/fs/fsnotify.hpp>

namespace purefs::fs
{
    /** Create inotify class which owns the notifier interface
     * @param[in] svc Owner of the service
     * @return Inotify object shared ptr or errno if nullptr
     */
    inline auto inotify_create(std::shared_ptr<sys::Service> svc) -> std::shared_ptr<inotify>
    {
        const auto vfs = subsystem::vfs_core();
        if (!vfs) {
            return nullptr;
        }
        return vfs->inotify_create(svc);
    }
} // namespace purefs::fs
