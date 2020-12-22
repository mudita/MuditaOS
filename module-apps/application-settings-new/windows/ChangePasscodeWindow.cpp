// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/widgets/ScreenLockBoxSettings.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "ChangePasscodeWindow.hpp"
#include "application-desktop/data/Style.hpp"
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include "AppWindow.hpp"
#include "widgets/PinLock.hpp"

namespace gui
{
    ChangePasscodeWindow::ChangePasscodeWindow(app::Application *app)
        : PinLockBaseWindow(app, gui::window::name::change_passcode)
    {
        buildInterface();
    }

    void ChangePasscodeWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void ChangePasscodeWindow::buildBottomBar()
    {
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        setBottomBarWidgetsActive(false, true, true);
    }

    void ChangePasscodeWindow::buildTopBar()
    {
        topBar->setActive(TopBar::Elements::SIM, false);
        topBar->setActive(TopBar::Elements::LOCK, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::TIME, true);
    }

    void ChangePasscodeWindow::buildTitleBar()
    {
        setTitle(utils::localize.get("app_settings_security_change_passcode"));
    }

    void ChangePasscodeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        PinLockBaseWindow::build();

        lockBox = std::make_unique<ScreenLockBoxSettings>(this);
        lockBox->buildLockBox(4U);

        pinLabelsBox->setPosition(248U, gui::Axis::Y);

        setVisibleState();
    }

    void ChangePasscodeWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void ChangePasscodeWindow::invalidate() noexcept
    {
        PinLockBaseWindow::invalidate();
        lockBox       = nullptr;
    }

    void ChangePasscodeWindow::setVisibleState()
    {
        lockBox->clear();
        switch (lockState) {
        case PinLock::LockState::PasscodeRequired: {
            primaryText->setText("Type current passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::PasscodeInvalidRetryRequired: {
            primaryText->setText("Type current passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
            secondaryText->setText("Wrong passcode!");
            secondaryText->setVisible(true);
            break;
        }
        case PinLock::LockState::NewPasscodeRequired: {
            primaryText->setText("Enter new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::NewPasscodeInvalidRetryRequired: {
            primaryText->setText("Enter new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
            secondaryText->setText("Wrong passcode!");
            secondaryText->setVisible(true);
            break;
        }
        case PinLock::LockState::NewPasscodeConfirmRequired: {
            primaryText->setText("Confirm new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::NewPasscodeInvalid: {
            primaryText->setText("Confirm new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
            secondaryText->setText("Wrong passcode!");
            secondaryText->setVisible(true);
            break;
        }
        case PinLock::LockState::Unlocked: {
            // infoImage->setVisible(true);
            primaryText->setVisible(false);
            secondaryText->setText("Passcode changed successfully");
            secondaryText->setVisible(true);
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            pinLabelsBox->setVisible(false);
            break;
        }
        default: {
            break;
        }
        }
    }

    auto ChangePasscodeWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        auto &lock = screenLockHandler.getLock();
        if (lock.isState(PinLock::LockState::Unlocked) && inputEvent.isShortPress()) {
            application->returnToPreviousWindow();
        }
        if (!inputEvent.isShortPress()) {
            return AppWindow::onInput(inputEvent);
        }

        auto keyCodeNum = gui::toNumeric(inputEvent.keyCode);
        if (0 <= keyCodeNum && keyCodeNum <= 9 && lock.canPut()) {
            lockBox->putChar(lock.getCharCount());
            lock.putNextChar(keyCodeNum);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_PND)) {
            lock.popChar();
            lockBox->popChar(lock.getCharCount());
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_ENTER)) {
            processPasscode();
            setVisibleState();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void ChangePasscodeWindow::processPasscode()
    {
        switch (lockState) {
        case PinLock::LockState::PasscodeRequired:
        case PinLock::LockState::PasscodeInvalidRetryRequired: {
            auto app  = static_cast<app::ApplicationSettingsNew *>(application);
            lockState = screenLockHandler.checkPasscode(app->getLockPassHash());
            break;
        }
        case PinLock::LockState::NewPasscodeRequired:
        case PinLock::LockState::NewPasscodeInvalidRetryRequired: {
            lockState = screenLockHandler.newPasscodeProvided();
            break;
        }
        case PinLock::LockState::NewPasscodeConfirmRequired:
        case PinLock::LockState::NewPasscodeInvalid: {
            lockState = screenLockHandler.newPasscodeConfirmed();
            if (lockState == PinLock::LockState::Unlocked) {
                auto app = static_cast<app::ApplicationSettingsNew *>(application);
                app->setLockPassHash(screenLockHandler.getNewPasscodeHash());
            }
            break;
        }
        default:
            break;
        }
    }
} // namespace gui
