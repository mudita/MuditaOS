// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"
#include "SystemMainWindow.hpp"
#include "DialogMetadataMessage.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>

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

        auto addFactoryResetOption = [&](UTF8 name, const std::string &window) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                utils::translateI18(name),
                [=](Item &item) {
                    auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                        gui::DialogMetadata{utils::localize.get("app_settings_factory_reset"),
                                            "info_big_circle_W_G",
                                            utils::localize.get("app_settings_display_factory_reset_confirmation"),
                                            "",
                                            [this]() {
                                                auto msg = std::make_shared<sdesktop::FactoryMessage>();
                                                application->bus.sendUnicast(msg, service::name::service_desktop);
                                                application->returnToPreviousWindow(2);
                                                return true;
                                            }});
                    LOG_INFO("switching to %s page", window.c_str());
                    application->switchWindow(window, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
                    return true;
                },
                nullptr,
                this,
                option::SettingRightItem::Disabled));
        };

        addOption("app_settings_language", gui::window::name::languages);
        addOption("app_settings_date_and_time", gui::window::name::date_and_time);
        addOption("app_settings_about_your_pure", gui::window::name::about_your_pure);
        addFactoryResetOption("app_settings_factory_reset", gui::window::name::factory_reset);

        return optionList;
    }
} // namespace gui
