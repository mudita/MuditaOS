//// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PinLockHandler.hpp"
#include "PinHash.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-desktop/data/LockPhoneData.hpp"
#include "application-desktop/windows/Names.hpp"

#include <module-utils/common_data/EventStore.hpp>

#include <service-appmgr/service-appmgr/data/SimActionsParams.hpp>
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
    } // namespace

    PinLockHandler::PinLockHandler(app::ApplicationDesktop *app, SettingsRecord &settings)
        : app(app), appSettings(settings), screenLock(PinLock::SimCard::NoCard,
                                                      PinLock::LockState::PasscodeRequired,
                                                      PinLock::LockType::Screen,
                                                      default_attempts),
          simLock(PinLock::SimCard::NoCard, PinLock::LockState::Unlocked, PinLock::LockType::SimPin, default_attempts)
    {
        reloadScreenLock();
        screenLock.setAutoActivate(true);

        simLock.setPinSizeBounds(sim_min_passcode_size, sim_max_passcode_size);
    }

    void PinLockHandler::handleScreenPin(const std::vector<unsigned int> &pin)
    {
        if (screenLock.getMaxPinSize() == screen_nopin_size) {
            screenLock.lockState = gui::PinLock::LockState::Unlocked;
        }
        else if (screenLock.value > 0) {
            std::hash<std::vector<unsigned int>> hashEngine;
            uint32_t hash = hashEngine(pin);
            screenLock.value--;
            if (hash == appSettings.lockPassHash) {
                screenLock.lockState = gui::PinLock::LockState::Unlocked;
                screenLock.value     = default_attempts;
            }
            else if (screenLock.value > 0) {
                screenLock.lockState = gui::PinLock::LockState::PasscodeInvalidRetryRequired;
            }
            else {
                screenLock.lockState = gui::PinLock::LockState::Blocked;
            }
        }
    }

    void PinLockHandler::handleScreenConfirm()
    {
        simLock.consumeState();
    }

    void PinLockHandler::reloadScreenLock()
    {
        screenLock.lockState = gui::PinLock::LockState::PasscodeRequired;
        unsigned int pinSize = appSettings.lockPassHash == 0 ? screen_nopin_size : default_screen_pin_size;
        screenLock.setPinSizeBounds(pinSize, pinSize);
        screenLock.value = default_attempts;
    }

    auto PinLockHandler::handlePasscodeParams(app::manager::actions::ActionParamsPtr &&data) const -> gui::PinLock
    {
        auto passcodeData = static_cast<app::manager::actions::PasscodeParams *>(data.get());

        gui::PinLock lock(parseSimCard(passcodeData->getSim()),
                          gui::PinLock::LockState::PasscodeRequired,
                          simLock.getLockType(),
                          passcodeData->getAttempts());

        if (simLock.getLockType() == lock.getLockType() && simLock.getState() == lock.getState() &&
            simLock.getValue() > lock.getValue()) {
            lock.lockState = PinLock::LockState::PasscodeInvalidRetryRequired;
        }
        lock.setPinSizeBounds(sim_min_passcode_size, sim_max_passcode_size);

        return lock;
    }

    void PinLockHandler::handlePinRequest(app::manager::actions::ActionParamsPtr &&data)
    {
        simLock          = handlePasscodeParams(std::move(data));
        simLock.lockType = gui::PinLock::LockType::SimPin;

        auto onActivatedCallback = [this](const std::vector<unsigned int> &data) { handlePasscode(data); };
        switchToPinlockWindow(onActivatedCallback);
    }

    void PinLockHandler::handlePukRequest(app::manager::actions::ActionParamsPtr &&data)
    {
        simLock          = handlePasscodeParams(std::move(data));
        simLock.lockType = gui::PinLock::LockType::SimPuk;

        auto onActivatedCallback = [this](const std::vector<unsigned int> &data) { handlePasscodeChange(data); };
        switchToPinlockWindow(onActivatedCallback);
    }

    void PinLockHandler::switchToPinlockWindow(
        std::function<void(const std::vector<unsigned int> &)> onLockActivatedCallback)
    {
        auto lock = std::make_unique<gui::PinLock>(simLock);

        if (lock->getState() == PinLock::LockState::PasscodeInvalidRetryRequired) {
            lock->onActivatedCallback = [this, onLockActivatedCallback](const std::vector<unsigned int> &) {
                simLock.consumeState();
                switchToPinlockWindow(onLockActivatedCallback);
            };
        }
        else {
            lock->onActivatedCallback = onLockActivatedCallback;
        }

        app->switchWindow(app::window::name::desktop_pin_lock,
                          gui::ShowMode::GUI_SHOW_INIT,
                          std::make_unique<gui::LockPhoneData>(std::move(lock)));
    }

    void PinLockHandler::handlePinChangeRequest(app::manager::actions::ActionParamsPtr &&data)
    {
        simLock          = handlePasscodeParams(std::move(data));
        simLock.lockType = gui::PinLock::LockType::SimPin;

        auto onActivatedCallback = [this](const std::vector<unsigned int> &data) { handlePasscodeChange(data); };
        switchToPinlockWindow(onActivatedCallback);
    }

    void PinLockHandler::handleSimBlocked(app::manager::actions::ActionParamsPtr &&data)
    {
        auto params = static_cast<app::manager::actions::SimStateParams *>(data.get());
        simLock     = PinLock(parseSimCard(params->getSim()),
                          gui::PinLock::LockState::Blocked,
                          gui::PinLock::LockType::SimPin,
                          blocked_sim_attempts);

        auto lock = std::make_unique<gui::PinLock>(simLock);
        app->switchWindow(app::window::name::desktop_pin_lock,
                          gui::ShowMode::GUI_SHOW_INIT,
                          std::make_unique<gui::LockPhoneData>(std::move(lock)));
    }

    void PinLockHandler::handleCMEError(app::manager::actions::ActionParamsPtr &&data) const
    {
        auto params = static_cast<app::manager::actions::UnhandledCMEParams *>(data.get());

        auto lock = std::make_unique<gui::PinLock>(parseSimCard(params->getSim()),
                                                   PinLock::LockState::ErrorOccurred,
                                                   PinLock::LockType::SimPin,
                                                   params->getCMECode());

        app->switchWindow(app::window::name::desktop_pin_lock,
                          gui::ShowMode::GUI_SHOW_INIT,
                          std::make_unique<gui::LockPhoneData>(std::move(lock)));
    }

    void PinLockHandler::handlePasscode(const std::vector<unsigned int> passcode) const
    {
        if (simLock.getLockType() == PinLock::LockType::SimPin) {
            sys::Bus::SendUnicast(
                std::make_shared<CellularSimPinDataMessage>(Store::GSM::SIM::SIM1, passcode), "ServiceCellular", app);
        }
        else if (simLock.getLockType() == PinLock::LockType::SimPuk) {
            handlePasscodeChange(passcode);
        }
    }
    void PinLockHandler::handlePasscodeChange(const std::vector<unsigned int> passcode) const
    {
        auto lock       = std::make_unique<gui::PinLock>(simLock);
        lock->lockState = PinLock::LockState::NewPasscodeRequired;

        lock->onActivatedCallback = [this, passcode](const std::vector<unsigned int> &pin) {
            handleNewPasscodeConfirmed(passcode, pin);
        };

        app->switchWindow(app::window::name::desktop_pin_lock,
                          gui::ShowMode::GUI_SHOW_INIT,
                          std::make_unique<gui::LockPhoneData>(std::move(lock)));
    }

    void PinLockHandler::handleNewPasscodeConfirmed(const std::vector<unsigned int> &passcode,
                                                    const std::vector<unsigned int> &pin) const
    {
        if (simLock.getLockType() == PinLock::LockType::SimPin) {
            sys::Bus::SendUnicast(std::make_shared<CellularSimNewPinDataMessage>(Store::GSM::SIM::SIM1, passcode, pin),
                                  "ServiceCellular",
                                  app);
        }
        else if (simLock.getLockType() == PinLock::LockType::SimPuk) {
            sys::Bus::SendUnicast(std::make_shared<CellularSimPukDataMessage>(Store::GSM::SIM::SIM1, passcode, pin),
                                  "ServiceCellular",
                                  app);
        }
    }

    auto PinLockHandler::parseSimCard(Store::GSM::SIM sim) const noexcept -> PinLock::SimCard
    {
        if (sim == Store::GSM::SIM::SIM1) {
            return gui::PinLock::SimCard::SIM1;
        }
        else if (sim == Store::GSM::SIM::SIM2) {
            return gui::PinLock::SimCard::SIM2;
        }
        return gui::PinLock::SimCard::NoCard;
    }

} // namespace gui
