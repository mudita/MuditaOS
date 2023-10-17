// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ModemResetHandler.hpp"

#include <module-utils/log/Logger.hpp>

namespace cellular::service
{
    auto ModemResetHandler::performSoftReset() -> bool
    {
        if (procedureInProgress != ProcedureInProgress::None) {
            LOG_ERROR("Modem reset procedure is already in progress! Skip request.");
            return false;
        }

        procedureInProgress = ProcedureInProgress::SoftReset;
        onAnyReset();
        onCellularStateSet(State::ST::PowerDownWaiting);
        return onSoftReset();
    }

    auto ModemResetHandler::performHardReset() -> bool
    {
        if (procedureInProgress != ProcedureInProgress::None) {
            LOG_ERROR("Modem reset procedure is already in progress! Skip request.");
            return false;
        }

        procedureInProgress = ProcedureInProgress::HardReset;
        onAnyReset();
        onCellularStateSet(State::ST::PowerDownWaiting);
        onHardReset();
        return false;
    }

    auto ModemResetHandler::performReboot() -> bool
    {
        if (procedureInProgress != ProcedureInProgress::None) {
            LOG_ERROR("Modem reset procedure is already in progress! Skip request.");
            return false;
        }

        procedureInProgress = ProcedureInProgress::Reboot;
        onAnyReset();
        onCellularStateSet(State::ST::PowerDownWaiting);
        onTurnModemOff();
        return false;
    }

    auto ModemResetHandler::handleStatusPinEvent(bool isActive) -> bool
    {
        if (isActive) {
            return handleSwitchToActive();
        }
        return handleSwitchToInactive();
    }

    auto ModemResetHandler::isResetInProgress() -> bool
    {
        return procedureInProgress != ProcedureInProgress::None;
    }
    auto ModemResetHandler::handleSwitchToInactive() -> bool
    {
        switch (procedureInProgress) {
        case ProcedureInProgress::None:
            return false;
        case ProcedureInProgress::SoftReset:
        case ProcedureInProgress::HardReset:
            onCellularStateSet(State::ST::PowerDown);
            return true;
        case ProcedureInProgress::Reboot:
            auto constexpr delay50ms = 50;
            vTaskDelay(delay50ms);
            onTurnModemOn();
            onCellularStateSet(State::ST::PowerDown);
            return true;
        }
        return false;
    }

    auto ModemResetHandler::handleSwitchToActive() -> bool
    {
        if (procedureInProgress == ProcedureInProgress::None) {
            return false;
        }

        procedureInProgress = ProcedureInProgress::None;
        onCellularStateSet(State::ST::StatusCheck);
        return true;
    }

    auto ModemResetHandler::performFunctionalityReset() -> bool
    {
        onAnyReset();

        if (onFunctionalityReset) {
            return onFunctionalityReset();
        }
        return false;
    }
} // namespace cellular::service
