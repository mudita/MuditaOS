#pragma once

#include "Application.hpp"
#include "ListItem.hpp"
#include "ListItemProvider.hpp"
#include "module-apps/application-phonebook/widgets/InputLineWithLabelItem.hpp"

class NewContactModel : public gui::ListItemProvider
{

    int modelIndex              = 0;
    unsigned int internalOffset = 0;
    unsigned int internalLimit  = 0;

  public:
    NewContactModel(app::Application *app);
    ~NewContactModel();

    vector<gui::ListItem *> internalData;

    int getItemCount() const override;

    unsigned int getMinimalItemHeight() override;

    gui::ListItem *getItem(gui::Order order) override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
