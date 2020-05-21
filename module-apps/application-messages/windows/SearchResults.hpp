#pragma once

#include "AppWindow.hpp"
#include <ListView.hpp>
#include "../models/SearchResultsModel.hpp"

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

      public:
        SearchResults(app::Application *app);
        /// needed to init populate db
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        /// needed to populate db responses
        auto onDatabaseMessage(sys::Message *msgl) -> bool override;
    };

} // namespace gui
