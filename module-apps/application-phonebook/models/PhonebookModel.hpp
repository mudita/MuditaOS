#pragma once

#include <vector>

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/widgets/PhonebookItem.hpp"
#include "Application.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"

#include <string>

class PhonebookModel : public app::DatabaseModel<ContactRecord>, public gui::ListItemProvider
{
  private:
    std::string queryFilter;

  public:
    PhonebookModel(app::Application *app, std::string filter = "");
    ~PhonebookModel() override = default;

    // virtual methods from DatabaseModel
    void requestRecordsCount() override;
    auto updateRecords(std::unique_ptr<std::vector<ContactRecord>> records,
                       const uint32_t offset = 0,
                       const uint32_t limit  = 0,
                       uint32_t count        = 0) -> bool override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    auto getMinimalItemHeight() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;

    [[nodiscard]] auto getItemCount() const -> int override
    {
        return recordsCount;
    };

    [[nodiscard]] auto getFilter() const -> const std::string &;

    // onClick callback to register
    std::function<bool(gui::PhonebookItem *item)> messagesSelectCallback = nullptr;
};
