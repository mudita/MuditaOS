#include "AllEventsWindow.hpp"
#include <gui/widgets/Window.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>
#include <service-appmgr/ApplicationManager.hpp>

namespace gui
{

    AllEventsWindow::AllEventsWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::all_events_window),
          allEventsModel{std::make_shared<AllEventsModel>(this->application)}
    {
        buildInterface();
    }

    void AllEventsWindow::rebuild()
    {
        buildInterface();
    }

    void AllEventsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get("app_calendar_bar_month"));

        setTitle(utils::localize.get("app_calendar_title_main"));
        leftArrowImage = new gui::Image(
            this, style::window::calendar::arrow_x, style::window::calendar::arrow_y, 0, 0, "arrow_left");
        newDayEventImage =
            new gui::Image(this, style::window::calendar::cross_x, style::window::calendar::cross_y, 0, 0, "cross");

        allEventsList = new gui::CalendarListView(this,
                                                  style::window::calendar::listView_x,
                                                  style::window::calendar::listView_y,
                                                  style::window::calendar::listView_w,
                                                  style::window::calendar::listView_h,
                                                  allEventsModel);

        allEventsList->setPenFocusWidth(style::window::default_border_no_focus_w);
        allEventsList->setPenWidth(style::window::default_border_no_focus_w);

        setFocusItem(allEventsList);
    }

    void AllEventsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        allEventsList->rebuildList();
    }

    bool AllEventsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.keyCode == gui::KeyCode::KEY_RF &&
            inputEvent.state == gui::InputEvent::State::keyReleasedShort) {
            LOG_DEBUG("Switch to desktop");
            sapm::ApplicationManager::messageSwitchPreviousApplication(application);
        }

        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
            LOG_DEBUG("Switch to new event window");
            std::unique_ptr<gui::SwitchData> data = std::make_unique<SwitchData>();
            data->setDescription("New");
            application->switchWindow(
                style::window::calendar::name::new_edit_event, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LF) {
            application->switchWindow(gui::name::window::main_window);
            LOG_DEBUG("Switch to month view - main window");
            return true;
        }

        return false;
    }
} /* namespace gui */
