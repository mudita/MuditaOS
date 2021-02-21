// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllEventsModel.hpp"
#include "application-calendar/widgets/AllEventsItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include <ListView.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <queries/calendar/QueryEventsGetAll.hpp>
#include <module-utils/time/TimeRangeParser.hpp>

AllEventsModel::AllEventsModel(app::Application *app) : MultiDayEventsDatabaseModel(app), app::AsyncCallbackReceiver{app}
{}

unsigned int AllEventsModel::requestRecordsCount()
{
    return recordsCount;
}

void AllEventsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    if (queryType == db::Query::Type::Update) {
        MultiDayEventsDatabaseModel::clearRecords();
    }

    MultiDayEventsDatabaseModel::setOffsetLimit(offset,limit);

    if (offset <= counter || queryType != db::Query::Type::Read) {
        queryType  = db::Query::Type::Read;
        auto query = std::make_unique<db::query::events::GetAll>();
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Events);
        task->setCallback([this](auto response) { return handleQueryResponse(response); });
        task->execute(application, this);
        LOG_DEBUG("Load records from db");
    }
    else {
        LOG_DEBUG("Update records without loading db");
        updateRecords(std::vector<std::pair<EventsRecord, TimePoint>>());
    }
}

unsigned int AllEventsModel::getMinimalItemHeight() const
{
    return style::window::label::big_h;
}

gui::ListItem *AllEventsModel::getItem(gui::Order order)
{
    auto record = getRecord(order);
    if (record == nullptr) {
        LOG_DEBUG("Empty record in AllEventsModel::GetItem");
        return nullptr;
    }
    auto *item = new gui::AllEventsItem();
    item->setEvent(std::make_shared<EventsRecord>(record->first));
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to event details window");
        auto rec       = std::make_unique<EventsRecord>(record->first);
        rec->date_from = record->second;
        auto data = std::make_unique<EventRecordData>(std::move(rec));
        application->switchWindow(style::window::calendar::name::details_window, std::move(data));
        return true;
    };

    return item;
}

bool AllEventsModel::updateRecords(std::vector<std::pair<EventsRecord, TimePoint>> records)
{
    MultiDayEventsDatabaseModel::updateRecords(std::move(records));
    list->setElementsCount(MultiDayEventsDatabaseModel::getBufferedRecordsSize());
    if (recordsCount != MultiDayEventsDatabaseModel::getBufferedRecordsSize()) {
        recordsCount = MultiDayEventsDatabaseModel::getBufferedRecordsSize();
        list->rebuildList(
            style::listview::RebuildType::OnOffset, MultiDayEventsDatabaseModel::getOffsetForCurrentTime(), true);
        return true;
    }
    list->onProviderDataUpdate();
    return true;
}

auto AllEventsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto response = dynamic_cast<db::query::events::GetAllResult *>(queryResult);
    assert(response != nullptr);

    auto records = response->getResult();
    if (counter != (records.size())) {
        MultiDayEventsDatabaseModel::clearRecords();
        counter      = records.size();
        recordsCount = counter;
        queryType    = db::Query::Type::Create;
        list->rebuildList(style::listview::RebuildType::Full, 0, true);
        return false;
    }

    auto app = dynamic_cast<app::ApplicationCalendar *>(application);
    assert(application != nullptr);
    if (counter == 0) {
        if (app->getEquivalentToEmptyWindow() == EquivalentWindow::AllEventsWindow) {
            app->switchToNoEventsWindow(utils::localize.get("app_calendar_title_main"), dateFilter);
        }
    }
    auto eventShift = app->getEventShift();
    if (eventShift) {
        for (auto &record : records) {
            record.date_from += std::chrono::hours(eventShift);
            record.date_till += std::chrono::hours(eventShift);
        }
    }
    return this->updateRecords(getEventsWithOriginalStartTime(records));
}


std::vector<std::pair<EventsRecord, TimePoint>> AllEventsModel::getEventsWithOriginalStartTime(
    const std::vector<EventsRecord> &rec)
{
    auto pairs = std::vector<std::pair<EventsRecord, TimePoint>>();
    pairs.reserve(rec.size());
    for (const auto &r : rec) {
        pairs.emplace_back(r, r.date_from);
    }
    return pairs;
}


void AllEventsModel::setDateFilter(TimePoint filter)
{
    dateFilter = filter;
}

void AllEventsModel::setQueryType(db::Query::Type type)
{
    queryType = type;
}
