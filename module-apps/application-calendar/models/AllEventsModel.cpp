#include "AllEventsModel.hpp"
#include "application-calendar/widgets/AllEventsItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <ListView.hpp>

AllEventsModel::AllEventsModel(app::Application *app)
{
    application = app;
    assert(app != nullptr);
}

void AllEventsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    list->onProviderDataUpdate();
}

unsigned int AllEventsModel::getMinimalItemHeight() const
{
    return style::window::label::big_h;
}

gui::ListItem *AllEventsModel::getItem(gui::Order order)
{
    auto *item = new gui::AllEventsItem();

    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("Switch to event details window");
        application->switchWindow(style::window::calendar::name::details_window);
        return true;
    };

    return item;
}
