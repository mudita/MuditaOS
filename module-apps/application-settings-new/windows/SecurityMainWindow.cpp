// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"
#include "SecurityMainWindow.hpp"

namespace gui
{
    SecurityMainWindow::SecurityMainWindow(app::Application *app) : BaseSettingsWindow(app, window::name::security)
    {}

    auto SecurityMainWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;

        optionList.emplace_back(std::make_unique<OptionSettings>(
            utils::translateI18("app_settings_security_lock_screen_passcode"),
            [=](Item &item) {
                lockScreenPasscodeOn = !lockScreenPasscodeOn;
                rebuildOptionList();
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::localize.get(style::strings::common::Switch),
                                           BottomBar::Side::CENTER);
                }
                else {
                    this->setBottomBarText(utils::localize.get(style::strings::common::select),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            nullptr,
            lockScreenPasscodeOn ? RightItem::On : RightItem::Off));

        if (lockScreenPasscodeOn) {
            optionList.emplace_back(std::make_unique<OptionSettings>(
                utils::translateI18("app_settings_security_change_passcode"),
                [=](Item &item) {
                    LOG_INFO("Not implemented");
                    return true;
                },
                nullptr,
                nullptr,
                RightItem::ArrowWhite));
        }

        return optionList;
    }
} // namespace gui
