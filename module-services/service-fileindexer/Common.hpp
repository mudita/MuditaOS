// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <algorithm>
#include <filesystem>
#include <array>

namespace service::detail
{
    namespace fs = std::filesystem;

    // File extensions indexing allow list
    constexpr std::array<std::string_view, 3> allowed_exts{".wav", ".mp3", ".flac"};

    // This is debug feature, it overrides allowed extension types above
    constexpr bool allow_all_exts = false;

    inline bool isExtSupported(const fs::path &path)
    {
        if (allow_all_exts) {
            return true;
        }

        return std::any_of(allowed_exts.begin(), allowed_exts.end(), [&path](const fs::path &ext) {
            if (path.has_extension() && path.extension() == ext) {
                return true;
            }
            // if empty string with extension only
            if (path == ext) {
                return true;
            }
            return false;
        });
    }
} // namespace service::detail
