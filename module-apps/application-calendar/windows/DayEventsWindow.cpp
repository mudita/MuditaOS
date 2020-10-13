#include "DayEventsWindow.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "module-apps/application-calendar/ApplicationCalendar.hpp"
#include <gui/widgets/Window.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>

#include <time/time_conversion.hpp>
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-services/service-db/messages/DBNotificationMessage.hpp>

namespace gui
{

    DayEventsWindow::DayEventsWindow(app::Application *app)
        : AppWindow(app, style::window::calendar::name::day_events_window),
          dayEventsModel{std::make_shared<DayEventsInternalModel>(this->application)}
    {
        buildInterface();
    }

    void DayEventsWindow::rebuild()
    {
        buildInterface();
    }
    void DayEventsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto filterTill = filterFrom + std::chrono::hours(style::window::calendar::time::max_hour_24H_mode + 1);
        auto query      = std::make_unique<db::query::events::GetFiltered>(filterFrom, filterTill);
        query->setQueryListener(
            db::QueryCallback::fromFunction([this](auto response) { return handleQueryResponse(response); }));
        DBServiceAPI::GetQuery(application, db::Interface::Name::Events, std::move(query));
        setTitle(dayMonthTitle);
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

        dayMonthTitle = item->getDayMonthText();
        filterFrom    = item->getDateFilter();
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

        topBar->setActive(gui::TopBar::Elements::TIME, true);
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
                                          dayEventsModel);
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
            rec->date_till = filterFrom;
            auto event     = std::make_shared<EventsRecord>(*rec);
            data->setData(event);
            application->switchWindow(
                style::window::calendar::name::new_edit_event, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }
        return false;
    }

    auto DayEventsWindow::handleQueryResponse(db::QueryResult *queryResult) -> bool
    {
        if (const auto response = dynamic_cast<db::query::events::GetFilteredResult *>(queryResult)) {
            std::unique_ptr<std::vector<EventsRecord>> records = response->getResult();
            auto app                                           = dynamic_cast<app::ApplicationCalendar *>(application);
            assert(application != nullptr);
            app->setEquivalentToEmptyWindow(EquivalentWindow::DayEventsWindow);
            if (records->empty()) {
                auto name = dayMonthTitle;
                app->switchToNoEventsWindow(name, filterFrom);
                return true;
            }
            dayEventsModel->loadData(std::move(records));
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        }
        LOG_DEBUG("Response False");
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
