// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListItem.hpp"
#include "ListItemProvider.hpp"

namespace gui
{
    class SideListView;

    class SideListItemProvider
    {

      public:
        SideListView *list = nullptr;

        virtual ~SideListItemProvider() = default;

        virtual unsigned int requestRecordsCount() = 0;

        virtual ListItem *getItem(Order order) = 0;
    };
} // namespace gui
