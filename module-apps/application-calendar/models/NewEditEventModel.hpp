#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/CalendarListItem.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

class NewEditEventModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    NewEditEventModel(app::Application *app);

    void loadData();

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] int getItemCount() const override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  private:
    void createData();
};
