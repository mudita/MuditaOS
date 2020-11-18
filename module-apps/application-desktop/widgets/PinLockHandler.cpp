// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PinLockHandler.hpp"
#include "PinHash.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include <service-cellular/ServiceCellular.hpp>
#include <module-utils/Utils.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>

namespace gui
{
    constexpr unsigned int default_screen_pin_size = 4;
    constexpr unsigned int default_screen_attempts = 4;

    PinLockHandler::PinLockHandler(app::ApplicationDesktop *app) : app(app), lock(this)
    {
        reloadScreenLock();
        app->settings->registerValueChange(
            ::Settings::SystemProperties::lockPassHash,
            [this](const std::string &name, std::optional<std::string> value) { lockPassHashChanged(name, value); });
    }

    auto PinLockHandler::handle(CellularSimResponseMessage *msg) -> bool
    {
        assert(msg);

        parseSimCard(msg);
        parseSimState(msg);
        parseAttemptsAndPinSize(msg);

        app->getWindow(app::window::name::desktop_pin_lock)->rebuild();
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
        case Store::GSM::SIM::SIM1:
            lock.additionalLockInfo[gui::PinLock::InfoName::LockName] = "SIM1";
            break;
        case Store::GSM::SIM::SIM2:
            lock.additionalLockInfo[gui::PinLock::InfoName::LockName] = "SIM2";
            break;
        default:
            LOG_ERROR("Unknown SIM");
            break;
        }
    }

    void PinLockHandler::parseAttemptsAndPinSize(CellularSimResponseMessage *msg)
    {
        if (msg->getSimState() == CellularSimResponseMessage::SimState::SIMUnlocked) {
            reloadScreenLock();
        }
        else {
            lock.pinValue          = std::vector<unsigned int>(msg->getPinSize(), 0);
            lock.remainingAttempts = msg->getAttemptsLeft();
        }
    }

    void PinLockHandler::handleScreenPin(const std::vector<unsigned int> &pin)
    {
        if (lock.remainingAttempts > 0) {
            std::hash<std::vector<unsigned int>> hashEngine;
            uint32_t hash = hashEngine(pin);
            lock.remainingAttempts--;
            if (hash == lockPassHash) {
                lock.remainingAttempts = default_screen_attempts;
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
        auto sim = Store::GSM::SIM::SIM1;
        sys::Bus::SendUnicast(
            std::make_shared<CellularSimVerifyPinRequestMessage>(sim, pin), ServiceCellular::serviceName, app);
    }

    void PinLockHandler::reloadScreenLock()
    {
        lock.type  = gui::PinLock::LockType::Screen;
        lock.state = gui::PinLock::State::EnterPin;

        unsigned int pinSize   = lockPassHash == 0 ? 0 : default_screen_pin_size;
        lock.pinValue          = std::vector<unsigned int>(pinSize, 0);
        lock.remainingAttempts = default_screen_attempts;
    }

    void PinLockHandler::lockPassHashChanged(const std::string &name, std::optional<std::string> value)
    {
        if (value.has_value() && !value.value().empty()) {
            lockPassHash = utils::getValue<unsigned int>(value.value());
        }
    }

} // namespace gui
