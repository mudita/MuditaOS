#include "gui/widgets/Window.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "DayWindow.hpp"

#include <time/time_conversion.hpp>
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace app
{

    DayWindow::DayWindow(app::Application *app, std::string name)
        : AppWindow(app, "DayWindow"), calendarModel{std::make_shared<CalendarModel>(this->application)}
    {
        buildInterface();
    }

    void DayWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void DayWindow::buildInterface()
    {
        AppWindow::buildInterface();

        auto ttime = utils::time::Time();
        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));

        setTitle(ttime.str("%d %B"));
        leftArrowImage = new gui::Image(
            this, style::window::calendar::arrow_x, style::window::calendar::arrow_y, 0, 0, "arrow_left");
        newDayEventImage =
            new gui::Image(this, style::window::calendar::cross_x, style::window::calendar::cross_y, 0, 0, "cross");

        list = new gui::ListView(this,
                                 style::window::calendar::listView_x,
                                 style::window::calendar::listView_y,
                                 style::window::calendar::listView_w,
                                 style::window::calendar::listView_h,
                                 calendarModel);

        list->setPenFocusWidth(0);
        list->setPenWidth(0);
        list->setVisible(true);

        list->focusChangedCallback = [=](gui::Item &) {
            bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
            return true;
        };
        setFocusItem(list);
    }

    void DayWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    void DayWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (calendarModel->getItemCount() == 0) {
            LOG_DEBUG("Switch to no events window");
            application->switchWindow("NoEventsWindow");
        }
    }

    bool DayWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.state == gui::InputEvent::State::keyReleasedShort ||
            inputEvent.state == gui::InputEvent::State::keyReleasedLong) {
            if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
                LOG_DEBUG("TODO: Switch to new window - edit window");
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
    }
} /* namespace app */
