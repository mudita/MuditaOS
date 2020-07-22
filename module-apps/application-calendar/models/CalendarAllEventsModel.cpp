#include "CalendarAllEventsModel.hpp"
#include "application-calendar/widgets/CalendarAllEventsItem.hpp"
#include <ListView.hpp>

CalendarAllEventsModel::CalendarAllEventsModel(app::Application *app)
{
    application = app;
}

void CalendarAllEventsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    list->onProviderDataUpdate();
}

unsigned int CalendarAllEventsModel::getMinimalItemHeight() const
{
    return style::window::label::big_h;
}

gui::ListItem *CalendarAllEventsModel::getItem(gui::Order order)
{
    gui::CalendarAllEventsItem *item = new gui::CalendarAllEventsItem();

    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to options/delete window");
        return true;
    };

    return item;
}
