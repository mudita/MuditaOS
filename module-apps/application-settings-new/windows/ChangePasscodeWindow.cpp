// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-desktop/data/AppDesktopStyle.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "ChangePasscodeWindow.hpp"
#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "windows/Dialog.hpp"

namespace lock_style        = style::window::pin_lock;
namespace screen_lock_style = style::window::screen_pin_lock;

namespace gui
{
    ChangePasscodeWindow::ChangePasscodeWindow(app::Application *app)
        : LockWindow(app, gui::window::name::change_passcode)
    {
        buildInterface();
    }

    auto ChangePasscodeWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        auto &lock = lockHandler.getLock();
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

    void ChangePasscodeWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void ChangePasscodeWindow::buildBottomBar()
    {
        LockWindow::buildBottomBar();
        setBottomBarWidgetsActive(false, true, true);
    }

    void ChangePasscodeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        LockWindow::build();

        lockBox = std::make_unique<ScreenLockBaseBox>(this);
        lockBox->buildLockBox(lockHandler.getLock().getMaxPinSize());
        primaryText->setPosition(screen_lock_style::primary_text::y, gui::Axis::Y);
        pinLabelsBox->setPosition(screen_lock_style::pin_label::y, gui::Axis::Y);
    }

    void ChangePasscodeWindow::buildTitleBar()
    {
        setTitle(utils::localize.get("app_settings_security_change_passcode"));
    }

    void ChangePasscodeWindow::destroyInterface()
    {
        erase();
    }

    void ChangePasscodeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data != nullptr) {
            auto changePasscodeData = static_cast<ChangePasscodeData *>(data);
            changePasscodeAction    = changePasscodeData->changePasscodeAction;
            if (ChangePasscodeAction::OnlyProvideNewPasscode == changePasscodeAction) {
                lockState = PinLock::LockState::NewPasscodeRequired;
            }
        }
        setVisibleState();
    }

    void ChangePasscodeWindow::processPasscode()
    {
        switch (lockState) {
        case PinLock::LockState::PasscodeRequired:
        case PinLock::LockState::PasscodeInvalidRetryRequired: {
            auto app  = static_cast<app::ApplicationSettingsNew *>(application);
            lockState = lockHandler.checkPasscode(app->getLockPassHash());
            if (ChangePasscodeAction::OnlyCheckCurrentPasscode == changePasscodeAction &&
                lockState == PinLock::LockState::NewPasscodeRequired) {
                application->setLockScreenPasscodeOn(false);

                auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{utils::localize.get("app_settings_security_change_passcode"),
                                        "success_icon_W_G",
                                        utils::localize.get("app_settings_security_passcode_disabled"),
                                        "",
                                        [this]() {
                                            application->switchWindow(gui::window::name::security);
                                            return true;
                                        }});
                application->switchWindow(
                    gui::window::name::dialog_confirm, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
                return;
            }
            break;
        }
        case PinLock::LockState::NewPasscodeRequired:
        case PinLock::LockState::NewPasscodeInvalidRetryRequired: {
            lockState = lockHandler.newPasscodeProvided();
            break;
        }
        case PinLock::LockState::NewPasscodeConfirmRequired:
        case PinLock::LockState::NewPasscodeInvalid: {
            lockState = lockHandler.newPasscodeConfirmed();
            if (lockState == PinLock::LockState::Unlocked) {
                auto app = static_cast<app::ApplicationSettingsNew *>(application);
                app->setLockPassHash(lockHandler.getNewPasscodeHash());
            }
            break;
        }
        default:
            break;
        }
    }

    void ChangePasscodeWindow::setVisibleState()
    {
        lockBox->clear();
        switch (lockState) {
        case PinLock::LockState::PasscodeRequired: {
            setText("app_settings_security_type_current_passcode", LockWindow::TextType::Primary);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::NewPasscodeRequired: {
            setText("app_settings_security_enter_new_passcode", LockWindow::TextType::Primary);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::NewPasscodeConfirmRequired: {
            setText("app_settings_security_confirm_new_passcode", LockWindow::TextType::Primary);
            secondaryText->setVisible(false);
            break;
        }
        case PinLock::LockState::PasscodeInvalidRetryRequired:
        case PinLock::LockState::NewPasscodeInvalidRetryRequired:
        case PinLock::LockState::NewPasscodeInvalid: {
            setText("app_settings_security_wrong_passcode", LockWindow::TextType::Secondary);
            break;
        }
        case PinLock::LockState::Unlocked: {
            application->setLockScreenPasscodeOn(true);

            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::localize.get("app_settings_security_change_passcode"),
                                    "success_icon_W_G",
                                    utils::localize.get("app_settings_security_passcode_changed_successfully"),
                                    "",
                                    [this]() {
                                        application->switchWindow(gui::window::name::security);
                                        return true;
                                    }});
            application->switchWindow(
                gui::window::name::dialog_confirm, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
            break;
        }
        default: {
            break;
        }
        }
    }
} // namespace gui
