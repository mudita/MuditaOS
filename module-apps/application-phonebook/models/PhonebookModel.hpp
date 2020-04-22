/*
 * @file PhonebookModel.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 10 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_PHONEBOOK_PHONEBOOKMODEL_HPP_
#define MODULE_APPS_APPLICATION_PHONEBOOK_PHONEBOOKMODEL_HPP_

#include <vector>

#include "Application.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"
/*
 *
 */
class PhonebookModel : public gui::ListItemProvider, public app::DatabaseModel<ContactRecord>
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
                       uint32_t count,
                       bool favourite);
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    gui::ListItem *getItem(int index) override;
    int getItemCount() const override
    {
        return recordsCount;
    };
};

#endif /* MODULE_APPS_APPLICATION_PHONEBOOK_PHONEBOOKMODEL_HPP_ */
