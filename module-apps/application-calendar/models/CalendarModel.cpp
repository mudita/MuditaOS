#include "CalendarModel.hpp"
#include "application-calendar/widgets/CalendarItem.hpp"

#include <ListView.hpp>

CalendarEventsModel::CalendarEventsModel(app::Application *app)
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
    LOG_DEBUG("Created new item in calendar day listView");
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to options/delete window");
        return true;
    };
    return item;
}
