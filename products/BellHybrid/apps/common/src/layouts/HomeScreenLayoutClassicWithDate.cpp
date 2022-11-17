// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutClassicWithDate.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/DuoHBox.hpp"

#include <date/date.h>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    HomeScreenLayoutClassicWithDate::HomeScreenLayoutClassicWithDate(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {
        buildInterface();
    }

    void HomeScreenLayoutClassicWithDate::buildInterface()
    {
        using namespace bellMainStyle;

        date = new Text(nullptr);
        date->setMaximumSize(style::homescreen_classic::status_box_layout_w, style::bell_base_layout::outer_layouts_h);
        date->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        date->setFont(mainWindow::bottomDescription::font_normal);
        date->setEdges(RectangleEdge::None);
        date->activeItem = false;
        date->drawUnderline(false);
        date->setText("00/00");
        date->setMargins({0, 0, 20, 0});

        statusBox->removeWidget(battery);
        statusBox->addWidget(date);
        statusBox->addWidget(battery);

        // Assume format is 12h
        time->setMargins({style::homescreen_classic::ampm_time_margin_w, 0, 0, 0});

        ampm = new TextFixedSize(timeHBox);
        ampm->setMinimumSize(style::homescreen_classic::ampm_layout_w, style::bell_base_layout::center_layout_h);
        ampm->setMaximumSize(style::homescreen_classic::ampm_layout_w, style::bell_base_layout::center_layout_h);
        ampm->setMargins({style::homescreen_classic::ampm_margin_w, 0, 0, 0});
        ampm->setFont(mainWindow::time::font_small);
        ampm->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Top));
        ampm->setEdges(RectangleEdge::None);
        ampm->activeItem = false;
        ampm->drawUnderline(false);

        resizeItems();
    }

    void HomeScreenLayoutClassicWithDate::setTime(std::time_t newTime)
    {
        HomeScreenLayoutClassic::setTime(newTime);

        const auto t = std::localtime(&newTime);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << t->tm_mday;
        ss << '/';
        ss << std::setfill('0') << std::setw(2) << (t->tm_mon + 1);

        date->setText(ss.str());

        if (ampm->visible) {
            const auto hours = std::chrono::hours{t->tm_hour};
            const auto isPM  = date::is_pm(hours);
            ampm->setText(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
        }
    }

    void HomeScreenLayoutClassicWithDate::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        HomeScreenLayoutClassic::setTimeFormat(fmt);
        if (fmt == utils::time::Locale::TimeFormat::FormatTime12H) {
            time->setMargins({style::homescreen_classic::ampm_time_margin_w, 0, 0, 0});
            ampm->setVisible(true);
        }
        else {
            time->setMargins({0, 0, 0, 0});
            ampm->setVisible(false);
        }
    }

    bool HomeScreenLayoutClassicWithDate::isBatteryVisibilityAllowed(const Store::Battery &batteryContext)
    {
        return true;
    }

}; // namespace gui
