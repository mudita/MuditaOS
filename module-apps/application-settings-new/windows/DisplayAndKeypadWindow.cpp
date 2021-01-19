// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayAndKeypadWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "windows/OptionWindow.hpp"
#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    DisplayAndKeypadWindow::DisplayAndKeypadWindow(app::Application *app)
        : OptionWindow(app, gui::window::name::display_and_keypad)
    {
        options = displayAndKeypadOptionsList();
        addOptions(options);
        setTitle(utils::localize.get("app_settings_disp_key"));
    }

    std::list<Option> DisplayAndKeypadWindow::displayAndKeypadOptionsList()
    {
        std::list<gui::Option> optionList;

        auto addMenu = [&](UTF8 name, std::string window) {
            optionList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                name,
                [=](gui::Item &item) {
                    if (window.empty()) {
                        return false;
                    }
                    LOG_INFO("switching to %s page", window.c_str());
                    application->switchWindow(window, nullptr);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                gui::option::SettingRightItem::ArrowWhite));
        };

        addMenu(utils::translateI18("app_settings_display_display_light"), gui::window::name::display_light);
        addMenu(utils::translateI18("app_settings_display_font_size"), gui::window::name::font_size);
        addMenu(utils::translateI18("app_settings_display_locked_screen"), gui::window::name::locked_screen);
        addMenu(utils::translateI18("app_settings_display_keypad_light"), gui::window::name::keypad_light);
        addMenu(utils::translateI18("app_settings_display_input_language"), gui::window::name::input_language);

        return optionList;
    }
} // namespace gui
