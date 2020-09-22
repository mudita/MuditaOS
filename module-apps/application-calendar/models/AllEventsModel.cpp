#include "AllEventsModel.hpp"
#include "application-calendar/widgets/AllEventsItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include "module-apps/application-calendar/ApplicationCalendar.hpp"
#include <ListView.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <queries/calendar/QueryEventsGetAllLimited.hpp>

AllEventsModel::AllEventsModel(app::Application *app) : DatabaseModel(app)
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
    query->setQueryListener(
        db::QueryCallback::fromFunction([this](auto response) { return handleQueryResponse(response); }));
    DBServiceAPI::GetQuery(application, db::Interface::Name::Events, std::move(query));
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
        data->setWindowName(style::window::calendar::name::all_events_window);
        application->switchWindow(style::window::calendar::name::details_window, std::move(data));
        return true;
    };

    return item;
}

bool AllEventsModel::updateRecords(std::unique_ptr<std::vector<EventsRecord>> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
}

auto AllEventsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto response = dynamic_cast<db::query::events::GetAllLimitedResult *>(queryResult);
    assert(response != nullptr);

    auto records_data = response->getResult();
    list->setElementsCount(*response->getCountResult());
    auto records = std::make_unique<std::vector<EventsRecord>>(records_data->begin(), records_data->end());

    if (records->empty()) {
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(application != nullptr);
        auto filter = std::chrono::system_clock::now();
        app->switchToNoEventsWindow(
            utils::localize.get("app_calendar_title_main"), filter, style::window::calendar::name::all_events_window);
    }

    return this->updateRecords(std::move(records));
}
