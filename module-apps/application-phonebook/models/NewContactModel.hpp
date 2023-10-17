// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"
#include "InternalModel.hpp"
#include "Application.hpp"

#include <ListItemProvider.hpp>
#include <SwitchData.hpp>

class NewContactModel : public app::InternalModel<gui::ContactListItem *>, public gui::ListItemProvider
{
  private:
    app::ApplicationCommon *application        = nullptr;
    PhonebookItemData::RequestType requestType = PhonebookItemData::RequestType::Internal;

    void openTextOptions(gui::Text *text);

  public:
    explicit NewContactModel(app::ApplicationCommon *app);

    void clearData();
    void saveData(std::shared_ptr<ContactRecord> contactRecord);
    void loadData(std::shared_ptr<ContactRecord> contactRecord, gui::SwitchData *data);
    void createData();
    bool verifyData();
    bool emptyData();
    bool readyToSave();
    bool isAnyUnsavedChange(std::shared_ptr<ContactRecord> contactRecord);
    [[nodiscard]] auto getRequestType() -> PhonebookItemData::RequestType;

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(std::uint32_t offset, std::uint32_t limit) override;
};
