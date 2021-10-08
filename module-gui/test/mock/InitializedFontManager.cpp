// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InitializedFontManager.hpp"
#include <mutex>

namespace mockup
{
    auto guard = std::mutex();

    auto fontManager() -> gui::FontManager &
    {
        std::lock_guard<std::mutex> scoped(guard);
        auto &fontmanager = gui::FontManager::getInstance();
        if (!fontmanager.isInitialized()) {
            fontmanager.init("assets");
        }
        return fontmanager;
    }
} // namespace mockup
