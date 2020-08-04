#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"
#include "InternalModel.hpp"
#include "Application.hpp"

#include <ListItemProvider.hpp>

class ContactDetailsModel : public app::InternalModel<gui::ContactListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    ContactDetailsModel(app::Application *app);

    void loadData(std::shared_ptr<ContactRecord> contactRecord);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  private:
    void createData(bool showInformationWidget = false, bool showAddressWidget = false, bool showNoteWidget = false);
};
