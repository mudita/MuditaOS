#pragma once

#include "Application.hpp"
#include "ListItemProvider.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"

class CalendarModel : public gui::ListItemProvider
{
    const int tempItemCount       = 5;
    app::Application *application = nullptr;

  public:
    CalendarModel(app::Application *app);

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    int getItemCount() const override
    {
        return tempItemCount;
    };
};
