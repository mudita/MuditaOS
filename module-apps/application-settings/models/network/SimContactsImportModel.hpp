// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <InternalModel.hpp>
#include <ListItemProvider.hpp>
#include <ContactRecord.hpp>
#include <Application.hpp>

class SimContactsImportModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
  private:
    app::Application *application = nullptr;
    std::vector<ContactRecord> importedRecords;

  public:
    explicit SimContactsImportModel(app::Application *app);

    void createData();
    void clearData();

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
