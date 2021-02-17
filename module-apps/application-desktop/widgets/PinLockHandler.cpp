//// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PinLockHandler.hpp"
#include "PinHash.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-desktop/data/LockPhoneData.hpp"
#include "application-desktop/windows/Names.hpp"

#include <module-utils/common_data/EventStore.hpp>
#include <service-appmgr/service-appmgr/data/SimActionsParams.hpp>
#include <service-appmgr/service-appmgr/Controller.hpp>
#include <service-cellular/CellularMessage.hpp>

namespace gui
{
    namespace
    {
        constexpr unsigned int default_screen_pin_size = 4;
        constexpr unsigned int screen_nopin_size       = 0;
        constexpr unsigned int default_attempts        = 4;
        constexpr unsigned int blocked_sim_attempts    = 0;
        constexpr unsigned int sim_max_passcode_size   = 8;
        constexpr unsigned int sim_min_passcode_size   = 4;

        constexpr inline auto serviceCellular = "ServiceCellular";
    } // namespace

    PinLockHandler::PinLockHandler(app::ApplicationDesktop *app)
        : app(app),
          screenLock(
              Store::GSM::SIM::NONE, PinLock::LockState::PasscodeRequired, PinLock::LockType::Screen, default_attempts),
          simLock(Store::GSM::SIM::NONE, PinLock::LockState::Unlocked, PinLock::LockType::SimPin, default_attempts)
    {
        simLock.setPinSizeBounds(sim_min_passcode_size, sim_max_passcode_size);
        screenLock.setPinSizeBounds(default_screen_pin_size, default_screen_pin_size);
        screenLock.setAutoActivate(true);
    }

    void PinLockHandler::handleScreenPin(const std::vector<unsigned int> &pin)
    {
        const uint32_t hash = GetPinHash(pin);
        screenLock.value--;
        if (hash == app->getLockPassHash()) {
            screenLock.lockState = gui::PinLock::LockState::Unlocked;
            screenLock.value     = default_attempts;
        }
        else if (screenLock.value > 0) {
            screenLock.lockState = gui::PinLock::LockState::PasscodeInvalidRetryRequired;
        }
        else {
            screenLock.lockState = gui::PinLock::LockState::Blocked;
        }
        unlock();
    }

    void PinLockHandler::handlePasscodeParams(PinLock::LockType type,
                                              PinLock::LockState state,
                                              app::manager::actions::ActionParamsPtr &&data)
    {
        auto passcodeData = static_cast<app::manager::actions::PasscodeParams *>(data.get());
        if (simLock.isSim(passcodeData->getSim()) && simLock.isType(type) && simLock.isState(state) &&
            simLock.getValue() > passcodeData->getAttempts()) {
            simLock.lockState = PinLock::LockState::PasscodeInvalidRetryRequired;
        }
        else {
            simLock.lockState = state;
            simLock.sim       = passcodeData->getSim();
            simLock.lockType  = type;
        }
        simLock.value = passcodeData->getAttempts();
    }

    void PinLockHandler::handlePasscodeRequest(PinLock::LockType type, app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling on of PasscodeRequest actions");
        promptSimLockWindow = true;
        handlePasscodeParams(type, PinLock::LockState::PasscodeRequired, std::move(data));
        if (!getStrongestLock().isType(PinLock::LockType::Screen)) {
            unlock();
        }
    }

    void PinLockHandler::handlePinChangeRequest(app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling RequestPinChange action");
        handlePasscodeParams(PinLock::LockType::SimPin, PinLock::LockState::PasscodeRequired, std::move(data));
        promptSimLockWindow      = true;
        auto onActivatedCallback = [this](PinLock::LockType type, const std::vector<unsigned int> &data) {
            handlePasscodeChange(data);
        };
        switchToPinLockWindow(onActivatedCallback);
    }

    void PinLockHandler::handleSimBlocked(app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling BlockSim action");
        auto params         = static_cast<app::manager::actions::SimStateParams *>(data.get());
        promptSimLockWindow = true;
        simLock.sim         = params->getSim();
        simLock.lockState   = gui::PinLock::LockState::Blocked;
        simLock.lockType    = gui::PinLock::LockType::SimPin;
        simLock.value       = blocked_sim_attempts;
        unlock();
    }

    void PinLockHandler::handleUnlockSim(app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling UnlockSim action");
        promptSimLockWindow = false;
        if (!simLock.isState(PinLock::LockState::Unlocked)) {
            simLock.lockState = PinLock::LockState::Unlocked;
            unlock();
        }
    }

    void PinLockHandler::handleCMEError(app::manager::actions::ActionParamsPtr &&data) const
    {
        LOG_DEBUG("Handling DisplayCMEError action");
        auto params = static_cast<app::manager::actions::UnhandledCMEParams *>(data.get());
        auto lock   = std::make_unique<gui::PinLock>(
            params->getSim(), PinLock::LockState::ErrorOccurred, PinLock::LockType::SimPin, params->getCMECode());
        lock->onActivatedCallback = [this](PinLock::LockType type, const std::vector<unsigned int> &data) {
            app->switchWindow(app::window::name::desktop_main_window);
        };
        app->switchWindow(app::window::name::desktop_pin_lock,
                          gui::ShowMode::GUI_SHOW_INIT,
                          std::make_unique<gui::LockPhoneData>(std::move(lock)));
    }

