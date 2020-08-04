#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"
#include "Application.hpp"
#include <ListItemProvider.hpp>

class DayEventsModel : public gui::ListItemProvider
{
    const int tempItemCount       = 5;
    app::Application *application = nullptr;

  public:
    DayEventsModel(app::Application *app);
    virtual ~DayEventsModel() override = default;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int requestRecordsCount() override
    {
        return tempItemCount;
    };
};
