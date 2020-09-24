#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "InternalModel.hpp"
#include <ListItem.hpp>
#include <ListItemProvider.hpp>
#include <module-db/Interface/EventsRecord.hpp>

class DayEventsInternalModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    DayEventsInternalModel(app::Application *app);

    void loadData(std::unique_ptr<std::vector<EventsRecord>> records);

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
