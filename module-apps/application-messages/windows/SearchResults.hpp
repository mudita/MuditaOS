#pragma once

#include "AppWindow.hpp"
#include <ListView.hpp>
#include "../models/SearchResultsModel.hpp"
#include "messages/QueryMessage.hpp"

namespace gui
{
    namespace name::window
    {
        inline std::string search_results = "SearchResults";
    }

    class SearchResults : public AppWindow
    {
        gui::Item *body     = nullptr;
        gui::ListView *list = nullptr;
        std::unique_ptr<model::SearchResultsModel> model;
        auto listViewRequest(const std::string &text) -> bool;

      public:
        SearchResults(app::Application *app);
        /// needed to init populate db
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        /// needed to populate db responses
        auto onDatabaseMessage(sys::Message *msgl) -> bool override;
        auto showEmptyResults() -> bool;
        /// listView update on DB notification
        auto listViewUpdate(db::query::SMSSearchResult *) -> bool;
    };

} // namespace gui
