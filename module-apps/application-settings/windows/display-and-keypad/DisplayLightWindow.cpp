#include "DisplayLightWindow.hpp"

#include <functional>

#include "service-appmgr/ApplicationManager.hpp"
#include "Label.hpp"
#include "Style.hpp"

namespace gui
{
    DisplayLightWindow::DisplayLightWindow(app::Application *app) : SettingsWindow(app, window_name)
    {
        buildInterface();
    }

    void DisplayLightWindow::buildInterface()
    {
        SettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_display_light"));
    }

} // namespace gui
