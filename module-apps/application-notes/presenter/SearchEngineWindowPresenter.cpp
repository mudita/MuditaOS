// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "SearchEngineWindowPresenter.hpp"
#include <utility>

namespace app::notes
{
    void SearchEngineWindowPresenter::searchFor(const std::string &searchText)
    {
        if (searchText.empty()) {
            getView()->emptySearch();
            return;
        }
        getView()->processValidSearch(searchText);
    }
} // namespace app::notes
