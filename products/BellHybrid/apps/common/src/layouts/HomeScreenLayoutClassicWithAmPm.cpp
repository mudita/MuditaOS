// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutClassicWithAmPm.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/DuoHBox.hpp"

#include <date/date.h>
#include <gui/widgets/text/TextFixedSize.hpp>

namespace gui
{
    HomeScreenLayoutClassicWithAmPm::HomeScreenLayoutClassicWithAmPm(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {
        buildInterface();
    }

    auto HomeScreenLayoutClassicWithAmPm::buildInterface() -> void
    {
        using namespace bellMainStyle;

        fmt = new TextFixedSize(nullptr, 0, 0, 0, 0);
        fmt->setMaximumSize(style::homescreen_classic::bottom_box_w, style::homescreen_classic::bottom_box_h);
        fmt->setFont(mainWindow::bottomDescription::font_normal);
        fmt->setEdges(RectangleEdge::None);
        fmt->activeItem = false;
        fmt->drawUnderline(false);
        fmt->setText("");

        statusBox->setItems(battery, fmt);
    }

    auto HomeScreenLayoutClassicWithAmPm::setTime(std::time_t newTime) -> void
    {
        HomeScreenLayoutClassic::setTime(newTime);

        const auto t     = std::localtime(&newTime);
        const auto hours = std::chrono::hours{t->tm_hour};
        const auto isPM  = date::is_pm(hours);
        fmt->setText(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
    }
}; // namespace gui
