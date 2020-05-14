#pragma once

#include <vector>

#include "../ApplicationMessages.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ThreadRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"

namespace gui::model
{

    class SearchResultsModel : public gui::ListItemProvider, public app::DatabaseModel<ThreadRecord>
    {
      public:
        SearchResultsModel(app::Application *app);

        gui::ListItem *getItem(int index) override;
        int getItemCount() const override
        {
            return 10;
        } // TODO

        void setResults(std::shared_ptr<std::vector<ThreadRecord>> _results)
        {}                                                                           // TODO
        void requestRecordsCount() override{};                                       // TODO
        void requestRecords(const uint32_t offset, const uint32_t limit) override{}; // TODO
    };
}; // namespace gui::model
