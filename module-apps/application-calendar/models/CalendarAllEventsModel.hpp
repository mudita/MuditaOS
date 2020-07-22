#pragma once

#include "Application.hpp"
#include <ListItemProvider.hpp>

class CalendarAllEventsModel : public gui::ListItemProvider
{
    const int tempItemCount       = 10;
    app::Application *application = nullptr;

  public:
    CalendarAllEventsModel(app::Application *app);
    virtual ~CalendarAllEventsModel() = default;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    int getItemCount() const override
    {
        return tempItemCount;
    };
};
