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

namespace gui
{

    DayEventsWindow::DayEventsWindow(app::Application *app, std::string name)
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
        DBServiceAPI::GetQuery(application,
                               db::Interface::Name::Events,
                               std::make_unique<db::query::events::GetFiltered>(filterFrom, filterTill));
        setTitle(dayMonthTitle);
        auto dataReceived = dynamic_cast<PrevWindowData *>(data);
        if (dataReceived != nullptr) {
            if (dataReceived->getData() == PrevWindowData::PrevWindow::Delete) {
                checkEmpty = true;
            }
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
            data->setDescription("New");
            auto rec       = new EventsRecord();
            rec->date_from = filterFrom;
            rec->date_till = filterFrom;
            auto event     = std::make_shared<EventsRecord>(*rec);
            data->setData(event);
            data->setWindowName(style::window::calendar::name::day_events_window);
            application->switchWindow(
                style::window::calendar::name::new_edit_event, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }
        return false;
    }

    bool DayEventsWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto msg = dynamic_cast<db::QueryResponse *>(msgl);
        if (msg != nullptr) {
            auto temp = msg->getResult();
            if (auto response = dynamic_cast<db::query::events::GetFilteredResult *>(temp.get())) {
                unique_ptr<vector<EventsRecord>> records = response->getResult();
                for (auto &rec : *records) {
                    LOG_DEBUG("record: %s", rec.title.c_str());
                }
                if (checkEmpty) {
                    if (records->size() == 0) {
                        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
                        assert(application != nullptr);
                        auto name = dayMonthTitle;
                        app->switchToNoEventsWindow(name, filterFrom, style::window::calendar::name::day_events_window);
                    }
                }
                dayEventsModel->loadData(std::move(records));
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            LOG_DEBUG("Response False");
            return false;
        }
        LOG_DEBUG("DayWindow DB Message != QueryResponse");
        return false;
    }
} /* namespace gui */