    void PinLockHandler::switchToPinLockWindow(
        std::function<void(PinLock::LockType, const std::vector<unsigned int> &)> onLockActivatedCallback)
    {
        auto lock = std::make_unique<gui::PinLock>(getStrongestLock());
        if (lock->isState(PinLock::LockState::PasscodeInvalidRetryRequired)) {
            getStrongestLock().consumeState();
            lock->onActivatedCallback = [this, onLockActivatedCallback](PinLock::LockType,
                                                                        const std::vector<unsigned int> &) {
                switchToPinLockWindow(onLockActivatedCallback);
            };
        }
        else if (lock->isState(gui::PinLock::LockState::Blocked)) {
            lock->onActivatedCallback = [this](PinLock::LockType type, const std::vector<unsigned int> &data) {
                setSimLockHandled();
                app->switchWindow(app::window::name::desktop_main_window);
            };
        }
        else if (lock->isState(gui::PinLock::LockState::Unlocked)) {
            setSimLockHandled();
            app->switchWindow(app::window::name::desktop_main_window);
            app::manager::Controller::sendAction(app, app::manager::actions::HomeUnlocked);
            return;
        }
        else {
            lock->onActivatedCallback = onLockActivatedCallback;
        }
        app->switchWindow(app::window::name::desktop_pin_lock,
                          gui::ShowMode::GUI_SHOW_INIT,
                          std::make_unique<gui::LockPhoneData>(std::move(lock)));
    }

    void PinLockHandler::switchToPinLockWindow(
        PinLock::LockState state,
        std::function<void(PinLock::LockType, const std::vector<unsigned int> &)> onLockActivatedCallback)
    {
        auto lock                 = std::make_unique<gui::PinLock>(getStrongestLock());
        lock->lockState           = state;
        lock->onActivatedCallback = onLockActivatedCallback;
        app->switchWindow(app::window::name::desktop_pin_lock,
                          gui::ShowMode::GUI_SHOW_INIT,
                          std::make_unique<gui::LockPhoneData>(std::move(lock)));
    }

    void PinLockHandler::handlePasscode(PinLock::LockType type, const std::vector<unsigned int> passcode)
    {
        if (type == PinLock::LockType::SimPin) {
            setSimLockHandled();
            app->bus.sendUnicast(std::make_shared<CellularSimPinDataMessage>(simLock.sim, passcode), serviceCellular);
        }
        else if (type == PinLock::LockType::SimPuk) {
            handlePasscodeChange(passcode);
        }
        else if (type == PinLock::LockType::Screen) {
            handleScreenPin(passcode);
        }
    }

    void PinLockHandler::handlePasscodeChange(const std::vector<unsigned int> passcode)
    {
        auto onActivatedCallback = [this, passcode](PinLock::LockType, const std::vector<unsigned int> &pin) {
            handleNewPasscodeUnconfirmed(passcode, pin);
        };
        switchToPinLockWindow(PinLock::LockState::NewPasscodeRequired, onActivatedCallback);
    }

    void PinLockHandler::handleNewPasscodeUnconfirmed(const std::vector<unsigned int> &passcode,
                                                      const std::vector<unsigned int> &pin)
    {
        auto onActivatedCallback = [this, passcode, pin](PinLock::LockType type,
                                                         const std::vector<unsigned int> &pinConfirmed) {
            if (pin == pinConfirmed) {
                handleNewPasscodeConfirmed(type, passcode, pin);
            }
            else {
                handleNewPasscodeInvalid(passcode);
            }
        };
        switchToPinLockWindow(PinLock::LockState::NewPasscodeConfirmRequired, onActivatedCallback);
    }

    void PinLockHandler::handleNewPasscodeConfirmed(PinLock::LockType type,
                                                    const std::vector<unsigned int> &passcode,
                                                    const std::vector<unsigned int> &pin)
    {
        setSimLockHandled();
        if (type == PinLock::LockType::SimPin) {
            app->bus.sendUnicast(std::make_shared<CellularSimNewPinDataMessage>(simLock.sim, passcode, pin),
                                 serviceCellular);
        }
        else if (type == PinLock::LockType::SimPuk) {
            app->bus.sendUnicast(std::make_shared<CellularSimPukDataMessage>(simLock.sim, passcode, pin),
                                 serviceCellular);
        }
    }

    void PinLockHandler::handleNewPasscodeInvalid(const std::vector<unsigned int> &passcode)
    {
        auto onActivatedCallback = [this, passcode](PinLock::LockType type, const std::vector<unsigned int> &pin) {
            handlePasscodeChange(passcode);
        };
        switchToPinLockWindow(PinLock::LockState::NewPasscodeInvalid, onActivatedCallback);
    }
    void PinLockHandler::unlock()
    {
        auto onActivatedCallback = [this](PinLock::LockType type, const std::vector<unsigned int> &data) {
            handlePasscode(type, data);
        };
        switchToPinLockWindow(onActivatedCallback);
    }

    auto PinLockHandler::getStrongestLock() noexcept -> gui::PinLock &
    {
        if (!screenLock.isState(PinLock::LockState::Unlocked)) {
            return screenLock;
        }
        else if (promptSimLockWindow && !simLock.isState(PinLock::LockState::Unlocked)) {
            return simLock;
        }
        return screenLock;
    }

    void PinLockHandler::lockScreen()
    {
        screenLock.lockState = PinLock::LockState::PasscodeRequired;
    }

    void PinLockHandler::unlockScreen()
    {
        if (getStrongestLock().isType(PinLock::LockType::Screen) && !app->isLockScreenPasscodeOn()) {
            screenLock.lockState = gui::PinLock::LockState::Unlocked;
        }
        unlock();
    }

    void PinLockHandler::setSimLockHandled() noexcept
    {
        if (!getStrongestLock().isType(PinLock::LockType::Screen)) {
            promptSimLockWindow = false;
        }
    }

} // namespace gui
