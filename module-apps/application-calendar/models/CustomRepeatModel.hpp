#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/CalendarListItem.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

class CustomRepeatModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    CustomRepeatModel(app::Application *app);

    void loadData(const std::shared_ptr<WeekDaysRepeatData> &data);

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    std::vector<gui::CalendarListItem *> getInternalData()
    {
        return internalData;
    }

  private:
    void createData(const std::shared_ptr<WeekDaysRepeatData> &data);
};
