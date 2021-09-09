// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseThreadsRecordModel.hpp"
#include "Style.hpp"

namespace gui::model
{

    class ThreadsSearchResultsModel : public BaseThreadsRecordModel, public app::AsyncCallbackReceiver
    {
        UTF8 textToSearch;

      public:
        ThreadsSearchResultsModel(app::ApplicationCommon *app);

        auto getMinimalItemSpaceRequired() const -> unsigned int override;
        auto getItem(Order order) -> ListItem * override;
        /// empty, size get in requestRecords
        void requestRecords(uint32_t offset, uint32_t limit) override;
        /// set what we need to search
        void setSearchValue(const UTF8 &search_value);

        auto handleQueryResponse(db::QueryResult *) -> bool;
    };
}; // namespace gui::model
