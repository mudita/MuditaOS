/*
 * @file PinLockWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
// application manager
#include "service-appmgr/ApplicationManager.hpp"

// module-gui
#include "gui/widgets/BottomBar.hpp"
#include "PinLockWindow.hpp"

#include "../ApplicationDesktop.hpp"
#include "../data/LockPhoneData.hpp"
#include "../widgets/ScreenLockBox.hpp"
#include "../widgets/SimLockBox.hpp"
#include "../widgets/PukLockBox.hpp"
#include <application-phonebook/ApplicationPhonebook.hpp>
#include <Style.hpp>

namespace gui
{

    PinLockWindow::PinLockWindow(app::Application *app, const std::string &window_name, PinLock *Lock)
        : PinLockBaseWindow(app, window_name, Lock), this_window_name(window_name)
    {
        makePinLockBox();
        buildInterface();
    }

    void PinLockWindow::rebuild()
    {
        // find which widget has focus
        destroyInterface();
        buildInterface();
        // set state
        focusItem = nullptr;
        setVisibleState(Lock->getState());
    }
    void PinLockWindow::buildInterface()
    {
        AppWindow::buildInterface();
        PinLockBaseWindow::build();
        LockBox->buildLockBox(Lock->getPinSize());
    }
    void PinLockWindow::destroyInterface()
    {
        erase();
        invalidate();
    }
    void PinLockWindow::invalidate() noexcept
    {
        titleLabel = nullptr;
        lockImage  = nullptr;
        infoImage  = nullptr;
        infoText   = nullptr;
        pinLabel   = nullptr;
        pinLabels.clear();
    }
    void PinLockWindow::setVisibleState(const PinLock::State state)
    {
        if (state == PinLock::State::EnterPin) {
            LockBox->setVisibleStateEnterPin();
        }
        else if (state == PinLock::State::VerifiedPin) {
            LockBox->setVisibleStateVerifiedPin();
        }
        else if (state == PinLock::State::InvalidPin) {
            LockBox->setVisibleStateInvalidPin(Lock->getRemainingAttempts());
        }
        else if (state == PinLock::State::PhoneBlocked) {
            LockBox->setVisibleStatePhoneBlocked();
        }
    }

    void PinLockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto lockData = dynamic_cast<LockPhoneData *>(data)) {
            lockTimeoutApplication = lockData->getPreviousApplication();
        }
        setVisibleState(Lock->getState());
    }

    bool PinLockWindow::onInput(const InputEvent &inputEvent)
    {
        auto state = Lock->getState();
        if (inputEvent.isShortPress() && (state == PinLock::State::VerifiedPin)) {
            return AppWindow::onInput(inputEvent);
        }

        if (!inputEvent.isShortPress()) {
            return AppWindow::onInput(inputEvent);
        }
        // accept only LF, enter, RF, #, and numeric values

        if (inputEvent.keyCode == KeyCode::KEY_LF && bottomBar->isActive(BottomBar::Side::LEFT)) {
            sapm::ApplicationManager::messageSwitchApplication(
                application, app::name_phonebook, gui::window::name::ice_contacts, nullptr);
            return true;
        }
        else if (inputEvent.keyCode == KeyCode::KEY_RF && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (state == PinLock::State::EnterPin) {
                Lock->clearAttempt();
                application->switchWindow(gui::name::window::main_window);
                return true;
            }
            else if (state == PinLock::State::InvalidPin) {
                Lock->consumeInvalidPinState();
                application->switchWindow(gui::name::window::main_window);
                return true;
            }
            else if (state == PinLock::State::PhoneBlocked) {
                application->switchWindow(gui::name::window::main_window);
                return true;
            }
        }
        else if (inputEvent.keyCode == KeyCode::KEY_PND) {
            if (state == PinLock::State::EnterPin) {
                Lock->popChar();
                LockBox->popChar(Lock->getCharCount());
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                return true;
            }
        }
        else if (0 <= gui::toNumeric(inputEvent.keyCode) && gui::toNumeric(inputEvent.keyCode) <= 9) {
            if (state == PinLock::State::EnterPin) {
                LockBox->putChar(Lock->getCharCount());
                Lock->putNextChar(gui::toNumeric(inputEvent.keyCode));
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

                state = Lock->getState();
                if (state == PinLock::State::VerifiedPin) {
                    LockBox->setVisibleStateVerifiedPin();
                    Lock->unlock();
                    application->switchWindow(gui::name::window::main_window);
                }
                else if (state == PinLock::State::InvalidPin || state == PinLock::State::PhoneBlocked) {
                    application->switchWindow(this_window_name);
                }
                return true;
            }
        }
        else if (inputEvent.keyCode == KeyCode::KEY_ENTER && bottomBar->isActive(BottomBar::Side::CENTER)) {
            if (state == PinLock::State::InvalidPin) {
                Lock->consumeInvalidPinState();
                application->switchWindow(this_window_name);
                return true;
            }
            else if (state == PinLock::State::PhoneBlocked) {
                application->switchWindow(gui::name::window::main_window);
                return true;
            }
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    void PinLockWindow::makePinLockBox()
    {
        auto lockType = Lock->getLockType();
        if (lockType == PinLock::LockType::Screen) {
            LockBox = std::make_unique<ScreenLockBox>(this);
        }
        else if (lockType == PinLock::LockType::PUK) {
            LockBox = std::make_unique<PukLockBox>(this);
        }
        else if (lockType == PinLock::LockType::SIM1 || lockType == PinLock::LockType::SIM2) {
            LockBox = std::make_unique<SimLockBox>(this);
        }
        assert(LockBox != nullptr);
    }
} /* namespace gui */
