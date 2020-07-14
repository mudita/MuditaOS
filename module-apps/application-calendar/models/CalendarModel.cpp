#include "CalendarModel.hpp"
#include "ListView.hpp"
#include "application-calendar/widgets/CalendarItem.hpp"

CalendarModel::CalendarModel(app::Application *app)
{
    application = app;
}

void CalendarModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    list->onProviderDataUpdate();
}

unsigned int CalendarModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::height;
}

gui::ListItem *CalendarModel::getItem(gui::Order order)
{
    gui::CalendarItem *item = new gui::CalendarItem();
    LOG_DEBUG("Created new item in calendar day listView");
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to options/delete window");
        return true;
    };
    return item;
}
