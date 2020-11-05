// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "DatabaseModel.hpp"
#include <ListItem.hpp>
#include <ListItemProvider.hpp>
#include <module-db/Interface/EventsRecord.hpp>

class DayEventsModel : public app::DatabaseModel<EventsRecord>,
                       public gui::ListItemProvider,
                       public app::AsyncCallbackReceiver
{
    app::Application *application = nullptr;
    std::string dayMonthTitle;
    utils::time::TimePoint filterFrom = utils::time::TIME_POINT_INVALID;
    utils::time::TimePoint filterTill = utils::time::TIME_POINT_INVALID;

  public:
    DayEventsModel(app::Application *app);

    void setFilters(utils::time::TimePoint from, utils::time::TimePoint till, const std::string &dayMonth);

    bool updateRecords(std::vector<EventsRecord> records) override;
    auto handleQueryResponse(db::QueryResult *) -> bool;

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
