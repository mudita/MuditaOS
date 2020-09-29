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

#include "application-desktop/ApplicationDesktop.hpp"
#include "application-desktop/data/LockPhoneData.hpp"
#include "ScreenLockBox.hpp"
#include "SimLockBox.hpp"
#include "PukLockBox.hpp"
#include <application-phonebook/ApplicationPhonebook.hpp>

namespace gui
{

    PinLockWindow::PinLockWindow(app::Application *app, const std::string &window_name, PinLock &lock)
        : PinLockBaseWindow(app, window_name, lock), this_window_name(window_name)
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
        setVisibleState(lock.getState());
    }
    void PinLockWindow::buildInterface()
    {
        AppWindow::buildInterface();
        PinLockBaseWindow::build();
        LockBox->buildLockBox(lock.getPinSize());
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
            LockBox->setVisibleStateInvalidPin();
        }
        else if (state == PinLock::State::Blocked) {
            LockBox->setVisibleStateBlocked();
        }
    }

    void PinLockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto lockData = dynamic_cast<LockPhoneData *>(data)) {
            lockTimeoutApplication = lockData->getPreviousApplication();
        }
        if (lock.unlock()) {
            application->switchWindow(gui::name::window::main_window);
        }
        setVisibleState(lock.getState());
    }

    bool PinLockWindow::onInput(const InputEvent &inputEvent)
    {
        auto state = lock.getState();
        if (!inputEvent.isShortPress() || state == PinLock::State::VerifiedPin) {
            return AppWindow::onInput(inputEvent);
        }
        // accept only LF, enter, RF, #, and numeric values;
        if (inputEvent.keyCode == KeyCode::KEY_LF && bottomBar->isActive(BottomBar::Side::LEFT)) {
            sapm::ApplicationManager::messageSwitchApplication(
                application, app::name_phonebook, gui::window::name::ice_contacts, nullptr);
            return true;
        }
        else if (inputEvent.keyCode == KeyCode::KEY_RF && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (state == PinLock::State::EnterPin) {
                lock.clearAttempt();
                clearPinLabels();
            }
            else if (state == PinLock::State::InvalidPin) {
                LockBox->setVisibleStateInvalidPin();
                lock.consumeInvalidPinState();
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
        else if (inputEvent.keyCode == KeyCode::KEY_PND) {
            if (state == PinLock::State::EnterPin) {
                lock.popChar();
                LockBox->popChar(lock.getCharCount());
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                return true;
            }
        }
        else if (0 <= gui::toNumeric(inputEvent.keyCode) && gui::toNumeric(inputEvent.keyCode) <= 9) {
            if (state == PinLock::State::EnterPin) {
                LockBox->putChar(lock.getCharCount());
                lock.putNextChar(gui::toNumeric(inputEvent.keyCode));
                if (lock.getLockType() == PinLock::LockType::Screen) {
                    lock.verifyPin();
                }

                state = lock.getState();
                if (state == PinLock::State::VerifiedPin) {
                    LockBox->setVisibleStateVerifiedPin();
                    application->switchWindow(gui::name::window::main_window);
                }
                application->switchWindow(this_window_name);
                return true;
            }
        }
        else if (inputEvent.keyCode == KeyCode::KEY_ENTER && bottomBar->isActive(BottomBar::Side::CENTER)) {
            if (state == PinLock::State::InvalidPin) {
                lock.consumeInvalidPinState();
                application->switchWindow(this_window_name);
                return true;
            }
            else if (state == PinLock::State::EnterPin) {
                lock.verifyPin();
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    void PinLockWindow::makePinLockBox()
    {
        auto lockType = lock.getLockType();
        if (lockType == PinLock::LockType::Screen) {
            LockBox = std::make_unique<ScreenLockBox>(this);
        }
        else if (lockType == PinLock::LockType::PUK) {
            LockBox = std::make_unique<PukLockBox>(this);
        }
        else if (lockType == PinLock::LockType::SIM) {
            LockBox = std::make_unique<SimLockBox>(this);
        }
        assert(LockBox != nullptr);
    }
} /* namespace gui */
