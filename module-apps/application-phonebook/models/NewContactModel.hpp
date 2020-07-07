#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"
#include "Application.hpp"

#include <ListItemProvider.hpp>

class NewContactModel : public gui::ListItemProvider
{

    int modelIndex              = 0;
    unsigned int internalOffset = 0;
    unsigned int internalLimit  = 0;
    vector<gui::ContactListItem *> internalData;
    app::Application *application = nullptr;

  public:
    NewContactModel(app::Application *app);
    virtual ~NewContactModel();

    void clearData();
    void saveData(std::shared_ptr<ContactRecord> contactRecord);
    void loadData(std::shared_ptr<ContactRecord> contactRecord);
    void createData();

    [[nodiscard]] auto getItemCount() const -> int override;

    auto getMinimalItemHeight() -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
