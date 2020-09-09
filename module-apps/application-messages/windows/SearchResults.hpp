#pragma once

#include "AppWindow.hpp"
#include <ListView.hpp>
#include "application-messages/models/ThreadsSearchResultsModel.hpp"
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
        std::shared_ptr<model::ThreadsSearchResultsModel> model;
        auto listViewRequest(const std::string &text) -> bool;

      public:
        SearchResults(app::Application *app);
        /// needed to init populate db
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto showEmptyResults() -> bool;
    };

} // namespace gui
