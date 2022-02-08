
// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithAmPm.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithBattery.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithTemp.hpp>

namespace gui
{
    std::map<std::string, LayoutGenerator> homeScreenLayouts()
    {
        return {{"ClassicWithTemp", []() { return new HomeScreenLayoutClassicWithTemp("ClassicWithTemp"); }},
                {"ClassicWithAmPm", []() { return new HomeScreenLayoutClassicWithAmPm("ClassicWithAmPm"); }},
                {"ClassicWithBattery", []() { return new HomeScreenLayoutClassicWithBattery("ClassicWithBattery"); }}};
    };

} // namespace gui
