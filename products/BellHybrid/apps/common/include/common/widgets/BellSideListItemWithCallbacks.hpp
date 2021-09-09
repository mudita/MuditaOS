// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/BellSideListItem.hpp>

namespace gui
{

    class BellSideListItemWithCallbacks : public BellSideListItem
    {
      public:
        explicit BellSideListItemWithCallbacks(const std::string &description);
        /// Fetch value from the list item and perform custom action.
        std::function<void()> getValue;
        /// Set list item's value and perform custom action.
        std::function<void()> setValue;
        std::function<void()> onExit;
    };

} // namespace gui
