// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/TextWithLabelItem.hpp"
#include "application-calendar/widgets/SeveralOptionsItem.hpp"
#include "application-calendar/widgets/CalendarDateAndTimeBox.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>
#include "widgets/DateWidget.hpp"
#include "widgets/TimeWidget.hpp"

namespace gui
{
    class NewEventCheckBoxWithLabel; // fw declaration
}

class NewEditEventModel : public app::InternalModel<gui::DateOrTimeListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

    gui::TextWithLabelItem *eventNameInput          = nullptr;
    gui::NewEventCheckBoxWithLabel *allDayEventCheckBox = nullptr;
    gui::CalendarDateAndTimeBox *calendarDateAndTimeBox = nullptr;
    gui::SeveralOptionsItem *reminder               = nullptr;
    gui::SeveralOptionsItem *repeat                 = nullptr;

  public:
    NewEditEventModel(app::Application *app);

    void loadData(std::shared_ptr<EventsRecord> record);
    void loadRepeat(const std::shared_ptr<EventsRecord> &record);
    void loadDataWithoutTimeItem();
    void reloadDataWithTimeItem();
    void saveData(std::shared_ptr<EventsRecord> event, EventAction action);
    void addReapetedRecords(std::shared_ptr<EventsRecord> event);

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    uint32_t getRepeatOptionValue();
    void setRepeatOptionValue(const uint32_t &value);

  private:
    void createData(bool allDayEvent);
};
