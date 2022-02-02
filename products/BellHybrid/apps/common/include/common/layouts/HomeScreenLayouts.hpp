// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "layouts/HomeScreenLayoutClassicWithAmPm.hpp"
#include "layouts/HomeScreenLayoutClassicWithBattery.hpp"
#include "layouts/HomeScreenLayoutClassicWithTemp.hpp"

#include <map>
#include <string>
#include <typeindex>

namespace gui
{
    using LayoutGenerator = std::function<std::unique_ptr<gui::BaseHomeScreenLayoutProvider>()>;

    std::map<std::string, LayoutGenerator> homeScreenLayouts = {
        {"ClassicWithTemp", []() { return std::make_unique<gui::HomeScreenLayoutClassicWithTemp>("ClassicWithTemp"); }},
        {"ClassicWithAmPm", []() { return std::make_unique<gui::HomeScreenLayoutClassicWithAmPm>("ClassicWithAmPm"); }},
        {"ClassicWithBattery",
         []() { return std::make_unique<gui::HomeScreenLayoutClassicWithBattery>("ClassicWithBattery"); }},
    };
}; // namespace gui
