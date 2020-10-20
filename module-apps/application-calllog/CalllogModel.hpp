// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>

#include "Interface/CalllogRecord.hpp"
#include "DatabaseModel.hpp"
#include "CalllogRecord.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"

/*
 *
 */
class CalllogModel : public app::DatabaseModel<CalllogRecord>, public gui::ListItemProvider
{
  public:
    CalllogModel() = delete;
    CalllogModel(app::Application *app);

    // virtual methods
    bool updateRecords(std::vector<CalllogRecord> records) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
};
