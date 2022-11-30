// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ModemCallApi.hpp"
#include <modem/mux/CellularMux.h>
#include <module-cellular/at/Commands.hpp>
#include <stdexcept>
#include <service-cellular/ServiceCellular.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace cellular
{
    Api::Api(ServiceCellular *cellular) : cellular{cellular}
    {}

    bool Api::handleEvent(at::AT modemCommand)
    {
        if (!cellular->cmux) {
            LOG_INFO("no cmux at this time - ignoring request");
            return false;
        }

        auto channel = cellular->cmux->get(CellularMux::Channel::Commands);
        if (!channel) {
            LOG_INFO("no cmux command channel at this time - ignoring request");
            return false;
        }

        if (!channel->cmd(modemCommand)) {
            LOG_WARN("AT+%s command failed - ignoring request", magic_enum::enum_name(modemCommand).data());
            return false;
        }

        return true;
    }

    bool Api::answerIncomingCall()
    {
        return handleEvent(at::AT::ATA);
    }

    bool Api::hangupCall()
    {
        return handleEvent(at::AT::ATH);
    }

    bool Api::rejectCall()
    {
        return handleEvent(at::AT::CHUP);
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
