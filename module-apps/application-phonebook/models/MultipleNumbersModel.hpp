// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <InternalModel.hpp>
#include <ListItemProvider.hpp>
#include <Application.hpp>

class MultipleNumbersModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
  private:
    std::shared_ptr<ContactRecord> contactRecord;
    app::ApplicationCommon *application = nullptr;

  public:
    explicit MultipleNumbersModel(app::ApplicationCommon *app);

    void createData(std::shared_ptr<ContactRecord> contactRecord);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
