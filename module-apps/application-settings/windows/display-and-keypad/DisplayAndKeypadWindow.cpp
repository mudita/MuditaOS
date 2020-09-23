#include "DisplayAndKeypadWindow.hpp"

#include "module-apps/application-settings/ApplicationSettings.hpp"

#include "InputLanguageWindow.hpp"
#include "LockedScreenWindow.hpp"
#include "FontSizeWindow.hpp"
#include "DisplayLightWindow.hpp"
#include "KeypadLightWindow.hpp"

#include "i18/i18.hpp"
#include "Label.hpp"
#include "Style.hpp"
#include "OptionWindow.hpp"

namespace gui
{
    std::list<gui::Option> displayAndKeypadOptions(app::Application *app)
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
                                           app->switchWindow(window, nullptr);
                                           return true;
                                       },
                                       gui::Arrow::Enabled});
        };

        addMenu(i18("app_settings_display_display_light"), DisplayLightWindow::window_name);
        addMenu(i18("app_settings_display_font_size"), FontSizeWindow::window_name);
        addMenu(i18("app_settings_display_locked_screen"), LockedScreenWindow::window_name);
        addMenu(i18("app_settings_display_keypad_light"), KeypadLightWindow::window_name);
        addMenu(i18("app_settings_display_input_language"), InputLanguageWindow::window_name);

        return l;
    }

} // namespace gui
