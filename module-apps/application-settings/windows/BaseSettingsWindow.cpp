#include "BaseSettingsWindow.hpp"

#include "service-appmgr/ApplicationManager.hpp"

#include "i18/i18.hpp"
#include "Label.hpp"

#include <Style.hpp>

namespace gui
{

    SettingsWindow::SettingsWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void SettingsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::TIME, true);
    }

    void SettingsWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void SettingsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

} // namespace gui
