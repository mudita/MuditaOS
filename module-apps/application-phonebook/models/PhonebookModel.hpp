#pragma once

#include <vector>

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/widgets/PhonebookItem.hpp"
#include "Application.hpp"
#include "Common/Query.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"

#include <string>

class PhonebookModel : public app::DatabaseModel<ContactRecord>, public gui::ListItemProvider, public db::QueryListener
{
  private:
    std::string queryFilter;
    std::uint32_t queryGroupFilter;

  public:
    PhonebookModel(app::Application *app, std::string filter = "", std::uint32_t groupFilter = 0);
    ~PhonebookModel() override = default;

    // virtual methods from DatabaseModel
    auto updateRecords(std::unique_ptr<std::vector<ContactRecord>> records) -> bool override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;

    // virtual method for db::QueryListener
    auto handleQueryResponse(db::QueryResult *) -> bool override;

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

    [[nodiscard]] auto getFilter() const -> const std::string &;

    // onClick callback to register
    std::function<bool(gui::PhonebookItem *item)> messagesSelectCallback = nullptr;
};
