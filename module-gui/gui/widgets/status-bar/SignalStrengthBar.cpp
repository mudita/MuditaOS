// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthBar.hpp"
#include "Style.hpp"
#include <string>
#include <Image.hpp>
#include <map>
#include <magic_enum.hpp>

namespace gui::status_bar
{
    namespace
    {
        constexpr auto signal_none = "gsm_0_noconnection";

        constexpr auto signal0 = "gsm_0";
        constexpr auto signal1 = "gsm_1";
        constexpr auto signal2 = "gsm_2";
        constexpr auto signal3 = "gsm_3";
        constexpr auto signal4 = "gsm_4";

        constexpr auto signal0_roaming = "gsm_0_roaming";
        constexpr auto signal1_roaming = "gsm_1_roaming";
        constexpr auto signal2_roaming = "gsm_2_roaming";
        constexpr auto signal3_roaming = "gsm_3_roaming";
        constexpr auto signal4_roaming = "gsm_4_roaming";

        using SignalMap = std::map<Store::RssiBar, std::string>;

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
        img = new Image(this, signal_none, style::status_bar::imageTypeSpecifier);
        setMinimumSize(img->getWidth(), style::status_bar::height);
    }

    bool SignalStrengthBar::update(const Store::SignalStrength &signal,
                                   const Store::Network::Status &status,
                                   const Store::Tethering &tethering)
    {
        try {
            if (img == nullptr) {
                LOG_ERROR("SignalStrength image is nullptr");
                return false;
            }

            /* Skip update if nothing has changed */
            if ((currentTethering == tethering) && (currentStatus == status) && (currentRssiBar == signal.rssiBar)) {
                return false;
            }

            if (tethering == Store::Tethering::On) {
                img->set(signal_none, style::status_bar::imageTypeSpecifier);
                currentTethering = tethering;
            }
            else if (status == Store::Network::Status::RegisteredRoaming) {
                img->set(signalMapRoaming.at(signal.rssiBar), style::status_bar::imageTypeSpecifier);
                currentStatus  = status;
                currentRssiBar = signal.rssiBar;
            }
            else if (status == Store::Network::Status::RegisteredHomeNetwork) {
                img->set(signalMapHomeCon.at(signal.rssiBar), style::status_bar::imageTypeSpecifier);
                currentStatus  = status;
                currentRssiBar = signal.rssiBar;
            }
            else {
                img->set(signal_none, style::status_bar::imageTypeSpecifier);
                currentTethering = tethering;
                currentStatus    = status;
                currentRssiBar   = signal.rssiBar;
            }
            return true;
        }
        catch (const std::exception &exception) {
            LOG_ERROR("Exception while updating signal image: %s", exception.what());
        }

        return false;
    }
} // namespace gui::status_bar
