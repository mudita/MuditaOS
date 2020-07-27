#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <ListItemProvider.hpp>

class CustomRepeatModel : public gui::ListItemProvider
{
    app::Application *application = nullptr;
    const int daysInWeek          = 7;
    int count                     = 7;

  public:
    CustomRepeatModel(app::Application *app);

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    int getItemCount() const override
    {
        return daysInWeek;
    };
};
