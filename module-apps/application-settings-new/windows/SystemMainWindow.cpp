// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"
#include "SystemMainWindow.hpp"

namespace gui
{
    SystemMainWindow::SystemMainWindow(app::Application *app) : BaseSettingsWindow(app, window::name::system)
    {}

    auto SystemMainWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;
        auto addOption = [&](UTF8 name, const std::string &window) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                utils::translateI18(name),
                [=](Item &item) {
                    LOG_INFO("switching to %s page", window.c_str());
                    application->switchWindow(window, nullptr);
                    return true;
                },
                nullptr,
                nullptr,
                option::SettingRightItem::ArrowWhite));
        };

        addOption("app_settings_language", gui::window::name::language);
        addOption("app_settings_date_and_time", gui::window::name::date_and_time);
        addOption("app_settings_factory_reset", gui::window::name::factory_reset);
        addOption("app_settings_about_your_pure", gui::window::name::about_your_pure);
        addOption("app_settings_certification", gui::window::name::certification);

        return optionList;
    }
} // namespace gui
