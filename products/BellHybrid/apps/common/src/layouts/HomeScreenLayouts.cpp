
// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/layouts/HomeScreenLayoutClassic.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithAmPm.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithBattery.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithTemp.hpp>
#include <common/layouts/HomeScreenLayoutVertical.hpp>
#include <common/layouts/HomeScreenLayoutVerticalSimple.hpp>
#include <common/layouts/HomeScreenLayoutVerticalWithAmPm.hpp>

namespace gui::factory
{
    std::map<std::string, LayoutGenerator> getLayoutsFormat24h()
    {
        return {{"Classic", []() { return new HomeScreenLayoutClassic("Classic"); }},
                {"ClassicWithBattery", []() { return new HomeScreenLayoutClassicWithBattery("ClassicWithBattery"); }},
                {"VerticalSimple", []() { return new HomeScreenLayoutVerticalSimple("VerticalSimple"); }}};
    };

    std::map<std::string, LayoutGenerator> getLayoutsFormat12h()
    {
        return {{"Classic", []() { return new HomeScreenLayoutClassic("Classic"); }},
                {"ClassicWithAmPm", []() { return new HomeScreenLayoutClassicWithAmPm("ClassicWithAmPm"); }},
                {"ClassicWithBattery", []() { return new HomeScreenLayoutClassicWithBattery("ClassicWithBattery"); }},
                {"VerticalSimple", []() { return new HomeScreenLayoutVerticalSimple("VerticalSimple"); }},
                {"VerticalWithAmPm", []() { return new HomeScreenLayoutVerticalWithAmPm("VerticalWithAmPm"); }}};
    };

    std::map<std::string, LayoutGenerator> getAllLayouts()
    {
        return getLayoutsFormat12h();
    };

    std::string getDefaultLayout()
    {
        return "Classic";
    }

} // namespace gui::factory
