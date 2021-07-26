// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SimContactsRepository.hpp"
#include <application-settings/widgets/network/SimContactImportSelectWidget.hpp>

#include <InternalModel.hpp>
#include <ListItemProvider.hpp>
#include <ContactRecord.hpp>
#include <Application.hpp>

class SimContactsImportModel : public app::InternalModel<gui::SimContactImportSelectWidget *>,
                               public gui::ListItemProvider
{
  private:
    app::Application *application = nullptr;
    std::shared_ptr<AbstractSimContactsRepository> contactsRepository;
    std::vector<bool> getSelectedContacts();

  public:
    SimContactsImportModel(app::Application *app, std::unique_ptr<AbstractSimContactsRepository> contactsRepository);

    void createSimImported();
    void createDuplicates();
    unsigned int getDuplicatesCount();
    void createData(const std::vector<ContactRecord> &importedRecords);
    void clearData();
    void eraseData();
    void requestSimContacts(std::function<void()> onSimContactsReadCallback);
    void saveData(std::function<void()> onSaveCallback);
    void findDuplicates(std::function<void(bool)> onDuplicatesCheckCallback);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
