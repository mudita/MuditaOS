// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ConfigurePasscodeWindow.hpp"
#include <popups/data/LockStyle.hpp>

#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <application-onboarding/data/OnBoardingSwitchData.hpp>
#include <application-settings-new/ApplicationSettings.hpp>
#include <application-settings-new/windows/ChangePasscodeWindow.hpp>

#include <DialogMetadata.hpp>
#include <DialogMetadataMessage.hpp>
#include <windows/Dialog.hpp>

namespace lock_style        = style::window::pin_lock;
namespace screen_lock_style = style::window::screen_pin_lock;

namespace gui
{
    ConfigurePasscodeWindow::ConfigurePasscodeWindow(app::Application *app) : ChangePasscodeWindow(app)
    {
        lockState = PinLock::LockState::NewPasscodeRequired;

        textForEnterNewPassword = "app_onboarding_set_password";
        testForConfirmPassword  = "app_onboarding_confirm_password";
        textForWrongPassword    = "app_onboarding_worng_password";
    }

    auto ConfigurePasscodeWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortPress() && inputEvent.is(KeyCode::KEY_LF)) {
            application->setLockScreenPasscodeOn(false);
            application->switchWindow(gui::window::name::onBoarding_date_and_time,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::make_unique<app::onBoarding::OnBoardingSwitchData>());
        }

        return ChangePasscodeWindow::onInput(inputEvent);
    }

    void ConfigurePasscodeWindow::buildBottomBar()
    {
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::skip));
    }

    void ConfigurePasscodeWindow::buildTitleBar()
    {
        setTitle(utils::translate("app_onboarding_passcode_configuration"));
    }

    void ConfigurePasscodeWindow::processPasscode()
    {
        switch (lockState) {
        case PinLock::LockState::NewPasscodeConfirmRequired:
        case PinLock::LockState::NewPasscodeInvalid: {
            lockState = lockHandler.newPasscodeConfirmed();
            if (lockState == PinLock::LockState::Unlocked) {
                auto app = static_cast<app::ApplicationOnBoarding *>(application);
                app->setLockPassHash(lockHandler.getNewPasscodeHash());
            }
            break;
        }
        default:
            return ChangePasscodeWindow::processPasscode();
        }
    }

    void ConfigurePasscodeWindow::setVisibleState()
    {
        switch (lockState) {
        case PinLock::LockState::NewPasscodeInvalid: {

            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::translate("app_onboarding_passcode_configuration"),
                                    "info_big_circle_W_G",
                                    utils::translate("app_onboarding_wrong_password"),
                                    "",
                                    [this]() {
                                        application->switchWindow(gui::window::name::onBoarding_configure_passcode,
                                                                  gui::ShowMode::GUI_SHOW_INIT,
                                                                  std::make_unique<ChangePasscodeData>(
                                                                      ChangePasscodeAction::OnlyProvideNewPasscode));
                                        return true;
                                    }});

            application->switchWindow(
                gui::window::name::dialog_confirm, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));

            break;
        }
        case PinLock::LockState::Unlocked: {
            application->setLockScreenPasscodeOn(true);
            application->switchWindow(gui::window::name::onBoarding_date_and_time,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::make_unique<app::onBoarding::OnBoardingSwitchData>());
            break;
        }
        default:
            ChangePasscodeWindow::setVisibleState();
            break;
        }
    }
} // namespace gui
