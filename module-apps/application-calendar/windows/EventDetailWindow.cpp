#include "EventDetailWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <gui/widgets/Window.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    EventDetailWindow::EventDetailWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::details_window), eventDetailModel{
                                                                             std::make_shared<EventDetailModel>(app)}
    {
        buildInterface();
    }

    void EventDetailWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void EventDetailWindow::buildInterface()
    {
        AppWindow::buildInterface();

        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));

        setTitle(utils::time::Time().str("%d %B"));

        bodyList = new gui::ListView(this,
                                     style::window::calendar::listView_x,
                                     style::window::calendar::listView_y,
                                     style::window::calendar::listView_w,
                                     style::window::calendar::listView_h,
                                     eventDetailModel);

        setFocusItem(bodyList);
    }

    void EventDetailWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (mode == gui::ShowMode::GUI_SHOW_INIT) {
            bodyList->rebuildList();
        }

        eventDetailModel->loadData();
    }

    bool EventDetailWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LF) {
            LOG_DEBUG("Switch to option window");
            application->switchWindow(style::window::calendar::name::events_options);
            return true;
        }

        return false;
    }
} /* namespace gui */
