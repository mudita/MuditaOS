// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthWidgetBar.hpp"
#include <string>
#include <Image.hpp>
#include <unordered_map>

namespace gui
{
    namespace
    {
        constexpr auto signal0 = "signal0";
        constexpr auto signal1 = "signal1";
        constexpr auto signal2 = "signal2";
        constexpr auto signal3 = "signal3";
        constexpr auto signal4 = "signal4";
        constexpr auto signal5 = "signal5";

        using SignalMap           = std::unordered_map<Store::RssiBar, std::string>;
        const SignalMap signalMap = {{Store::RssiBar::zero, signal0},
                                     {Store::RssiBar::one, signal1},
                                     {Store::RssiBar::two, signal2},
                                     {Store::RssiBar::three, signal3},
                                     {Store::RssiBar::four, signal4},
                                     {Store::RssiBar::five, signal5}};
    } // namespace

    SignalStrengthWidgetBar::SignalStrengthWidgetBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : SignalStrengthWidgetBase(parent, x, y, w, h)
    {
        img                             = new Image(this, signalMap.at(Store::RssiBar::zero));
        constexpr auto signalBarMarginX = 10u;
        img->setMargins(gui::Margins(signalBarMarginX, 0, 0, 0));
    }

    void SignalStrengthWidgetBar::show(const Store::SignalStrength data, bool shown)
    {
        setVisible(shown);
        if (shown) {
            img->set(signalMap.at(data.rssiBar));
            return;
        }
    }

} // namespace gui
