#pragma once

#include <vector>

#include "Application.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"

class SearchResultsModel : public gui::ListItemProvider
{
  public:
    SearchResultsModel(app::Application *app);
    virtual ~SearchResultsModel();

    gui::ListItem *getItem(int index, int firstElement, int prevIndex, uint32_t count, int remaining, bool topDown) override;
    int getItemCount();
    void setResults(std::shared_ptr<std::vector<ContactRecord>> _results)
    {
        results = _results;
    }

  private:
    std::shared_ptr<std::vector<ContactRecord>> results;
};
