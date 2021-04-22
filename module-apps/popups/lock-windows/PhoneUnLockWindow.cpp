// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// application manager
#include "InputEvent.hpp"
#include "service-appmgr/Controller.hpp"

// module-gui
#include "gui/widgets/BottomBar.hpp"
#include "PhoneUnLockWindow.hpp"

#include "popups/data/LockData.hpp"
#include "popups/lock-widgets/PhoneLockBox.hpp"
#include <application-phonebook/ApplicationPhonebook.hpp>
#include <module-apps/popups/data/PopupRequestParams.hpp>
#include <module-apps/popups/data/LockData.hpp>

namespace gui
{
    PhoneUnlockWindow::PhoneUnlockWindow(app::Application *app, const std::string &window_name)
        : PinLockBaseWindow(app, window_name), this_window_name(window_name)
    {
        buildInterface();
    }

    void PhoneUnlockWindow::rebuild()
    {
        // find which widget has focus
        destroyInterface();
        buildInterface();
        // set state
        focusItem = nullptr;
    }
    void PhoneUnlockWindow::buildInterface()
    {
        AppWindow::buildInterface();
        LockWindow::build();
    }

    void PhoneUnlockWindow::destroyInterface()
    {
        erase();
    }

    void PhoneUnlockWindow::setVisibleState()
    {
        restore();
        if (lock->isState(lock::Lock::LockState::InputRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvidePasscode);
        }
        else if (lock->isState(lock::Lock::LockState::InputInvalidRetryRequired)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::InvalidPasscode, lock->getAttemptsLeft());
        }
        else if (lock->isState(lock::Lock::LockState::Blocked)) {
            LockBox->setVisibleStateBlocked();
        }
        //        else if (lock->isState(PinLock::LockState::NewPasscodeRequired)) {
        //            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvideNewPasscode);
        //        }
        //        else if (lock->isState(PinLock::LockState::NewPasscodeConfirmRequired)) {
        //            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ConfirmNewPasscode);
        //        }
        //        else if (lock->isState(PinLock::LockState::NewPasscodeInvalid)) {
        //            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed,
        //                                               lock->getValue());
        //        }
        //        else if (lock->isState(PinLock::LockState::ErrorOccurred)) {
        //            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::UnhandledError,
        //            lock->getValue());
        //        }
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void PhoneUnlockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {

        LOG_ERROR("Wchodze sobie do tego okna ?");

        if (auto lockData = dynamic_cast<lock::LockData *>(data)) {

            LOG_ERROR("Cast sie udal?");

            rebuild();
            lock = lockData->getLock();
            assert(lock);

            LockBox = std::make_unique<PhoneLockBox>(this);

            LockBox->buildLockBox(lock->getMaxInputSize());

            if (lock->isState(lock::Lock::LockState::InputRequired)) {
                currentPasscodeType = PinLockBox::EnterPasscodeType::ProvidePasscode;
            }

            setVisibleState();
        }
    }

    bool PhoneUnlockWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return AppWindow::onInput(inputEvent);
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (usesNumericKeys()) {
                lock->clearAttempt();
            }
            else if (lock->isState(lock::Lock::LockState::InputInvalidRetryRequired)) {

                LOG_ERROR("Tutaj powinienes byc prawda?");

                lock->consumeState();
            }
            //            application->returnToPreviousWindow();
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

            LOG_ERROR("Tutaj powinienem wchodzić");

            if (usesNumericKeys() && lock->canPut()) {

                LOG_ERROR("I o tutaj");

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

    // Co to do kurwy za funkcja jest ....
    auto PhoneUnlockWindow::usesNumericKeys() const noexcept -> bool
    {
        return lock && (lock->isState(lock::Lock::LockState::InputRequired));

        //        return lock && (lock->isState(PinLock::LockState::PasscodeRequired) ||
        //                        lock->isState(PinLock::LockState::NewPasscodeRequired) ||
        //                        lock->isState(PinLock::LockState::NewPasscodeConfirmRequired));
    }
} /* namespace gui */
