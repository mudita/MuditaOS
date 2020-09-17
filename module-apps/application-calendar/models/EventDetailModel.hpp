#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/CalendarListItem.hpp"
#include "module-apps/application-calendar/ApplicationCalendar.hpp"
#include "Application.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

class EventDetailModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    EventDetailModel(app::Application *app);

    void loadData(std::shared_ptr<EventsRecord> record);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  private:
    void createData();
};
