// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditEventModel.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "AppWindow.hpp"
#include "messages/DialogMetadataMessage.hpp"
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

void NewEditEventModel::createData()
{
    auto app = application;
    assert(app != nullptr);

    eventNameInput = new gui::TextWithLabelItem(
        utils::localize.get("app_calendar_new_edit_event_name"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); });

    startDate = new gui::DateTimeItem(app, DateTimeType::Start);
    endDate   = new gui::DateTimeItem(app, DateTimeType::End);

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

    internalData.push_back(eventNameInput);
    internalData.push_back(startDate);
    internalData.push_back(endDate);
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

    createData();

    setDateItemInputCallback(startDate, utils::localize.get("app_calendar_start_date"), record);
    setDateItemInputCallback(endDate, utils::localize.get("app_calendar_end_date"), record);

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(record);
        }
    }

    list->rebuildList();
}

void NewEditEventModel::setDateItemInputCallback(gui::DateTimeItem *dateItem,
                                                 const std::string &description,
                                                 const std::shared_ptr<EventsRecord> &record)
{
    dateItem->inputCallback = [=](gui::Item &item, const gui::InputEvent &event) {
        if (event.isShortPress() && (event.is(gui::KeyCode::KEY_LF) || gui::toNumeric(event.keyCode) >= 0)) {
            auto data = std::make_unique<EventRecordData>(record);
            data->setDescription(description);
            application->switchWindow(style::window::calendar::name::date_time_window, std::move(data));
            return true;
        }
        return false;
    };
}

void NewEditEventModel::loadRepeat(const std::shared_ptr<EventsRecord> &record)
{
    if (repeat->onLoadCallback) {
        repeat->onLoadCallback(record);
    }
}

void NewEditEventModel::saveData(std::shared_ptr<EventsRecord> event, EventAction action)
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(event);
        }
    }

    if (event->date_from > event->date_till) {
        dateNotValid(action);
    }
    else {
        if (action == EventAction::Edit) {
            auto record            = event.get();
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
                auto dateFrom   = TimePointToYearMonthDay(record->date_from);
                auto filterDate = TimePointFromYearMonthDay(dateFrom);
                std::string monthStr =
                    utils::time::Locale::get_month(utils::time::Locale::Month(unsigned(dateFrom.month()) - 1));
                data->setData(std::to_string(unsigned(dateFrom.day())) + " " + monthStr, filterDate);

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
}

void NewEditEventModel::loadStartDate(const std::shared_ptr<EventsRecord> &record)
{
    if (startDate->onLoadCallback) {
        startDate->onLoadCallback(record);
    }
}

void NewEditEventModel::loadEndDate(const std::shared_ptr<EventsRecord> &record)
{
    if (endDate->onLoadCallback) {
        endDate->onLoadCallback(record);
    }
}

void NewEditEventModel::dateNotValid(EventAction action)
{
    LOG_DEBUG("Start date is not before end date");
    gui::DialogMetadata meta;

    meta.action = [=]() -> bool {
        application->returnToPreviousWindow();
        return true;
    };
    meta.text = utils::localize.get("app_calendar_wrong_date_info");
    if (action == EventAction::Edit) {
        meta.title = utils::localize.get("app_calendar_edit_event_title");
    }
    else {
        meta.title = utils::localize.get("app_calendar_new_event_title");
    }
    meta.icon = "X_image";

    application->switchWindow(style::window::calendar::name::dialog_confirm,
                              std::make_unique<gui::DialogMetadataMessage>(meta));
}

void NewEditEventModel::reloadWithoutRepeat()
{
    if (const auto it = std::find(internalData.begin(), internalData.end(), repeat); it != internalData.end()) {
        internalData.erase(std::find(internalData.begin(), internalData.end(), repeat));
        list->rebuildList();
    }
}
