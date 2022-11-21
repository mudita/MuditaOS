// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InitializedFontManager.hpp"
#include <mutex>
#include <purefs/filesystem_paths.hpp>

namespace mockup
{
    auto guard = std::mutex();

    auto fontManager() -> gui::FontManager &
    {
        std::lock_guard<std::mutex> scoped(guard);
        auto &fontmanager = gui::FontManager::getInstance();
        if (!fontmanager.isInitialized()) {
            fontmanager.init(purefs::dir::getAssetsDirPath());
        }
        return fontmanager;
    }
} // namespace mockup
