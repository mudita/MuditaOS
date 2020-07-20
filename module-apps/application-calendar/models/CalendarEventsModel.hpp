#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"
#include "Application.hpp"
#include <ListItemProvider.hpp>

class CalendarEventsModel : public gui::ListItemProvider
{
    const int tempItemCount       = 5;
    app::Application *application = nullptr;

  public:
    CalendarEventsModel(app::Application *app);

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    int getItemCount() const override
    {
        return tempItemCount;
    };
};
