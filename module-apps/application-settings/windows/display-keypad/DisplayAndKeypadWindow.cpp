// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayAndKeypadWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    DisplayAndKeypadWindow::DisplayAndKeypadWindow(app::ApplicationCommon *app)
        : OptionWindow(app, gui::window::name::display_and_keypad)
    {
        addOptions(displayAndKeypadOptionsList());
        setTitle(utils::translate("app_settings_disp_key"));
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
                        this->setBottomBarText(utils::translate(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                gui::option::SettingRightItem::ArrowWhite));
        };

        addMenu(utils::translate("app_settings_display_display_light"), gui::window::name::display_light);

#if DISABLED_SETTINGS_OPTIONS == 1
        addMenu(utils::translate("app_settings_display_font_size"), gui::window::name::font_size);
        addMenu(utils::translate("app_settings_display_wallpaper"), gui::window::name::wallpaper);
#endif // DISABLED_SETTINGS_OPTIONS

        addMenu(utils::translate("app_settings_display_keypad_light"), gui::window::name::keypad_light);
        addMenu(utils::translate("app_settings_display_input_language"), gui::window::name::input_language);

        return optionList;
    }
} // namespace gui
