// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/service-appmgr/Actions.hpp>
#include <module-utils/common_data/EventStore.hpp>

namespace app::manager::actions
{
    /** Action parameters for
     * RequestPinAction
     * RequestPukAction
     * ChangePinAction
     */
    class PasscodeParams : public ActionParams
    {
        Store::GSM::SIM sim;
        unsigned int attempts;
        /// passcodeName stands for PIN1/PIN2 or PUK1/PUK2 type
        std::string passcodeName;

      public:
        PasscodeParams(Store::GSM::SIM _sim, unsigned int _attempts, std::string _passcodeName);

        [[nodiscard]] Store::GSM::SIM getSim() const noexcept;
        [[nodiscard]] unsigned int getAttempts() const noexcept;
        [[nodiscard]] const std::string &getPasscodeName() const noexcept;

        static constexpr auto numOfAttemptsForEnteringPIN = 3u;
        static constexpr auto numOfAttemptsForEnteringPUK = 10u;

        static constexpr auto pinName = "PIN";
        static constexpr auto pukName = "PUK";
    };

    /** Action parameters for
     * SimUnlockedAction
     * SimBlockedAction
     */
    class SimStateParams : public ActionParams
    {
        Store::GSM::SIM sim;

      public:
        explicit SimStateParams(Store::GSM::SIM _sim);

        [[nodiscard]] Store::GSM::SIM getSim() const noexcept;
    };

    /** Action parameters for
     * UnhandledCMEError
     */
    class UnhandledCMEParams : public ActionParams
    {
        Store::GSM::SIM sim;
        unsigned int cmeCode;

      public:
        UnhandledCMEParams(Store::GSM::SIM _sim, unsigned int _cmeCode);

        [[nodiscard]] Store::GSM::SIM getSim() const noexcept;
        [[nodiscard]] unsigned int getCMECode() const noexcept;
    };
} // namespace app::manager::actions
