// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>

#include "DatabaseModel.hpp"
#include "CalllogRecord.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"

class CalllogModel : public app::DatabaseModel<CalllogRecord>,
                     public gui::ListItemProvider,
                     public app::AsyncCallbackReceiver
{
  public:
    explicit CalllogModel(app::Application *app);

    [[nodiscard]] unsigned int requestRecordsCount() override;
    [[nodiscard]] bool updateRecords(std::vector<CalllogRecord> records) override;
    void requestRecords(uint32_t offset, uint32_t limit) override;

    [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;
    [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;

  private:
    bool onCalllogRetrieved(const std::vector<CalllogRecord> &records, unsigned int repoCount);
};
