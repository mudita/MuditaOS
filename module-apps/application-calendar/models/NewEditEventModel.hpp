// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>
#include <application-calendar/widgets/CalendarListItem.hpp>
#include <application-calendar/data/CalendarData.hpp>

namespace gui
{
    class TextWithLabelItem;
    class NewEventCheckBoxWithLabel;
    class CalendarDateItem;
    class CalendarTimeItem;
    class SeveralOptionsItem;
} // namespace gui

class NewEditEventModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

    TimePoint startTimeBuffer;
    TimePoint endTimeBuffer;

    gui::TextWithLabelItem *eventNameInput              = nullptr;
    gui::NewEventCheckBoxWithLabel *allDayEventCheckBox = nullptr;
    gui::CalendarDateItem *dateItem  = nullptr;
    gui::CalendarTimeItem *startTime = nullptr;
    gui::CalendarTimeItem *endTime   = nullptr;
    gui::SeveralOptionsItem *reminder                   = nullptr;
    gui::SeveralOptionsItem *repeat                     = nullptr;

    void createData(bool allDayEvent);
    void createTimeItems();
    void eraseTimeItems();
    void clearData();

  public:
    explicit NewEditEventModel(app::Application *app);

    void loadData(std::shared_ptr<EventsRecord> record);
    void loadRepeat(const std::shared_ptr<EventsRecord> &record);
    void saveData(std::shared_ptr<EventsRecord> event, EventAction action);

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
