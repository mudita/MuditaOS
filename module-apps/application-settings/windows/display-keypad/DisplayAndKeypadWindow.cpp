// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayAndKeypadWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    DisplayAndKeypadWindow::DisplayAndKeypadWindow(app::ApplicationCommon *app,
                                                   std::unique_ptr<display_mode::DisplayModeModel> &&displayMode)
        : OptionWindow(app, gui::window::name::display_and_keypad), displayMode{std::move(displayMode)}
    {
        addOptions(displayAndKeypadOptionsList(this->displayMode->isDarkModeEnabled()));
        setTitle(utils::translate("app_settings_disp_key"));
    }

    std::list<Option> DisplayAndKeypadWindow::displayAndKeypadOptionsList(bool isDarkModeEnabled)
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
                        this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                    }
                    return true;
                },
                this,
                gui::option::SettingRightItem::ArrowWhite));
        };

        auto addOnOffOption = [&](const UTF8 &text, std::function<bool(gui::Item &)> onActivated) {
            optionList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                text,
                [=](gui::Item &item) mutable { return onActivated(item); },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                    }
                    return true;
                },
                this,
                isDarkModeEnabled ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));
        };

        addMenu(utils::translate("app_settings_display_display_light"), gui::window::name::display_light);
#if DISABLED_SETTINGS_OPTIONS == 1
        addMenu(utils::translate("app_settings_display_font_size"), gui::window::name::font_size);
#endif // DISABLED_SETTINGS_OPTIONS
        addMenu(utils::translate("app_settings_display_wallpaper"), gui::window::name::wallpaper);
        addMenu(utils::translate("app_settings_display_keypad_light"), gui::window::name::keypad_light);
        addMenu(utils::translate("app_settings_display_input_language"), gui::window::name::input_language);
        addOnOffOption(utils::translate("app_settings_display_dark_mode"), [this](gui::Item & /*item*/) {
            switchDisplayMode();
            return true;
        });
        return optionList;
    }

    void DisplayAndKeypadWindow::switchDisplayMode()
    {
        auto onSuccess = [this](display_mode::Mode mode) {
            refreshOptions(displayAndKeypadOptionsList(mode == display_mode::Mode::Dark));
        };
        displayMode->setMode(displayMode->isDarkModeEnabled() ? display_mode::Mode::Light : display_mode::Mode::Dark,
                             std::move(onSuccess));
    }
} // namespace gui
