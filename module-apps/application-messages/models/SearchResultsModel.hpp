#pragma once

#include "BaseThreadRecordModel.hpp"
#include "Style.hpp"

namespace gui::model
{

    class SearchResultsModel : public BaseThreadRecordModel
    {
        UTF8 search_value;
        /// definition of how many items we shall request from db per page : as many will fit + 1
        const unsigned int max_search_items_on_screen =
            (style::window_height / style::window::messages::sms_thread_item_h) + 1;

      public:
        SearchResultsModel(app::Application *app);

        int getMinimalItemHeight() override;
        ListItem *getItem(Order order) override;
        /// resquest data for model - not needed
        void requestRecordsCount() override;
        /// empty, size get in requestRecords
        void requestRecords(uint32_t offset, uint32_t limit) override;
        /// set count in top model class - used for scroll bar
        void setRecordsCount(uint32_t count);
        /// set what we need to search
        void setSearchValue(const UTF8 &search_value);
        auto getMaxItemsOnScreen() const -> unsigned int
        {
            return max_search_items_on_screen;
        }
    };
}; // namespace gui::model
