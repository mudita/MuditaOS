// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsMainWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <service-appmgr/Controller.hpp>

std::list<gui::Option> mainWindowOptionsNew(app::Application *app)
{
    std::list<gui::Option> l;

    auto i18     = [](const std::string &text) { return utils::translate(text); };
    auto addMenu = [&l, &app](UTF8 name, const std::string &window = "") {
        l.emplace_back(gui::Option{name,
                                   [=](gui::Item &item) {
                                       if (window.empty()) {
                                           return false;
                                       }
                                       LOG_INFO("switching to %s page", window.c_str());
                                       app->switchWindow(window, nullptr);
                                       return true;
                                   },
                                   gui::option::Arrow::Enabled});
    };

#if DEVELOPER_SETTINGS_OPTIONS == 1
    addMenu(i18("app_settings_advanced"), gui::window::name::advanced);
#endif // DEVELOPER_SETTINGS_OPTIONS

    addMenu(i18("app_settings_bt"), gui::window::name::bluetooth);
    addMenu(i18("app_settings_net"), gui::window::name::network);
    addMenu(i18("app_settings_disp_key"), gui::window::name::display_and_keypad);
    addMenu(i18("app_settings_phone_modes"), gui::window::name::phone_modes);
    addMenu(i18("app_settings_apps"), gui::window::name::apps);
    addMenu(i18("app_settings_security"), gui::window::name::security);
    addMenu(i18("app_settings_system"), gui::window::name::system);
    return l;
}
