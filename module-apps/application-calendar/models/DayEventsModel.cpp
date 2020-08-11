#include "DayEventsModel.hpp"
#include "application-calendar/widgets/DayEventsItem.hpp"
#include <ListView.hpp>

DayEventsModel::DayEventsModel(app::Application *app) : DatabaseModel(app)
{
    application = app;
    assert(app != nullptr);
}

void DayEventsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    list->onProviderDataUpdate();
}

unsigned int DayEventsModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::dayEvents::height;
}

gui::ListItem *DayEventsModel::getItem(gui::Order order)
{
    auto item = new gui::DayEventsItem();

    auto record = getRecord(order);
    if (record != nullptr) {
        item->setEvent(record);
    }
    else {
        LOG_DEBUG("Empty record in EventsModel::GetItem");
        return nullptr;
    }
    LOG_DEBUG("Created new item in calendar day listView");
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to options/delete window");
        application->switchWindow(style::window::calendar::name::events_options);
        return true;
    };
    return item;
}

bool DayEventsModel::updateRecords(std::unique_ptr<std::vector<EventsRecord>> records)
{
    DatabaseModel::updateRecords(std::move(records));
    modelIndex = 0;
    list->onProviderDataUpdate();

    return true;
}

void DayEventsModel::setRecordsCount(const uint32_t &count)
{
    recordsCount = count;
}
