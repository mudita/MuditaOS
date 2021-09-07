// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListItem.hpp"

namespace gui
{
    class ListViewEngine;

    enum class Order
    {
        Next,
        Previous
    };

    class ListItemProvider
    {
      public:
        virtual ~ListItemProvider() = default;

        ListViewEngine *list = nullptr;

        virtual unsigned int requestRecordsCount() = 0;

        [[nodiscard]] virtual unsigned int getMinimalItemSpaceRequired() const = 0;

        virtual ListItem *getItem(Order order) = 0;

        virtual void requestRecords(uint32_t offset, uint32_t limit) = 0;
    };

} // namespace gui
