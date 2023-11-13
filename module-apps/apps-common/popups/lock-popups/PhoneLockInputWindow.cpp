// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockInputWindow.hpp"

#include <locks/input/PhoneLockedKeysWhitelist.hpp>
#include <service-appmgr/Controller.hpp>
#include <locks/data/LockData.hpp>
#include <locks/widgets/PhoneLockBox.hpp>
#include <popups/data/PopupRequestParams.hpp>
#include <EventStore.hpp>

namespace gui
{
    PhoneLockInputWindow::PhoneLockInputWindow(app::ApplicationCommon *app, const std::string &window_name)
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

    status_bar::Configuration PhoneLockInputWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        if (phoneLockInputTypeAction == locks::PhoneLockInputTypeAction::Unlock) {
            appConfiguration.disable(status_bar::Indicator::Time);
            appConfiguration.enable(status_bar::Indicator::Lock);
            appConfiguration.enable(status_bar::Indicator::PhoneMode);
            appConfiguration.enable(status_bar::Indicator::Battery);
            appConfiguration.enable(status_bar::Indicator::Signal);
            appConfiguration.enable(status_bar::Indicator::SimCard);
            appConfiguration.enable(status_bar::Indicator::Bluetooth);
            appConfiguration.enable(status_bar::Indicator::AlarmClock);
            appConfiguration.enable(status_bar::Indicator::Tethering);
        }
        else {
            appConfiguration.enable(status_bar::Indicator::Time);
            appConfiguration.disable(status_bar::Indicator::Lock);
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
        lockBox->update(lock->getCharCount());

        setVisibleState();
    }

    bool PhoneLockInputWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease()) {
            return AppWindow::onInput(inputEvent);
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && navBar->isActive(nav_bar::Side::Right)) {
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
                navBar->setActive(nav_bar::Side::Center, lock->canVerify());
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
        else if (inputEvent.is(KeyCode::KEY_ENTER) && navBar->isActive(nav_bar::Side::Center)) {
            if (lock->isState(locks::Lock::LockState::Blocked)) {
                application->switchWindow(popup::window::phone_lock_window);
            }
            else {
                lock->consumeState();
                lock->clearAttempt();
                setVisibleState();
            }
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_LF) && navBar->isActive(nav_bar::Side::Left)) {
            application->getPhoneLockSubject().skipSetPhoneLock();
            lock->consumeState();
            lock->clearAttempt();
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_LEFT) && isIceVisible() && Store::GSM::get()->simCardInserted()) {
            app::manager::Controller::sendAction(application,
                                                 app::manager::actions::EmergencyDial,
                                                 std::make_unique<gui::SwitchData>(),
                                                 app::manager::OnSwitchBehaviour::RunInBackground);
            return true;
        }

        // check if any of the lower inheritance onInput methods catch the event
        if (locks::PhoneLockedKeysWhitelist::isOnWhitelist(inputEvent)) {
            return AppWindow::onInput(inputEvent);
        }
        return true;
    }
} /* namespace gui */
