#include "CalendarModel.hpp"
#include "ListView.hpp"
#include "../widgets/CalendarItem.hpp"
#include "../widgets/CalendarItem.cpp"

CalendarModel::CalendarModel(app::Application *app)
{}

void CalendarModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    list->onProviderDataUpdate(); // listView refresh()
}

unsigned int CalendarModel::getMinimalItemHeight()
{
    return itemHeight; // item height in ListView
}

gui::ListItem *CalendarModel::getItem(gui::Order order)
{
    // ----- temporary --------
    static int items = getItemCount() + 1;
    if (items == 0) {
        items = getItemCount();
        return nullptr;
    }
    // ------------------------
    gui::CalendarItem *item = new gui::CalendarItem(this);
    LOG_DEBUG("Created new item in calendar day listView");
    items--;
    return item;
}
