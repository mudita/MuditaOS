// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PinLockHandler.hpp"
#include "PinHash.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include <service-cellular/ServiceCellular.hpp>

namespace gui
{
    constexpr unsigned int defaultScreenPinSize  = 4;
    constexpr unsigned int defaultScreenAttempts = 4;
    constexpr unsigned int maxSimPasscodesSize   = 10;
    constexpr unsigned int minSimPasscodesSize   = 4;

    PinLockHandler::PinLockHandler(app::ApplicationDesktop *app, SettingsRecord &settings)
        : app(app), appSettings(settings), lock(this)
    {
        reloadScreenLock();
    }

    auto PinLockHandler::handle(CellularSimResponseMessage *msg) -> bool
    {
        assert(msg);
        parseSimCard(msg);
        parseSimState(msg);
        parseAttemptsAndPinSize(msg);
        lock.additionalLockInfo[gui::PinLock::InfoName::PhoneNum] = msg->getPhoneNumber().getFormatted();

        auto currentWindow = app->getWindow(app::window::name::desktop_pin_lock);
        if (currentWindow != nullptr && currentWindow->getName() == gui::name::window::main_window) {
            currentWindow->rebuild();
        }
        app->switchWindow(gui::name::window::main_window);
        return true;
    }

    void PinLockHandler::handle(const std::vector<unsigned int> &pin)
    {
        if (lock.getLockType() == gui::PinLock::LockType::Screen) {
            handleScreenPin(pin);
        }
        else {
            handleSimPinOrPuk(pin);
        }
    }

    void PinLockHandler::parseSimState(CellularSimResponseMessage *msg)
    {
        assert(msg);
        switch (msg->getSimState()) {
        case CellularSimResponseMessage::SimState::SIMUnlocked:
            lock.type  = gui::PinLock::LockType::Screen;
            lock.state = gui::PinLock::State::EnterPin;
            break;
        case CellularSimResponseMessage::SimState::PINRequired:
            lock.type  = gui::PinLock::LockType::SIM;
            lock.state = gui::PinLock::State::EnterPin;
            break;
        case CellularSimResponseMessage::SimState::PINInvalidRetryPossible:
            lock.type  = gui::PinLock::LockType::SIM;
            lock.state = gui::PinLock::State::InvalidPin;
            break;
        case CellularSimResponseMessage::SimState::PUKRequired:
            lock.type  = gui::PinLock::LockType::PUK;
            lock.state = gui::PinLock::State::EnterPin;
            break;
        case CellularSimResponseMessage::SimState::PUKInvalidRetryPossible:
            lock.type  = gui::PinLock::LockType::PUK;
            lock.state = gui::PinLock::State::InvalidPin;
            break;
        case CellularSimResponseMessage::SimState::SIMBlocked:
            lock.type  = gui::PinLock::LockType::PUK;
            lock.state = gui::PinLock::State::Blocked;
            break;
        }
    }

    void PinLockHandler::parseSimCard(CellularSimResponseMessage *msg)
    {
        assert(msg);
        switch (msg->getSimCard()) {
        case CellularSimMessage::SimCard::SIM1:
            lock.additionalLockInfo[gui::PinLock::InfoName::LockName] = "SIM1";
            break;
        case CellularSimMessage::SimCard::SIM2:
            lock.additionalLockInfo[gui::PinLock::InfoName::LockName] = "SIM2";
            break;
        }
    }

    void PinLockHandler::parseAttemptsAndPinSize(CellularSimResponseMessage *msg)
    {
        if (msg->getSimState() == CellularSimResponseMessage::SimState::SIMUnlocked) {
            reloadScreenLock();
        }
        else {
            lock.pinValue          = std::vector<unsigned int>();
            lock.maxPinSize        = maxSimPasscodesSize;
            lock.minPinSize        = minSimPasscodesSize;
            lock.remainingAttempts = msg->getAttemptsLeft();
        }
    }

    void PinLockHandler::handleScreenPin(const std::vector<unsigned int> &pin)
    {
        if (lock.remainingAttempts > 0) {
            std::hash<std::vector<unsigned int>> hashEngine;
            uint32_t hash = hashEngine(pin);
            lock.remainingAttempts--;
            if (hash == appSettings.lockPassHash) {
                lock.remainingAttempts = defaultScreenAttempts;
                lock.state             = gui::PinLock::State::VerifiedPin;
            }
            else if (lock.remainingAttempts > 0) {
                lock.state = gui::PinLock::State::InvalidPin;
            }
            else {
                lock.state = gui::PinLock::State::Blocked;
            }

            app->switchWindow(app::window::name::desktop_pin_lock);
        }
    }

    void PinLockHandler::handleSimPinOrPuk(const std::vector<unsigned int> &pin)
    {
        auto sim = CellularSimMessage::SimCard::SIM1;
        sys::Bus::SendUnicast(
            std::make_shared<CellularSimVerifyPinRequestMessage>(sim, pin), ServiceCellular::serviceName, app);
    }

    void PinLockHandler::reloadScreenLock()
    {
        auto type            = gui::PinLock::LockType::Screen;
        auto state           = gui::PinLock::State::EnterPin;
        unsigned int pinSize = appSettings.lockPassHash == 0 ? 0 : defaultScreenPinSize;
        lock.reset(type, state, defaultScreenAttempts, pinSize, pinSize);
    }

} // namespace gui
