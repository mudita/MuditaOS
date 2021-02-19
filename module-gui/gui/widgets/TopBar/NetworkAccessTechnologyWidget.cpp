// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkAccessTechnologyWidget.hpp"
#include "Item.hpp"

namespace gui
{
    NetworkAccessTechnologyWidget::NetworkAccessTechnologyWidget(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h)
    {
        setFilled(false);
        setBorderColor(gui::ColorNoColor);
        setFont(style::header::font::modes);
        setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
    }

    void NetworkAccessTechnologyWidget::update(const Store::Network::AccessTechnology accessTechnology)
    {
        _accessTechnology      = accessTechnology;
        constexpr auto text2g  = "2G";
        constexpr auto text3g  = "3G";
        constexpr auto textLte = "LTE";

        switch (accessTechnology) {
        case Store::Network::AccessTechnology::Gsm:
        case Store::Network::AccessTechnology::GsmWEgprs:
            setText(text2g);
            break;
        case Store::Network::AccessTechnology::Utran:
        case Store::Network::AccessTechnology::UtranWHsdpa:
        case Store::Network::AccessTechnology::UtranWHsupa:
        case Store::Network::AccessTechnology::UtranWHsdpaAndWHsupa:
            setText(text3g);
            break;
        case Store::Network::AccessTechnology::EUtran:
            setText(textLte);
            break;
        case Store::Network::AccessTechnology::Unknown:
            setText("");
            break;
        }
    }
} // namespace gui
