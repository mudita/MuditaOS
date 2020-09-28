#include "DisplayLightWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>

namespace gui
{
    DisplayLightWindow::DisplayLightWindow(app::Application *app) : BaseSettingsWindow(app, window::name::display_light)
    {
        buildInterface();
    }

    void DisplayLightWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_display_light"));
    }

} // namespace gui
