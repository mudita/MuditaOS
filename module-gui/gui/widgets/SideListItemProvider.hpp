// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SideListItem.hpp"

namespace gui
{

    class SideListView;

    class SideListItemProvider
    {

      public:
        SideListView *list = nullptr;

        virtual ~SideListItemProvider() = default;

        virtual unsigned int requestRecordsCount() = 0;

        virtual unsigned int getCurrentIndex() = 0;

        virtual SideListItem *getItem(Order order) = 0;
    };

} // namespace gui
