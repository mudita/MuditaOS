// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "layouts/HomeScreenLayoutClassicWithTemp.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/DuoHBox.hpp"

#include <gui/widgets/text/TextFixedSize.hpp>

namespace gui
{
    HomeScreenLayoutClassicWithTemp::HomeScreenLayoutClassicWithTemp(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {
        buildInterface();
    }

    auto HomeScreenLayoutClassicWithTemp::buildInterface() -> void
    {
        using namespace bellMainStyle;

        tempText = new TextFixedSize(nullptr);
        tempText->setMaximumSize(style::homescreen_classic::temperature_w, style::homescreen_classic::temperature_h);
        tempText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        tempText->setFont(mainWindow::bottomDescription::font_normal);
        tempText->setEdges(RectangleEdge::None);
        tempText->activeItem = false;
        tempText->drawUnderline(false);
        tempText->setMargins(Margins(0, style::bell_base_layout::info_box_top_margin, 0, 0));

        infoBox->addWidget(tempText);
    }

    auto HomeScreenLayoutClassicWithTemp::setTemperature(utils::temperature::Temperature newTemp) -> void
    {
        tempText->setText(utils::temperature::tempToStrDec(newTemp));
        infoBox->resizeItems();
    }
} // namespace gui
