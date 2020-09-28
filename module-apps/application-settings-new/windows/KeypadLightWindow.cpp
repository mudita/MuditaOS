#include "KeypadLightWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>

namespace gui
{

    KeypadLightWindow::KeypadLightWindow(app::Application *app) : BaseSettingsWindow(app, window::name::keypad_light)
    {
        buildInterface();
    }

    void KeypadLightWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_keypad_light"));
    }

} // namespace gui
