﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditEventModel.hpp"
#include "AppWindow.hpp"
#include <application-calendar/widgets/NewEventCheckBoxWithLabel.hpp>
#include <application-calendar/widgets/TextWithLabelItem.hpp>
#include <application-calendar/widgets/CalendarStyle.hpp>
#include <application-calendar/widgets/CalendarDateItem.hpp>
#include <application-calendar/widgets/CalendarTimeItem.hpp>
#include <application-calendar/widgets/SeveralOptionsItem.hpp>
#include <application-calendar/data/CalendarData.hpp>
#include <application-calendar/ApplicationCalendar.hpp>
#include <module-db/queries/calendar/QueryEventsAdd.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>

#include <service-db/DBServiceAPI.hpp>
#include <time/time_conversion.hpp>
#include <ListView.hpp>

NewEditEventModel::NewEditEventModel(app::Application *app) : application(app)
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

gui::ListItem *NewEditEventModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void NewEditEventModel::createData(bool allDayEvent)
{
    auto app = application;
    assert(app != nullptr);

    eventNameInput = new gui::TextWithLabelItem(
        utils::translate("app_calendar_new_edit_event_name"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); });

    allDayEventCheckBox = new gui::NewEventCheckBoxWithLabel(
        application, utils::translate("app_calendar_new_edit_event_allday"), [this](bool isChecked) {
            isChecked ? createTimeItems() : eraseTimeItems();
            const auto it = std::find(std::begin(internalData), std::end(internalData), allDayEventCheckBox);
            list->rebuildList(gui::listview::RebuildType::OnPageElement, std::distance(std::begin(internalData), it));
        });

    dateItem = new gui::CalendarDateItem();

    reminder = new gui::SeveralOptionsItem(
        application,
        utils::translate("app_calendar_event_detail_reminder"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    repeat = new gui::SeveralOptionsItem(
        application,
        utils::translate("app_calendar_event_detail_repeat"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    allDayEventCheckBox->setConnectionToDateItem(dateItem->getDateWidget());

    internalData.push_back(eventNameInput);
    internalData.push_back(allDayEventCheckBox);
    internalData.push_back(dateItem);
    internalData.push_back(reminder);
    internalData.push_back(repeat);

    if (!allDayEvent) {
        createTimeItems();
    }
    else {
        startTimeBuffer +=
            TimePointToHourMinSec(TimePointNow()).hours() + TimePointToHourMinSec(TimePointNow()).minutes();
        endTimeBuffer = startTimeBuffer + std::chrono::hours(1);
    }

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}

void NewEditEventModel::loadData(std::shared_ptr<EventsRecord> record)
{
    clearData();
    startTimeBuffer = record->date_from;
    endTimeBuffer   = record->date_till;

    createData(gui::allDayEvents::isAllDayEvent(startTimeBuffer, endTimeBuffer));

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(record);
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

void NewEditEventModel::saveData(std::shared_ptr<EventsRecord> event, EventAction action)
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(event);
        }
    }

    std::string errorMessage;
    if (!isDataCorrect(event, errorMessage)) {

        std::string windowTitle = (action == EventAction::Edit) ? utils::translate("app_calendar_edit_event_title")
                                                                : utils::translate("app_calendar_new_event_title");

        application->switchWindow(
            gui::window::name::dialog_confirm,
            gui::ShowMode::GUI_SHOW_INIT,
            std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                std::move(windowTitle), "emergency_W_G", std::move(errorMessage), "", [=]() -> bool {
                    application->returnToPreviousWindow();
                    return true;
                }}));

        return;
    }

    if (action == EventAction::Edit) {
        saveEditData(event);
    }
    else {
        saveNewData(event);
    }

    clearData();
}

void NewEditEventModel::saveNewData(std::shared_ptr<EventsRecord> event)
{
    DBServiceAPI::GetQuery(application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*event));

    auto data            = std::make_unique<DayMonthData>();
    auto startDate       = TimePointToYearMonthDay(event->date_from);
    auto filterDate      = TimePointFromYearMonthDay(startDate);
    std::string monthStr = utils::time::Locale::get_month(utils::time::Locale::Month(unsigned(startDate.month()) - 1));
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

void NewEditEventModel::saveEditData(std::shared_ptr<EventsRecord> event)
{
    event->reminder_fired = TIME_POINT_INVALID;
    if (!event->title.empty()) {
        DBServiceAPI::GetQuery(
            application, db::Interface::Name::Events, std::make_unique<db::query::events::Edit>(*event));
    }
    auto rec  = std::make_unique<EventsRecord>(*event);
    auto data = std::make_unique<EventRecordData>(std::move(rec));
    application->switchWindow(style::window::calendar::name::details_window, std::move(data));
}

bool NewEditEventModel::isDataCorrect(std::shared_ptr<EventsRecord> event, std::string &message)
{
    if (event->title.empty()) {
        LOG_WARN("Event data has empty title!");
        message = utils::translate("app_calendar_event_error_empty_name");
        return false;
    }

    if (event->date_from > event->date_till) {
        LOG_WARN("Event data has wrong dates!");
        message = utils::translate("app_calendar_event_error_dates");
        return false;
    }

    return true;
}

void NewEditEventModel::createTimeItems()
{
    auto create = [this](gui::CalendarTimeItem *&item, const std::string &description, gui::TimeWidget::Type type) {
        if (item == nullptr) {
            auto app = application;
            item     = new gui::CalendarTimeItem(
                description,
                type,
                [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
                [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
                startTimeBuffer,
                endTimeBuffer);

            item->deleteByList = false;
            item->setConnectionToDateItem(dateItem);
            internalData.insert(std::find(internalData.begin(), internalData.end(), reminder), item);
        }
    };

    create(startTime, utils::translate("app_calendar_new_edit_event_start"), gui::TimeWidget::Type::Start);
    create(endTime, utils::translate("app_calendar_new_edit_event_end"), gui::TimeWidget::Type::End);

    startTime->setConnectionToSecondItem(endTime);
    endTime->setConnectionToSecondItem(startTime);
}

void NewEditEventModel::eraseTimeItems()
{
    auto erase = [this](gui::CalendarTimeItem *&item) {
        if (item != nullptr) {
            item->deleteByList = true;
            internalData.erase(std::find(internalData.begin(), internalData.end(), item));
            item = nullptr;
        }
    };

    startTimeBuffer = startTime->getFromTillDate()->from;
    endTimeBuffer   = endTime->getFromTillDate()->till;
    erase(startTime);
    erase(endTime);
}

void NewEditEventModel::clearData()
{
    list->reset();
    eraseInternalData();
    startTime       = nullptr;
    endTime         = nullptr;
    startTimeBuffer = TimePoint();
    endTimeBuffer   = TimePoint();
}
