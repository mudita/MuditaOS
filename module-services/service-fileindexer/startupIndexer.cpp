// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "startupIndexer.hpp"
#include <filesystem>
#include <vfs>

namespace service::detail
{

    namespace
    {
        const std::vector<std::string_view> allowed_idxs{
            ".png",
            ".jpg",
            ".jpeg",
            ".bmp",
            ".wav",
            ".mp3",
            ".flac",
        };
    }
    auto startupIndexer::fileShouldBeIndexed(std::string_view path) -> bool
    {
        namespace fs = std::filesystem;
        for (const auto &ext : allowed_idxs) {
            if (fs::path(path).extension() == ext) {
                return true;
            }
        }
        return false;
    }

    // Collect startup files when service starts
    auto startupIndexer::collectStartupFiles() -> void
    {}
    // Setup timers for notification
    auto startupIndexer::setupTimers(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void
    {}
} // namespace service::detail
