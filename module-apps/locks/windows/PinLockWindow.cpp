// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// application manager
#include "InputEvent.hpp"
#include "service-appmgr/Controller.hpp"

// module-gui
#include "gui/widgets/BottomBar.hpp"
#include "PinLockWindow.hpp"

#include "application-desktop/ApplicationDesktop.hpp"
#include "locks/data/LockData.hpp"
#include "locks/widgets/PhoneLockBox.hpp"
#include "locks/widgets/SimLockBox.hpp"
#include "locks/widgets/PukLockBox.hpp"
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
            lockBox->setVisibleStateInputRequired(LockBox::InputActionType::ProvideInput);
        }
        else if (lock->isState(Lock::LockState::InputInvalidRetryRequired)) {
            lockBox->setVisibleStateInputInvalid(LockBox::InputErrorType::InvalidInput, lock->getAttemptsLeft());
        }
        else if (lock->isState(Lock::LockState::Blocked)) {
            lockBox->setVisibleStateBlocked();
        }
        else if (lock->isState(Lock::LockState::NewInputRequired)) {
            lockBox->setVisibleStateInputRequired(LockBox::InputActionType::ProvideNewInput);
        }
        else if (lock->isState(Lock::LockState::NewInputConfirmRequired)) {
            lockBox->setVisibleStateInputRequired(LockBox::InputActionType::ConfirmNewInput);
        }
        else if (lock->isState(Lock::LockState::NewInputInvalid)) {
            lockBox->setVisibleStateInputInvalid(LockBox::InputErrorType::NewInputConfirmFailed,
                                                 lock->getAttemptsLeft());
        }
        else if (lock->isState(Lock::LockState::ErrorOccurred)) {
            lockBox->setVisibleStateInputInvalid(LockBox::InputErrorType::UnhandledError, lock->getAttemptsLeft());
        }
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void PinLockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto lockData = dynamic_cast<LockData *>(data)) {
            rebuild();
            lock = std::make_unique<locks::Lock>(lockData->getLock());

            buildPinLockBox();
            lockBox->buildLockBox(lock->getMaxInputSize());

            if (lock->isState(Lock::LockState::InputRequired)) {
                currentPasscodeType = LockBox::InputActionType::ProvideInput;
            }
            else if (lock->isState(Lock::LockState::NewInputRequired)) {
                currentPasscodeType = LockBox::InputActionType::ProvideNewInput;
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
                lockBox->popChar(lock->getCharCount());
                bottomBar->setActive(BottomBar::Side::CENTER, lock->canVerify());
                return true;
            }
        }
        else if (0 <= gui::toNumeric(inputEvent.keyCode) && gui::toNumeric(inputEvent.keyCode) <= 9) {
            if (usesNumericKeys() && lock->canPut()) {
                lockBox->putChar(lock->getCharCount());
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
            lockBox = std::make_unique<PhoneLockBox>(this);
        }
        else if (lockType == Lock::LockType::SimPuk) {
            lockBox = std::make_unique<PukLockBox>(this);
            setTitleBar(true, true);
            setText("app_desktop_header_sim_setup",
                    TextType::Title,
                    true,
                    {{getToken(Token::Sim), utils::enumToString(lock->getSim())}});
        }
        else if (lockType == Lock::LockType::SimPin) {
            lockBox = std::make_unique<SimLockBox>(this);
            setTitleBar(true, false);
            setText("app_desktop_header_sim_setup",
                    TextType::Title,
                    true,
                    {{getToken(Token::Sim), utils::enumToString(lock->getSim())}});
        }
        assert(lockBox != nullptr);
    }

    auto PinLockWindow::usesNumericKeys() const noexcept -> bool
    {
        return lock &&
               (lock->isState(Lock::LockState::InputRequired) || lock->isState(Lock::LockState::NewInputRequired) ||
                lock->isState(Lock::LockState::NewInputConfirmRequired));
    }
} /* namespace gui */
