// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllEventsModel.hpp"
#include "application-calendar/widgets/AllEventsItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include "module-apps/application-calendar/ApplicationCalendar.hpp"
#include <ListView.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <queries/calendar/QueryEventsGetAllLimited.hpp>

AllEventsModel::AllEventsModel(app::Application *app) : DatabaseModel(app), app::AsyncCallbackReceiver{app}
{
    application = app;
    assert(app != nullptr);
}

unsigned int AllEventsModel::requestRecordsCount()
{
    return recordsCount;
}

void AllEventsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    auto query = std::make_unique<db::query::events::GetAllLimited>(offset, limit);
    auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Events);
    task->setCallback([this](auto response) { return handleQueryResponse(response); });
    task->execute(application, this);
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
    item->setEvent(record);
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to event details window");
        auto rec  = std::make_unique<EventsRecord>(*record);
        auto data = std::make_unique<EventRecordData>(std::move(rec));
        application->switchWindow(style::window::calendar::name::details_window, std::move(data));
        return true;
    };

    return item;
}

bool AllEventsModel::updateRecords(std::vector<EventsRecord> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
}

auto AllEventsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto response = dynamic_cast<db::query::events::GetAllLimitedResult *>(queryResult);
    assert(response != nullptr);

    auto records = response->getResult();
    list->setElementsCount(response->getCountResult());

    auto app = dynamic_cast<app::ApplicationCalendar *>(application);
    assert(application != nullptr);

    if (records.empty()) {

        if (app->getEquivalentToEmptyWindow() == EquivalentWindow::AllEventsWindow) {
            auto filter = utils::time::TimePointNow();
            app->switchToNoEventsWindow(utils::localize.get("app_calendar_title_main"), filter);
        }
    }
    auto eventShift = app->getEventShift();
    if (eventShift) {
        for (auto &record : records) {
            record.date_from += std::chrono::hours(eventShift);
            record.date_till += std::chrono::hours(eventShift);
        }
    }
    return this->updateRecords(std::move(records));
}
