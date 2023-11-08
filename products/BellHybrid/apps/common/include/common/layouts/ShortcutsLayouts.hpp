// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <map>
#include <string>

namespace gui
{
    class ShortcutsLayoutProvider;
    using LayoutGenerator = std::function<ShortcutsLayoutProvider *()>;

    namespace factory
    {
        std::vector<LayoutGenerator> getLayouts();
    } // namespace factory
};    // namespace gui
