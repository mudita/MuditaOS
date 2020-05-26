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
        ListView *list = nullptr;

        virtual int getItemCount() const = 0;

        virtual ListItem *getItem(Order order) = 0;

        virtual void requestRecords(const uint32_t offset, const uint32_t limit) = 0;
    };

} // namespace gui
