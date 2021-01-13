// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/ContactRecord.hpp>
#include "BaseThreadsRecordModel.hpp"

class ThreadsModel : public BaseThreadsRecordModel, public app::AsyncCallbackReceiver
{
  public:
    explicit ThreadsModel(app::Application *app);

    void requestRecords(uint32_t offset, uint32_t limit) override;
    [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;
    [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;

    auto handleQueryResponse(db::QueryResult *queryResult) -> bool;
};
