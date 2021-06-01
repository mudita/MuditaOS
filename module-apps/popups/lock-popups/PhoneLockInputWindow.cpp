// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockInputWindow.hpp"

#include <service-appmgr/Controller.hpp>
#include <locks/data/LockData.hpp>
#include <locks/widgets/PhoneLockBox.hpp>
#include <popups/data/PopupRequestParams.hpp>

namespace gui
{
    PhoneLockInputWindow::PhoneLockInputWindow(app::Application *app, const std::string &window_name)
        : LockInputWindow(app, window_name)
    {
        buildInterface();
    }

    void PhoneLockInputWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void PhoneLockInputWindow::buildInterface()
    {
        AppWindow::buildInterface();
        LockInputWindow::build();
    }

    void PhoneLockInputWindow::destroyInterface()
    {
        erase();
    }

    top_bar::Configuration PhoneLockInputWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        if (phoneLockInputTypeAction == locks::PhoneLockInputTypeAction::Unlock) {
            appConfiguration.enable(top_bar::Indicator::Lock);
            appConfiguration.disable(top_bar::Indicator::Time);
        }
        else {
            appConfiguration.enable(top_bar::Indicator::Time);
            appConfiguration.disable(top_bar::Indicator::Lock);
        }
        return appConfiguration;
    }

    void PhoneLockInputWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto lockData = dynamic_cast<locks::LockData *>(data)) {
            lock                     = std::make_unique<locks::Lock>(lockData->getLock());
            phoneLockInputTypeAction = lockData->getPhoneLockInputTypeAction();
        }

        // Lock need to exist in that window flow
        assert(lock);

        rebuild();
        lockBox = std::make_unique<PhoneLockBox>(this, phoneLockInputTypeAction);
        lockBox->buildLockBox(lock->getMaxInputSize());

        setVisibleState();
    }

    bool PhoneLockInputWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease()) {
            return AppWindow::onInput(inputEvent);
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (isInInputState()) {
                lock->clearAttempt();
            }
            else if (lock->isState(locks::Lock::LockState::InputInvalid)) {
                lock->consumeState();
            }
            application->returnToPreviousWindow();
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_PND)) {
            if (isInInputState()) {
                lock->popChar();
                lockBox->popChar(lock->getCharCount());
                bottomBar->setActive(BottomBar::Side::CENTER, lock->canVerify());
                return true;
            }
        }
        else if (inputEvent.isDigit()) {

            if (isInInputState() && lock->canPut()) {

                lockBox->putChar(lock->getCharCount());
                lock->putNextChar(inputEvent.numericValue());

                if (lock->canVerify()) {
                    application->getPhoneLockSubject().verifyInput(lock->getInput());
                    lock->consumeState();
                    lock->clearAttempt();
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
        else if (inputEvent.is(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
            application->getPhoneLockSubject().skipSetPhoneLock();
            lock->consumeState();
            lock->clearAttempt();
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_LEFT) && isIceVisible()) {
            app::manager::Controller::sendAction(application,
                                                 app::manager::actions::EmergencyDial,
                                                 std::make_unique<gui::SwitchData>(),
                                                 app::manager::OnSwitchBehaviour::RunInBackground);
            return true;
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }
} /* namespace gui */
