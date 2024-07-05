// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <purefs/filesystem_paths.hpp>

#include <string>
#include <optional>

namespace utils
{
    inline constexpr auto defaultPreviousVersion = "0.00.0";
    std::optional<std::string> getPreviousVersionFromFile(const std::filesystem::path &path);
} // namespace utils
