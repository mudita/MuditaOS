
#include "CalendarEventsModel.hpp"
#include "application-calendar/widgets/CalendarItem.hpp"

#include <ListView.hpp>

CalendarEventsModel::CalendarEventsModel(app::Application *app) : DatabaseModel(app)
{
    application = app;
}

void CalendarEventsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    list->onProviderDataUpdate();
}

unsigned int CalendarEventsModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::height;
}

gui::ListItem *CalendarEventsModel::getItem(gui::Order order)
{
    gui::CalendarItem *item = new gui::CalendarItem();

    auto record = getRecord(order);
    if (record != nullptr) {
        item->setEvent(record);
    }
    else {
        LOG_DEBUG("Empty record in EventsMODEL::GetItem!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
    LOG_DEBUG("Created new item in calendar day listView");
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to options/delete window");
        application->switchWindow(style::window::calendar::name::events_options);
        return true;
    };
    return item;
}
bool CalendarEventsModel::updateRecords(std::unique_ptr<std::vector<EventsRecord>> records,
                                        uint32_t offset,
                                        uint32_t limit,
                                        uint32_t count)
{
    DatabaseModel::updateRecords(std::move(records), offset, limit, count);
    modelIndex = 0;
    list->onProviderDataUpdate();

    return true;
}
