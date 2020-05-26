#pragma once

#include <vector>

#include "Application.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"

class SearchResultsModel : public gui::ListItemProvider
{

    int internalOffset            = 0;
    int internalLimit             = 0;
    app::Application *application = nullptr;
    std::shared_ptr<std::vector<ContactRecord>> results;

  public:
    SearchResultsModel(app::Application *app);
    virtual ~SearchResultsModel();

    // virtual methods for ListViewProvider
    gui::ListItem *getItem(gui::Order order) override;

    int getItemCount() const override;
    void setResults(std::shared_ptr<std::vector<ContactRecord>> _results);
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
