// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <common/layouts/HomeScreenLayoutNames.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/layouts/HomeScreenLayoutClassic.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithAmPm.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithBattery.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithDate.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithQuotes.hpp>
#include <common/layouts/HomeScreenLayoutClassicWithQuotesAmPm.hpp>
#include <common/layouts/HomeScreenLayoutBinary.hpp>
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
        return {
            {layout::Classic, []() { return new HomeScreenLayoutClassic(layout::Classic); }},
            {layout::ClassicWithBattery,
             []() { return new HomeScreenLayoutClassicWithBattery(layout::ClassicWithBattery); }},
            {layout::ClassicWithDate, []() { return new HomeScreenLayoutClassicWithDate(layout::ClassicWithDate); }},
            {layout::ClassicWithQuotes,
             []() { return new HomeScreenLayoutClassicWithQuotes(layout::ClassicWithQuotes); }},
            {layout::VerticalSimple, []() { return new HomeScreenLayoutVerticalSimple(layout::VerticalSimple); }},
            {layout::VerticalWithDate, []() { return new HomeScreenLayoutVerticalWithDate(layout::VerticalWithDate); }},
            {layout::Binary, []() { return new HomeScreenLayoutBinary(layout::Binary); }}
#if CONFIG_ENABLE_TEMP == 1
            ,
            {layout::ClassicWithTemp, []() { return new HomeScreenLayoutClassicWithTemp(layout::ClassicWithTemp); }}
#endif
        };
    }

    std::map<std::string, LayoutGenerator> getLayoutsFormat12h()
    {
        return {
            {layout::Classic, []() { return new HomeScreenLayoutClassic(layout::Classic); }},
            {layout::ClassicWithAmPm, []() { return new HomeScreenLayoutClassicWithAmPm(layout::ClassicWithAmPm); }},
            {layout::ClassicWithBattery,
             []() { return new HomeScreenLayoutClassicWithBattery(layout::ClassicWithBattery); }},
            {layout::ClassicWithDate, []() { return new HomeScreenLayoutClassicWithDate(layout::ClassicWithDate); }},
            {layout::ClassicWithQuotes,
             []() { return new HomeScreenLayoutClassicWithQuotes(layout::ClassicWithQuotes); }},
            {layout::ClassicWithQuotesAmPm,
             []() { return new HomeScreenLayoutClassicWithQuotesAmPm(layout::ClassicWithQuotesAmPm); }},
            {layout::VerticalSimple, []() { return new HomeScreenLayoutVerticalSimple(layout::VerticalSimple); }},
            {layout::VerticalWithAmPm, []() { return new HomeScreenLayoutVerticalWithAmPm(layout::VerticalWithAmPm); }},
            {layout::VerticalWithDate,
             []() { return new HomeScreenLayoutVerticalWithDate(layout::VerticalWithDateAmPm); }}
#if CONFIG_ENABLE_TEMP == 1
            ,
            {layout::ClassicWithTemp, []() { return new HomeScreenLayoutClassicWithTemp(layout::ClassicWithTemp); }}
#endif
        };
    }

    std::map<std::string, LayoutGenerator> getAllLayouts()
    {
        auto allLayouts = getLayoutsFormat12h();
        auto layouts24h = getLayoutsFormat24h();

        allLayouts.merge(std::move(layouts24h));

        return allLayouts;
    }

    std::string getDefaultLayout()
    {
        return layout::ClassicWithBattery;
    }
} // namespace gui::factory
