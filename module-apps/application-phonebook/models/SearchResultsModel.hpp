#pragma once

#include <vector>

#include "Application.hpp"
#include "DatabaseModel_old.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider_old.hpp"
#include "NotesRecord.hpp"

class SearchResultsModel : public gui::ListItemProvider_old, public app::DatabaseModel_old<ContactRecord>
{
  public:
    SearchResultsModel(app::Application *app);
    virtual ~SearchResultsModel();

    gui::ListItem *getItem(
        int index, int firstElement, int prevIndex, uint32_t count, int remaining, bool topDown) override;
    int getItemCount() const override;
    void setResults(std::shared_ptr<std::vector<ContactRecord>> _results);
    void requestFavouritesCount();
    void requestRecordsCount() override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  private:
    std::shared_ptr<std::vector<ContactRecord>> results;
    uint32_t favouriteCount = 0;
};
