#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/CalendarListItem.hpp"
#include "application-calendar/widgets/TextWithLabelItem.hpp"
#include "application-calendar/widgets/EventTimeItem.hpp"
#include "application-calendar/widgets/SeveralOptionsItem.hpp"
#include "application-calendar/widgets/CheckBoxWithLabelItem.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

class NewEditEventModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;
    bool mode24H                  = false;

    gui::TextWithLabelItem *eventNameInput          = nullptr;
    gui::CheckBoxWithLabelItem *allDayEventCheckBox = nullptr;
    gui::EventTimeItem *startTime                   = nullptr;
    gui::EventTimeItem *endTime                     = nullptr;
    gui::SeveralOptionsItem *reminder               = nullptr;
    gui::SeveralOptionsItem *repeat                 = nullptr;

  public:
    NewEditEventModel(app::Application *app, bool mode24H = false);

    void loadData(bool allDayEvent);
    void loadDataWithoutTimeItem();
    void reloadDataWithTimeItem();
    void saveData();

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  private:
    void createData(bool allDayEvent);
};
