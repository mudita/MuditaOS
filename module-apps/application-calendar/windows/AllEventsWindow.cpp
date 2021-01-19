// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllEventsWindow.hpp"
#include "InputEvent.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>
#include <gui/widgets/Window.hpp>
#include <service-appmgr/Controller.hpp>

#include <module-db/queries/calendar/QueryEventsGetAllLimited.hpp>
#include <time/time_conversion.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>

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
        setFocusItem(allEventsList);
    }

    void AllEventsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        allEventsList->rebuildList();
    }

    auto AllEventsWindow::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }
        auto *item = dynamic_cast<DayMonthData *>(data);
        if (item == nullptr) {
            return false;
        }
        dateFilter = item->getDateFilter();
        return true;
    }

    bool AllEventsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.keyCode == gui::KeyCode::KEY_RF &&
            inputEvent.state == gui::InputEvent::State::keyReleasedShort) {
            LOG_DEBUG("Switch to desktop");
            app::manager::Controller::switchBack(application);
        }

        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
            LOG_DEBUG("Switch to new event window");
            std::unique_ptr<EventRecordData> data = std::make_unique<EventRecordData>();
            data->setDescription(style::window::calendar::new_event);
            auto event       = std::make_shared<EventsRecord>();
            event->date_from = dateFilter;
            event->date_till = dateFilter + std::chrono::hours(style::window::calendar::time::max_hour_24H_mode) +
                               std::chrono::minutes(style::window::calendar::time::max_minutes);
            data->setData(event);
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

    bool AllEventsWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msgNotification != nullptr) {
            if (msgNotification->interface == db::Interface::Name::Events) {
                if (msgNotification->dataModified()) {
                    allEventsList->rebuildList(style::listview::RebuildType::InPlace);
                    return true;
                }
            }
        }
        return false;
    }
} /* namespace gui */
