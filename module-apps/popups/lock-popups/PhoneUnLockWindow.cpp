// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneUnLockWindow.hpp"

#include <service-appmgr/Controller.hpp>
#include <locks/data/LockData.hpp>
#include <locks/widgets/PhoneLockBox.hpp>
#include <popups/data/PopupRequestParams.hpp>

namespace gui
{
    PhoneUnlockWindow::PhoneUnlockWindow(app::Application *app, const std::string &window_name)
        : PinLockBaseWindow(app, window_name)
    {
        buildInterface();
    }

    void PhoneUnlockWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
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
        if (lock->isState(locks::Lock::LockState::InputRequired)) {
            lockBox->setVisibleStateInputRequired(LockBox::InputActionType::ProvideInput);
        }
        else if (lock->isState(locks::Lock::LockState::InputInvalidRetryRequired)) {
            lockBox->setVisibleStateInputInvalid(LockBox::InputErrorType::InvalidInput, lock->getAttemptsLeft());
        }
        else if (lock->isState(locks::Lock::LockState::Blocked)) {
            lockBox->setVisibleStateBlocked();
        }
    }

    top_bar::Configuration PhoneUnlockWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.disable(top_bar::Indicator::Time);
        appConfiguration.enable(top_bar::Indicator::PhoneMode);
        appConfiguration.enable(top_bar::Indicator::Lock);
        appConfiguration.enable(top_bar::Indicator::Battery);
        appConfiguration.enable(top_bar::Indicator::Signal);
        appConfiguration.enable(top_bar::Indicator::SimCard);
        return appConfiguration;
    }

    void PhoneUnlockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto lockData = dynamic_cast<locks::LockData *>(data)) {

            rebuild();

            lock    = std::make_unique<locks::Lock>(lockData->getLock());
            lockBox = std::make_unique<PhoneLockBox>(this);
            lockBox->buildLockBox(lock->getMaxInputSize());
            setVisibleState();
        }

        // Lock need to exist in that window flow
        assert(lock);
    }

    bool PhoneUnlockWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease()) {
            return AppWindow::onInput(inputEvent);
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (usesNumericKeys()) {
                lock->clearAttempt();
            }
            else if (lock->isState(locks::Lock::LockState::InputInvalidRetryRequired)) {
                lock->consumeState();
            }
            application->returnToPreviousWindow();
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
        else if (inputEvent.isDigit()) {

            if (usesNumericKeys() && lock->canPut()) {

                lockBox->putChar(lock->getCharCount());
                lock->putNextChar(inputEvent.numericValue());

                if (lock->canVerify()) {
                    application->verifyPhoneLockInput(lock->getInput());
                }

                return true;
            }
        }

        else if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
            if (lock->isState(locks::Lock::LockState::Blocked)) {
                application->returnToPreviousWindow();
            }
            else {
                lock->consumeState();
                lock->clearAttempt();
                setVisibleState();
            }
            return true;
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    auto PhoneUnlockWindow::usesNumericKeys() const noexcept -> bool
    {
        return lock && (lock->isState(locks::Lock::LockState::InputRequired));
    }
} /* namespace gui */
