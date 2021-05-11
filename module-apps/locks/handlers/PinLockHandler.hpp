// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "locks/widgets/Lock.hpp"

#include <module-services/service-appmgr/service-appmgr/messages/ActionRequest.hpp>
#include <module-services/service-appmgr/service-appmgr/Actions.hpp>
#include <service-cellular/CellularMessage.hpp>

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

        void handlePasscode(Lock::LockType type, const std::vector<unsigned int> passcode);
        void handlePasscodeChange(const std::vector<unsigned int> passcode);
        void handleNewPasscodeUnconfirmed(const std::vector<unsigned int> &passcode,
                                          const std::vector<unsigned int> &pin);
        void handleNewPasscodeConfirmed(Lock::LockType type,
                                        const std::vector<unsigned int> &passcode,
                                        const std::vector<unsigned int> &pin);
        void handleNewPasscodeInvalid(const std::vector<unsigned int> &passcode);
        void handlePasscodeParams(Lock::LockType type,
                                  Lock::LockState state,
                                  app::manager::actions::ActionParamsPtr &&data);
        void switchToPinLockWindow(
            std::function<void(Lock::LockType, const std::vector<unsigned int> &)> onLockActivatedCallback);
        void switchToPinLockWindow(
            Lock::LockState type,
            std::function<void(Lock::LockType, const std::vector<unsigned int> &)> onLockActivatedCallback);

        auto getStrongestLock() noexcept -> gui::Lock &;
        void unlock();
        void setSimLockHandled() noexcept;

      public:
        PinLockHandler(app::ApplicationDesktop *app);

        void handlePasscodeRequest(Lock::LockType type, app::manager::actions::ActionParamsPtr &&data);
        void handlePinChangeRequest(app::manager::actions::ActionParamsPtr &&data);
        void handlePinEnableRequest(app::manager::actions::ActionParamsPtr &&data,
                                    CellularSimCardLockDataMessage::SimCardLock simCardLock);
        void handlePinEnableRequestFailed(CellularSimCardLockDataMessage::SimCardLock simCardLock);
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
