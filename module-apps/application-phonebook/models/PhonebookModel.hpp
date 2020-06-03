#pragma once

#include <vector>

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "Application.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"

class PhonebookModel : public app::DatabaseModel<ContactRecord>, public gui::ListItemProvider
{
    /**
     * Number of favourite records in the database.
     */
    uint32_t favouriteCount = 0;

  public:
    PhonebookModel(app::Application *app);
    virtual ~PhonebookModel() = default;

    // virtual methods from DatabaseModel
    void requestRecordsCount() override;
    bool updateRecords(std::unique_ptr<std::vector<ContactRecord>> records,
                       const uint32_t offset,
                       const uint32_t limit,
                       uint32_t count) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() override;
    gui::ListItem *getItem(gui::Order order) override;

    int getItemCount() const override
    {
        return recordsCount;
    };
};
