// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

        tempText = new TextFixedSize(statusBox);
        tempText->setMaximumSize(style::homescreen_classic::temperature_w, style::homescreen_classic::temperature_h);
        tempText->setFont(mainWindow::bottomDescription::font_normal);
        tempText->setEdges(RectangleEdge::None);
        tempText->activeItem = false;
        tempText->drawUnderline(false);
    }

    auto HomeScreenLayoutClassicWithTemp::setTemperature(utils::temperature::Temperature newTemp) -> void
    {
        tempText->setText(utils::temperature::tempToStrDec(newTemp));
        statusBox->resizeItems();
    }
}; // namespace gui
