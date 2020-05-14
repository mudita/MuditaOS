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
        std::unique_ptr<gui::ListItemProvider> provider;

      public:
        SearchResults(app::Application *app);
    };

} // namespace gui
