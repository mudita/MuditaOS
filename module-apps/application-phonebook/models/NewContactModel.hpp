#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "module-apps/application-phonebook/widgets/ContactListItem.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"

class NewContactModel : public gui::ListItemProvider
{

    int modelIndex              = 0;
    unsigned int internalOffset = 0;
    unsigned int internalLimit  = 0;
    vector<gui::ContactListItem *> internalData;

    shared_ptr<ContactRecord> contact = nullptr;

  public:
    NewContactModel(app::Application *app);
    ~NewContactModel();

    void saveData();
    void loadData();

    int getItemCount() const override;

    unsigned int getMinimalItemHeight() override;

    gui::ListItem *getItem(gui::Order order) override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
