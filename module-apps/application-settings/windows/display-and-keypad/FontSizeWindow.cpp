#include "FontSizeWindow.hpp"

#include "service-appmgr/ApplicationManager.hpp"
#include "Label.hpp"
#include <Style.hpp>

namespace gui
{

    FontSizeWindow::FontSizeWindow(app::Application *app) : SettingsWindow(app, window_name)
    {
        buildInterface();
    }

    void FontSizeWindow::buildInterface()
    {
        SettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_font_size"));
    }

} // namespace gui
