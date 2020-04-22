#pragma once

#include "ListItem.hpp"
#include "ListView.hpp"

namespace gui
{

    class ListView;

    class ListItemProvider
    {

      public:

        bool listDataAvailable = false;

        virtual int getItemCount() const = 0;

        virtual ListItem *getItem(int index) = 0;

        virtual void requestRecords(const uint32_t offset, const uint32_t limit) = 0;
    };

} // namespace gui
