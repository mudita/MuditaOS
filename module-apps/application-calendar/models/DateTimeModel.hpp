// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-calendar/widgets/CalendarListItem.hpp"
#include "application-calendar/widgets/EventDateItem.hpp"
#include "application-calendar/widgets/EventTimeItem.hpp"
#include "Application.hpp"
#include "InternalModel.hpp"

namespace gui
{
    class NewEventCheckBoxWithLabel;
}

class DateTimeModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application            = nullptr;
    bool mode24H                             = false;
    gui::EventDateItem *dateItem             = nullptr;
    gui::EventTimeItem *timeItem             = nullptr;
    gui::NewEventCheckBoxWithLabel *checkBox = nullptr;
    void createData(bool isAllDayEvent, DateTimeType dateTimeType);

  public:
    DateTimeModel(app::Application *app, bool mode24H = false);

    void loadData(std::shared_ptr<EventsRecord> record, DateTimeType dateTimeType);
    void reloadDataWithTimeItem();
    void loadDataWithoutTimeItem();
    [[nodiscard]] TimePoint getChosenDate(std::shared_ptr<EventsRecord> record, DateTimeType dateTimeType) const;

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(uint32_t offset, uint32_t limit) override;
};
