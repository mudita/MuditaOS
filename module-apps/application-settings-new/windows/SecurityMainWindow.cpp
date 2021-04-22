// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-apps/locks/widgets/Lock.hpp>
#include <module-apps/locks/data/LockData.hpp>
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/data/ChangePasscodeData.hpp"
#include "module-apps/application-desktop/windows/Names.hpp"
#include "OptionSetting.hpp"
#include "SecurityMainWindow.hpp"

namespace gui
{
    SecurityMainWindow::SecurityMainWindow(app::Application *app, app::settingsInterface::SecuritySettings *settings)
        : BaseSettingsWindow(app, window::name::security), lockScreenPasscodeIsOn(app->isLockScreenPasscodeOn()),
          securitySettings(settings)
    {}

    auto SecurityMainWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;

        optionList.emplace_back(std::make_unique<option::OptionSettings>(
            utils::translate("app_settings_security_lock_screen_passcode"),
            [=](Item &item) {
                lockScreenPasscodeIsOn = !lockScreenPasscodeIsOn;
                LOG_INFO("switching to %s page", window::name::change_passcode);
                auto data = lockScreenPasscodeIsOn
                                ? std::make_unique<ChangePasscodeData>(ChangePasscodeAction::OnlyProvideNewPasscode)
                                : std::make_unique<ChangePasscodeData>(ChangePasscodeAction::OnlyCheckCurrentPasscode);
                application->switchWindow(window::name::change_passcode, std::move(data));
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translate(style::strings::common::Switch), BottomBar::Side::CENTER);
                }
                else {
                    this->setBottomBarText(utils::translate(style::strings::common::select), BottomBar::Side::CENTER);
                }
                return true;
            },
            nullptr,
            lockScreenPasscodeIsOn ? option::SettingRightItem::On : option::SettingRightItem::Off));

        optionList.emplace_back(std::make_unique<option::OptionSettings>(
            utils::translate("app_settings_security_usb_passcode"),
            [=](Item &item) {
                auto lock = std::make_unique<lock::Lock>(
                    Store::GSM::SIM::NONE, lock::Lock::LockState::InputRequired, lock::Lock::LockType::Screen);
                lock->onActivatedCallback = [this](lock::Lock::LockType type, const std::vector<unsigned int> &data) {
                    securitySettings->setUSBSecurity(!securitySettings->isUSBSecured());
                    application->returnToPreviousWindow();
                };
                application->switchWindow(app::window::name::desktop_pin_lock,
                                          gui::ShowMode::GUI_SHOW_INIT,
                                          std::make_unique<lock::LockData>(*lock));
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translate(style::strings::common::Switch), BottomBar::Side::CENTER);
                }
                else {
                    this->setBottomBarText(utils::translate(style::strings::common::select), BottomBar::Side::CENTER);
                }
                return true;
            },
            nullptr,
            securitySettings->isUSBSecured() ? option::SettingRightItem::On : option::SettingRightItem::Off));

        if (lockScreenPasscodeIsOn) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                utils::translate("app_settings_security_change_passcode"),
                [=](Item &item) {
                    LOG_INFO("switching to %s page", window::name::change_passcode);
                    application->switchWindow(
                        window::name::change_passcode,
                        std::make_unique<ChangePasscodeData>(ChangePasscodeAction::ChangeCurrentPasscode));
                    return true;
                },
                nullptr,
                nullptr,
                option::SettingRightItem::ArrowWhite));
        }

        return optionList;
    }
} // namespace gui
