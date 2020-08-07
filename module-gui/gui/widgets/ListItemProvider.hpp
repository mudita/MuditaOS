#pragma once

#include "ListItem.hpp"

namespace gui
{

    class ListView;

    enum class Order
    {
        Next,
        Previous
    };

    class ListItemProvider
    {

      public:
        virtual ~ListItemProvider() = default;

        ListView *list = nullptr;

        virtual unsigned int requestRecordsCount() = 0;

        virtual unsigned int getMinimalItemHeight() const = 0;

        virtual ListItem *getItem(Order order) = 0;

        virtual void requestRecords(const uint32_t offset, const uint32_t limit) = 0;
    };

} // namespace gui
