// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// application manager
#include "InputEvent.hpp"
#include "service-appmgr/Controller.hpp"

// module-gui
#include "gui/widgets/BottomBar.hpp"
#include "PinLockWindow.hpp"

#include "application-desktop/ApplicationDesktop.hpp"
#include "popups/data/LockData.hpp"
#include "popups/lock-widgets/PhoneLockBox.hpp"
#include "popups/lock-widgets/SimLockBox.hpp"
#include "popups/lock-widgets/PukLockBox.hpp"
#include <application-phonebook/ApplicationPhonebook.hpp>

namespace gui
{

    PinLockWindow::PinLockWindow(app::Application *app, const std::string &window_name)
        : PinLockBaseWindow(app, window_name), this_window_name(window_name)
    {
        buildInterface();
    }

    void PinLockWindow::rebuild()
    {
        // find which widget has focus
        destroyInterface();
        buildInterface();
        // set state
        focusItem = nullptr;
    }
    void PinLockWindow::buildInterface()
    {
        AppWindow::buildInterface();
        PinLockBaseWindow::build();
    }

    void PinLockWindow::destroyInterface()
    {
        erase();
    }

    void PinLockWindow::setVisibleState()
    {
        restore();
        if (lock->isState(Lock::LockState::InputRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvidePasscode);
        }
        else if (lock->isState(Lock::LockState::InputInvalidRetryRequired)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::InvalidPasscode, lock->getAttemptsLeft());
        }
        else if (lock->isState(Lock::LockState::Blocked)) {
            LockBox->setVisibleStateBlocked();
        }
        else if (lock->isState(Lock::LockState::NewInputRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvideNewPasscode);
        }
        else if (lock->isState(Lock::LockState::NewInputConfirmRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ConfirmNewPasscode);
        }
        else if (lock->isState(Lock::LockState::NewInputInvalid)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed,
                                               lock->getAttemptsLeft());
        }
        else if (lock->isState(Lock::LockState::ErrorOccurred)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::UnhandledError, lock->getAttemptsLeft());
        }
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void PinLockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto lockData = dynamic_cast<LockData *>(data)) {
            rebuild();
            lock                   = lockData->getLock();
            assert(lock);

            buildPinLockBox();
            LockBox->buildLockBox(lock->getMaxInputSize());

            if (lock->isState(Lock::LockState::InputRequired)) {
                currentPasscodeType = PinLockBox::EnterPasscodeType::ProvidePasscode;
            }
            else if (lock->isState(Lock::LockState::NewInputRequired)) {
                currentPasscodeType = PinLockBox::EnterPasscodeType::ProvideNewPasscode;
            }
            setVisibleState();
        }
    }

    bool PinLockWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return AppWindow::onInput(inputEvent);
        }
        // accept only LF, enter, RF, #, and numeric values;
        if (inputEvent.is(KeyCode::KEY_LEFT) && iceBox->visible) {
            app::manager::Controller::sendAction(application, app::manager::actions::EmergencyDial);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (usesNumericKeys()) {
                lock->clearAttempt();
            }
            else if (lock->isState(Lock::LockState::InputInvalidRetryRequired)) {
                lock->consumeState();
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_PND)) {
            if (usesNumericKeys()) {
                lock->popChar();
                LockBox->popChar(lock->getCharCount());
                bottomBar->setActive(BottomBar::Side::CENTER, lock->canVerify());
                return true;
            }
        }
        else if (0 <= gui::toNumeric(inputEvent.keyCode) && gui::toNumeric(inputEvent.keyCode) <= 9) {
            if (usesNumericKeys() && lock->canPut()) {
                LockBox->putChar(lock->getCharCount());
                lock->putNextChar(gui::toNumeric(inputEvent.keyCode));
                bottomBar->setActive(BottomBar::Side::CENTER, lock->canVerify());
                return true;
            }
        }
        else if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
            lock->activate();
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    void PinLockWindow::buildPinLockBox()
    {
        auto lockType = lock->getLockType();
        if (lockType == Lock::LockType::Screen) {
            LockBox = std::make_unique<PhoneLockBox>(this);
        }
        else if (lockType == Lock::LockType::SimPuk) {
            LockBox = std::make_unique<PukLockBox>(this);
            setTitleBar(true, true);
            setText("app_desktop_header_sim_setup",
                    TextType::Title,
                    true,
                    {{getToken(Token::Sim), utils::enumToString(lock->getSim())}});
        }
        else if (lockType == Lock::LockType::SimPin) {
            LockBox = std::make_unique<SimLockBox>(this);
            setTitleBar(true, false);
            setText("app_desktop_header_sim_setup",
                    TextType::Title,
                    true,
                    {{getToken(Token::Sim), utils::enumToString(lock->getSim())}});
        }
        assert(LockBox != nullptr);
    }

    auto PinLockWindow::usesNumericKeys() const noexcept -> bool
    {
        return lock &&
               (lock->isState(Lock::LockState::InputRequired) || lock->isState(Lock::LockState::NewInputRequired) ||
                lock->isState(Lock::LockState::NewInputConfirmRequired));
    }
} /* namespace gui */
