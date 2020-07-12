#include "gui/widgets/Window.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "DayWindow.hpp"
#include "../models/CalendarModel.hpp"
#include "../models/CalendarModel.cpp"

#include <time/time_conversion.hpp>
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace app
{

    DayWindow::DayWindow(app::Application *app, std::string name)
        : AppWindow(app, "DayWindow"), model{new CalendarModel(app)}
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
        leftArrowImage   = new gui::Image(this, calendarStyle::arrow_x, calendarStyle::arrow_y, 0, 0, "arrow_left");
        newDayEventImage = new gui::Image(this, calendarStyle::cross_x, calendarStyle::cross_y, 0, 0, "cross");

        list = new gui::ListView(this,
                                 calendarStyle::listView_x,
                                 calendarStyle::listView_y,
                                 calendarStyle::listView_w,
                                 calendarStyle::listView_h);

        list->setPenFocusWidth(0);
        list->setPenWidth(0);
        list->setProvider(model);
        list->setVisible(true);

        list->focusChangedCallback = [=](gui::Item & /*item*/) {
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
        if (model->getItemCount() == 0) {
            LOG_DEBUG("Switch to no events window");
            application->switchWindow("NoEventsWindow");
        }
    }

    bool DayWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if ((inputEvent.state != gui::InputEvent::State::keyReleasedShort) ||
            (inputEvent.state != gui::InputEvent::State::keyReleasedLong)) {
            if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
                LOG_DEBUG("Switch to new window - edit window");
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
    }
} /* namespace app */
