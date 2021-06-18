// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthText.hpp"
#include "Label.hpp"
#include <EventStore.hpp>
#include <Utils.hpp>
#include "Style.hpp"

namespace gui::status_bar
{
    SignalStrengthText::SignalStrengthText(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : SignalStrengthBase(parent, x, y, w, h)
    {
        label = new Label(this, 0, 0, 0, 0);
        label->setFilled(false);
        label->setBorderColor(gui::ColorNoColor);
        label->setFont(style::status_bar::signal::font);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));

        setMinimumSize(style::status_bar::signal::maxX, style::status_bar::height);
        label->setMaximumSize(style::status_bar::signal::maxX, style::status_bar::height);
    }

    void SignalStrengthText::update(const Store::SignalStrength &signal,
                                    [[maybe_unused]] const Store::Network::Status &status)
    {
        label->setText(utils::to_string(signal.rssidBm) + " dBm");
    }

} // namespace gui::status_bar
