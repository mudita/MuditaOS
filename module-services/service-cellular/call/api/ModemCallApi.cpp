// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ModemCallApi.hpp"
#include <modem/mux/CellularMux.h>
#include <stdexcept>
#include <service-cellular/ServiceCellular.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace cellular
{
    Api::Api(ServiceCellular *cellular, CellularMux *cmux) : cellular(cellular), cmux(cmux)
    {}

    bool Api::answerIncomingCall()
    {
        if (cmux == nullptr) {
            throw std::runtime_error("call api not initialized");
        }
        auto channel = cmux->get(CellularMux::Channel::Commands);
        if (channel != nullptr) {
            auto response = channel->cmd(at::AT::ATA);
            if (response) {
                return true;
            }
        }
        return false;
    }

    bool Api::hangupCall()
    {
        if (cmux == nullptr) {
            throw std::runtime_error("call api not initialized");
        }
        auto channel = cmux->get(CellularMux::Channel::Commands);
        return channel != nullptr && channel->cmd(at::AT::ATH);
    }

    bool Api::rejectCall()
    {
        if (cmux == nullptr) {
            throw std::runtime_error("call api not initialized");
        }
        auto channel = cmux->get(CellularMux::Channel::Commands);
        return channel != nullptr && channel->cmd(at::AT::CHUP);
    }

    bool Api::areCallsFromFavouritesEnabled()
    {
        if (cellular == nullptr) {
            throw std::runtime_error("call api not initialized");
        }

        return utils::getNumericValue<bool>(
            cellular->settings->getValue(settings::Offline::callsFromFavorites, settings::SettingsScope::Global));
    }

    sys::phone_modes::PhoneMode Api::getMode()
    {
        if (cellular == nullptr) {
            throw std::runtime_error("call api not initialized");
        }
        return cellular->phoneModeObserver->getCurrentPhoneMode();
    }

    sys::phone_modes::Tethering Api::getTethering()
    {
        if (cellular == nullptr) {
            throw std::runtime_error("call api not initialized");
        }
        return cellular->phoneModeObserver->isTetheringOn() ? sys::phone_modes::Tethering::On
                                                            : sys::phone_modes::Tethering::Off;
    }
} // namespace cellular
