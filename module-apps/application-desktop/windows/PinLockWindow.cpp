// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// application manager
#include "InputEvent.hpp"
#include "service-appmgr/Controller.hpp"

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
        invalidate();
    }

    void PinLockWindow::invalidate() noexcept
    {
        titleLabel = nullptr;
        lockImage  = nullptr;
        infoImage  = nullptr;
        infoText   = nullptr;
        pinLabelsBox = nullptr;
    }

    void PinLockWindow::setVisibleState()
    {
        if (lock->is(PinLock::LockState::PasscodeRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvidePasscode);
        }
        else if (lock->is(PinLock::LockState::PasscodeInvalidRetryRequired)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::InvalidPasscode, lock->getValue());
        }
        else if (lock->is(PinLock::LockState::Blocked)) {
            LockBox->setVisibleStateBlocked();
        }
        else if (lock->is(PinLock::LockState::NewPasscodeRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvideNewPasscode);
        }
        else if (lock->is(PinLock::LockState::NewPasscodeConfirmRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ConfirmNewPasscode);
        }
        else if (lock->is(PinLock::LockState::NewPasscodeInvalid)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed,
                                               lock->getValue());
        }
        else if (lock->is(PinLock::LockState::ErrorOccurred)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::UnhandledError, lock->getValue());
        }
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void PinLockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            rebuild();
        }
        if (auto lockData = dynamic_cast<LockPhoneData *>(data)) {
            assert(lockData);
            lockTimeoutApplication = lockData->getPreviousApplication();
            lock                   = lockData->getLock();
            assert(lock);

            buildPinLockBox();
            LockBox->buildLockBox(lock->getMaxPinSize());

            if (lock->is(PinLock::LockState::PasscodeRequired)) {
                currentPasscodeType = PinLockBox::EnterPasscodeType::ProvidePasscode;
            }
            else if (lock->is(PinLock::LockState::NewPasscodeRequired)) {
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
        if (inputEvent.is(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
            app::manager::Controller::sendAction(application, app::manager::actions::ShowEmergencyContacts);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (usesNumericKeys()) {
                lock->clearAttempt();
            }
            else if (lock->is(PinLock::LockState::PasscodeInvalidRetryRequired)) {
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
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    void PinLockWindow::buildPinLockBox()
    {
        auto lockType = lock->getLockType();
        if (lockType == PinLock::LockType::Screen) {
            LockBox = std::make_unique<ScreenLockBox>(this);
        }
        else if (lockType == PinLock::LockType::SimPuk) {
            LockBox = std::make_unique<PukLockBox>(this);
        }
        else if (lockType == PinLock::LockType::SimPin) {
            LockBox = std::make_unique<SimLockBox>(this);
        }
        assert(LockBox != nullptr);
    }

    auto PinLockWindow::usesNumericKeys() const noexcept -> bool
    {
        return lock->is(PinLock::LockState::PasscodeRequired) || lock->is(PinLock::LockState::NewPasscodeRequired) ||
               lock->is(PinLock::LockState::NewPasscodeConfirmRequired);
    }
} /* namespace gui */
