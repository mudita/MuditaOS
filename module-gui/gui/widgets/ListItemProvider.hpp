#pragma once

#include "ListItem.hpp"
#include "ListView.hpp"

namespace gui
{

    class ListView;

    class ListItemProvider
    {

      public:
        ListView *list                       = nullptr;
        style::listview::Direction direction = style::listview::Direction::Bottom;

        virtual int getItemCount() const = 0;

        virtual ListItem *getItem(int index) = 0;

        virtual void requestRecords(const uint32_t offset, const uint32_t limit) = 0;
    };

} // namespace gui
