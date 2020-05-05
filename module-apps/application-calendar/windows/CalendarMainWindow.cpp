#include "gui/widgets/Window.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "CalendarMainWindow.hpp"

namespace app
{

    CalendarMainWindow::CalendarMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void CalendarMainWindow::rebuild()
    {}

    void CalendarMainWindow::buildInterface()
    {
        AppWindow::buildInterface();
    }
    void CalendarMainWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }
} /* namespace app */
