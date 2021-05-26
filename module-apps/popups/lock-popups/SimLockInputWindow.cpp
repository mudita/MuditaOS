// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimLockInputWindow.hpp"

#include <locks/data/LockData.hpp>
#include <locks/widgets/SimLockBox.hpp>
#include <locks/widgets/LockHash.hpp>

#include <service-appmgr/Controller.hpp>
#include <popups/data/PopupRequestParams.hpp>

namespace gui
{
    SimLockInputWindow::SimLockInputWindow(app::Application *app, const std::string &window_name)
        : LockInputWindow(app, window_name)
    {
        buildInterface();
    }

    void SimLockInputWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void SimLockInputWindow::buildInterface()
    {
        AppWindow::buildInterface();
        LockInputWindow::build();
    }

    void SimLockInputWindow::destroyInterface()
    {
        erase();
    }

    void SimLockInputWindow::setVisibleState()
    {
        LockInputWindow::setVisibleState();

        if (lock->isState(locks::Lock::LockState::ErrorOccurred)) {
            lockBox->setVisibleStateError(errorCode);
        }
    }

    top_bar::Configuration SimLockInputWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.enable(top_bar::Indicator::Time);
        appConfiguration.enable(top_bar::Indicator::PhoneMode);
        appConfiguration.enable(top_bar::Indicator::Battery);
        appConfiguration.enable(top_bar::Indicator::Signal);
        appConfiguration.enable(top_bar::Indicator::SimCard);
        return appConfiguration;
    }

    void SimLockInputWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto SimLockData = dynamic_cast<locks::SimLockData *>(data)) {
            lock                   = std::make_unique<locks::Lock>(SimLockData->getLock());
            simLockInputTypeAction = SimLockData->getSimInputTypeAction();
            errorCode              = SimLockData->getErrorCode();
        }

        // Lock need to exist in that window flow
        assert(lock);

        rebuild();
        lockBox = std::make_unique<SimLockBox>(this, simLockInputTypeAction);
        lockBox->buildLockBox(lock->getMaxInputSize());

        setVisibleState();
    }

    bool SimLockInputWindow::onInput(const InputEvent &inputEvent)
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

                bottomBar->setActive(BottomBar::Side::CENTER, lock->canVerify());
                return true;
            }
        }
        else if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
            if (isInInputState()) {
                application->getSimLockSubject().verifyInput(lock->getInput());
            }
            else if (isInInvalidInputState()) {
                lock->consumeState();
                lock->clearAttempt();
                setVisibleState();
            }
            return true;
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

} /* namespace gui */
