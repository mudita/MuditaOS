// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditEventModel.hpp"
#include "application-calendar/widgets/NewEventCheckBoxWithLabel.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "AppWindow.hpp"
#include <BottomBar.hpp>
#include <ListView.hpp>
#include <module-db/queries/calendar/QueryEventsAdd.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <time/time_conversion.hpp>

NewEditEventModel::NewEditEventModel(app::Application *app, bool mode24H) : application(app), mode24H(mode24H)
{}

unsigned int NewEditEventModel::requestRecordsCount()
{
    return internalData.size();
}

unsigned int NewEditEventModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::checkBox::height;
}

void NewEditEventModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

uint32_t NewEditEventModel::getRepeatOptionValue()
{
    if (repeat != nullptr) {
        return repeat->repeatOptionValue;
    }
    else {
        LOG_ERROR("Repeat option value returned before assigned!");
        return 0;
    }
}

void NewEditEventModel::setRepeatOptionValue(const uint32_t &value)
{
    if (repeat != nullptr) {
        repeat->repeatOptionValue = value;
    }
    else {
        LOG_ERROR("Value not assigned. Repeat option item was not ready!");
    }
}

gui::ListItem *NewEditEventModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void NewEditEventModel::createData(bool allDayEvent)
{
    auto app = application;
    assert(app != nullptr);

    eventNameInput = new gui::TextWithLabelItem(
        utils::localize.get("app_calendar_new_edit_event_name"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); });

    allDayEventCheckBox = new gui::NewEventCheckBoxWithLabel(
        application, utils::localize.get("app_calendar_new_edit_event_allday"), this);

    dateItem = new gui::EventDateItem();

    startTime = new gui::EventTimeItem(
        utils::localize.get("app_calendar_new_edit_event_start"),
        mode24H,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    endTime = new gui::EventTimeItem(
        utils::localize.get("app_calendar_new_edit_event_end"),
        mode24H,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    reminder = new gui::SeveralOptionsItem(
        application,
        utils::localize.get("app_calendar_event_detail_reminder"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    repeat = new gui::SeveralOptionsItem(
        application,
        utils::localize.get("app_calendar_event_detail_repeat"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    endTime->setConnectionToSecondItem(startTime);
    startTime->setConnectionToSecondItem(endTime);

    startTime->setConnectionToDateItem(dateItem);
    endTime->setConnectionToDateItem(dateItem);

    allDayEventCheckBox->setConnectionToDateItem(dateItem);

    internalData.push_back(eventNameInput);
    internalData.push_back(allDayEventCheckBox);
    internalData.push_back(dateItem);
    if (!allDayEvent) {
        internalData.push_back(startTime);
        internalData.push_back(endTime);
    }
    internalData.push_back(reminder);
    internalData.push_back(repeat);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}

void NewEditEventModel::loadData(std::shared_ptr<EventsRecord> record)
{
    list->clear();
    eraseInternalData();
    auto start_time    = TimePointToHourMinSec(record->date_from);
    auto end_time      = TimePointToHourMinSec(record->date_till);
    auto isAllDayEvent = [&]() -> bool {
        return start_time.hours().count() == 0 && start_time.minutes().count() == 0 &&
               end_time.hours().count() == style::window::calendar::time::max_hour_24H_mode &&
               end_time.minutes().count() == style::window::calendar::time::max_minutes;
    };

    createData(isAllDayEvent());

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(record);
        }
    }

    if (isAllDayEvent()) {
        record->date_from = record->date_from - TimePointToHourMinSec(record->date_from).hours() -
                            TimePointToHourMinSec(record->date_from).minutes() +
                            TimePointToHourMinSec(TimePointNow()).hours() +
                            TimePointToHourMinSec(TimePointNow()).minutes();
        record->date_till = record->date_from + std::chrono::hours(1);
        if (startTime->onLoadCallback) {
            startTime->onLoadCallback(record);
        }
        if (endTime->onLoadCallback) {
            endTime->onLoadCallback(record);
        }
    }

    list->rebuildList();
}

void NewEditEventModel::loadRepeat(const std::shared_ptr<EventsRecord> &record)
{
    if (repeat->onLoadCallback) {
        repeat->onLoadCallback(record);
    }
}

void NewEditEventModel::loadDataWithoutTimeItem()
{
    internalData.erase(std::find(internalData.begin(), internalData.end(), startTime));
    internalData.erase(std::find(internalData.begin(), internalData.end(), endTime));
    list->rebuildList();
}

void NewEditEventModel::reloadDataWithTimeItem()
{
    internalData.clear();

    internalData.push_back(eventNameInput);
    internalData.push_back(allDayEventCheckBox);
    internalData.push_back(dateItem);
    internalData.push_back(startTime);
    internalData.push_back(endTime);
    internalData.push_back(reminder);
    internalData.push_back(repeat);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }

    list->rebuildList();
}

void NewEditEventModel::saveData(std::shared_ptr<EventsRecord> event, EventAction action)
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(event);
        }
    }

    if (action == EventAction::Edit) {
        auto record = event.get();
        record->reminder_fired = TIME_POINT_INVALID;
        if (!record->title.empty()) {
            DBServiceAPI::GetQuery(
                application, db::Interface::Name::Events, std::make_unique<db::query::events::Edit>(*record));
        }
        auto rec  = std::make_unique<EventsRecord>(*record);
        auto data = std::make_unique<EventRecordData>(std::move(rec));
        application->switchWindow(style::window::calendar::name::details_window, std::move(data));
    }
    else {
        auto record = event.get();
        if (!record->title.empty()) {
            DBServiceAPI::GetQuery(
                application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));

            auto data       = std::make_unique<DayMonthData>();
            auto startDate  = TimePointToYearMonthDay(record->date_from);
            auto filterDate = TimePointFromYearMonthDay(startDate);
            std::string monthStr =
                utils::time::Locale::get_month(utils::time::Locale::Month(unsigned(startDate.month()) - 1));
            data->setData(std::to_string(unsigned(startDate.day())) + " " + monthStr, filterDate);

            if (application->getPrevWindow() == style::window::calendar::name::no_events_window) {
                auto app = dynamic_cast<app::ApplicationCalendar *>(application);
                assert(app != nullptr);
                if (app->getEquivalentToEmptyWindow() == EquivalentWindow::DayEventsWindow) {
                    app->popToWindow(gui::name::window::main_window);
                    app->switchWindow(style::window::calendar::name::day_events_window, std::move(data));
                }
                else if (app->getEquivalentToEmptyWindow() == EquivalentWindow::AllEventsWindow) {
                    app->popToWindow(gui::name::window::main_window);
                    app->switchWindow(style::window::calendar::name::all_events_window, std::move(data));
                }
            }
            else {
                if (application->getPrevWindow() == style::window::calendar::name::day_events_window) {
                    application->switchWindow(style::window::calendar::name::day_events_window, std::move(data));
                }
                application->returnToPreviousWindow();
            }
        }
        else {
            application->returnToPreviousWindow();
        }
    }

    list->clear();
    eraseInternalData();
}
