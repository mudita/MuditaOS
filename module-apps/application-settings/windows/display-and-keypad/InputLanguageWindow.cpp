#include "InputLanguageWindow.hpp"

#include <functional>

#include "service-appmgr/ApplicationManager.hpp"
#include "i18/i18.hpp"
#include "Label.hpp"
#include "Style.hpp"

namespace gui
{

    InputLanguageWindow::InputLanguageWindow(app::Application *app) : SettingsWindow(app, window_name)
    {
        buildInterface();
    }

    void InputLanguageWindow::buildInterface()
    {
        SettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_input_language"));
    }
} // namespace gui
