// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include <application-calendar/widgets/CalendarDateItem.hpp>
#include <application-calendar/widgets/CalendarTimeItem.hpp>
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/TextWithLabelItem.hpp"
#include "application-calendar/widgets/SeveralOptionsItem.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

namespace gui
{
    class NewEventCheckBoxWithLabel; // fw declaration
}

class NewEditEventModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;
    bool mode24H                  = false;

    gui::NewEventCheckBoxWithLabel *allDayEventCheckBox = nullptr;

    gui::CalendarDateItem *dateItem  = nullptr;
    gui::CalendarTimeItem *startTime = nullptr;
    gui::CalendarTimeItem *endTime   = nullptr;

    gui::SeveralOptionsItem *reminder      = nullptr;
    gui::SeveralOptionsItem *repeat        = nullptr;
    gui::TextWithLabelItem *eventNameInput = nullptr;

  public:
    NewEditEventModel(app::Application *app, bool mode24H = false);

    void loadData(std::shared_ptr<EventsRecord> record);
    void loadRepeat(const std::shared_ptr<EventsRecord> &record);
    void loadDataWithoutTimeItem();
    void reloadDataWithTimeItem();
    void saveData(std::shared_ptr<EventsRecord> event, EventAction action);

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    uint32_t getRepeatOptionValue();
    void setRepeatOptionValue(const uint32_t &value);

  private:
    void createData(bool allDayEvent);
};
