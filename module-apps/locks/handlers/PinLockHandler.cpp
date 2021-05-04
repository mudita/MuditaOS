//// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "locks/data/LockData.hpp"
#include "PinLockHandler.hpp"
#include "locks/widgets/LockHash.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-desktop/windows/Names.hpp"
#include <module-utils/common_data/EventStore.hpp>
#include <service-appmgr/service-appmgr/data/SimActionsParams.hpp>

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
          screenLock(Store::GSM::SIM::NONE, Lock::LockState::InputRequired, Lock::LockType::Screen, default_attempts),
          simLock(Store::GSM::SIM::NONE, Lock::LockState::Unlocked, Lock::LockType::SimPin, default_attempts)
    {
        simLock.setInputSizeBounds(sim_min_passcode_size, sim_max_passcode_size);
        screenLock.setInputSizeBounds(default_screen_pin_size, default_screen_pin_size);
        screenLock.setAutoActivate(true);
    }

    void PinLockHandler::handleScreenPin(const std::vector<unsigned int> &pin)
    {
        const uint32_t hash = getHash(pin);
        screenLock.attemptsLeft--;
        if (hash == app->getLockPassHash()) {
            screenLock.lockState    = gui::Lock::LockState::Unlocked;
            screenLock.attemptsLeft = default_attempts;
        }
        else if (screenLock.attemptsLeft > 0) {
            screenLock.lockState = gui::Lock::LockState::InputInvalidRetryRequired;
        }
        else {
            screenLock.lockState = gui::Lock::LockState::Blocked;
        }
        unlock();
    }

    void PinLockHandler::handlePasscodeParams(Lock::LockType type,
                                              Lock::LockState state,
                                              app::manager::actions::ActionParamsPtr &&data)
    {
        auto passcodeData = static_cast<app::manager::actions::PasscodeParams *>(data.get());
        if (simLock.isSim(passcodeData->getSim()) && simLock.isType(type) && simLock.isState(state) &&
            simLock.getAttemptsLeft() > passcodeData->getAttempts()) {
            simLock.lockState = Lock::LockState::InputInvalidRetryRequired;
        }
        else {
            simLock.lockState = state;
            simLock.sim       = passcodeData->getSim();
            simLock.lockType  = type;
        }
        simLock.lockName     = passcodeData->getPasscodeName();
        simLock.attemptsLeft = passcodeData->getAttempts();
    }

    void PinLockHandler::handlePasscodeRequest(Lock::LockType type, app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling on of PasscodeRequest actions");
        promptSimLockWindow = true;
        handlePasscodeParams(type, Lock::LockState::InputRequired, std::move(data));
        if (!getStrongestLock().isType(Lock::LockType::Screen)) {
            unlock();
        }
    }

    void PinLockHandler::handlePinChangeRequest(app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling RequestPinChange action");
        handlePasscodeParams(Lock::LockType::SimPin, Lock::LockState::InputRequired, std::move(data));
        promptSimLockWindow      = true;
        auto onActivatedCallback = [this](Lock::LockType type, const std::vector<unsigned int> &data) {
            handlePasscodeChange(data);
        };
        switchToPinLockWindow(onActivatedCallback);
    }

    void PinLockHandler::handlePinEnableRequest(app::manager::actions::ActionParamsPtr &&data,
                                                CellularSimCardLockDataMessage::SimCardLock simCardLock)
    {
        LOG_DEBUG("Handling PinEnableRequest action, simCardLock = %d", static_cast<int>(simCardLock));
        handlePasscodeParams(Lock::LockType::SimPin, Lock::LockState::InputRequired, std::move(data));
        promptSimLockWindow      = true;
        auto onActivatedCallback = [this, simCardLock](Lock::LockType type, const std::vector<unsigned int> &data) {
            app->bus.sendUnicast(
                std::make_shared<CellularSimCardLockDataMessage>(Store::GSM::get()->selected, simCardLock, data),
                serviceCellular);
        };
        switchToPinLockWindow(onActivatedCallback);
    }

    void PinLockHandler::handlePinEnableRequestFailed(CellularSimCardLockDataMessage::SimCardLock simCardLock)
    {
        LOG_DEBUG("Handling PinEnableRequestFailed action, simCardLock = %d, simLock.value = %u",
                  static_cast<int>(simCardLock),
                  simLock.attemptsLeft);
        using namespace app::manager::actions;
        if (simLock.attemptsLeft > 0) {
            --simLock.attemptsLeft;
        }
        else {
            LOG_ERROR("Number of attempts left is equal to zero before decrementation!");
        }
        if (simLock.attemptsLeft > 0) {
            simLock.lockState        = Lock::LockState::InputInvalidRetryRequired;
            auto onActivatedCallback = [this, simCardLock](Lock::LockType type, const std::vector<unsigned int> &data) {
                auto params = std::make_unique<PasscodeParams>(
                    Store::GSM::get()->selected, simLock.attemptsLeft, PasscodeParams::pinName);
                handlePinEnableRequest(std::move(params), simCardLock);
            };
            switchToPinLockWindow(Lock::LockState::InputInvalidRetryRequired, onActivatedCallback);
        }
        else {
            auto params = std::make_unique<PasscodeParams>(
                Store::GSM::get()->selected, PasscodeParams::numOfAttemptsForEnteringPUK, PasscodeParams::pukName);
            handlePasscodeRequest(gui::Lock::LockType::SimPuk, std::move(params));
        }
    }

    void PinLockHandler::handlePinChangeRequestFailed()
    {
        LOG_DEBUG("Handling PinChangeRequestFailed action, simLock.value = %u", simLock.attemptsLeft);
        using namespace app::manager::actions;
        if (simLock.attemptsLeft > 0) {
            --simLock.attemptsLeft;
        }
        else {
            LOG_ERROR("Number of attempts left is equal to zero before decrementation!");
        }
        if (simLock.attemptsLeft > 0) {
            simLock.lockState        = Lock::LockState::InputInvalidRetryRequired;
            auto onActivatedCallback = [this](Lock::LockType type, const std::vector<unsigned int> &data) {
                auto params = std::make_unique<PasscodeParams>(
                    Store::GSM::get()->selected, simLock.attemptsLeft, PasscodeParams::pinName);
                handlePinChangeRequest(std::move(params));
            };
            switchToPinLockWindow(Lock::LockState::InputInvalidRetryRequired, onActivatedCallback);
        }
        else {
            auto params = std::make_unique<PasscodeParams>(
                Store::GSM::get()->selected, PasscodeParams::numOfAttemptsForEnteringPUK, PasscodeParams::pukName);
            handlePasscodeRequest(gui::Lock::LockType::SimPuk, std::move(params));
        }
    }

    void PinLockHandler::handleSimBlocked(app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling BlockSim action");
        auto params          = static_cast<app::manager::actions::SimStateParams *>(data.get());
        promptSimLockWindow  = true;
        simLock.sim          = params->getSim();
        simLock.lockState    = gui::Lock::LockState::Blocked;
        simLock.lockType     = gui::Lock::LockType::SimPin;
        simLock.attemptsLeft = blocked_sim_attempts;
        unlock();
    }

    void PinLockHandler::handleUnlockSim(app::manager::actions::ActionParamsPtr &&data)
    {
        LOG_DEBUG("Handling UnlockSim action");
        promptSimLockWindow = false;
        if (!simLock.isState(Lock::LockState::Unlocked)) {
            simLock.lockState = Lock::LockState::Unlocked;
            unlock();
        }
    }

    void PinLockHandler::handleCMEError(app::manager::actions::ActionParamsPtr &&data) const
    {
        LOG_DEBUG("Handling DisplayCMEError action");
        auto params = static_cast<app::manager::actions::UnhandledCMEParams *>(data.get());
        auto lock   = std::make_unique<gui::Lock>(
            params->getSim(), Lock::LockState::ErrorOccurred, Lock::LockType::SimPin, params->getCMECode());
        lock->onActivatedCallback = [this](Lock::LockType type, const std::vector<unsigned int> &data) {
            app->switchWindow(app::window::name::desktop_main_window);
        };
        app->switchWindow(
            app::window::name::desktop_pin_lock, gui::ShowMode::GUI_SHOW_INIT, std::make_unique<gui::LockData>(*lock));
    }

    void PinLockHandler::switchToPinLockWindow(
        std::function<void(Lock::LockType, const std::vector<unsigned int> &)> onLockActivatedCallback)
    {
        auto lock = std::make_unique<gui::Lock>(getStrongestLock());
        if (lock->isState(Lock::LockState::InputInvalidRetryRequired)) {
            getStrongestLock().consumeState();
            lock->onActivatedCallback = [this, onLockActivatedCallback](Lock::LockType,
                                                                        const std::vector<unsigned int> &) {
                switchToPinLockWindow(onLockActivatedCallback);
            };
        }
        else if (lock->isState(gui::Lock::LockState::Blocked)) {
            lock->onActivatedCallback = [this](Lock::LockType type, const std::vector<unsigned int> &data) {
                setSimLockHandled();
                app->switchWindow(app::window::name::desktop_main_window);
            };
        }
        else if (lock->isState(gui::Lock::LockState::Unlocked)) {
            setSimLockHandled();
            app->switchWindow(app::window::name::desktop_main_window);
            return;
        }
        else {
            lock->onActivatedCallback = onLockActivatedCallback;
        }
        app->switchWindow(
            app::window::name::desktop_pin_lock, gui::ShowMode::GUI_SHOW_INIT, std::make_unique<gui::LockData>(*lock));
    }

    void PinLockHandler::switchToPinLockWindow(
        Lock::LockState state,
        std::function<void(Lock::LockType, const std::vector<unsigned int> &)> onLockActivatedCallback)
    {
        auto lock                 = std::make_unique<gui::Lock>(getStrongestLock());
        lock->lockState           = state;
        lock->onActivatedCallback = onLockActivatedCallback;
        app->switchWindow(
            app::window::name::desktop_pin_lock, gui::ShowMode::GUI_SHOW_INIT, std::make_unique<gui::LockData>(*lock));
    }

    void PinLockHandler::handlePasscode(Lock::LockType type, const std::vector<unsigned int> passcode)
    {
        if (type == Lock::LockType::SimPin) {
            setSimLockHandled();
            app->bus.sendUnicast(std::make_shared<CellularSimPinDataMessage>(simLock.sim, passcode), serviceCellular);
        }
        else if (type == Lock::LockType::SimPuk) {
            handlePasscodeChange(passcode);
        }
        else if (type == Lock::LockType::Screen) {
            handleScreenPin(passcode);
        }
    }

    void PinLockHandler::handlePasscodeChange(const std::vector<unsigned int> passcode)
    {
        auto onActivatedCallback = [this, passcode](Lock::LockType, const std::vector<unsigned int> &pin) {
            handleNewPasscodeUnconfirmed(passcode, pin);
        };
        switchToPinLockWindow(Lock::LockState::NewInputRequired, onActivatedCallback);
    }

    void PinLockHandler::handleNewPasscodeUnconfirmed(const std::vector<unsigned int> &passcode,
                                                      const std::vector<unsigned int> &pin)
    {
        auto onActivatedCallback = [this, passcode, pin](Lock::LockType type,
                                                         const std::vector<unsigned int> &pinConfirmed) {
            if (pin == pinConfirmed) {
                handleNewPasscodeConfirmed(type, passcode, pin);
            }
            else {
                handleNewPasscodeInvalid(passcode);
            }
        };
        switchToPinLockWindow(Lock::LockState::NewInputConfirmRequired, onActivatedCallback);
    }

    void PinLockHandler::handleNewPasscodeConfirmed(Lock::LockType type,
                                                    const std::vector<unsigned int> &passcode,
                                                    const std::vector<unsigned int> &pin)
    {
        if (type == Lock::LockType::SimPin) {
            app->bus.sendUnicast(std::make_shared<CellularSimNewPinDataMessage>(simLock.sim, passcode, pin),
                                 serviceCellular);
        }
        else if (type == Lock::LockType::SimPuk) {
            app->bus.sendUnicast(std::make_shared<CellularSimPukDataMessage>(simLock.sim, passcode, pin),
                                 serviceCellular);
        }
    }

    void PinLockHandler::handleNewPasscodeInvalid(const std::vector<unsigned int> &passcode)
    {
        auto onActivatedCallback = [this, passcode](Lock::LockType type, const std::vector<unsigned int> &pin) {
            handlePasscodeChange(passcode);
        };
        switchToPinLockWindow(Lock::LockState::NewInputInvalid, onActivatedCallback);
    }
    void PinLockHandler::unlock()
    {
        auto onActivatedCallback = [this](Lock::LockType type, const std::vector<unsigned int> &data) {
            handlePasscode(type, data);
        };
        switchToPinLockWindow(onActivatedCallback);
    }

    auto PinLockHandler::getStrongestLock() noexcept -> gui::Lock &
    {
        if (!screenLock.isState(Lock::LockState::Unlocked)) {
            return screenLock;
        }
        else if (promptSimLockWindow && !simLock.isState(Lock::LockState::Unlocked)) {
            return simLock;
        }
        return screenLock;
    }

    void PinLockHandler::lockScreen()
    {
        screenLock.lockState = Lock::LockState::InputRequired;
    }

    void PinLockHandler::unlockScreen()
    {
        if (getStrongestLock().isType(Lock::LockType::Screen)) {
            screenLock.lockState = gui::Lock::LockState::Unlocked;
        }
    }

    void PinLockHandler::setSimLockHandled() noexcept
    {
        if (!getStrongestLock().isType(Lock::LockType::Screen)) {
            promptSimLockWindow = false;
        }
    }

} // namespace gui
