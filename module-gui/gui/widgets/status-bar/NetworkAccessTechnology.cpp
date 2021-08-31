// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkAccessTechnology.hpp"
#include "Item.hpp"
#include "Style.hpp"

namespace gui::status_bar
{
    NetworkAccessTechnology::NetworkAccessTechnology(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h)
    {
        setEdges(RectangleEdge::None);
        setFont(style::status_bar::nat::font);
        setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
    }

    void NetworkAccessTechnology::update(const Store::Network::AccessTechnology accessTechnology)
    {
        setMaximumSize(this->getWidth(), this->getHeight());
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
        case Store::Network::AccessTechnology::Cdma:
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
} // namespace gui::status_bar
