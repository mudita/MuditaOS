// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateTimeModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/NewEventCheckBoxWithLabel.hpp"
#include <ListView.hpp>
#include <module-utils/time/TimeRangeParser.hpp>

DateTimeModel::DateTimeModel(app::Application *app, bool mode24H) : application(app), mode24H(mode24H)
{}

unsigned int DateTimeModel::requestRecordsCount()
{
    return internalData.size();
}

unsigned int DateTimeModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::checkBox::height;
}

void DateTimeModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

gui::ListItem *DateTimeModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void DateTimeModel::loadData(std::shared_ptr<EventsRecord> record, DateTimeType dateTimeType)
{
    list->clear();
    eraseInternalData();

    auto startTime     = TimePointToHourMinSec(record->date_from);
    auto endTime       = TimePointToHourMinSec(record->date_till);
    auto isAllDayEvent = [&]() -> bool {
        return startTime.hours().count() == 0 && startTime.minutes().count() == 0 &&
               endTime.hours().count() == style::window::calendar::time::max_hour_24H_mode &&
               endTime.minutes().count() == style::window::calendar::time::max_minutes;
    };

    createData(isAllDayEvent(), dateTimeType);

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(record);
        }
    }

    if (isAllDayEvent()) {
        auto dateFilter = record->date_from - TimePointToHourMinSec(record->date_from).hours() -
                          TimePointToHourMinSec(record->date_from).minutes();
        auto [start, end] = utils::time::TimeRangeParser::getPrepopulatedStartAndEndTime(dateFilter);
        auto rec          = std::make_shared<EventsRecord>();
        rec->date_from    = start;
        rec->date_till    = end;
        if (timeItem->onLoadCallback) {
            timeItem->onLoadCallback(rec);
        }
    }

    list->rebuildList();
}

void DateTimeModel::createData(bool isAllDayEvent, DateTimeType dateTimeType)
{
    dateItem = new gui::EventDateItem(dateTimeType);
    timeItem = new gui::EventTimeItem(
        utils::localize.get("app_calendar_time"),
        dateTimeType,
        mode24H,
        [this](const UTF8 &text) { application->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [this]() { application->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
    checkBox = new gui::NewEventCheckBoxWithLabel(application, utils::localize.get("app_calendar_all_day"), this);

    internalData.push_back(dateItem);
    if (!isAllDayEvent) {
        internalData.push_back(timeItem);
    }
    if (dateTimeType == DateTimeType::Start) {
        internalData.push_back(checkBox);
    }

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}

void DateTimeModel::reloadDataWithTimeItem()
{
    internalData.clear();

    internalData.push_back(dateItem);
    internalData.push_back(timeItem);
    internalData.push_back(checkBox);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }

    list->rebuildList();
}

void DateTimeModel::loadDataWithoutTimeItem()
{
    internalData.erase(std::find(internalData.begin(), internalData.end(), timeItem));
    list->rebuildList();
}

TimePoint DateTimeModel::getChosenDate(std::shared_ptr<EventsRecord> record, DateTimeType dateTimeType) const
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(record);
        }
    }
    if (dateTimeType == DateTimeType::Start) {
        return record->date_from;
    }
    return record->date_till;
}
