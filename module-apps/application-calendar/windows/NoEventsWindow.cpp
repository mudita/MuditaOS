#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "NoEventsWindow.hpp"

#include <time/time_conversion.hpp>
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace app
{

    NoEventsWindow::NoEventsWindow(app::Application *app, std::string name) : AppWindow(app, "NoEventsWindow")
    {
        buildInterface();
    }

    void NoEventsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void NoEventsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        auto ttime = utils::time::Time();
        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setTitle(ttime.str("%d %B"));
        leftArrowImage   = new gui::Image(this, calendarStyle::arrow_x, calendarStyle::arrow_y, 0, 0, "arrow_left");
        newDayEventImage = new gui::Image(this, calendarStyle::cross_x, calendarStyle::cross_y, 0, 0, "cross");

        emptyListIcon = new gui::Icon(this,
                                      0,
                                      style::header::height,
                                      style::window_width,
                                      style::window_height - style::header::height - style::footer::height,
                                      "phonebook_empty_grey_circle_W_G",
                                      "No events yet.\nPress left arrow to add new.");

        emptyListIcon->setVisible(true);
    }

    void NoEventsWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    bool NoEventsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.state == gui::InputEvent::State::keyReleasedShort ||
            inputEvent.state == gui::InputEvent::State::keyReleasedLong) {
            if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
                // application->switchWindow("");
                LOG_DEBUG("New window - edit window");
                return true;
            }

            if (inputEvent.keyCode == gui::KeyCode::KEY_RF) {
                application->switchWindow("MainWindow");
                LOG_DEBUG("Switch to main window");
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
    }
} /* namespace app */
