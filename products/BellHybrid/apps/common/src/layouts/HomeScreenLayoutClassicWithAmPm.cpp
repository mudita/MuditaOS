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

        battery->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        fmt = new TextFixedSize(statusBox);
        fmt->setMaximumSize(style::homescreen_classic::status_box_layout_w, style::bell_base_layout::outer_layouts_h);
        fmt->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        fmt->setFont(mainWindow::bottomDescription::font_normal);
        fmt->setEdges(RectangleEdge::None);
        fmt->activeItem = false;
        fmt->drawUnderline(false);
        fmt->setText("");
    }

    auto HomeScreenLayoutClassicWithAmPm::setTime(std::time_t newTime) -> void
    {
        HomeScreenLayoutClassic::setTime(newTime);

        const auto t     = std::localtime(&newTime);
        const auto hours = std::chrono::hours{t->tm_hour};
        const auto isPM  = date::is_pm(hours);
        fmt->setText(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
        fmt->setMinimumWidthToFitText();
        fmt->setMaximumWidth(fmt->widgetMinimumArea.w);
    }

    bool HomeScreenLayoutClassicWithAmPm::isBatteryVisibilityAllowed(const Store::Battery &batteryContext)
    {
        return HomeScreenLayoutClassic::isBatteryVisibilityAllowed(batteryContext);
    }

    void HomeScreenLayoutClassicWithAmPm::handleContentChanged()
    {
        if (battery->visible) {
            fmt->setMargins({20, 0, 0, 0});
        }
        else {
            fmt->setMargins({0, 0, 0, 0});
        }
        HomeScreenLayoutClassic::handleContentChanged();
    }
}; // namespace gui
