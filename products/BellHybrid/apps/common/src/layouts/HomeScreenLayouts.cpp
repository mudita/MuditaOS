
// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/layouts/HomeScreenLayoutNames.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/layouts/HomeScreenLayoutClassic.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithAmPm.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithBattery.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithDate.hpp>
#if CONFIG_ENABLE_TEMP == 1
#include <common/layouts/HomeScreenLayoutClassicWithTemp.hpp>
#endif
#include <common/layouts/HomeScreenLayoutVerticalSimple.hpp>
#include <common/layouts/HomeScreenLayoutVerticalWithAmPm.hpp>
#include <common/layouts/HomeScreenLayoutVerticalWithDate.hpp>
#include <ProductConfig.hpp>

namespace gui::factory
{
    std::map<std::string, LayoutGenerator> getLayoutsFormat24h()
    {
        return
        {
            {gui::layout::Classic, []() { return new HomeScreenLayoutClassic(gui::layout::Classic); }},
                {gui::layout::ClassicWithBattery,
                 []() { return new HomeScreenLayoutClassicWithBattery(gui::layout::ClassicWithBattery); }},
                {gui::layout::ClassicWithDate,
                 []() { return new HomeScreenLayoutClassicWithDate(gui::layout::ClassicWithDate); }},
                {gui::layout::VerticalSimple,
                 []() { return new HomeScreenLayoutVerticalSimple(gui::layout::VerticalSimple); }},
            {
                gui::layout::VerticalWithDate,
                    []() { return new HomeScreenLayoutVerticalWithDate(gui::layout::VerticalWithDate); }
            }
#if CONFIG_ENABLE_TEMP == 1
            ,
            {
                gui::layout::ClassicWithTemp,
                    []() { return new HomeScreenLayoutClassicWithTemp(gui::layout::ClassicWithTemp); }
            }
#endif
        };
    }

    std::map<std::string, LayoutGenerator> getLayoutsFormat12h()
    {
        return
        {
            {gui::layout::Classic, []() { return new HomeScreenLayoutClassic(gui::layout::Classic); }},
                {gui::layout::ClassicWithAmPm,
                 []() { return new HomeScreenLayoutClassicWithAmPm(gui::layout::ClassicWithAmPm); }},
                {gui::layout::ClassicWithBattery,
                 []() { return new HomeScreenLayoutClassicWithBattery(gui::layout::ClassicWithBattery); }},
                {gui::layout::ClassicWithDate,
                 []() { return new HomeScreenLayoutClassicWithDate(gui::layout::ClassicWithDate); }},
                {gui::layout::VerticalSimple,
                 []() { return new HomeScreenLayoutVerticalSimple(gui::layout::VerticalSimple); }},
                {gui::layout::VerticalWithAmPm,
                 []() { return new HomeScreenLayoutVerticalWithAmPm(gui::layout::VerticalWithAmPm); }},
            {
                gui::layout::VerticalWithDate,
                    []() { return new HomeScreenLayoutVerticalWithDate(gui::layout::VerticalWithDateAmPm); }
            }
#if CONFIG_ENABLE_TEMP == 1
            ,
            {
                "ClassicWithTemp", []() { return new HomeScreenLayoutClassicWithTemp("ClassicWithTemp"); }
            }
#endif
        };
    }

    std::map<std::string, LayoutGenerator> getAllLayouts()
    {
        return getLayoutsFormat12h();
    };

    std::string getDefaultLayout()
    {
        return gui::layout::ClassicWithBattery;
    }

} // namespace gui::factory
