#include "DayEventsModel.hpp"
#include "application-calendar/widgets/DayEventsItem.hpp"
#include <ListView.hpp>

DayEventsModel::DayEventsModel(app::Application *app)
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
    return style::window::calendar::item::height;
}

gui::ListItem *DayEventsModel::getItem(gui::Order order)
{
    auto *item              = new gui::DayEventsItem();
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to options/delete window");
        application->switchWindow(style::window::calendar::name::details_window);
        return true;
    };
    return item;
}
