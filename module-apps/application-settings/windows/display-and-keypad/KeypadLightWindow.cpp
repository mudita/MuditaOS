#include "KeypadLightWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"
#include "i18/i18.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "Style.hpp"

namespace gui
{

    KeypadLightWindow::KeypadLightWindow(app::Application *app) : SettingsWindow(app, window_name)
    {
        buildInterface();
    }

    void KeypadLightWindow::buildInterface()
    {
        SettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_keypad_light"));
    }

} // namespace gui
