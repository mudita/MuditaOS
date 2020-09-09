#pragma once

#include "BaseThreadsRecordModel.hpp"
#include "Style.hpp"

namespace gui::model
{

    class ThreadsSearchResultsModel : public BaseThreadsRecordModel
    {
        UTF8 search_value;

      public:
        ThreadsSearchResultsModel(app::Application *app);

        unsigned int getMinimalItemHeight() const override;
        ListItem *getItem(Order order) override;
        /// empty, size get in requestRecords
        void requestRecords(uint32_t offset, uint32_t limit) override;
        /// set what we need to search
        void setSearchValue(const UTF8 &search_value);

        // virtual method for db::QueryListener
        auto handleQueryResponse(db::QueryResult *) -> bool override;
    };
}; // namespace gui::model
