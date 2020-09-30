#include "PinLockHandler.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include <service-cellular/ServiceCellular.hpp>

namespace gui
{
    static const unsigned int default_screen_pin_size = 4;
    static const unsigned int default_screen_attempts = 4;

    auto PinLockHandler::handle(CellularSimResponseMessage *msg) -> bool
    {
        assert(msg);

        parseSimCard(msg);
        parseSimState(msg);
        parseAttemptsAndPinSize(msg);
        lock.additionalLockInfo[gui::PinLock::InfoName::PhoneNum] = msg->number.getFormatted();

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
        switch (msg->state) {
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
        switch (msg->sim) {
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
        if (msg->state == CellularSimResponseMessage::SimState::SIMUnlocked) {
            reloadScreenLock();
        }
        else {
            lock.pinValue          = std::vector<unsigned int>(msg->pinSize, 0);
            lock.remainingAttempts = msg->attemptsLeft;
        }
    }

    void PinLockHandler::handleScreenPin(const std::vector<unsigned int> &pin)
    {
        if (lock.remainingAttempts > 0) {
            uint32_t hash = 0;
            for (auto i : pin) {
                hash = 10 * hash + i;
            }
            lock.remainingAttempts--;
            if (hash == appSettings->lockPassHash) {
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
        auto sim = CellularSimMessage::SimCard::SIM1;
        sys::Bus::SendUnicast(
            std::make_shared<CellularSimVerifyPinRequestMessage>(sim, pin), ServiceCellular::serviceName, app);
    }

    void PinLockHandler::reloadScreenLock()
    {
        lock.type  = gui::PinLock::LockType::Screen;
        lock.state = gui::PinLock::State::EnterPin;

        unsigned int pinSize   = appSettings->lockPassHash == 0 ? 0 : default_screen_pin_size;
        lock.pinValue          = std::vector<unsigned int>(pinSize, 0);
        lock.remainingAttempts = default_screen_attempts;
    }
    void PinLockHandler::init(SettingsRecord *settings)
    {
        appSettings = settings;
        reloadScreenLock();
    }
} // namespace gui
