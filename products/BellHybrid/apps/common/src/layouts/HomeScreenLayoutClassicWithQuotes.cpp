// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutClassicWithQuotes.hpp"
#include "data/BellMainStyle.hpp"
// #include "widgets/BellBattery.hpp"
// #include "widgets/DuoHBox.hpp"

// #include <date/date.h>
// #include <gui/widgets/text/TextFixedSize.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

// #include <service-time/api/TimeSettingsApi.hpp>

namespace
{
    constexpr inline gui::Length maxTimeHeight{148U};
}; // namespace

namespace gui
{
    HomeScreenLayoutClassicWithQuotes::HomeScreenLayoutClassicWithQuotes(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {
        buildInterface();
    }

    void HomeScreenLayoutClassicWithQuotes::buildInterface()
    {
        using namespace bellMainStyle;

        timeHBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        time->setMaximumHeight(maxTimeHeight);
        time->setFont(style::window::font::huge);
        time->setMargins({mainWindow::ampm::time_margin_w, 0, 0, 0});

        resizeItems();
    }

    void HomeScreenLayoutClassicWithQuotes::setTime(std::time_t newTime)
    {
        HomeScreenLayoutClassic::setTime(newTime);
        // const auto t = std::localtime(&newTime);

        // if (ampm->visible) {
        //     const auto hours = std::chrono::hours{t->tm_hour};
        //     const auto isPM  = date::is_pm(hours);
        //     ampm->setText(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
        // }
    }

    void HomeScreenLayoutClassicWithQuotes::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        HomeScreenLayoutClassic::setTimeFormat(fmt);
        if (fmt == utils::time::Locale::TimeFormat::FormatTime12H) {
            time->setMargins({bellMainStyle::mainWindow::ampm::time_margin_w, 0, 0, 0});
            // ampm->setVisible(true);
        }
        else {
            time->setMargins({0, 0, 0, 0});
            // ampm->setVisible(false);
        }
    }

}; // namespace gui
