// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutClassicWithQuotesAmPm.hpp"
#include "data/BellMainStyle.hpp"

#include <widgets/text/TextFixedSize.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    HomeScreenLayoutClassicWithQuotesAmPm::HomeScreenLayoutClassicWithQuotesAmPm(std::string name)
        : HomeScreenLayoutClassicWithQuotes(std::move(name))
    {
        buildInterface();
    }

    void HomeScreenLayoutClassicWithQuotesAmPm::buildInterface()
    {
        using namespace bellMainStyle;

        time->setMargins({mainWindow::ampm::time_margin_w, 0, 0, 0});

        ampm = new TextFixedSize(timeHBox);
        ampm->setMinimumSize(mainWindow::ampm::layout_w, style::bell_base_layout::center_layout_h);
        ampm->setMaximumSize(mainWindow::ampm::layout_w, style::bell_base_layout::center_layout_h);
        ampm->setMargins({mainWindow::ampm::margin_w, 0, 0, 0});
        ampm->setFont(mainWindow::time::font_small);
        ampm->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Top));
        ampm->setEdges(RectangleEdge::None);
        ampm->activeItem = false;
        ampm->drawUnderline(false);

        resizeItems();
    }

    void HomeScreenLayoutClassicWithQuotesAmPm::setTime(std::time_t newTime)
    {
        HomeScreenLayoutClassic::setTime(newTime);
        const auto t = std::localtime(&newTime);

        if (ampm->visible) {
            const auto hours = std::chrono::hours{t->tm_hour};
            const auto isPM  = date::is_pm(hours);
            ampm->setText(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
        }
    }

    void HomeScreenLayoutClassicWithQuotesAmPm::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        HomeScreenLayoutClassic::setTimeFormat(fmt);
        if (fmt == utils::time::Locale::TimeFormat::FormatTime12H) {
            time->setMargins({bellMainStyle::mainWindow::ampm::time_margin_w, 0, 0, 0});
            ampm->setVisible(true);
        }
        else {
            time->setMargins({0, 0, 0, 0});
            ampm->setVisible(false);
        }
    }
} // namespace gui
