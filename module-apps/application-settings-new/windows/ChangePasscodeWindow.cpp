// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/ApplicationSettings.hpp"
#include "ChangePasscodeWindow.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"

namespace lock_style        = style::window::pin_lock;
namespace screen_lock_style = style::window::screen_pin_lock;
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
        lockBox->buildLockBox(screenLockHandler.getLock().getMaxPinSize());
        primaryText->setPosition(screen_lock_style::primary_text::y, gui::Axis::Y);
        pinLabelsBox->setPosition(screen_lock_style::pin_label::y, gui::Axis::Y);
        tickImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, "small_tick_W_M");
        tickImage->setVisible(false);

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
            setText("app_settings_security_type_current_passcode", PinLockBaseWindow::TextType::Primary);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::NewPasscodeRequired: {
            setText("app_settings_security_enter_new_passcode", PinLockBaseWindow::TextType::Primary);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::NewPasscodeConfirmRequired: {
            setText("app_settings_security_confirm_new_passcode", PinLockBaseWindow::TextType::Primary);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::PasscodeInvalidRetryRequired:
        case PinLock::LockState::NewPasscodeInvalidRetryRequired:
        case PinLock::LockState::NewPasscodeInvalid: {
            setText("app_settings_security_wrong_passcode", PinLockBaseWindow::TextType::Secondary);
            secondaryText->setVisible(true);
            break;
        }
        case PinLock::LockState::Unlocked: {
            tickImage->setVisible(true);
            primaryText->setVisible(false);
            setText("app_settings_security_passcode_changed_successfully", PinLockBaseWindow::TextType::Secondary);
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
