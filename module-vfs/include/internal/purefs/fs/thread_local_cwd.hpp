// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>

namespace purefs::fs::internal
{
    auto get_thread_local_cwd_path() noexcept -> std::string_view;
    auto set_thread_cwd_path(std::string_view path) noexcept -> int;
} // namespace purefs::fs::internal
