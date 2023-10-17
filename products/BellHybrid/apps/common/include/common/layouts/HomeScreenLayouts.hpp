// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <map>
#include <string>

namespace gui
{
    class BaseHomeScreenLayoutProvider;
    using LayoutGenerator = std::function<BaseHomeScreenLayoutProvider *()>;

    namespace factory
    {
        std::map<std::string, LayoutGenerator> getLayoutsFormat24h();
        std::map<std::string, LayoutGenerator> getLayoutsFormat12h();

        std::map<std::string, LayoutGenerator> getAllLayouts();
        std::string getDefaultLayout();
    } // namespace factory
};    // namespace gui
