#include "DisplayAndKeypadWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>
#include <OptionWindow.hpp>

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
        std::list<gui::Option> l;

        auto i18     = [](std::string text) { return utils::localize.get(text); };
        auto addMenu = [&](UTF8 name, std::string window = "") {
            l.emplace_back(gui::Option{name,
                                       [=](gui::Item &item) {
                                           if (window == "") {
                                               return false;
                                           }
                                           LOG_INFO("switching to %s page", window.c_str());
                                           application->switchWindow(window, nullptr);
                                           return true;
                                       },
                                       gui::Arrow::Enabled});
        };

        addMenu(i18("app_settings_display_display_light"), gui::window::name::display_light);
        addMenu(i18("app_settings_display_font_size"), gui::window::name::font_size);
        addMenu(i18("app_settings_display_locked_screen"), gui::window::name::locked_screen);
        addMenu(i18("app_settings_display_keypad_light"), gui::window::name::keypad_light);
        addMenu(i18("app_settings_display_input_language"), gui::window::name::input_language);

        return l;
    }
} // namespace gui
