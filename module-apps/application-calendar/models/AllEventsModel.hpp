// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include "application-calendar/models/MultiDayEventsDatabaseModel.hpp"
#include <ListItemProvider.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <module-db/Common/Query.hpp>

class AllEventsModel : public app::MultiDayEventsDatabaseModel,
                       public gui::ListItemProvider,
                       public app::AsyncCallbackReceiver
{
    uint32_t counter          = 0;
    TimePoint dateFilter      = TimePointNow();
    db::Query::Type queryType = db::Query::Type::Create;
    std::vector<std::pair<EventsRecord, TimePoint>> getEventsAndEventsStartTime(const std::vector<EventsRecord> &rec);

  public:
    explicit AllEventsModel(app::Application *app);

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    bool updateRecords(std::vector<std::pair<EventsRecord, TimePoint>> records) override;

    auto handleQueryResponse(db::QueryResult *) -> bool;
    void setDateFilter(TimePoint filter);
    void setQueryType(db::Query::Type type);
    // virtual methods for ListViewProvider
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
};
