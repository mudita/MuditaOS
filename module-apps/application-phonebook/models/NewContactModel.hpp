// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"
#include "InternalModel.hpp"
#include "Application.hpp"

#include <ListItemProvider.hpp>

class NewContactModel : public app::InternalModel<gui::ContactListItem *>, public gui::ListItemProvider
{
  private:
    app::ApplicationCommon *application = nullptr;

    void openTextOptions(gui::Text *text);

  public:
    explicit NewContactModel(app::ApplicationCommon *app);

    void clearData();
    void saveData(std::shared_ptr<ContactRecord> contactRecord);
    void loadData(std::shared_ptr<ContactRecord> contactRecord);
    void createData();
    bool verifyData();
    bool emptyData();

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
