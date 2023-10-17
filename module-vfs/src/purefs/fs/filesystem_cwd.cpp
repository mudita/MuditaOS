// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <purefs/fs/filesystem.hpp>
#include <purefs/fs/thread_local_cwd.hpp>
#include <unistd.h>

namespace purefs::fs
{
    auto filesystem::getcwd() noexcept -> std::string_view
    {
        return internal::get_thread_local_cwd_path();
    }
    auto filesystem::chdir(std::string_view name) noexcept -> int
    {
        struct stat sts;
        int ret;
        ret = this->stat(name, sts);
        if (ret) {
            return ret;
        }
        if ((sts.st_mode & S_IFMT) != S_IFDIR) {
            return -ENOTDIR;
        }
        const auto abspath = absolute_path(name);
        ret                = internal::set_thread_cwd_path(abspath);
        return ret;
    }
} // namespace purefs::fs
