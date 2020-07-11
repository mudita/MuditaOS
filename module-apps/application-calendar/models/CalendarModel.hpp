#pragma once

#include "Application.hpp"
#include "ListItemProvider.hpp"

class CalendarModel : public gui::ListItemProvider
{
    const unsigned int itemHeight = 90;
    const int tempItemCount       = 5; // temporary
  public:
    CalendarModel(app::Application *app);
    virtual ~CalendarModel() = default;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() override;
    gui::ListItem *getItem(gui::Order order) override;
    int getItemCount() const override
    {
        return tempItemCount;
    };
};
