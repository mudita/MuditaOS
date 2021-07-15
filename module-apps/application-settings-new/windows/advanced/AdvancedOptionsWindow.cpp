// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AdvancedOptionsWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>

std::list<gui::Option> advancedOptions(app::Application *app)
{
    std::list<gui::Option> l;

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

    addMenu("Information", gui::window::name::information);
    addMenu("UI Test", gui::window::name::ui_test);
    addMenu("Eink Mode", gui::window::name::eink_mode);
    addMenu("Color Test", gui::window::name::color_test_window);

    return l;
}
