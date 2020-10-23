// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "vfsNotifier.hpp"
#include <vfs.hpp>
#include <cstring>

namespace vfsn::utility
{

    auto vfsNotifier::onFileOpen(const char *filename, const char *mode, const FILE *file) noexcept -> void
    {
        if (file && std::strpbrk(mode, "+wa")) {
            cpp_freertos::LockGuard _lock(mMutex);
            mOpenedMap.emplace(file, vfs.getAbsolutePath(filename));
        }
    }
    auto vfsNotifier::onFileClose(const FILE *file) noexcept -> void
    {
        cpp_freertos::LockGuard _lock(mMutex);
        const auto item = mOpenedMap.find(file);
        if (item != mOpenedMap.end()) {
            const auto path = item->second;
            mOpenedMap.erase(item);
            notify(path, FsEvent::modified);
        }
    }

    auto vfsNotifier::onFileRemove(std::string_view filename) noexcept -> void
    {
        notify(vfs.getAbsolutePath(filename), FsEvent::deleted);
    }

    auto vfsNotifier::onFileRename(std::string_view new_file, std::string_view old_file) noexcept -> void
    {
        notify(vfs.getAbsolutePath(new_file), FsEvent::renamed, vfs.getAbsolutePath(old_file));
    }
} // namespace vfsn::utility
