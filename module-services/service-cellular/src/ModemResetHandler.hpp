// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/State.hpp>

#include <functional>

namespace cellular::service
{

    class ModemResetHandler
    {

      public:
        /**
         * It performs modem reset via AT command
         * @return true on success, false on fail
         */
        auto performSoftReset() -> bool;
        /**
         * It performs modem reset via modem's Reset pin
         * @return
         */
        auto performHardReset() -> bool;
        /**
         * It performs power cycle modem's via PWR_KEY pin
         * @return
         */
        auto performReboot() -> bool;
        /**
         * It handles STATUS pin changed event when any reset procedure is in progress, other case it's doing nothing.
         * @param isActive STATUS pin value. True when STATUS is active, false when it's inactive
         * @return true when event is handled, false when not
         */
        auto handleStatusPinEvent(bool isActive) -> bool;
        /**
         * It checks if any reset procedure is in progress.
         * @return true when any reset procedure is in progress, false when not
         */
        auto isResetInProgress() -> bool;
        /**
         * User defined callbacks
         */
        std::function<void(cellular::service::State::ST)> onCellularStateSet;
        std::function<void()> onTurnModemOn;
        std::function<void()> onTurnModemOff;
        std::function<bool()> onSoftReset;
        std::function<void()> onHardReset;

      private:
        enum class ProcedureInProgress
        {
            None,
            SoftReset,
            HardReset,
            Reboot
        };

        ProcedureInProgress procedureInProgress = ProcedureInProgress::None;

        auto handleSwitchToInactive() -> bool;
        auto handleSwitchToActive() -> bool;
    };
} // namespace cellular::service
