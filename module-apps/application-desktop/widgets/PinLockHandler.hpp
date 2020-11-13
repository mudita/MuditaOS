// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PinLock.hpp"

#include "Interface/SettingsRecord.hpp"

#include <module-services/service-appmgr/service-appmgr/messages/ActionRequest.hpp>
#include <module-services/service-appmgr/service-appmgr/Actions.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PinLockHandler
    {
        app::ApplicationDesktop *app = nullptr;
        const SettingsRecord &appSettings;

        auto parseSimCard(Store::GSM::SIM sim) const noexcept -> PinLock::SimCard;

        void handlePasscode(const std::vector<unsigned int> passcode) const;
        void handlePasscodeChange(const std::vector<unsigned int> passcode) const;
        void handleNewPasscodeConfirmed(const std::vector<unsigned int> &passcode,
                                        const std::vector<unsigned int> &pin) const;
        auto handlePasscodeParams(app::manager::actions::ActionParamsPtr &&data) const -> gui::PinLock;

        void switchToPinlockWindow(std::function<void(const std::vector<unsigned int> &)> onLockActivatedCallback);

      public:
        PinLockHandler(app::ApplicationDesktop *app, SettingsRecord &settings);
        void reloadScreenLock();

        void handleScreenPin(const std::vector<unsigned int> &pin);
        void handleScreenConfirm();
        void handlePinRequest(app::manager::actions::ActionParamsPtr &&data);
        void handlePukRequest(app::manager::actions::ActionParamsPtr &&data);
        void handlePinChangeRequest(app::manager::actions::ActionParamsPtr &&data);

        void handleSimBlocked(app::manager::actions::ActionParamsPtr &&data);
        void handleCMEError(app::manager::actions::ActionParamsPtr &&data) const;

        [[nodiscard]] bool isScreenLocked() const noexcept
        {
            return screenLock.getState() != PinLock::LockState::Unlocked;
        }
        void lockScreen() noexcept
        {
            screenLock.lockState = PinLock::LockState::PasscodeRequired;
        }

        gui::PinLock screenLock;
        gui::PinLock simLock;
    };
} // namespace gui
