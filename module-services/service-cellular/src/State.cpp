// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-cellular/State.hpp>
#include <Service/Service.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-cellular/api/common.hpp>

namespace cellular::service
{
    const char *State::c_str(State::ST state)
    {
        switch (state) {
        case ST::Idle:
            return "Idle";
        case ST::WaitForStartPermission:
            return "WaitForStartPermission";
        case ST::PowerUpRequest:
            return "PowerUpRequest";
        case ST::StatusCheck:
            return "StatusCheck";
        case ST::PowerUpInProgress:
            return "PowerUpInProgress";
        case ST::PowerUpProcedure:
            return "PowerUpProcedure";
        case ST::BaudDetect:
            return "BaudDetect";
        case ST::AudioConfigurationProcedure:
            return "AudioConfigurationProcedure";
        case ST::CellularPrivInit:
            return "CellularPrivInit";
        case ST::APNConfProcedure:
            return "APNConfProcedure";
        case ST::ModemOn:
            return "ModemOn";
        case ST::URCReady:
            return "URCReady";
        case ST::Failed:
            return "Failed";
        case ST::SanityCheck:
            return "SanityCheck";
        case ST::ModemFatalFailure:
            return "ModemFatalFailure";
        case ST::CellularConfProcedure:
            return "CellularStartConfProcedure";
        case ST::Ready:
            return "Ready";
        case ST::PowerDownStarted:
            return "PowerDownStarted";
        case ST::PowerDownWaiting:
            return "PowerDownWaiting";
        case ST::PowerDown:
            return "PowerDown";
        }
        return "";
    }

    State::State(sys::Service *owner) : owner(owner)
    {}

    const char *State::c_str() const
    {
        return State::c_str(state);
    }

    void State::set(ST state)
    {
        LOG_DEBUG("GSM state: (%s) -> (%s)", c_str(this->state), c_str(state));
        this->state = state;
        auto msg    = std::make_shared<StateChange>(state);
        owner->bus.sendUnicast(msg, cellular::service::name);
    }

    State::ST State::get() const
    {
        return this->state;
    }
} // namespace cellular::service
