// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "locks/widgets/Lock.hpp"

#include <module-services/service-appmgr/service-appmgr/messages/ActionRequest.hpp>
#include <module-services/service-appmgr/service-appmgr/Actions.hpp>
#include <service-cellular/api/common.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    using namespace locks;

    class PinLockHandler
    {
        app::ApplicationDesktop *app = nullptr;
        Lock screenLock;
        Lock simLock;
        bool promptSimLockWindow = true;

        void handleScreenPin(const cellular::api::PassCode &pin);
        void handlePasscode(Lock::LockType type, const cellular::api::PassCode &passcode);
        void handlePasscodeChange(const cellular::api::PassCode &passcode);
        void handleNewPasscodeUnconfirmed(const cellular::api::PassCode &passcode, const cellular::api::PassCode &pin);
        void handleNewPasscodeConfirmed(Lock::LockType type,
                                        const cellular::api::PassCode &passcode,
                                        const cellular::api::PassCode &pin);
        void handleNewPasscodeInvalid(const cellular::api::PassCode &passcode);
        void handlePasscodeParams(Lock::LockType type,
                                  Lock::LockState state,
                                  app::manager::actions::ActionParamsPtr &&data);
        void switchToPinLockWindow(
            std::function<void(Lock::LockType, const cellular::api::PassCode &)> onLockActivatedCallback);
        void switchToPinLockWindow(
            Lock::LockState type,
            std::function<void(Lock::LockType, const cellular::api::PassCode &)> onLockActivatedCallback);

        auto getStrongestLock() noexcept -> gui::Lock &;
        void unlock();
        void setSimLockHandled() noexcept;

      public:
        PinLockHandler(app::ApplicationDesktop *app);

        void handlePasscodeRequest(Lock::LockType type, app::manager::actions::ActionParamsPtr &&data);
        void handlePinChangeRequest(app::manager::actions::ActionParamsPtr &&data);
        void handlePinEnableRequest(app::manager::actions::ActionParamsPtr &&data,
                                    cellular::api::SimLockState simCardLock);
        void handlePinEnableRequestFailed(cellular::api::SimLockState simCardLock);
        void handleSimBlocked(app::manager::actions::ActionParamsPtr &&data);
        void handleUnlockSim(app::manager::actions::ActionParamsPtr &&data);
        void handleCMEError(app::manager::actions::ActionParamsPtr &&data) const;
        void handlePinChangeRequestFailed();

        [[nodiscard]] auto isScreenLocked() const noexcept -> bool
        {
            return !screenLock.isState(Lock::LockState::Unlocked);
        }
        [[nodiscard]] auto isScreenBlocked() const noexcept -> bool
        {
            return screenLock.isState(Lock::LockState::Blocked);
        }
        void lockScreen();
        void unlockScreen();
    };
} // namespace gui
