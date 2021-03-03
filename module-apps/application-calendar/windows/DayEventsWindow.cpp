// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DayEventsWindow.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include <gui/widgets/Window.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>

#include <time/time_conversion.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>

namespace gui
{

    DayEventsWindow::DayEventsWindow(app::Application *app)
        : AppWindow(app, style::window::calendar::name::day_events_window),
          dayEventsModel{std::make_shared<DayEventsModel>(this->application)}
    {
        buildInterface();
    }

    void DayEventsWindow::rebuild()
    {
        buildInterface();
    }
    void DayEventsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == gui::ShowMode::GUI_SHOW_INIT) {
            dayEventsList->rebuildList();
        }
    }

    auto DayEventsWindow::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<DayMonthData *>(data);
        if (item == nullptr) {
            return false;
        }

        dayMonthTitle   = item->getDayMonthText();
        filterFrom      = item->getDateFilter();
        auto filterTill = filterFrom + date::days{1};
        dayEventsModel->setFilters(filterFrom, filterTill, dayMonthTitle);
        LOG_DEBUG("FILTER 1: %s", TimePointToString(filterFrom).c_str());
        setTitle(dayMonthTitle);
        if (dayMonthTitle == "") {
            return false;
        }

        return true;
    }

    void DayEventsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));

        setTitle(dayMonthTitle);
        leftArrowImage = new gui::Image(
            this, style::window::calendar::arrow_x, style::window::calendar::arrow_y, 0, 0, "arrow_left");
        newDayEventImage =
            new gui::Image(this, style::window::calendar::cross_x, style::window::calendar::cross_y, 0, 0, "cross");

        dayEventsList = new gui::ListView(this,
                                          style::window::calendar::listView_x,
                                          style::window::calendar::listView_y,
                                          style::window::calendar::listView_w,
                                          style::window::calendar::listView_h,
                                          dayEventsModel,
                                          style::listview::ScrollBarType::Fixed);
        setFocusItem(dayEventsList);
    }

    bool DayEventsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
            LOG_DEBUG("Switch to new window - edit window");
            std::unique_ptr<EventRecordData> data = std::make_unique<EventRecordData>();
            data->setDescription(style::window::calendar::new_event);
            auto rec       = new EventsRecord();
            rec->date_from = filterFrom;
            rec->date_till = filterFrom + std::chrono::hours(utils::time::Locale::max_hour_24H_mode) +
                             std::chrono::minutes(utils::time::Locale::max_minutes);
            auto event = std::make_shared<EventsRecord>(*rec);
            data->setData(event);
            application->switchWindow(
                style::window::calendar::name::new_edit_event, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }
        return false;
    }

    bool DayEventsWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msgNotification != nullptr) {
            if (msgNotification->interface == db::Interface::Name::Events) {
                if (msgNotification->dataModified()) {
                    dayEventsList->rebuildList(style::listview::RebuildType::InPlace);
                    return true;
                }
            }
        }
        return false;
    }
} /* namespace gui */
