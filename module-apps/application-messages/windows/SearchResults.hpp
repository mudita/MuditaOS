// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ThreadsSearchResultsModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>

namespace gui
{
    namespace name::window
    {
        inline constexpr auto search_results = "SearchResults";
    }

    class SearchResults : public AppWindow
    {
        gui::Item *body     = nullptr;
        gui::ListView *list = nullptr;
        std::shared_ptr<model::ThreadsSearchResultsModel> model;
        auto listViewRequest(const std::string &text) -> bool;

      public:
        SearchResults(app::ApplicationCommon *app);
        /// needed to init populate db
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto showEmptyResults() -> bool;
    };

} // namespace gui
