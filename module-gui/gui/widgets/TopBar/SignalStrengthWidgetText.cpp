// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthWidgetText.hpp"
#include "Label.hpp"
#include "common_data/EventStore.hpp"
#include <Utils.hpp>
#include <Style.hpp>

namespace gui
{
    SignalStrengthWidgetText::SignalStrengthWidgetText(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : SignalStrengthWidgetBase(parent, x, y, w, h)
    {
        label = new Label(this, 0, 0, 0, 0);
        label->setFilled(false);
        label->setBorderColor(gui::ColorNoColor);
        label->setFont(style::header::font::modes);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        label->setMaximumSize(this->getWidth(), this->getHeight());
    }

    void SignalStrengthWidgetText::update()
    {
        label->setText(utils::to_string(signalStrength.rssidBm) + " dBm");
    }

} // namespace gui
