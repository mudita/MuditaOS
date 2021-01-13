// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include <ListItemProvider.hpp>
#include <DatabaseModel.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <module-db/Common/Query.hpp>

class AllEventsModel : public app::DatabaseModel<EventsRecord>,
                       public gui::ListItemProvider,
                       public app::AsyncCallbackReceiver
{
    app::Application *application  = nullptr;

  public:
    AllEventsModel(app::Application *app);

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    bool updateRecords(std::vector<EventsRecord> records) override;

    auto handleQueryResponse(db::QueryResult *) -> bool;
    // virtual methods for ListViewProvider
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
};
