#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/CalendarListItem.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

class NewEditEventModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;
    bool mode24H                  = false;

  public:
    NewEditEventModel(app::Application *app, bool mode24H = false);

    void loadData(bool allDayEvent);
    void loadDataWithoutTimeItem();
    void reloadDataWithTimeItem();
    void saveData();

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] int getItemCount() const override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  private:
    void createData(bool allDayEvent);
};
