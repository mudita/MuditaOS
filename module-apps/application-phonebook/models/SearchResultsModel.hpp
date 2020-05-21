#pragma once

#include <vector>

#include "Application.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"

class SearchResultsModel : public gui::ListItemProvider, public app::DatabaseModel<ContactRecord>
{

    uint32_t internalOffset = 0;

  public:
    SearchResultsModel(app::Application *app);
    virtual ~SearchResultsModel();

    // virtual methods for ListViewProvider
    gui::ListItem *getItem(int index) override;

    int getItemCount() const override;
    void setResults(std::shared_ptr<std::vector<ContactRecord>> _results);
    void requestFavouritesCount();
    void requestRecordsCount() override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  private:
    std::shared_ptr<std::vector<ContactRecord>> results;
    uint32_t favouriteCount = 0;
};
