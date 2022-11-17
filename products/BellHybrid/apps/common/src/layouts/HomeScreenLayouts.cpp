
// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
            {"Classic", []() { return new HomeScreenLayoutClassic("Classic"); }},
                {"ClassicWithBattery", []() { return new HomeScreenLayoutClassicWithBattery("ClassicWithBattery"); }},
                {"ClassicWithDate", []() { return new HomeScreenLayoutClassicWithDate("ClassicWithDate"); }},
                {"VerticalSimple", []() { return new HomeScreenLayoutVerticalSimple("VerticalSimple"); }},
            {
                "VerticalWithDate", []() { return new HomeScreenLayoutVerticalWithDate("VerticalWithDate"); }
            }
#if CONFIG_ENABLE_TEMP == 1
            ,
            {
                "ClassicWithTemp", []() { return new HomeScreenLayoutClassicWithTemp("ClassicWithTemp"); }
            }
#endif
        };
    }

    std::map<std::string, LayoutGenerator> getLayoutsFormat12h()
    {
        return
        {
            {"Classic", []() { return new HomeScreenLayoutClassic("Classic"); }},
                {"ClassicWithAmPm", []() { return new HomeScreenLayoutClassicWithAmPm("ClassicWithAmPm"); }},
                {"ClassicWithBattery", []() { return new HomeScreenLayoutClassicWithBattery("ClassicWithBattery"); }},
                {"ClassicWithDate", []() { return new HomeScreenLayoutClassicWithDate("ClassicWithDateAmPm"); }},
                {"VerticalSimple", []() { return new HomeScreenLayoutVerticalSimple("VerticalSimple"); }},
                {"VerticalWithAmPm", []() { return new HomeScreenLayoutVerticalWithAmPm("VerticalWithAmPm"); }},
            {
                "VerticalWithDate", []() { return new HomeScreenLayoutVerticalWithDate("VerticalWithDateAmPm"); }
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
        return "ClassicWithBattery";
    }

} // namespace gui::factory
