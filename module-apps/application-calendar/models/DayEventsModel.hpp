#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"
#include "Application.hpp"
#include "DatabaseModel.hpp"
#include <ListItemProvider.hpp>
#include <module-db/Interface/EventsRecord.hpp>

class DayEventsModel :  public app::DatabaseModel<EventsRecord>, public gui::ListItemProvider
{
    const int tempItemCount       = 5;
    app::Application *application = nullptr;

  public:
    DayEventsModel(app::Application *app);
    virtual ~DayEventsModel() override = default;
    auto updateRecords(std::unique_ptr<std::vector<EventsRecord>> records,
                       const uint32_t offset = 0,
                       const uint32_t limit  = 0,
                       uint32_t count        = 0) -> bool override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int requestRecordsCount() override
    {
        return tempItemCount;
    };
};
