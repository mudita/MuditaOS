#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <ListItemProvider.hpp>

class CustomRepeatModel : public gui::ListItemProvider
{
    app::Application *application = nullptr;
    int count                     = 7;

  public:
    CustomRepeatModel(app::Application *app);

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    // virtual methods for ListViewProvider
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] int getItemCount() const override
    {
        return style::window::calendar::week_days_number;
    };
};
