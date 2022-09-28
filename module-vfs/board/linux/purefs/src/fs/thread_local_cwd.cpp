// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <purefs/fs/thread_local_cwd.hpp>

namespace purefs::fs::internal
{

    namespace
    {
        thread_local std::string cwd_per_thread;
        std::string default_cwd{"/"};
    } // namespace
    auto get_thread_local_cwd_path() noexcept -> std::string_view
    {
        if (cwd_per_thread.empty()) {
            cwd_per_thread = default_cwd;
        }
        return cwd_per_thread;
    }
    auto set_thread_cwd_path(std::string_view path) noexcept -> int
    {
        cwd_per_thread = path;
        return 0;
    }
    auto cleanup_thread_local_cwd_mem() -> void
    {
        cwd_per_thread.erase();
    }
    auto set_default_thread_cwd(std::string_view str) noexcept -> void
    {
        default_cwd = str;
    }
} // namespace purefs::fs::internal
