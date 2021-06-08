// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthBar.hpp"
#include <string>
#include <Image.hpp>
#include <unordered_map>

namespace gui::status_bar
{
    namespace
    {
        constexpr auto signal_none = "signal_noconn";

        constexpr auto signal0 = "signal0";
        constexpr auto signal1 = "signal1";
        constexpr auto signal2 = "signal2";
        constexpr auto signal3 = "signal3";
        constexpr auto signal4 = "signal4";

        constexpr auto signal0_roaming = "signal0_roaming";
        constexpr auto signal1_roaming = "signal1_roaming";
        constexpr auto signal2_roaming = "signal2_roaming";
        constexpr auto signal3_roaming = "signal3_roaming";
        constexpr auto signal4_roaming = "signal4_roaming";

        using SignalMap           = std::unordered_map<Store::RssiBar, std::string>;

        const SignalMap signalMapHomeCon = {{Store::RssiBar::zero, signal0},
                                            {Store::RssiBar::one, signal1},
                                            {Store::RssiBar::two, signal2},
                                            {Store::RssiBar::three, signal3},
                                            {Store::RssiBar::four, signal4}};

        const SignalMap signalMapRoaming = {{Store::RssiBar::zero, signal0_roaming},
                                            {Store::RssiBar::one, signal1_roaming},
                                            {Store::RssiBar::two, signal2_roaming},
                                            {Store::RssiBar::three, signal3_roaming},
                                            {Store::RssiBar::four, signal4_roaming}};
    } // namespace

    SignalStrengthBar::SignalStrengthBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : SignalStrengthBase(parent, x, y, w, h)
    {
        img = new Image(this, signal_none);
        setMinimumSize(img->getWidth(), style::header::status_bar::height);
    }

    void SignalStrengthBar::update(const Store::SignalStrength &signal, const Store::Network::Status &status)
    {
        if (status == Store::Network::Status::RegisteredRoaming) {
            img->set(signalMapRoaming.at(signal.rssiBar));
        }
        else if (status == Store::Network::Status::RegisteredHomeNetwork) {
            img->set(signalMapHomeCon.at(signal.rssiBar));
        }
        else {
            img->set(signal_none);
        }
    }

} // namespace gui::status_bar
