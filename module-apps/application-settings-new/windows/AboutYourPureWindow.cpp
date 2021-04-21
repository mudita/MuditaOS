// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"
#include "AboutYourPureWindow.hpp"

namespace gui
{
    AboutYourPureWindow::AboutYourPureWindow(app::Application *app)
        : BaseSettingsWindow(app, window::name::about_your_pure)
    {
        setTitle(utils::translate("app_settings_about_your_pure"));
    }

    auto AboutYourPureWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;
        auto addOption = [&](UTF8 name, const std::string &window) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                utils::translate(name),
                [=](Item &item) {
                    LOG_INFO("switching to %s page", window.c_str());
                    application->switchWindow(window, nullptr);
                    return true;
                },
                nullptr,
                nullptr,
                option::SettingRightItem::ArrowWhite));
        };

        addOption("app_settings_technical_information", gui::window::name::technical_information);
        addOption("app_settings_certification", gui::window::name::certification);
        addOption("app_settings_sar", gui::window::name::sar);

        return optionList;
    }
} // namespace gui
