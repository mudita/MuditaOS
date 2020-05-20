#pragma once

#include "BaseThreadRecordModel.hpp"

namespace gui::model
{

    class SearchResultsModel : public BaseThreadRecordModel
    {
        UTF8 search_value = "";

      public:
        SearchResultsModel(app::Application *app);
        gui::ListItem *getItem(int index) override;
        /// resquest data for model
        void requestRecordsCount() override;
        /// set count in top model class - used for scroll bar
        void setRecordsCount(uint32_t count);
        /// set what we need to search
        void setSearchValue(const UTF8 &search_value);
    };
}; // namespace gui::model
